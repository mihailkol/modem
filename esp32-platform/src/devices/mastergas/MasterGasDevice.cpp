#ifdef DEVICE_MASTERGAS
#include "MasterGasDevice.h"
#include "../../core/ConfigManager.h"
#include "../../core/WebHandler.h"
#include <AsyncJson.h>
#include <ArduinoJson.h>

#ifdef MODULE_MQTT
#include "../../modules/mqtt/MqttHandler.h"
#endif

MasterGasConfig masterGasCfg;
MasterGasState  masterGasState;

uint32_t MasterGasDevice::_lastPub = 0;

// UART1 — только приём, общая шина котёл<->пульт (boilerRead)
static HardwareSerial _boilerSerial(1);
// UART2 — приём линии пульта (remoteRead) + передача ответа на шину (boilerAnswer)
static HardwareSerial _remoteSerial(2);

// ── HTML вкладки ─────────────────────────────────────────────────────────────
static const char MASTERGAS_TAB_HTML[] PROGMEM = R"html(
<div class="settings-group">
  <h3>🔥 Котёл</h3>
  <div id="mg_boiler" style="font-size:13px;line-height:1.8">загрузка...</div>
</div>

<div class="settings-group">
  <h3>🎛 Пульт</h3>
  <div id="mg_remote" style="font-size:13px;line-height:1.8">загрузка...</div>
</div>

<div class="settings-group">
  <h3>⚙️ Управление</h3>
  <label class="toggle-row">
    <span>Passthrough (пульт отвечает сам)</span>
    <input type="checkbox" id="mg_passthrough" onchange="mgToggle()">
  </label>
  <div id="mg_override_fields" style="display:none">
    <label class="toggle-row">
      <span>Питание (override)</span>
      <input type="checkbox" id="mg_o_power">
    </label>
    <label>Уставка температуры, °C</label>
    <input type="number" id="mg_o_temp" min="30" max="85">
  </div>
  <button onclick="mgSave()" style="margin-top:8px">💾 Сохранить</button>
  <div class="info-row">
    В override штатный пульт отключается от шины (remoteEn), и ESP отвечает
    котлу сам заданными power/temp. 0xFA (расширенные данные) в этом режиме
    остаются неотвеченными — see README OpenMasterGas.
  </div>
</div>

<div class="settings-group">
  <h3>📋 Диагностика</h3>
  <div id="mg_diag" style="font-size:12px;color:var(--muted);line-height:1.7">...</div>
  <div style="margin-top:8px">
    <div class="info-row">raw status (0xF9/0x00):</div>
    <pre id="mg_raw_status" style="font-size:11px;color:var(--muted);background:var(--bg);
      padding:6px;border-radius:6px;border:1px solid var(--border)">—</pre>
    <div class="info-row">raw extended (0xFA):</div>
    <pre id="mg_raw_ext" style="font-size:11px;color:var(--muted);background:var(--bg);
      padding:6px;border-radius:6px;border:1px solid var(--border);word-break:break-all">—</pre>
  </div>
</div>

<script>
let _mgCfg = {};

function mgToggle() {
  document.getElementById('mg_override_fields').style.display =
    document.getElementById('mg_passthrough').checked ? 'none' : 'block';
}

function mgAge(ms) {
  if (ms < 0) return 'нет данных';
  return (ms / 1000).toFixed(0) + ' с назад';
}

async function loadMgStatus() {
  try {
    const r = await fetch('/api/mastergas/status');
    const d = await r.json();
    _mgCfg = d;

    document.getElementById('mg_boiler').innerHTML =
      `<b>Горелка:</b> ${d.burner ? '🔥 работает' : '⚪ выкл'}<br>
       <b>T теплоносителя:</b> ${d.coolant_t}°C<br>
       <b>T ГВС:</b> ${d.water_t}°C`;

    document.getElementById('mg_remote').innerHTML =
      `<b>Питание:</b> ${d.r_power ? '✅ вкл' : '⛔ выкл'}<br>
       <b>Уставка:</b> ${d.r_target_t}°C<br>
       <b>T воздуха (датчик пульта):</b> ${d.r_air_t}°C`;

    document.getElementById('mg_passthrough').checked = d.passthrough;
    document.getElementById('mg_o_power').checked = !!d.o_power;
    document.getElementById('mg_o_temp').value = d.o_target_t;
    mgToggle();

    document.getElementById('mg_diag').innerHTML =
      `Пакетов OK: ${d.pkt_ok} &nbsp; CRC ошибок: ${d.pkt_crc_err}<br>
       Последний пакет от котла: ${mgAge(d.boiler_age_ms)}<br>
       Последний пакет от пульта: ${mgAge(d.remote_age_ms)}`;
  } catch(e) {}
}

