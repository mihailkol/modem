#ifdef MODULE_MODEM
#include "ModemHandler.h"
#include "../../core/ConfigManager.h"
#include "../../core/WebHandler.h"
#include <AsyncJson.h>
#include <ArduinoJson.h>
#include <LittleFS.h>

#ifdef MODULE_MQTT
#include "../mqtt/MqttHandler.h"
#endif
#ifdef MODULE_TELEGRAM
#include "../telegram/TgHandler.h"
#endif

// ── Статические данные ───────────────────────────────────────────────────────
ModemConfig modemCfg;
ModemStatus modemStatus;

// Глобальные переменные для доступа из других модулей без include
int  _modemCreg    = -1;
bool _modemEnabled = false;

void (*ModemHandler::onDTMFResult)(const char*, const char*) = nullptr;

static HardwareSerial _sim(2);

// ── Состояние звонка ─────────────────────────────────────────────────────────
enum CallState { CALL_IDLE, CALL_RINGING, CALL_IN_CALL };
static CallState _callState = CALL_IDLE;
static String   _lineBuffer = "";
static String   _modemLog   = "";
static String   _callerNum  = "";
static String   _dtmfSeq    = "";
static uint32_t _callStart  = 0;
static String   _callLogBuffer = "";

// SMS
static bool   _smsNextLine  = false;  // ждём тело SMS после заголовка +CMT:
static String _smsSender    = "";
static String _smsInbox[10];          // кольцевой буфер последних 10 SMS
static uint8_t _smsInboxIdx = 0;

// Неблокирующая задержка между ATA и AT+CMUT=1
static bool     _answerPending    = false;
static uint32_t _answerAt         = 0;

// Флаг: loop() поймал "Call Ready" во время фазы INIT_RST_HIGH
static bool     _callReadyReceived = false;

// Флаг ручного ресета из веб-интерфейса
static bool     _manualRestart = false;

// ── Стейт-машина инициализации ────────────────────────────────────────────────
//
//  INIT_RST_LOW  → держим RST=LOW 200 мс
//  INIT_RST_HIGH → подняли RST, ждём "Call Ready" до 15 сек
//  INIT_CMD      → шлём команды по одной, ждём 500 мс между ними
//  INIT_DONE     → инициализация завершена, loop() работает штатно
//
enum InitState { INIT_RST_LOW, INIT_RST_HIGH, INIT_CMD, INIT_DONE };
static InitState _initState = INIT_RST_LOW;
static bool _simCmdsAllowed = false;  // разрешено отправлять SIM команды

static uint32_t _initTimer   = 0;
static uint8_t  _initCmdIdx  = 0;

static const char* _initCmds[] = {
    "AT",
    "ATE0",
    "AT+CLTS=1",
    "AT&W",
    "AT+CREG=2",
};

static const uint8_t _initCmdsCount = sizeof(_initCmds) / sizeof(_initCmds[0]);

// ── Состояние опросчика ───────────────────────────────────────────────────────
enum PollState { POLL_IDLE, POLL_WAIT_CREG, POLL_WAIT_CSQ, POLL_WAIT_CBC, POLL_WAIT_COPS };
static PollState _pollState = POLL_IDLE;

static uint32_t _pollTimer    = 0;
static uint32_t _pollCmdTimer = 0;
static ModemStatus _newStatus;

// ── Вотчдог регистрации ───────────────────────────────────────────────────────
//
//  WD_OK   → сеть есть, опрос AT+CREG? раз в 30 сек
//  WD_WARN → сеть пропала, опрос каждые 10 сек
//
//  WD_OK  → WD_WARN: creg=0/3 из опроса или URC из потока
//  WD_WARN → WD_OK:  creg=1/5
//  6 плохих ответов подряд → перезагрузка модема
//
enum WdState { WD_OK, WD_WARN };
static WdState  _wdState      = WD_OK;
static uint32_t _wdTimer      = 0;
static uint8_t  _wdFailCount  = 0;
static bool     _wdPollActive  = false;  // true = текущий опрос инициирован вотчдогом
static bool     _wdStartupGrace = true;   // true = ждём после старта

static bool    _simInitDone  = false;  // команды требующие SIM уже отправлены
static uint8_t _simCmdIdx    = 0;
static uint32_t _simCmdTimer = 0;

static const char* _simCmds[] = {
    "AT+CLIP=1",
    "AT+DDET=1,0,0",
    "AT+CMGF=0",
    "AT+CNMI=2,2,0,0,0",
};
static const uint8_t _simCmdsCount = sizeof(_simCmds) / sizeof(_simCmds[0]);

static uint32_t _wdGraceTimer   = 0;
static bool _timeSync = false;

static bool     _smsSending    = false;
static bool _smsCmgfDone = false;  // AT+CMGF=0 отправлен и получен OK
static uint32_t _smsCmdTimer   = 0;
static String   _smsPduPending = "";

static bool   _smsPendingFlag = false;
static String _smsPendingTo   = "";
static String _smsPendingText = "";

#define WD_INTERVAL_OK    30000u   // мс между опросами в норме
#define WD_INTERVAL_WARN  10000u   // мс между опросами при потере сети
#define WD_MAX_FAILS      12        // сколько плохих ответов до перезагрузки


// ── HTML ─────────────────────────────────────────────────────────────────────

static const char MODEM_TAB_HTML[] PROGMEM = R"html(
<div class="settings-group">
  <h3>📊 Статус модема</h3>
  <div id="modem_status" style="font-size:12px;color:var(--muted)">загрузка...</div>
</div>

<div class="settings-group">
  <h3>✉️ SMS</h3>
  <div style="display:flex;gap:8px;margin-bottom:8px">
    <input type="text" id="sms_to" placeholder="+79139158466" style="margin:0;flex:1">
    <button onclick="sendSmsWeb()" style="width:auto;padding:10px 16px;margin:0">📤 Отправить</button>
  </div>
  <textarea id="sms_text" rows="3" placeholder="Текст сообщения" style="width:100%;
    box-sizing:border-box;padding:8px;border-radius:6px;border:1px solid var(--border);
    background:var(--bg);color:var(--text);resize:vertical;margin-bottom:8px"></textarea>
  <h4 style="margin:8px 0 4px">📥 Входящие</h4>
  <pre id="sms_inbox" style="font-size:11px;color:var(--muted);height:120px;
    overflow-y:auto;white-space:pre-wrap;background:var(--bg);padding:8px;
    border-radius:6px;border:1px solid var(--border)">загрузка...</pre>
