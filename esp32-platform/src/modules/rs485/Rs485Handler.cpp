#ifdef MODULE_RS485
#include "Rs485Handler.h"
#include "ConfigManager.h"
#include "WebHandler.h"
#include <AsyncJson.h>

Rs485Config rs485Cfg;
Rs485Status rs485Status;
void (*Rs485Handler::onFrame)(const uint8_t*, size_t) = nullptr;

HardwareSerial* Rs485Handler::_serial     = nullptr;
uint32_t        Rs485Handler::_lastByteMs = 0;
uint8_t         Rs485Handler::_rxBuf[256];
size_t          Rs485Handler::_rxLen      = 0;

// ── Hex-консоль: HTML+JS, вкладка "RS485" ──────────────────────────
// Лог TX/RX через polling /api/rs485/status раз в 500мс (сравниваем
// last_rx_hex с локально запомненным — если изменился, дописываем
// строку в лог). last_rx_hex обновляется на КАЖДЫЙ принятый фрейм,
// включая автоматические ответы EkoNomDevice — так что видно, приходит
// ли вообще что-то с шины, даже ничего не отправляя руками.
static const char RS485_TAB_HTML[] PROGMEM = R"html(
<style>
.r-log{background:var(--bg);border:1px solid var(--border);border-radius:8px;
  padding:8px;height:260px;overflow-y:auto;font-family:monospace;font-size:11px;
  display:flex;flex-direction:column-reverse}