async function loadMgRaw() {
  try {
    const r = await fetch('/api/mastergas/raw');
    const d = await r.json();
    document.getElementById('mg_raw_status').textContent = d.raw_status || '—';
    document.getElementById('mg_raw_ext').textContent = d.raw_ext || '—';
  } catch(e) {}
}

async function mgSave() {
  const data = {
    passthrough:    document.getElementById('mg_passthrough').checked,
    override_power: document.getElementById('mg_o_power').checked,
    override_temp:  +document.getElementById('mg_o_temp').value,
  };
  const r = await fetch('/api/mastergas/save', {method:'POST',
    headers:{'Content-Type':'application/json'}, body:JSON.stringify(data)});
  if (r.ok) alert('Сохранено');
}

loadMgStatus();
loadMgRaw();
setInterval(loadMgStatus, 2000);
setInterval(loadMgRaw, 5000);
</script>
)html";

// ── Config ───────────────────────────────────────────────────────────────────
bool MasterGasDevice::loadConfig() {
    JsonDocument doc;
    if (!ConfigManager::loadJson("/mastergas.json", doc)) return false;
    masterGasCfg.enabled        = doc["enabled"]        | true;
    masterGasCfg.passthrough    = doc["passthrough"]    | true;
    masterGasCfg.override_power = doc["override_power"] | true;
    masterGasCfg.override_temp  = doc["override_temp"]  | 50;
    masterGasCfg.pub_interval   = doc["pub_interval"]   | 10000;
    return true;
}

bool MasterGasDevice::saveConfig() {
    JsonDocument doc;
    doc["enabled"]        = masterGasCfg.enabled;
    doc["passthrough"]    = masterGasCfg.passthrough;
    doc["override_power"] = masterGasCfg.override_power;
    doc["override_temp"]  = masterGasCfg.override_temp;
    doc["pub_interval"]   = masterGasCfg.pub_interval;
    return ConfigManager::saveJson("/mastergas.json", doc);
}

// ── Протокол (порт OpenMasterGas) ─────────────────────────────────────────────
// Чек-сумма: сумма первых 6 байт, инвертировать, +1.
uint8_t MasterGasDevice::_calcCrc(const uint8_t* buf) {
    uint8_t sum = 0;
    for (int n = 0; n < 6; n++) sum += buf[n];
    return (uint8_t)((sum ^ 0xFF) + 1);
}

// Не блокирует: возвращает false, если 7 байт ещё не накопилось.
// При ложном старте (первый байт не 0xF9/0xFA) съедает один байт и ждёт
// следующего вызова — как и в оригинальном протоколе; при потоке мусора
// возможна временная десинхронизация до следующего валидного заголовка.
bool MasterGasDevice::_waitPacket(HardwareSerial& serial, uint8_t* buf) {
    if (serial.available() < 7) return false;

    int c = serial.read();
    if (c != 0xF9 && c != 0xFA) return false;
    buf[0] = (uint8_t)c;
    for (int n = 1; n < 7; n++) buf[n] = (uint8_t)serial.read();

    if (buf[6] == _calcCrc(buf)) {
        masterGasState.pkt_ok++;
        return true;
    }
    masterGasState.pkt_crc_err++;
    return false;
}

bool MasterGasDevice::_isFromBoiler(const uint8_t* buf) {
    return buf[0] == 0xFA || (buf[0] == 0xF9 && buf[1] == 0x00);
}