</div>

<div class="settings-group">
  <h3>📋 Системный лог</h3>
  <pre id="sys_log" style="font-size:11px;color:var(--muted);height:160px;overflow-y:auto;
    white-space:pre-wrap;background:var(--bg);padding:8px;border-radius:6px;
    border:1px solid var(--border)"></pre>
  <button onclick="clearSysLog()" style="margin-top:6px">🗑 Очистить</button>
</div>

<div class="settings-group">
  <h3>📟 Терминал</h3>
  <div style="display:flex;gap:8px;margin-bottom:8px">
    <select id="at_preset" onchange="atPresetSelect()" style="margin:0;padding:10px 8px;
      border-radius:6px;border:1px solid var(--border);background:var(--bg);color:var(--text)">
      <option value="AT">AT — проверка связи</option>
      <option value="AT+CREG?">AT+CREG? — регистрация</option>
      <option value="AT+CSQ">AT+CSQ — уровень сигнала</option>
      <option value="AT+CBC">AT+CBC — напряжение</option>
      <option value="AT+COPS?">AT+COPS? — оператор</option>
      <option value="AT+CCLK?">AT+CCLK? — время</option>
      <option value="AT+CMGF?">AT+CMGF? — режим SMS</option>
      <option value="AT+CNMI?">AT+CNMI? — уведомления SMS</option>
      <option value="AT+CLTS?">AT+CLTS? — синхронизация времени</option>
      <option value="AT+GMR">AT+GMR — версия прошивки</option>
      <option value="AT+CMGDA=&quot;DEL ALL&quot;">AT+CMGDA — удалить все SMS</option>
    </select>
    <input type="text" id="at_cmd" placeholder="команда" style="margin:0;flex:1">
    <button onclick="sendAT()" style="width:auto;padding:10px 16px;margin:0">▶</button>
  </div>
  <pre id="modem_log" style="font-size:11px;color:var(--muted);height:220px;overflow-y:auto;
    white-space:pre-wrap;background:var(--bg);padding:8px;border-radius:6px;
    border:1px solid var(--border)"></pre>
  <button onclick="clearModemLog()" style="margin-top:6px">🗑 Очистить</button>
</div>

<div class="settings-group">
  <h3>⚙️ Настройки</h3>
  <label class="toggle-row">
    <span>Включить модем</span>
    <input type="checkbox" id="modem_enabled" onchange="modemToggle()">
  </label>
  <div id="modem_fields">
    <label>Таймаут DTMF (сек)</label>
    <input type="number" name="modem_dtmf_timeout" placeholder="10">
    <label>Интервал опроса статуса (сек)</label>
    <input type="number" name="modem_poll_interval" placeholder="60">
    <div style="display:flex;gap:8px;margin-top:12px">
      <button onclick="saveModem()" style="flex:1">💾 Сохранить</button>
      <button onclick="restartModem()" class="btn-danger" style="flex:1">🔄 Перезагрузить модем</button>
    </div>
  </div>
</div>

<script>
function atPresetSelect() {
  const v = document.getElementById('at_preset').value;
  if (v) document.getElementById('at_cmd').value = v;
}
function signalBars(csq) {
  if (csq == 99 || csq < 0) {
    return '<span style="letter-spacing:2px;font-weight:bold;color:var(--muted)">||||||||</span>';
  }
  // CSQ 0-31: 0-4 низкий, 5-14 средний, 15-31 хороший
  // Переводим в 0-8 делений
  const level = Math.round((Math.min(csq, 31) / 31) * 8);
  let bars = '';
  for (let i = 1; i <= 8; i++) {
    let color;
    if (i > level) {
      color = 'var(--muted)';
    } else if (level <= 2) {
      color = '#e74c3c';  // красный
    } else if (level <= 4) {
      color = '#f39c12';  // жёлтый
    } else {
      color = '#2ecc71';  // зелёный
    }
    bars += `<span style="color:${color}">|</span>`;
  }
  return `<span style="letter-spacing:2px;font-weight:bold">${bars}</span>`;
}
async function loadModem() {
  const r = await fetch('/api/modem/config');
  const d = await r.json();
  document.getElementById('modem_enabled').checked = d.enabled;
  ['dtmf_timeout','poll_interval'].forEach(k => {
    const el = document.querySelector('[name="modem_'+k+'"]');
    if (el) el.value = d[k];
  });
  modemToggle();
}
function modemToggle() {
  document.getElementById('modem_fields').style.display =
    document.getElementById('modem_enabled').checked ? 'block' : 'none';
}
async function saveModem() {
  const d = {
    enabled:       document.getElementById('modem_enabled').checked,
    dtmf_timeout:  +document.querySelector('[name="modem_dtmf_timeout"]').value,
    poll_interval: +document.querySelector('[name="modem_poll_interval"]').value,
  };
  const r = await fetch('/api/modem/save',
    {method:'POST', headers:{'Content-Type':'application/json'}, body:JSON.stringify(d)});
  if (r.ok) alert('Сохранено, перезагрузите устройство');
}
async function sendAT() {
  const cmd = document.getElementById('at_cmd').value.trim();
  if (!cmd) return;
  await fetch('/api/modem/at', {method:'POST',
    headers:{'Content-Type':'application/json'}, body:JSON.stringify({cmd})});
  document.getElementById('at_cmd').value = '';
}
document.getElementById('at_cmd').addEventListener('keydown', e => {
  if (e.key === 'Enter') sendAT();
});
async function loadModemLog() {
  const r = await fetch('/api/modem/log');
  const d = await r.json();
  const el = document.getElementById('modem_log');
  const atBottom = el.scrollHeight - el.scrollTop <= el.clientHeight + 5;
  el.textContent = d.log || '';
  if (atBottom) el.scrollTop = el.scrollHeight;
}
async function loadModemStatus() {
  const r = await fetch('/api/modem/status');
  const d = await r.json();
  const regMap = {'-1':'неизвестно','0':'не зарегистрирован','1':'домашняя сеть',
    '2':'поиск...','3':'отказ','5':'роуминг'};
  document.getElementById('modem_status').innerHTML =
    `<b>Сеть:</b> ${regMap[String(d.creg)] || d.creg} &nbsp;
     <b>Сигнал:</b> ${signalBars(d.csq)} &nbsp;
     <b>Питание:</b> ${d.vbat > 0 ? (d.vbat/1000).toFixed(3)+'V' : '—'} &nbsp;
     <b>Оператор:</b> ${d.oper || '—'}`;
}
async function clearModemLog() {
  await fetch('/api/modem/log/clear', {method:'POST'});
  document.getElementById('modem_log').textContent = '';
}
async function loadSysLog() {
  const r = await fetch('/api/syslog');
  const d = await r.json();
  const el = document.getElementById('sys_log');
  const atBottom = el.scrollHeight - el.scrollTop <= el.clientHeight + 5;
  el.textContent = d.log || '';
  if (atBottom) el.scrollTop = el.scrollHeight;
}
async function clearSysLog() {
  await fetch('/api/syslog/clear', {method:'POST'});
  document.getElementById('sys_log').textContent = '';
}
async function restartModem() {
  if (!confirm('Перезагрузить модем?')) return;
  await fetch('/api/modem/restart', {method:'POST'});
}
async function sendSmsWeb() {
  const to   = document.getElementById('sms_to').value.trim();
  const text = document.getElementById('sms_text').value.trim();
  if (!to || !text) return;
  const r = await fetch('/api/modem/sms/send', {
    method: 'POST',
    headers: {'Content-Type': 'application/json'},
    body: JSON.stringify({to, text})
  });
  if (r.ok) { alert('SMS отправлена'); document.getElementById('sms_text').value = ''; }
}
async function loadSmsInbox() {
  const r = await fetch('/api/modem/sms/inbox');
  const d = await r.json();
  document.getElementById('sms_inbox').textContent =
    (d.messages || []).map(m => m).join('\n') || 'нет сообщений';
}
loadModem();
loadModemLog();
loadModemStatus();
loadSmsInbox();
loadSysLog();
setInterval(loadModemLog, 2000);
setInterval(loadModemStatus, 10000);
setInterval(loadSysLog, 5000);
setInterval(loadSmsInbox, 10000);
</script>
)html";