.r-log .tx{color:var(--accent)}
.r-log .rx{color:var(--ok,#3ec97a)}
.r-row{display:flex;gap:8px}
.r-row>div{flex:1;min-width:0}
</style>
<div class="settings-group">
  <h3>🔌 RS485 — статус</h3>
  <div class="info-row" id="r_status">загрузка...</div>
</div>
<div class="settings-group">
  <h3>⌨️ Hex-консоль</h3>
  <div class="info-row" style="margin-bottom:8px">
    Байты через пробел, например: 01 03 00 00 00 0C 45 CF (это как раз
    запрос R1 к EkoNom). Кнопка +CRC16 добавляет корректный CRC для
    Modbus RTU к уже введённым байтам (без него их дописывать вручную).
  </div>
  <div style="display:flex;gap:8px">
    <input type="text" id="r_cmd" placeholder="01 03 00 00 00 0C" style="margin:0;flex:1">
  </div>
  <div style="display:flex;gap:8px;margin-top:8px">
    <button onclick="rAppendCrc()" class="btn-secondary" style="margin:0">+CRC16</button>
    <button onclick="rSend()" style="margin:0">▶ Отправить</button>
  </div>
  <label style="margin-top:10px"><input type="checkbox" id="r_auto" checked onchange="rAutoToggle()"> Авто-обновление лога</label>
  <div class="r-log" id="r_log"></div>
</div>
<div class="settings-group">
  <h3>⚙️ Настройки порта</h3>
  <div class="r-row">
    <div><label>Baud</label><input type="number" step="1" name="baud"></div>
    <div><label>Тишина между фреймами, мс</label><input type="number" step="1" name="inter_frame"></div>
  </div>
  <div class="r-row">
    <div><label>Чётность (0=нет,1=even,2=odd)</label><input type="number" step="1" name="parity"></div>
    <div><label>Стоп-биты</label><input type="number" step="1" name="stop_bits"></div>
  </div>
  <label>DE/RE пин трансивера (-1 = не используется, полудуплекс без ручного управления направлением)</label>
  <input type="number" step="1" name="de_pin">
  <button onclick="rSaveConfig()">💾 Сохранить и применить</button>
</div>
<script>
let _rLastRx = '';
let _rAutoTimer = null;

function rLogAdd(cls, hex) {
  const log = document.getElementById('r_log');
  const row = document.createElement('div');
  row.className = cls;
  const t = new Date().toLocaleTimeString();
  row.textContent = t + ' ' + (cls === 'tx' ? 'TX' : 'RX') + ': ' + hex;
  log.prepend(row);
  while (log.children.length > 300) log.removeChild(log.lastChild);
}

function crc16modbus(bytes) {
  let crc = 0xFFFF;
  for (const b of bytes) {
    crc ^= b;
    for (let i = 0; i < 8; i++) {
      crc = (crc & 1) ? ((crc >> 1) ^ 0xA001) : (crc >> 1);
    }
  }
  return crc;
}
function rAppendCrc() {
  const input = document.getElementById('r_cmd');
  const bytes = input.value.trim().split(/\s+/).filter(s => s).map(s => parseInt(s, 16));
  if (!bytes.length || bytes.some(isNaN)) return alert('Некорректные байты');
  const crc = crc16modbus(bytes);
  const lo = (crc & 0xFF).toString(16).padStart(2, '0');
  const hi = ((crc >> 8) & 0xFF).toString(16).padStart(2, '0');
  input.value = bytes.map(b => b.toString(16).padStart(2, '0')).join(' ') + ' ' + lo + ' ' + hi;
}
async function rSend() {
  const hex = document.getElementById('r_cmd').value.trim();
  if (!hex) return;
  rLogAdd('tx', hex.toUpperCase());
  await fetch('/api/rs485/send', {method: 'POST', headers: {'Content-Type': 'application/json'},
    body: JSON.stringify({hex})});
}
async function rPoll() {
  try {
    const d = await (await fetch('/api/rs485/status')).json();
    document.getElementById('r_status').innerHTML =
      (d.enabled ? 'включён' : '<span style="color:var(--danger)">выключен</span>') +
      ' — RX: ' + d.rx_bytes + ' байт, TX: ' + d.tx_bytes + ' байт';
    if (d.last_rx_hex && d.last_rx_hex !== _rLastRx) {
      _rLastRx = d.last_rx_hex;
      rLogAdd('rx', d.last_rx_hex);
    }
  } catch (e) {}
}
function rAutoToggle() {
  if (_rAutoTimer) { clearInterval(_rAutoTimer); _rAutoTimer = null; }
  if (document.getElementById('r_auto').checked) _rAutoTimer = setInterval(rPoll, 500);
}
async function rLoadConfig() {
  const d = await (await fetch('/api/rs485/config')).json();
  Object.keys(d).forEach(k => {
    const el = document.querySelector('[name="'+k+'"]');
    if (el) el.value = d[k];
  });
}
async function rSaveConfig() {
  const fields = ['baud', 'inter_frame', 'parity', 'stop_bits', 'de_pin'];
  const data = {};
  fields.forEach(k => data[k] = parseInt(document.querySelector('[name="'+k+'"]').value, 10));
  const r = await fetch('/api/rs485/save', {method: 'POST', headers: {'Content-Type': 'application/json'},
    body: JSON.stringify(data)});
  if (r.ok) alert('Сохранено и применено (без перезагрузки)');
}
document.getElementById('r_cmd').addEventListener('keydown', e => { if (e.key === 'Enter') rSend(); });
rLoadConfig();
rPoll();
rAutoToggle();
</script>
)html";

String Rs485Handler::_toHex(const uint8_t* data, size_t len) {
    String s; s.reserve(len * 3);
    for (size_t i = 0; i < len; i++) {
        if (i) s += ' ';
        char b[3]; snprintf(b, 3, "%02X", data[i]);
        s += b;
    }
    return s;
}

size_t Rs485Handler::_fromHex(const String& hex, uint8_t* buf, size_t maxLen) {
    size_t count = 0;
    int i = 0;
    while (i < (int)hex.length() && count < maxLen) {
        while (i < (int)hex.length() && hex[i] == ' ') i++;
        if (i + 1 >= (int)hex.length()) break;
        char b[3] = { hex[i], hex[i + 1], 0 };
        buf[count++] = (uint8_t)strtol(b, nullptr, 16);
        i += 2;
    }
    return count;
}

void Rs485Handler::_initSerial() {
    uint32_t config = SERIAL_8N1;
    if      (rs485Cfg.parity == 1 && rs485Cfg.stop_bits == 1) config = SERIAL_8E1;
    else if (rs485Cfg.parity == 1 && rs485Cfg.stop_bits == 2) config = SERIAL_8E2;
    else if (rs485Cfg.parity == 2 && rs485Cfg.stop_bits == 1) config = SERIAL_8O1;
    else if (rs485Cfg.parity == 2 && rs485Cfg.stop_bits == 2) config = SERIAL_8O2;
    else if (rs485Cfg.stop_bits == 2)                          config = SERIAL_8N2;

    if (!_serial) _serial = new HardwareSerial(2);
    _serial->begin(rs485Cfg.baud, config, rs485Cfg.rx_pin, rs485Cfg.tx_pin);

    if (rs485Cfg.de_pin >= 0) {
        pinMode(rs485Cfg.de_pin, OUTPUT);
        digitalWrite(rs485Cfg.de_pin, LOW);
    }
    Serial.printf("[RS485] UART2 %d baud RX=%d TX=%d DE=%d\n",
                  rs485Cfg.baud, rs485Cfg.rx_pin, rs485Cfg.tx_pin, rs485Cfg.de_pin);
}

void Rs485Handler::sendBytes(const uint8_t* data, size_t len) {
    if (!_serial) return;
    if (rs485Cfg.de_pin >= 0) { digitalWrite(rs485Cfg.de_pin, HIGH); delayMicroseconds(200); }
    _serial->write(data, len);
    _serial->flush();
    if (rs485Cfg.de_pin >= 0) { delayMicroseconds(200); digitalWrite(rs485Cfg.de_pin, LOW); }
    rs485Status.tx_bytes += len;
    rs485Status.last_tx_hex = _toHex(data, len);
}

void Rs485Handler::sendHex(const String& hexStr) {
    uint8_t buf[256];
    size_t len = _fromHex(hexStr, buf, sizeof(buf));
    if (len > 0) sendBytes(buf, len);
}

void Rs485Handler::_readSerial() {
    if (!_serial) return;
    uint32_t now = millis();
    while (_serial->available() && _rxLen < sizeof(_rxBuf)) {
        _rxBuf[_rxLen++] = _serial->read();
        _lastByteMs = now;
    }
    if (_rxLen > 0 && (now - _lastByteMs) >= rs485Cfg.inter_frame) {
        rs485Status.rx_bytes += _rxLen;
        rs485Status.last_rx_hex = _toHex(_rxBuf, _rxLen);
        if (onFrame) onFrame(_rxBuf, _rxLen);
        _rxLen = 0;
    }
}

void Rs485Handler::init() {
    loadConfig();
    if (!rs485Cfg.enabled) { Serial.println("[RS485] Disabled"); return; }
    _initSerial();

    // Скрыта из nav по умолчанию (see BaseConfig::show_diagnostics) — не
    // нужна в обычной эксплуатации, но пригодилась при вводе EkoNom в
    // строй и может понадобиться снова, поэтому не убираем совсем.
    WebHandler::registerTab({"rs485", "RS485", "🔌", RS485_TAB_HTML, &baseCfg.show_diagnostics});

    extern AsyncWebServer server;
    server.on("/api/rs485/status", HTTP_GET, [](AsyncWebServerRequest* req) {
        JsonDocument doc;
        doc["enabled"]      = rs485Cfg.enabled;
        doc["rx_bytes"]     = rs485Status.rx_bytes;
        doc["tx_bytes"]     = rs485Status.tx_bytes;
        doc["last_rx_hex"]  = rs485Status.last_rx_hex;
        doc["last_tx_hex"]  = rs485Status.last_tx_hex;
        String out; serializeJson(doc, out);
        req->send(200, "application/json", out);
    });

    server.on("/api/rs485/config", HTTP_GET, [](AsyncWebServerRequest* req) {
        JsonDocument doc;
        doc["baud"]        = rs485Cfg.baud;
        doc["parity"]      = rs485Cfg.parity;
        doc["stop_bits"]   = rs485Cfg.stop_bits;
        doc["de_pin"]      = rs485Cfg.de_pin;
        doc["inter_frame"] = rs485Cfg.inter_frame;
        String out; serializeJson(doc, out);
        req->send(200, "application/json", out);
    });

    auto* saveH = new AsyncCallbackJsonWebHandler("/api/rs485/save",
        [](AsyncWebServerRequest* req, JsonVariant& json) {
            JsonObject o = json.as<JsonObject>();
            rs485Cfg.baud        = o["baud"]        | 9600;
            rs485Cfg.parity      = o["parity"]      | 0;
            rs485Cfg.stop_bits   = o["stop_bits"]   | 1;
            rs485Cfg.de_pin      = o["de_pin"]      | -1;
            rs485Cfg.inter_frame = o["inter_frame"] | 4;
            Rs485Handler::saveConfig();
            Rs485Handler::_initSerial();  // применяем сразу, без перезагрузки
            req->send(200, "application/json", "{\"status\":\"ok\"}");
        }
    );
    server.addHandler(saveH);

    auto* sendH = new AsyncCallbackJsonWebHandler("/api/rs485/send",
        [](AsyncWebServerRequest* req, JsonVariant& json) {
            JsonObject o = json.as<JsonObject>();
            String hex = o["hex"] | "";
            Rs485Handler::sendHex(hex);
            req->send(200, "application/json", "{\"status\":\"ok\"}");
        }
    );
    server.addHandler(sendH);

    Serial.println("[RS485] Init OK");
}

void Rs485Handler::loop() {
    if (!rs485Cfg.enabled) return;
    _readSerial();
}

bool Rs485Handler::loadConfig() {
    JsonDocument doc;
    if (!ConfigManager::loadJson("/rs485.json", doc)) return false;
    rs485Cfg.enabled     = doc["enabled"]     | true;
    rs485Cfg.rx_pin      = doc["rx_pin"]      | RS485_RX_PIN;
    rs485Cfg.tx_pin      = doc["tx_pin"]      | RS485_TX_PIN;
    rs485Cfg.de_pin      = doc["de_pin"]      | -1;
    rs485Cfg.baud        = doc["baud"]        | 2400;
    rs485Cfg.data_bits   = doc["data_bits"]   | 8;
    rs485Cfg.parity      = doc["parity"]      | 1;
    rs485Cfg.stop_bits   = doc["stop_bits"]   | 1;
    rs485Cfg.inter_frame = doc["inter_frame"] | 50;
    return true;
}

bool Rs485Handler::saveConfig() {
    JsonDocument doc;
    doc["enabled"]     = rs485Cfg.enabled;
    doc["rx_pin"]      = rs485Cfg.rx_pin;
    doc["tx_pin"]      = rs485Cfg.tx_pin;
    doc["de_pin"]      = rs485Cfg.de_pin;
    doc["baud"]        = rs485Cfg.baud;
    doc["data_bits"]   = rs485Cfg.data_bits;
    doc["parity"]      = rs485Cfg.parity;
    doc["stop_bits"]   = rs485Cfg.stop_bits;
    doc["inter_frame"] = rs485Cfg.inter_frame;
    return ConfigManager::saveJson("/rs485.json", doc);
}

#endif // MODULE_RS485