void MasterGasDevice::_parsePacket(const uint8_t* buf) {
    if (buf[0] == 0xFA) {
        // Расширенные данные, offset 0x00..0x14 с шагом 4 — смысл части полей
        // не расшифрован (см. апстрим), храним как есть для диагностики.
        uint8_t offset = buf[1];
        if (offset <= sizeof(masterGasState.raw_ext) - 4) {
            for (int n = 0; n < 4; n++)
                masterGasState.raw_ext[offset + n] = buf[2 + n];
        }
        masterGasState.last_boiler_rx_ms = millis();
        return;
    }

    if (buf[0] == 0xF9 && buf[1] == 0x00) {
        // Состояние котла: бит 6 байта [2] — горелка, [3] — T теплоносителя,
        // [4] — T ГВС (все — прямое значение в °C, без масштаба)
        masterGasState.burner_on    = (buf[2] >> 6) & 1;
        masterGasState.coolant_temp = buf[3];
        masterGasState.water_temp   = buf[4];
        for (int n = 0; n < 4; n++) masterGasState.raw_status[n] = buf[2 + n];
        masterGasState.last_boiler_rx_ms = millis();
        return;
    }

    if (buf[0] == 0xF9 && buf[1] == 0x80) {
        // Ответ пульта: [2] бит0 — питание, [3] — уставка °C, [5] — T воздуха
        masterGasState.remote_power       = buf[2] & 0x01;
        masterGasState.remote_target_temp = buf[3];
        masterGasState.remote_air_temp    = buf[5];
        masterGasState.last_remote_rx_ms  = millis();
        return;
    }
}

// Формат ответа пульта на 0xF9/0x00 — байты [4],[5] взяты из наблюдаемых
// реальных ответов пульта (см. апстрим), их смысл не расшифрован, но котёл
// на них не реагирует отказом — используем как константу.
void MasterGasDevice::_sendReply() {
    uint8_t reply[7] = {0xF9, 0x80, 0x00, 0x00, 0x28, 0x18, 0x00};
    reply[2] = masterGasCfg.override_power ? 0x01 : 0x00;
    reply[3] = masterGasCfg.override_temp;
    reply[6] = _calcCrc(reply);
    _remoteSerial.write(reply, 7);
    _remoteSerial.flush();
}

// ── Init ─────────────────────────────────────────────────────────────────────
void MasterGasDevice::init() {
    loadConfig();
    if (!masterGasCfg.enabled) { Serial.println("[MASTERGAS] Disabled"); return; }

    pinMode(MASTERGAS_REMOTE_EN_PIN, OUTPUT);
    digitalWrite(MASTERGAS_REMOTE_EN_PIN, HIGH); // пульт подключен к шине штатно

    // Инверсия сигнала — как в апстриме (OpenMasterGas): линия boilerRead
    // (общая шина) идёт БЕЗ инверсии, а линия пульта (remoteRead) и передатчик
    // ответа на шину (boilerAnswer) — ИНВЕРТИРОВАНЫ (там же rx_remote и Serial1
    // поднимались с invert=true). Это свойство их согласующей электроники, а
    // не протокола — если на реальной плате схема интерфейса другая, первым
    // делом при "пустых"/битых пакетах проверяйте именно этот флаг.
    _boilerSerial.begin(MASTERGAS_BAUD, SERIAL_8N1, MASTERGAS_BOILER_RX_PIN, -1);
    _remoteSerial.begin(MASTERGAS_BAUD, SERIAL_8N1, MASTERGAS_REMOTE_RX_PIN, MASTERGAS_ANSWER_TX_PIN, true);

    WebHandler::registerTab({"mastergas", "Котёл 🔥", "🔥", MASTERGAS_TAB_HTML});

    extern AsyncWebServer server;

    server.on("/api/mastergas/status", HTTP_GET, [](AsyncWebServerRequest* req) {
        JsonDocument doc;
        doc["burner"]      = masterGasState.burner_on;
        doc["coolant_t"]   = masterGasState.coolant_temp;
        doc["water_t"]     = masterGasState.water_temp;
        doc["r_power"]     = masterGasState.remote_power;
        doc["r_target_t"]  = masterGasState.remote_target_temp;
        doc["r_air_t"]     = masterGasState.remote_air_temp;
        doc["passthrough"] = masterGasCfg.passthrough;
        doc["o_power"]     = masterGasCfg.override_power;
        doc["o_target_t"]  = masterGasCfg.override_temp;
        doc["pkt_ok"]      = masterGasState.pkt_ok;
        doc["pkt_crc_err"] = masterGasState.pkt_crc_err;
        doc["boiler_age_ms"] = masterGasState.last_boiler_rx_ms
            ? (int32_t)(millis() - masterGasState.last_boiler_rx_ms) : -1;
        doc["remote_age_ms"] = masterGasState.last_remote_rx_ms
            ? (int32_t)(millis() - masterGasState.last_remote_rx_ms) : -1;
        String out; serializeJson(doc, out);
        req->send(200, "application/json", out);
    });

    server.on("/api/mastergas/raw", HTTP_GET, [](AsyncWebServerRequest* req) {
        static const char* H = "0123456789ABCDEF";
        char hexExt[49] = {0};
        for (int i = 0; i < 24; i++) {
            hexExt[i*2]   = H[masterGasState.raw_ext[i] >> 4];
            hexExt[i*2+1] = H[masterGasState.raw_ext[i] & 0x0F];
        }
        char hexStatus[9] = {0};
        for (int i = 0; i < 4; i++) {
            hexStatus[i*2]   = H[masterGasState.raw_status[i] >> 4];
            hexStatus[i*2+1] = H[masterGasState.raw_status[i] & 0x0F];
        }
        JsonDocument doc;
        doc["raw_ext"]    = hexExt;
        doc["raw_status"] = hexStatus;
        String out; serializeJson(doc, out);
        req->send(200, "application/json", out);
    });

    auto* hSave = new AsyncCallbackJsonWebHandler("/api/mastergas/save",
        [](AsyncWebServerRequest* req, JsonVariant& json) {
            JsonObject o = json.as<JsonObject>();
            masterGasCfg.passthrough    = o["passthrough"]    | true;
            masterGasCfg.override_power = o["override_power"] | true;
            masterGasCfg.override_temp  = o["override_temp"]  | 50;
            MasterGasDevice::saveConfig();
            req->send(200, "application/json", "{\"status\":\"ok\"}");
        }
    );
    server.addHandler(hSave);

    Serial.println("[MASTERGAS] Init OK");
}