// ── Config ───────────────────────────────────────────────────────────────────
bool ModemHandler::loadConfig() {
    JsonDocument doc;
    if (!ConfigManager::loadJson("/modem.json", doc)) return false;
    modemCfg.enabled       = doc["enabled"]       | true;
    modemCfg.rx_pin        = doc["rx_pin"]         | 2;
    modemCfg.tx_pin        = doc["tx_pin"]         | 4;
    modemCfg.rst_pin       = doc["rst_pin"]        | 33;
    modemCfg.ri_pin        = doc["ri_pin"]         | -1;
    modemCfg.baud          = doc["baud"]           | 9600;
    modemCfg.dtmf_timeout  = doc["dtmf_timeout"]   | 10;
    modemCfg.poll_interval = doc["poll_interval"]  | 60;
    _modemCreg    = modemStatus.creg;
    _modemEnabled = modemCfg.enabled;
    return true;
}

bool ModemHandler::saveConfig() {
    JsonDocument doc;
    doc["enabled"]       = modemCfg.enabled;
    doc["rx_pin"]        = modemCfg.rx_pin;
    doc["tx_pin"]        = modemCfg.tx_pin;
    doc["rst_pin"]       = modemCfg.rst_pin;
    doc["ri_pin"]        = modemCfg.ri_pin;
    doc["baud"]          = modemCfg.baud;
    doc["dtmf_timeout"]  = modemCfg.dtmf_timeout;
    doc["poll_interval"] = modemCfg.poll_interval;
    return ConfigManager::saveJson("/modem.json", doc);
}

// ── Init ─────────────────────────────────────────────────────────────────────
void ModemHandler::init() {
    loadConfig();
    if (!modemCfg.enabled) { Serial.println("[MODEM] Disabled"); return; }

    if (modemCfg.ri_pin >= 0)
        pinMode(modemCfg.ri_pin, INPUT);

    _sim.begin(modemCfg.baud, SERIAL_8N1, modemCfg.rx_pin, modemCfg.tx_pin);

    WebHandler::registerTab({"modem", "Модем 📞", "📞", MODEM_TAB_HTML});

    extern AsyncWebServer server;

    server.on("/api/modem/config", HTTP_GET, [](AsyncWebServerRequest* req) {
        JsonDocument doc;
        doc["enabled"]       = modemCfg.enabled;
        doc["rx_pin"]        = modemCfg.rx_pin;
        doc["tx_pin"]        = modemCfg.tx_pin;
        doc["rst_pin"]       = modemCfg.rst_pin;
        doc["ri_pin"]        = modemCfg.ri_pin;
        doc["baud"]          = modemCfg.baud;
        doc["dtmf_timeout"]  = modemCfg.dtmf_timeout;
        doc["poll_interval"] = modemCfg.poll_interval;
        String out; serializeJson(doc, out);
        req->send(200, "application/json", out);
    });

    auto* hSave = new AsyncCallbackJsonWebHandler("/api/modem/save",
        [](AsyncWebServerRequest* req, JsonVariant& json) {
            JsonObject o = json.as<JsonObject>();
            modemCfg.enabled       = o["enabled"]       | true;
            modemCfg.rx_pin        = o["rx_pin"]         | 4;
            modemCfg.tx_pin        = o["tx_pin"]         | 2;
            modemCfg.rst_pin       = o["rst_pin"]        | 33;
            modemCfg.ri_pin        = o["ri_pin"]         | -1;
            modemCfg.baud          = o["baud"]           | 9600;
            modemCfg.dtmf_timeout  = o["dtmf_timeout"]   | 10;
            modemCfg.poll_interval = o["poll_interval"]  | 60;
            ModemHandler::saveConfig();
            req->send(200, "application/json", "{\"status\":\"ok\"}");
        }
    );
    server.addHandler(hSave);

    auto* hAT = new AsyncCallbackJsonWebHandler("/api/modem/at",
        [](AsyncWebServerRequest* req, JsonVariant& json) {
            String cmd = json["cmd"] | "";
            if (cmd.length() > 0) ModemHandler::sendAT(cmd.c_str());
            req->send(200, "application/json", "{\"status\":\"ok\"}");
        }
    );
    server.addHandler(hAT);

    server.on("/api/modem/log", HTTP_GET, [](AsyncWebServerRequest* req) {
        JsonDocument doc;
        doc["log"] = _modemLog;
        String out; serializeJson(doc, out);
        req->send(200, "application/json", out);
    });

    server.on("/api/modem/log/clear", HTTP_POST, [](AsyncWebServerRequest* req) {
        _modemLog = "";
        req->send(200, "application/json", "{\"status\":\"ok\"}");
    });

    server.on("/api/modem/status", HTTP_GET, [](AsyncWebServerRequest* req) {
        JsonDocument doc;
        doc["creg"] = modemStatus.creg;
        doc["csq"]  = modemStatus.csq;
        doc["vbat"] = modemStatus.vbat;
        doc["oper"] = modemStatus.oper;
        String out; serializeJson(doc, out);
        req->send(200, "application/json", out);
    });

    server.on("/api/modem/restart", HTTP_POST, [](AsyncWebServerRequest* req) {
        _manualRestart = true;
        req->send(200, "application/json", "{\"status\":\"ok\"}");
    });

    auto* hSms = new AsyncCallbackJsonWebHandler("/api/modem/sms/send",
        [](AsyncWebServerRequest* req, JsonVariant& json) {
            String to   = json["to"]   | "";
            String text = json["text"] | "";
            if (to.length() > 0 && text.length() > 0)
                _smsPendingTo   = json["to"]   | "";
                _smsPendingText = json["text"] | "";
                _smsPendingFlag = true;
            req->send(200, "application/json", "{\"status\":\"ok\"}");
        }
    );
    server.addHandler(hSms);

    server.on("/api/modem/sms/inbox", HTTP_GET, [](AsyncWebServerRequest* req) {
        JsonDocument doc;
        JsonArray arr = doc["messages"].to<JsonArray>();
        for (int i = 0; i < 10; i++) {
            if (_smsInbox[i].length() > 0)
                arr.add(_smsInbox[i]);
        }
        String out; serializeJson(doc, out);
        req->send(200, "application/json", out);
    });

    // Запускаем стейт-машину инициализации с RST_LOW (или сразу RST_HIGH если
    // пин не задан)
    if (modemCfg.rst_pin >= 0) {
        pinMode(modemCfg.rst_pin, OUTPUT);
        digitalWrite(modemCfg.rst_pin, LOW);
        _initState = INIT_RST_LOW;
        _addLog("Resetting modem...");
    } else {
        // RST не используется — сразу ждём Call Ready
        _initState = INIT_RST_HIGH;
        _addLog("Waiting for Call Ready...");
    }
    _initTimer = millis();

    // Первый опрос запустим после завершения инициализации (_initTick сам
    // выставит _pollTimer в момент перехода в INIT_DONE)
    Serial.println("[MODEM] Init started (non-blocking)");
}

// ── Стейт-машина инициализации ────────────────────────────────────────────────
void ModemHandler::_initTick() {
    switch (_initState) {

    case INIT_RST_LOW:
        // Держим RST=LOW 200 мс, затем поднимаем
        if (millis() - _initTimer >= 200) {
            digitalWrite(modemCfg.rst_pin, HIGH);
            _initState = INIT_RST_HIGH;
            _initTimer = millis();
            _addLog("Waiting for Call Ready...");
        }
        break;

    case INIT_RST_HIGH:
        // Ждём "Call Ready" — детект идёт в loop() через _callReadyReceived.
        // Таймаут 30 сек — SIM800L может стартовать долго.
        if (_callReadyReceived || millis() - _initTimer >= 30000) {
            if (_callReadyReceived)
                _addLog("Call Ready received");
            else
                _addLog("Call Ready timeout, proceeding anyway");
            _callReadyReceived = false;
            _initCmdIdx = 0;
            _initState  = INIT_CMD;
            _initTimer  = millis();
            sendAT(_initCmds[_initCmdIdx]);
        }
        break;

    case INIT_CMD:
        // Пауза 500 мс после каждой команды, затем следующая
        if (millis() - _initTimer >= 500) {
            _initCmdIdx++;
            if (_initCmdIdx >= _initCmdsCount) {
                _addLog("Init done");
                _initState  = INIT_DONE;
                _pollTimer  = millis();   // первый опрос через poll_interval
                _wdStartupGrace = true;
                _wdGraceTimer   = millis();
                Serial.println("[MODEM] Ready");
            } else {
                sendAT(_initCmds[_initCmdIdx]);
                _initTimer = millis();
            }
        }
        break;

    case INIT_DONE:
        // Ничего — обычный loop() работает
        break;
    }
}

// ── Loop ─────────────────────────────────────────────────────────────────────
void ModemHandler::loop() {
    if (!modemCfg.enabled) return;

    // Читаем все доступные байты из UART
    while (_sim.available()) {
        char c = (char)_sim.read();
        _modemLog += c;
        if (c == '>' && _smsSending && _smsCmgfDone) {
            _sim.read(); // съедаем пробел после '>'
            _sim.print(_smsPduPending);
            _sim.write(0x1A);
            _smsSending    = false;
            _smsPduPending = "";
            _addLog("SMS PDU sent");
            continue;
        }
        if (_modemLog.length() > 4096)
            _modemLog = _modemLog.substring(_modemLog.length() - 4096);

        // SIM800L завершает строки через \r\n или только \r
        // Обрабатываем буфер при любом из них
        if (c == '\n' || c == '\r') {
            _lineBuffer.trim();
            if (_lineBuffer.length() > 0) {
                // "Call Ready" детектируем всегда — нужен и во время INIT_RST_HIGH
                if (_lineBuffer == "Call Ready")
                    _callReadyReceived = true;

                if (_initState == INIT_DONE) {
                    _processLine(_lineBuffer);
                    _parsePollResponse(_lineBuffer);
                }
            }
            _lineBuffer = "";
        } else {
            _lineBuffer += c;
        }
    }

    // Тик инициализации (завершается переходом в INIT_DONE)
    if (_initState != INIT_DONE) {
        _initTick();
        return;   // пока не инициализировались — больше ничего не делаем
    }

    // Ручной ресет из веб-интерфейса
    if (_manualRestart) {
        _manualRestart = false;
        _sysLog("Manual modem restart requested");
        _wdRestart();
        return;
    }

    // Таймаут poll — проверяем здесь, а не внутри _parsePollResponse,
    // чтобы он срабатывал даже если модем полностью молчит
    if (_pollState != POLL_IDLE && millis() - _pollCmdTimer > 15000) {
        _addLog("Poll timeout");
        _pollState = POLL_IDLE;
        _pollTimer = millis();
    }

    if (_smsSending && millis() - _smsCmdTimer > 30000) {
        _addLog("SMS send timeout");
        _sim.write(0x1B); // ESC — отменяем отправку
        _smsSending    = false;
        _smsPduPending = "";
    }
        
    // Отложенная отправка SMS — ждём POLL_IDLE
    if (_smsPendingFlag && _pollState == POLL_IDLE && !_smsSending) {
        _smsPendingFlag = false;
        ModemHandler::sendSms(_smsPendingTo, _smsPendingText);
    }

    _checkAnswerDelay();
    _checkTimeout();
    _wdTick();
    _simInitTick();
    _pollTick();
}