// ── Loop ─────────────────────────────────────────────────────────────────────
void MasterGasDevice::loop() {
    if (!masterGasCfg.enabled) return;

    static uint8_t buf[7];

    if (_waitPacket(_boilerSerial, buf)) {
        if (masterGasCfg.passthrough) {
            // Штатный пульт отвечает сам — только слушаем и парсим шину
            _parsePacket(buf);
        } else if (_isFromBoiler(buf)) {
            _parsePacket(buf);

            // Отключаем штатный пульт от шины — дальше отвечаем сами
            digitalWrite(MASTERGAS_REMOTE_EN_PIN, LOW);

            if (buf[0] == 0xF9) {
                // Тайминг из реверс-инжиниринга протокола (см. README
                // OpenMasterGas) — котёл ждёт паузу перед ответом пульта.
                // Суммарно ~34мс, вотчдогу (8с) ничем не грозит.
                delay(4);
                _sendReply();
                delay(30);
            } else {
                // 0xFA (расширенные данные) — ESP их не подменяет, только
                // подсматривает настоящий ответ пульта на его собственной
                // линии (remoteRead), пока тот отключен от шины. Сам запрос
                // при этом остаётся без ответа котлу — так же ведёт себя
                // и апстрим-проект.
                while (_remoteSerial.available()) _remoteSerial.read();
                delay(34);
                uint8_t rbuf[7];
                if (_waitPacket(_remoteSerial, rbuf)) _parsePacket(rbuf);
            }

            digitalWrite(MASTERGAS_REMOTE_EN_PIN, HIGH);
        }
    }

#ifdef MODULE_MQTT
    if (millis() - _lastPub >= masterGasCfg.pub_interval) {
        _lastPub = millis();
        _publishMqtt();
    }
#endif
}

#ifdef MODULE_MQTT
void MasterGasDevice::_publishMqtt() {
    JsonDocument doc;
    doc["burner"]     = masterGasState.burner_on;
    doc["coolant_t"]  = masterGasState.coolant_temp;
    doc["water_t"]    = masterGasState.water_temp;
    doc["r_power"]    = masterGasState.remote_power;
    doc["r_target_t"] = masterGasState.remote_target_temp;
    doc["r_air_t"]    = masterGasState.remote_air_temp;
    String payload; serializeJson(doc, payload);
    String topic = String(baseCfg.device_name) + "/mastergas/state";
    MqttHandler::publish(topic.c_str(), payload.c_str(), true);
}
#endif

#endif // DEVICE_MASTERGAS