// ── AT helper ────────────────────────────────────────────────────────────────
void ModemHandler::sendAT(const char* cmd) {
    _sim.println(cmd);
    _addLog(String(">> ") + cmd);
}

// ── Парсер входящих строк (звонок) ───────────────────────────────────────────
void ModemHandler::_processLine(const String& line) {
    _addLog("LINE:[" + line + "]"); 
    Serial.println("[MODEM] << " + line);

    if (line == "RING") {
        if (_callState == CALL_IDLE) {
            _callState = CALL_RINGING;
            _callerNum = "";
            _dtmfSeq   = "";
        }
        return;
    }
    if (line.startsWith("+CLIP:") && _callState == CALL_RINGING) {
        _onClip(line); return;
    }
    if (line.startsWith("+DTMF:") && _callState == CALL_IN_CALL) {
        String rest = line.substring(6); rest.trim();
        if (rest.length() > 0) _onDTMF(rest.charAt(0));
        return;
    }
    if (line == "NO CARRIER" || line == "BUSY" || line == "NO ANSWER") {
        _onNoCarrier(); return;
    }
    // URC: модем сам сообщает об изменении регистрации (AT+CREG=2 включён при init)
    // URC формат: +CREG: <stat>,"<lac>","<ci>"  — stat сразу после пробела, без <n>
    // Отличие от ответа на запрос: в URC нет числа перед первой запятой (там буква или кавычка)
    if (line.startsWith("+CREG:") && _pollState == POLL_IDLE) {
        int sp = line.indexOf(' ');
        if (sp < 0) return;
        // stat — первый символ после пробела
        int stat = line.substring(sp + 1, sp + 2).toInt();
        _addLog("URC CREG: " + String(stat));
        _wdHandleCreg(stat);
        modemStatus.creg = stat;
        return;
    }

    if (line.startsWith("+CCLK:")) {
        _parseTime(line);
        return;
    }

    // Входящее SMS — две строки: заголовок и PDU
    _addLog("GOT > at " + String(millis()) + " sending=" + String(_smsSending));

    if (line == ">" && _smsSending && _smsCmgfDone) {
        _sim.print(_smsPduPending);
        _sim.write(0x1A);
        _smsSending    = false;
        _smsPduPending = "";
        _addLog("SMS PDU sent");
        return;
    }
    // OK на AT+CMGF=0 — теперь отправляем AT+CMGS
    if (line == "OK" && _smsSending && !_smsCmgfDone) {
        _smsCmgfDone  = true;
        _smsCmdTimer  = millis();
        // Длина PDU = (длина hex строки / 2) - 1 (без SMSC байта)
        int pduLen = (_smsPduPending.length() / 2) - 1;
        sendAT(("AT+CMGS=" + String(pduLen)).c_str());
        return;
    }

    if (_smsNextLine) {
        _smsNextLine = false;
        _onSmsReceived(_smsSender, line);
        return;
    }
    if (line.startsWith("+CMT:")) {
        // +CMT: "",<len>  — в PDU режиме номер в самом PDU
        _smsSender   = "";
        _smsNextLine = true;
        return;
    }
}

void ModemHandler::_parseTime(const String& line) {
    // Формат: +CCLK: "26/04/21,14:30:00+12"
    int q1 = line.indexOf('"');
    int q2 = line.indexOf('"', q1 + 1);
    if (q1 < 0 || q2 < 0) return;
    String t = line.substring(q1 + 1, q2);
    // yy/MM/dd,hh:mm:ss±zz
    int year  = t.substring(0, 2).toInt() + 2000;
    int month = t.substring(3, 5).toInt();
    int day   = t.substring(6, 8).toInt();
    int hour  = t.substring(9, 11).toInt();
    int min   = t.substring(12, 14).toInt();
    int sec   = t.substring(15, 17).toInt();
    if (year < 2024) { _addLog("CCLK: invalid time"); return; }
    struct tm tm = {};
    tm.tm_year = year - 1900;
    tm.tm_mon  = month - 1;
    tm.tm_mday = day;
    tm.tm_hour = hour;
    tm.tm_min  = min;
    tm.tm_sec  = sec;
    time_t t_unix = mktime(&tm);
    struct timeval tv = { t_unix, 0 };
    settimeofday(&tv, nullptr);
    #ifdef MODULE_TIME
        TimeManager::notifySynced(TimeSource::MODEM);
    #endif
    _timeSync = true;
    _addLog("Time synced: " + t);
}

// ── Парсер ответов опроса ─────────────────────────────────────────────────────
void ModemHandler::_parsePollResponse(const String& line) {
    if (line.startsWith("+CREG:") && _pollState == POLL_WAIT_CREG) {
        // AT+CREG=2 возвращает: +CREG: <n>,<stat>[,<lac>,<ci>[,<AcT>]]
        // AT+CREG=0 возвращает: +CREG: <stat>
        // Нам нужен <stat> — второй элемент при наличии запятой, иначе первый
        int comma = line.indexOf(',');
        if (comma > 0) {
            // Берём следующий символ после запятой (stat — одна цифра)
            _newStatus.creg = line.substring(comma + 1, comma + 2).toInt();
        } else {
            // Нет запятой: +CREG: <stat>
            int space = line.indexOf(' ');
            _newStatus.creg = (space > 0)
                ? line.substring(space + 1, space + 2).toInt()
                : line.substring(7, 8).toInt();
        }
        // Сообщаем вотчдогу результат
        _wdHandleCreg(_newStatus.creg);

        // Если опрос инициирован вотчдогом — только CREG, без CSQ/CBC/COPS
        if (_wdPollActive) {
            _wdPollActive = false;
            modemStatus.creg = _newStatus.creg;
            _pollState = POLL_IDLE;
            // При WD_WARN считаем failures
            if (_wdState == WD_WARN && !_wdStartupGrace) {
                if (_newStatus.creg != 1 && _newStatus.creg != 5) {
                    _wdFailCount++;
                    _addLog("Watchdog: fail " + String(_wdFailCount) + "/" + String(WD_MAX_FAILS));
                    if (_wdFailCount >= WD_MAX_FAILS)
                        _wdRestart();
                }
            }
            return;
        }

        // Опрос инициирован pollTick — тоже считаем failures при WD_WARN
        if (_wdState == WD_WARN) {
            if (_newStatus.creg != 1 && _newStatus.creg != 5) {
                _wdFailCount++;
                _addLog("Watchdog: fail " + String(_wdFailCount) + "/" + String(WD_MAX_FAILS));
                if (_wdFailCount >= WD_MAX_FAILS)
                    _wdRestart();
            }
        }

        _pollState    = POLL_WAIT_CSQ;
        _pollCmdTimer = millis();
        sendAT("AT+CSQ");
        return;
    }
    if (line.startsWith("+CSQ:") && _pollState == POLL_WAIT_CSQ) {
        int s = line.indexOf(' '), c = line.indexOf(',');
        if (s > 0 && c > s) _newStatus.csq = line.substring(s + 1, c).toInt();
        _pollState    = POLL_WAIT_CBC;
        _pollCmdTimer = millis();
        sendAT("AT+CBC");
        return;
    }
    if (line.startsWith("+CBC:") && _pollState == POLL_WAIT_CBC) {
        int c1 = line.indexOf(',');
        int c2 = (c1 > 0) ? line.indexOf(',', c1 + 1) : -1;
        if (c2 > 0) _newStatus.vbat = line.substring(c2 + 1).toInt();
        _pollState    = POLL_WAIT_COPS;
        _pollCmdTimer = millis();
        sendAT("AT+COPS?");
        return;
    }
    if (line.startsWith("+COPS:") && _pollState == POLL_WAIT_COPS) {
        int q1 = line.indexOf('"');
        int q2 = (q1 >= 0) ? line.indexOf('"', q1 + 1) : -1;
        if (q2 > q1)
            strlcpy(_newStatus.oper, line.substring(q1 + 1, q2).c_str(), sizeof(_newStatus.oper));
        else
            strlcpy(_newStatus.oper, "", sizeof(_newStatus.oper));
        _pollState = POLL_IDLE;
        _pollTimer = millis();
        _publishStatusIfChanged();
        return;
    }
}

// ── Запуск цикла опроса ───────────────────────────────────────────────────────
void ModemHandler::_pollTick() {
    if (_smsSending) return;
    if (_callState != CALL_IDLE)   return;
    if (_pollState != POLL_IDLE) return;
    if (millis() - _pollTimer < (uint32_t)modemCfg.poll_interval * 1000) return;
    _newStatus    = ModemStatus();
    _pollState    = POLL_WAIT_CREG;
    _pollCmdTimer = millis();
    sendAT("AT+CREG?");
}

// ── Публикация если изменилось ────────────────────────────────────────────────
void ModemHandler::_publishStatusIfChanged() {
    bool changed =
        _newStatus.creg != modemStatus.creg ||
        _newStatus.csq  != modemStatus.csq  ||
        abs(_newStatus.vbat - modemStatus.vbat) > 50 ||
        strcmp(_newStatus.oper, modemStatus.oper) != 0;

    if (!changed) { _addLog("Status poll: no changes"); return; }

    modemStatus = _newStatus;

    JsonDocument doc;
    doc["creg"] = modemStatus.creg;
    doc["csq"]  = modemStatus.csq;
    doc["vbat"] = modemStatus.vbat;
    doc["oper"] = modemStatus.oper;
    String payload;
    serializeJson(doc, payload);
    _addLog("Status changed: " + payload);

#ifdef MODULE_MQTT
    String topic = String(baseCfg.device_name) + "/modem/status";
    MqttHandler::publish(topic.c_str(), payload.c_str(), true);
#endif
}

// ── Обработка звонка ─────────────────────────────────────────────────────────
void ModemHandler::_onClip(const String& line) {
    int s = line.indexOf('"');
    int e = (s >= 0) ? line.indexOf('"', s + 1) : -1;
    _callerNum = (e > s) ? _normalizePhone(line.substring(s + 1, e)) : "unknown";
    _addLog("Incoming: " + _callerNum);

    // Снимаем трубку, затем AT+CMUT=1 через 500 мс — неблокирующий таймер
    sendAT("ATA");
    _answerPending = true;
    _answerAt      = millis();

    _callStart = millis();
    _dtmfSeq   = "";
}

// Вызывается каждый loop() — отправляет AT+CMUT=1 через 500 мс после ATA
void ModemHandler::_checkAnswerDelay() {
    if (!_answerPending) return;
    if (millis() - _answerAt < 400) return;

    sendAT("AT+CMUT=1");
    _callState     = CALL_IN_CALL;
    _answerPending = false;
    _addLog("Answered, waiting DTMF...");
}

void ModemHandler::_onDTMF(char digit) {
    _dtmfSeq += digit;
    _addLog(String("DTMF: ") + digit + " (seq: " + _dtmfSeq + ")");
}

void ModemHandler::_onNoCarrier() {
    if (_callLogBuffer.length() > 0) {
        _sysLog("=== Call " + _callerNum + " (no DTMF) ===\n" + _callLogBuffer);
        _callLogBuffer = "";
    }
    if (_callState == CALL_IN_CALL && _dtmfSeq.length() > 0) {
        _addLog("Call ended, result: " + _callerNum + " - " + _dtmfSeq);
        _notifyResult();
    } else if (_callState == CALL_IN_CALL) {
        _addLog("Call ended, no DTMF");
    }
    _answerPending = false;
    _callState     = CALL_IDLE;
    _callerNum     = "";
    _dtmfSeq       = "";
}

void ModemHandler::_checkTimeout() {
    if (_callState != CALL_IN_CALL) return;
    if (millis() - _callStart < (uint32_t)modemCfg.dtmf_timeout * 1000) return;
    _addLog("DTMF timeout");
    sendAT("ATH");
    if (_callLogBuffer.length() > 0) {
        _sysLog("=== Call " + _callerNum + " (timeout) ===\n" + _callLogBuffer);
        _callLogBuffer = "";
    }
    // Уведомляем до сброса состояния, чтобы колбэк видел актуальные данные
    if (_dtmfSeq.length() > 0) _notifyResult();
    else _addLog("No DTMF received");
    _callState = CALL_IDLE;
    _callerNum = "";
    _dtmfSeq   = "";
}

void ModemHandler::_notifyResult() {
    if (_callLogBuffer.length() > 0) {
        _sysLog("=== Call " + _callerNum + " ===\n" + _callLogBuffer);
        _callLogBuffer = "";
    }
    String msg = _callerNum + " - " + _dtmfSeq;
    _addLog("Publishing DTMF: " + msg);
#ifdef MODULE_MQTT
    String topic = String(baseCfg.device_name) + "/modem/dtmf";
    MqttHandler::publish(topic.c_str(), msg.c_str());
#endif
#ifdef MODULE_TELEGRAM
    TgHandler::sendMessage("📞 " + msg);
#endif
    if (onDTMFResult) onDTMFResult(_callerNum.c_str(), _dtmfSeq.c_str());
}

// ── Helpers ──────────────────────────────────────────────────────────────────
String ModemHandler::_normalizePhone(const String& phone) {
    String result;
    bool leadingPlusDone = false;
    for (unsigned int i = 0; i < phone.length(); i++) {
        char c = phone.charAt(i);
        if (!leadingPlusDone && c == '+') {
            result += '+';
            leadingPlusDone = true;
            continue;
        }
        if (isdigit(c)) {
            result += c;
            leadingPlusDone = true;   // после первой цифры '+' уже недопустим
        }
    }
    if (result.startsWith("8") && result.length() == 11)
        result = "+7" + result.substring(1);
    return result;
}

// ── Вотчдог: обработка результата creg ───────────────────────────────────────
// Вызывается из _parsePollResponse и при URC +CREG: в _processLine
void ModemHandler::_wdHandleCreg(int stat) {
    bool ok = (stat == 1 || stat == 5);

    if (ok) {
        _modemCreg    = modemStatus.creg;
        _modemEnabled = modemCfg.enabled;
        if (!_simInitDone) {
            _simCmdsAllowed = true;  // ← вместо _simInitDone = false
            _simCmdIdx      = 0;
            _simCmdTimer    = millis() - 500;
        }
        if (_wdState == WD_WARN)
            _addLog("Watchdog: network restored");
        _wdState      = WD_OK;
        _wdFailCount  = 0;
        _wdTimer      = millis();
        // Запускаем команды требующие SIM если ещё не отправляли
        if (!_simInitDone) {
            _simInitDone = false;  // будет выставлен после последней команды
            _simCmdIdx   = 0;
            _simCmdTimer = millis() - 500;  // чтобы первая команда ушла сразу
        }
        // Синхронизируем время при восстановлении сети
        sendAT("AT+CCLK?");
        return;
    }

    // stat == 0 — явная потеря сети → входим в режим восстановления
    // stat == 2 — поиск (временно, ждём) → не трогаем
    // stat == 3 — отказ регистрации → ждём, модем сам повторит попытку
    if (_wdState == WD_OK && stat == 0 && !_wdStartupGrace) {
        _addLog("Watchdog: network lost, entering recovery mode");
        _wdState     = WD_WARN;
        _wdFailCount = 0;
        _wdTimer     = millis();
    }
}

// ── Вотчдог: тик (вызывается из loop каждый цикл) ────────────────────────────
void ModemHandler::_wdTick() {
    if (_smsSending) return;
    if (_callState != CALL_IDLE) return;    // не трогаем во время звонка
    if (_pollState != POLL_IDLE) return;    // poll уже идёт — подождём

    if (_wdStartupGrace) {
        if (millis() - _wdGraceTimer < 120000) return;
        _wdStartupGrace = false;
        _wdTimer = millis();
        _addLog("Watchdog: active");
    }

    uint32_t interval = (_wdState == WD_WARN) ? WD_INTERVAL_WARN : WD_INTERVAL_OK;
    if (millis() - _wdTimer < interval) return;

    _wdTimer = millis();

    // Отправляем AT+CREG? — ответ придёт через _parsePollResponse
    // Используем отдельный мини-опрос: только CREG, без CSQ/CBC/COPS
    _pollState    = POLL_WAIT_CREG;
    _pollCmdTimer = millis();
    _newStatus    = modemStatus;   // сохраняем остальные поля как есть
    _wdPollActive = true;          // этот опрос — только CREG, без CSQ/CBC/COPS
    sendAT("AT+CREG?");
}

// ── Перезагрузка модема вотчдогом ────────────────────────────────────────────
void ModemHandler::_wdRestart() {
    _addLog("Watchdog: restarting modem after " + String(WD_MAX_FAILS) + " failures");
    _smsCmgfDone = false;
    _simInitDone = false;
    _simCmdsAllowed = false;
    _simCmdIdx   = 0;

#ifdef MODULE_MQTT
    JsonDocument doc;
    doc["creg"]         = modemStatus.creg;
    doc["csq"]          = modemStatus.csq;
    doc["vbat"]         = modemStatus.vbat;
    doc["oper"]         = modemStatus.oper;
    doc["reboot_reason"] = "watchdog";
    String payload;
    serializeJson(doc, payload);
    String topic = String(baseCfg.device_name) + "/modem/status";
    MqttHandler::publish(topic.c_str(), payload.c_str(), true);
#endif

    // Сбрасываем состояние вотчдога
    _wdState      = WD_OK;
    _wdFailCount  = 0;
    _wdPollActive = false;
    _wdTimer      = millis();
    _wdStartupGrace = true;
    _wdGraceTimer   = millis();

    // Сбрасываем состояние звонка на случай зависшего IN_CALL
    _callState     = CALL_IDLE;
    _answerPending = false;
    _callerNum     = "";
    _dtmfSeq       = "";

    // Запускаем полный рестарт модема через statemachine
    if (modemCfg.rst_pin >= 0) {
        digitalWrite(modemCfg.rst_pin, LOW);
        _initState = INIT_RST_LOW;
    } else {
        _initState = INIT_RST_HIGH;
    }
    _initTimer  = millis();
    _initCmdIdx = 0;
    _pollState  = POLL_IDLE;
}


// ── Системный лог (важные события → LittleFS /syslog.txt) ──────────────────
void ModemHandler::_sysLog(const String& msg) {
    // Формируем строку: uptime + сообщение
    uint32_t s = millis() / 1000;
    String ts;
    if (_timeSync) {
        time_t now = time(nullptr);
        struct tm* tm = localtime(&now);
        char buf[32];
        strftime(buf, sizeof(buf), "[%d.%m.%Y %H:%M:%S] ", tm);
        ts = String(buf);
    } else {
        uint32_t s = millis() / 1000;
        char buf[16];
        snprintf(buf, sizeof(buf), "[%02lu:%02lu:%02lu] ", s/3600, (s%3600)/60, s%60);
        ts = String(buf);
    }
    String line = ts + msg + "\n";
    Serial.println("[SYSLOG] " + msg);

    // Дописываем в файл, ротация: если > 8 КБ — обрезаем старое
    File f = LittleFS.open("/syslog.txt", "a");
    if (f) {
        f.print(line);
        f.close();
    }

    // Ротация
    File fr = LittleFS.open("/syslog.txt", "r");
    if (fr && fr.size() > 8192) {
        fr.close();
        // Читаем, отрезаем первую половину
        File fr2 = LittleFS.open("/syslog.txt", "r");
        String content = "";
        if (fr2) {
            fr2.seek(fr2.size() / 2);
            // Пропускаем до первого \n чтобы не начинать с середины строки
            while (fr2.available() && fr2.read() != '\n') {}
            while (fr2.available()) content += (char)fr2.read();
            fr2.close();
        }
        File fw = LittleFS.open("/syslog.txt", "w");
        if (fw) { fw.print(content); fw.close(); }
    } else if (fr) {
        fr.close();
    }
}

// ── SMS ──────────────────────────────────────────────────────────────────────
void ModemHandler::_onSmsReceived(const String& sender, const String& pdu) {
    _addLog("SMS received, PDU len: " + String(pdu.length()));
    // Сохраняем в кольцевой буфер для веб-интерфейса
    _smsInbox[_smsInboxIdx] = pdu;
    _smsInboxIdx = (_smsInboxIdx + 1) % 10;
    _publishSms(sender, pdu);
    _sysLog("SMS received: " + pdu.substring(0, 40));
}

void ModemHandler::_publishSms(const String& sender, const String& pdu) {
#ifdef MODULE_MQTT
    JsonDocument doc;
    doc["pdu"] = pdu;
    String payload;
    serializeJson(doc, payload);
    String topic = String(baseCfg.device_name) + "/modem/sms/inbox";
    MqttHandler::publish(topic.c_str(), payload.c_str());
#endif
}

// UTF-8 → UCS2 hex для отправки SMS с кириллицей
String ModemHandler::_utf8ToUcs2Hex(const String& text) {
    String result = "";
    char buf[5];
    int i = 0;
    while (i < (int)text.length()) {
        uint8_t c = (uint8_t)text[i];
        uint32_t cp;
        if (c < 0x80) {
            cp = c; i++;
        } else if ((c & 0xE0) == 0xC0) {
            cp = (c & 0x1F) << 6 | ((uint8_t)text[i+1] & 0x3F);
            i += 2;
        } else if ((c & 0xF0) == 0xE0) {
            cp = (c & 0x0F) << 12 | ((uint8_t)text[i+1] & 0x3F) << 6
                                   | ((uint8_t)text[i+2] & 0x3F);
            i += 3;
        } else { i++; continue; }
        snprintf(buf, sizeof(buf), "%04X", (unsigned)cp);
        result += buf;
    }
    return result;
}

void ModemHandler::sendSms(const String& to, const String& text) {
    String ucs2 = _utf8ToUcs2Hex(text);
    // Формируем минимальный PDU: нет SMSC + TP-MTI/VPF + номер + UCS2 + текст
    // Длина текста в символах UCS2 (каждый символ = 2 байта = 4 hex символа)
    int charCount = ucs2.length() / 4;

    // Нормализуем номер
    String phone = _normalizePhone(to);
    if (phone.startsWith("+")) phone = phone.substring(1);

    // Тип номера: 91 = international, 81 = unknown
    String toa = phone.startsWith("7") ? "91" : "81";

    // Padding номера до чётной длины
    String phoneHex = "";
    String p = phone;
    if (p.length() % 2) p += "F";
    for (int i = 0; i < (int)p.length(); i += 2) {
        phoneHex += p[i+1];
        phoneHex += p[i];
    }
    // Собираем PDU
    // 00 - нет SMSC
    // 11 - TP-MTI=SMS-SUBMIT, TP-VPF=relative
    // 00 - TP-MR
    // <len> - длина номера в цифрах
    // <toa> - тип номера
    // <phone> - номер
    // 00 - TP-PID
    // 08 - TP-DCS = UCS2
    // AA - TP-VP = 4 дня
    // <len> - длина текста в октетах (charCount * 2)
    // <ucs2> - текст
    String lenPhone = String(phone.length(), HEX);
    if (lenPhone.length() < 2) lenPhone = "0" + lenPhone;
    lenPhone.toUpperCase();

    String lenText = String(charCount * 2, HEX);
    if (lenText.length() < 2) lenText = "0" + lenText;
    lenText.toUpperCase();

    String pdu = "001100" + lenPhone + toa + phoneHex + "0008AA" + lenText + ucs2;    pdu.toUpperCase();

    // PDU длина без SMSC октета (всё кроме первых двух символов "00")
    int pduLen = (pdu.length() / 2) - 1;

    _addLog("Sending SMS to " + to + ", PDU len: " + String(pduLen));
    _smsPduPending = pdu;
    _smsSending    = true;
    _smsCmgfDone   = false;
    _smsCmdTimer   = millis();
    _pollState     = POLL_IDLE;
    _pollTimer     = millis();
    sendAT("AT+CMGF=0");
}

void ModemHandler::_addLog(const String& msg) {
    Serial.println("[MODEM] " + msg);
    _modemLog += msg + "\n";
    if (_modemLog.length() > 4096)
        _modemLog = _modemLog.substring(_modemLog.length() - 4096);

    // Во время звонка буферизуем всё
    if (_callState != CALL_IDLE || _answerPending) {
        _callLogBuffer += msg + "\n";
        return;
    }

    // Вне звонка — только важные события в syslog
    if (msg.startsWith("Watchdog") ||
        msg.startsWith("Init done") ||
        msg.startsWith("Call Ready") ||
        msg.startsWith("Manual") ||
        msg.startsWith("Incoming"))
        _sysLog(msg);
}

void ModemHandler::_simInitTick() {
    if (_simInitDone) return;
    if (!_simCmdsAllowed) return;
    if (_pollState != POLL_IDLE) return;
    if (_smsSending) return;
    if (millis() - _simCmdTimer < 500) return;

    sendAT(_simCmds[_simCmdIdx]);
    _simCmdTimer = millis();
    _simCmdIdx++;
    if (_simCmdIdx >= _simCmdsCount) {
        _simInitDone = true;
        _addLog("SIM init done");
    }
}

#endif // MODULE_MODEM