#ifdef DEVICE_KC868_A16
#include "A16Device.h"
#include "../../core/ConfigManager.h"
#include "../../core/WebHandler.h"
#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <AsyncJson.h>
#include <ArduinoJson.h>

#ifdef MODULE_MQTT
#include "../../modules/mqtt/MqttHandler.h"
#endif

A16Config a16Cfg;
A16State  a16State;

uint32_t A16Device::_lastPoll   = 0;
uint32_t A16Device::_lastOwRead = 0;
bool     A16Device::_owScanned  = false;

// OneWire на трёх пинах
static OneWire         ow1(A16_OW_PIN1);
static OneWire         ow2(A16_OW_PIN2);
static OneWire         ow3(A16_OW_PIN3);
static DallasTemperature dt1(&ow1);
static DallasTemperature dt2(&ow2);
static DallasTemperature dt3(&ow3);

// ============================================================
//  HTML ВКЛАДКИ (PROGMEM)
// ============================================================
static const char A16_PERIPH_HTML[] PROGMEM = R"html(
<div class="a16-page">

  <!-- ЦИФРОВЫЕ ВХОДЫ -->
  <div class="a16-section">
    <div class="a16-section-title">ЦИФРОВЫЕ ВХОДЫ</div>
    <div class="a16-grid16" id="din_grid">
      <!-- генерируется JS -->
    </div>
  </div>

  <!-- ЦИФРОВЫЕ ВЫХОДЫ -->
  <div class="a16-section">
    <div class="a16-section-title">РЕЛЕ ВЫХОДЫ</div>
    <div class="a16-grid16" id="dout_grid">
      <!-- генерируется JS -->
    </div>
  </div>

  <!-- АНАЛОГОВЫЕ ВХОДЫ -->
  <div class="a16-section">
    <div class="a16-section-title">АНАЛОГОВЫЕ ВХОДЫ</div>
    <div class="a16-grid4" id="adc_grid">
      <!-- генерируется JS -->
    </div>
  </div>

  <!-- 1-WIRE ДАТЧИКИ -->
  <div class="a16-section">
    <div class="a16-section-title">1-WIRE ТЕМПЕРАТУРА</div>
    <div id="ow_grid">
      <!-- генерируется JS -->
    </div>
    <button class="a16-btn-sm" onclick="owScan()">🔍 Сканировать шину</button>
  </div>

</div>

<style>
.a16-page { display:flex; flex-direction:column; gap:12px; }
.a16-section {
  background: var(--card);
  border-radius:10px;
  padding:14px;
  border:1px solid var(--border);
}
.a16-section-title {
  font-size:10px; font-weight:700;
  letter-spacing:.12em; text-transform:uppercase;
  color:var(--muted); margin-bottom:12px;
}
.a16-grid16 {
  display:grid;
  grid-template-columns: repeat(8,1fr);
  gap:6px;
}
.a16-grid4 {
  display:grid;
  grid-template-columns: repeat(2,1fr);
  gap:8px;
}
@media(max-width:400px) {
  .a16-grid16 { grid-template-columns: repeat(4,1fr); }
}

/* Ячейка входа */
.din-cell {
  background:#111318;
  border:1px solid var(--border);
  border-radius:6px;
  padding:6px 4px;
  text-align:center;
  cursor:default;
  transition: border-color .2s;
}
.din-cell.on {
  border-color:var(--ok);
  background:rgba(62,201,122,.1);
}
.din-num { font-size:9px; color:var(--muted); margin-bottom:3px; }
.din-dot { width:10px; height:10px; border-radius:50%;
  background:var(--border); margin:0 auto 3px; transition:background .2s; }
.din-cell.on .din-dot { background:var(--ok); box-shadow:0 0 6px var(--ok); }
.din-cnt { font-size:9px; color:var(--muted); }

/* Ячейка выхода */
.dout-cell {
  background:#111318;
  border:1px solid var(--border);
  border-radius:6px;
  padding:6px 4px;
  text-align:center;
  cursor:pointer;
  transition: all .15s;
  user-select:none;
}
.dout-cell:active { opacity:.7; }
.dout-cell.on {
  border-color:var(--accent);
  background:rgba(240,165,0,.1);
}
.dout-num { font-size:9px; color:var(--muted); margin-bottom:3px; }
.dout-dot { width:10px; height:10px; border-radius:50%;
  background:var(--border); margin:0 auto 3px; transition:background .2s; }
.dout-cell.on .dout-dot { background:var(--accent); box-shadow:0 0 6px var(--accent); }
.dout-lbl { font-size:9px; color:var(--muted); }

/* Аналог */
.adc-cell {
  background:#111318;
  border:1px solid var(--border);
  border-radius:8px;
  padding:12px;
}
.adc-lbl { font-size:10px; color:var(--muted); margin-bottom:4px; }
.adc-val { font-size:22px; font-weight:800; color:var(--accent); }
.adc-unit { font-size:11px; color:var(--muted); margin-left:2px; }
.adc-bar { height:4px; background:var(--border); border-radius:2px; margin-top:8px; }
.adc-bar-fill { height:100%; background:var(--accent);
  border-radius:2px; transition:width .5s; }

/* 1-Wire */
.ow-row {
  display:flex; align-items:center; gap:10px;
  background:#111318; border:1px solid var(--border);
  border-radius:6px; padding:10px 12px; margin-bottom:6px;
}
.ow-temp { font-size:20px; font-weight:800; color:var(--cool); margin-left:auto; }
.ow-addr { font-size:10px; color:var(--muted); font-family:monospace; }
.ow-lbl  { font-size:12px; color:var(--text); }

.a16-btn-sm {
  width:auto; padding:6px 14px;
  font-size:12px; margin-top:8px;
  background:var(--border); color:var(--text);
}
</style>

<script>
let a16Labels = { din:[], dout:[], adc:[], ow:[] };

// Генерация сеток при загрузке
async function initA16() {
  const r = await fetch('/api/a16/config');
  const cfg = await r.json();
  a16Labels = { din: cfg.din_label, dout: cfg.dout_label,
                adc: cfg.adc_label, ow:  cfg.ow_label };

  // DIN сетка
  const dinGrid = document.getElementById('din_grid');
  dinGrid.innerHTML = '';
  for (let i=0; i<16; i++) {
    dinGrid.innerHTML += `
      <div class="din-cell" id="din_${i}">
        <div class="din-num">${i+1}</div>
        <div class="din-dot"></div>
        <div class="din-cnt" id="cnt_${i}">0</div>
      </div>`;
  }

  // DOUT сетка
  const doutGrid = document.getElementById('dout_grid');
  doutGrid.innerHTML = '';
  for (let i=0; i<16; i++) {
    doutGrid.innerHTML += `
      <div class="dout-cell" id="dout_${i}" onclick="toggleOut(${i})">
        <div class="dout-num">${i+1}</div>
        <div class="dout-dot"></div>
        <div class="dout-lbl">${cfg.dout_label[i]||''}</div>
      </div>`;
  }

  // ADC сетка
  const adcGrid = document.getElementById('adc_grid');
  adcGrid.innerHTML = '';
  for (let i=0; i<4; i++) {
    adcGrid.innerHTML += `
      <div class="adc-cell">
        <div class="adc-lbl">${cfg.adc_label[i]||'АЦП '+(i+1)}</div>
        <div><span class="adc-val" id="adc_v${i}">--</span>
             <span class="adc-unit" id="adc_u${i}"></span></div>
        <div class="adc-bar"><div class="adc-bar-fill" id="adc_b${i}" style="width:0%"></div></div>
      </div>`;
  }

  updateA16();
}

async function updateA16() {
  try {
    const r = await fetch('/api/a16/data');
    const d = await r.json();

    // DIN
    for (let i=0; i<16; i++) {
      const cell = document.getElementById('din_'+i);
      if (!cell) continue;
      cell.classList.toggle('on', d.din[i]);
      document.getElementById('cnt_'+i).textContent = d.din_counter[i];
    }

    // DOUT
    for (let i=0; i<16; i++) {
      const cell = document.getElementById('dout_'+i);
      if (cell) cell.classList.toggle('on', d.dout[i]);
    }

    // ADC
    for (let i=0; i<4; i++) {
      const el = document.getElementById('adc_v'+i);
      const un = document.getElementById('adc_u'+i);
      const bar = document.getElementById('adc_b'+i);
      const pct = Math.min(100, Math.max(0, d.adc_pct[i]));
      if (bar) bar.style.width = pct+'%';
      if (i < 2) {
        // 4-20мА каналы — показываем мА
        const ma = d.adc_ma ? d.adc_ma[i] : null;
        if (el) el.textContent = ma != null ? ma.toFixed(2) : '--';
        if (un) un.textContent = 'мА';
      } else {
        // 0-5В каналы — показываем вольты
        if (el) el.textContent = d.adc_value[i].toFixed(2);
        if (un) un.textContent = 'В';
      }
    }

    // 1-Wire
    const owGrid = document.getElementById('ow_grid');
    if (d.ow && d.ow.length) {
      owGrid.innerHTML = d.ow.map((s,i) => `
        <div class="ow-row">
          <div>
            <div class="ow-lbl">${a16Labels.ow[i]||'Датчик '+(i+1)}</div>
            <div class="ow-addr">${s.addr}</div>
          </div>
          <div class="ow-temp">${s.ok ? s.temp.toFixed(1)+'°C' : '—'}</div>
        </div>`).join('');
    } else {
      owGrid.innerHTML = '<div style="color:var(--muted);font-size:12px">Датчики не найдены. Нажмите Сканировать.</div>';
    }
  } catch(e) {}
}

async function toggleOut(ch) {
  await fetch('/api/a16/toggle', {
    method:'POST',
    headers:{'Content-Type':'application/json'},
    body: JSON.stringify({ch})
  });
}

async function owScan() {
  await fetch('/api/a16/ow-scan', {method:'POST'});
  setTimeout(updateA16, 3000);
}

initA16();
setInterval(updateA16, 1000);
</script>
)html";

// ============================================================
//  INIT
// ============================================================
void A16Device::init() {
    loadConfig();

    Wire.begin(A16_I2C_SDA, A16_I2C_SCL);

    // Все выходы выключены при старте
    writePCF(A16_PCF_OUT_0, 0xFF);  // PCF8574: лог.1 = реле выключено
    writePCF(A16_PCF_OUT_1, 0xFF);

    // 1-Wire
    dt1.begin(); dt2.begin(); dt3.begin();
    dt1.setWaitForConversion(false);
    dt2.setWaitForConversion(false);
    dt3.setWaitForConversion(false);

    // Регистрация вкладки
    WebHandler::registerTab({"periph", "Периферия", "🔌", A16_PERIPH_HTML});

    // ── API ──────────────────────────────────────────────────
    extern AsyncWebServer server;

    // Данные
    server.on("/api/a16/data", HTTP_GET, [](AsyncWebServerRequest* req) {
        JsonDocument doc;

        // DIN
        JsonArray din = doc["din"].to<JsonArray>();
        JsonArray cnt = doc["din_counter"].to<JsonArray>();
        for (int i = 0; i < 16; i++) {
            din.add(a16State.din[i]);
            cnt.add(a16State.din_counter[i]);
        }

        // DOUT
        JsonArray dout = doc["dout"].to<JsonArray>();
        for (int i = 0; i < 16; i++) dout.add(a16State.dout[i]);

        // ADC
        JsonArray adcV = doc["adc_value"].to<JsonArray>();
        JsonArray adcP = doc["adc_pct"].to<JsonArray>();
        JsonArray adcR = doc["adc_raw"].to<JsonArray>();
        JsonArray adcM = doc["adc_ma"].to<JsonArray>();  
        for (int i = 0; i < 4; i++) {
            adcV.add(a16State.adc_value[i]);
            adcR.add(a16State.adc_raw[i]);
            adcM.add(a16State.adc_ma[i]); 
            float range = a16Cfg.adc_max[i] - a16Cfg.adc_min[i];
            float pct = (range > 0)
                ? (a16State.adc_value[i] - a16Cfg.adc_min[i]) / range * 100.0f
                : 0;
            adcP.add(constrain(pct, 0, 100));
        }

        // 1-Wire
        JsonArray ow = doc["ow"].to<JsonArray>();
        for (int i = 0; i < a16State.ow_count; i++) {
            JsonObject s = ow.add<JsonObject>();
            char addr[17];
            snprintf(addr, sizeof(addr), "%016llX", a16State.ow_addr[i]);
            s["addr"] = addr;
            s["temp"] = a16State.ow_temp[i];
            s["ok"]   = a16State.ow_ok[i];
        }

        String out; serializeJson(doc, out);
        req->send(200, "application/json", out);
    });

    // Конфиг (метки)
    server.on("/api/a16/config", HTTP_GET, [](AsyncWebServerRequest* req) {
        JsonDocument doc;
        JsonArray dl = doc["din_label"].to<JsonArray>();
        JsonArray ql = doc["dout_label"].to<JsonArray>();
        JsonArray al = doc["adc_label"].to<JsonArray>();
        JsonArray ol = doc["ow_label"].to<JsonArray>();
        for (int i = 0; i < 16; i++) { dl.add(a16Cfg.din_label[i]); ql.add(a16Cfg.dout_label[i]); }
        for (int i = 0; i < 4;  i++) al.add(a16Cfg.adc_label[i]);
        for (int i = 0; i < A16_MAX_OW_SENSORS; i++) ol.add(a16Cfg.ow_label[i]);
        String out; serializeJson(doc, out);
        req->send(200, "application/json", out);
    });

    // Переключение выхода
    auto* toggleH = new AsyncCallbackJsonWebHandler("/api/a16/toggle",
        [](AsyncWebServerRequest* req, JsonVariant& json) {
            int ch = json["ch"] | -1;
            if (ch >= 0 && ch < 16) A16Device::toggleOutput(ch);
            req->send(200, "application/json", "{\"ok\":true}");
        }
    );
    server.addHandler(toggleH);

    // Сброс счётчика
    auto* resetH = new AsyncCallbackJsonWebHandler("/api/a16/reset-counter",
        [](AsyncWebServerRequest* req, JsonVariant& json) {
            int ch = json["ch"] | -1;
            if (ch >= 0 && ch < 16) a16State.din_counter[ch] = 0;
            req->send(200, "application/json", "{\"ok\":true}");
        }
    );
    server.addHandler(resetH);

    // Сканирование 1-Wire
    server.on("/api/a16/ow-scan", HTTP_POST, [](AsyncWebServerRequest* req) {
        A16Device::scanOneWire();
        req->send(200, "application/json", "{\"ok\":true}");
    });

    Serial.println("[A16] Device init OK");
}

// ============================================================
//  LOOP
// ============================================================
void A16Device::loop() {
    if (millis() - _lastPoll > a16Cfg.poll_interval) {
        _lastPoll = millis();
        readDigitalInputs();
        readAnalog();
    }

    // 1-Wire: первое сканирование при старте, потом каждые 5 сек
    if (!_owScanned) { scanOneWire(); _owScanned = true; }
    if (millis() - _lastOwRead > 5000) {
        _lastOwRead = millis();
        readOneWire();
    }

    // Безопасная перезагрузка
    xSemaphoreTake(coreMutex, portMAX_DELAY);
    bool reboot = sysState.pendingReboot && (millis() - sysState.rebootAt > 500);
    xSemaphoreGive(coreMutex);
    if (reboot) ESP.restart();
}

// ============================================================
//  ЦИФРОВЫЕ ВХОДЫ
// ============================================================
void A16Device::readDigitalInputs() {
    // Входы читаются через PCF8574, не напрямую с GPIO
    uint8_t byte0 = readPCF(A16_PCF_DI_0);  // IN1-IN8
    uint8_t byte1 = readPCF(A16_PCF_DI_1);  // IN9-IN16

    for (int i = 0; i < 8; i++) {
        // PCF8574: лог.0 = вход активен (оптопара)
        bool state = !((byte0 >> i) & 1);
        if (state && !a16State.din_prev[i]) a16State.din_counter[i]++;
        a16State.din_prev[i] = state;
        a16State.din[i] = state;
    }
    for (int i = 0; i < 8; i++) {
        bool state = !((byte1 >> i) & 1);
        if (state && !a16State.din_prev[i+8]) a16State.din_counter[i+8]++;
        a16State.din_prev[i+8] = state;
        a16State.din[i+8] = state;
    }
}

// ============================================================
//  АНАЛОГОВЫЕ ВХОДЫ
// ============================================================
void A16Device::readAnalog() {
    const uint8_t pins[4] = {A16_ADC_CH1, A16_ADC_CH3, A16_ADC_CH4, A16_ADC_CH2};
    for (int i = 0; i < 4; i++) {
        // Среднее из 4 измерений для стабильности
        int32_t sum = 0;
        for (int j = 0; j < 4; j++) sum += analogRead(pins[i]);
        a16State.adc_raw[i] = sum / 4;

        float raw = a16State.adc_raw[i];

        switch (a16Cfg.adc_mode[i]) {
            case AnalogMode::MODE_4_20MA: {
                // Скользящее среднее по буферу
                if (i < 2) {
                    a16State.adc_ma_buf[i][a16State.adc_ma_idx[i]] = raw;
                    a16State.adc_ma_idx[i] = (a16State.adc_ma_idx[i] + 1) % 10;
                    if (a16State.adc_ma_idx[i] == 0) a16State.adc_ma_full[i] = true;
                    uint8_t cnt = a16State.adc_ma_full[i] ? 10 : a16State.adc_ma_idx[i];
                    int32_t sum = 0;
                    for (uint8_t k = 0; k < cnt; k++) sum += a16State.adc_ma_buf[i][k];
                    raw = (cnt > 0) ? (sum / cnt) : raw;
                }

                float ma = 4.0f + (raw - 557.0f) / (4402.0f - 557.0f) * 16.0f;
                ma = constrain(ma, 4.0f, 20.0f);
                a16State.adc_ma[i] = ma;

                float pct = (ma - 4.0f) / 16.0f;
                a16State.adc_value[i] = a16Cfg.adc_min[i] +
                                        pct * (a16Cfg.adc_max[i] - a16Cfg.adc_min[i]);
                break;
            }
            case AnalogMode::MODE_0_5V: {
                // 0-5В через делитель: Vout = Vin * R2/(R1+R2)
                // По схеме делитель 510/100 → коэф ~0.164
                // Но ESP32 ADC до 3.3В, поэтому 5В → 0.82В → ~1020
                float volt = raw / 4095.0f * 3.3f / 0.164f;
                a16State.adc_value[i] = constrain(volt, 0.0f, 5.0f);
                break;
            }
            default:
                a16State.adc_value[i] = raw;
        }
    }
}

// ============================================================
//  1-WIRE
// ============================================================
void A16Device::scanOneWire() {
    a16State.ow_count = 0;
    uint8_t addr[8];

    auto scan = [&](OneWire& ow) {
        ow.reset_search();
        while (ow.search(addr) && a16State.ow_count < A16_MAX_OW_SENSORS) {
            if (OneWire::crc8(addr, 7) != addr[7]) continue;
            uint64_t id = 0;
            for (int i = 0; i < 8; i++) id = (id << 8) | addr[i];
            a16State.ow_addr[a16State.ow_count++] = id;
        }
    };

    scan(ow1); scan(ow2); scan(ow3);
    Serial.printf("[A16] 1-Wire: found %d sensors\n", a16State.ow_count);
}

void A16Device::readOneWire() {
    if (a16State.ow_count == 0) return;

    dt1.requestTemperatures();
    dt2.requestTemperatures();
    dt3.requestTemperatures();
    delay(100);  // DS18B20 conversion time (async mode)

    for (int i = 0; i < a16State.ow_count; i++) {
        uint8_t addr[8];
        for (int b = 7; b >= 0; b--) {
            addr[7-b] = (a16State.ow_addr[i] >> (b*8)) & 0xFF;
        }
        // Пробуем все три шины
        float t = DEVICE_DISCONNECTED_C;
        DeviceAddress da; memcpy(da, addr, 8);
        if (t == DEVICE_DISCONNECTED_C) t = dt1.getTempC(da);
        if (t == DEVICE_DISCONNECTED_C) t = dt2.getTempC(da);
        if (t == DEVICE_DISCONNECTED_C) t = dt3.getTempC(da);

        a16State.ow_ok[i]   = (t != DEVICE_DISCONNECTED_C);
        a16State.ow_temp[i] = a16State.ow_ok[i] ? t : 0.0f;
    }
}

// ============================================================
//  PCF8574 I/O
// ============================================================
void A16Device::writePCF(uint8_t addr, uint8_t data) {
    Wire.beginTransmission(addr);
    Wire.write(data);
    Wire.endTransmission();
}

uint8_t A16Device::readPCF(uint8_t addr) {
    Wire.requestFrom(addr, (uint8_t)1);
    return Wire.available() ? Wire.read() : 0xFF;
}

void A16Device::syncOutputs() {
    // PCF8574: лог.0 = реле включено, лог.1 = выключено
    uint8_t byte0 = 0xFF, byte1 = 0xFF;
    for (int i = 0; i < 8;  i++) if (a16State.dout[i])   byte0 &= ~(1 << i);
    for (int i = 0; i < 8;  i++) if (a16State.dout[i+8]) byte1 &= ~(1 << i);
    writePCF(A16_PCF_OUT_0, byte0);
    writePCF(A16_PCF_OUT_1, byte1);
}

bool A16Device::setOutput(uint8_t ch, bool state) {
    if (ch >= 16) return false;
    a16State.dout[ch] = state;
    syncOutputs();
    return true;
}

bool A16Device::toggleOutput(uint8_t ch) {
    return setOutput(ch, !a16State.dout[ch]);
}

void A16Device::setAllOutputs(uint16_t mask) {
    for (int i = 0; i < 16; i++) a16State.dout[i] = (mask >> i) & 1;
    syncOutputs();
}

// ============================================================
//  CONFIG
// ============================================================
bool A16Device::loadConfig() {
    JsonDocument doc;
    if (!ConfigManager::loadJson("/device.json", doc)) {
        // Дефолтные метки
        for (int i = 0; i < 16; i++) {
            snprintf(a16Cfg.din_label[i],  20, "IN%d",  i+1);
            snprintf(a16Cfg.dout_label[i], 20, "OUT%d", i+1);
        }
        snprintf(a16Cfg.adc_label[0], 20, "АЦП 1 (4-20мА)");
        snprintf(a16Cfg.adc_label[1], 20, "АЦП 2 (4-20мА)");
        snprintf(a16Cfg.adc_label[2], 20, "АЦП 3 (0-5В)");
        snprintf(a16Cfg.adc_label[3], 20, "АЦП 4 (0-5В)");
        return false;
    }

    for (int i = 0; i < 16; i++) {
        strlcpy(a16Cfg.din_label[i],  doc["din_label"][i]  | "", 20);
        strlcpy(a16Cfg.dout_label[i], doc["dout_label"][i] | "", 20);
    }
    for (int i = 0; i < 4; i++) {
        strlcpy(a16Cfg.adc_label[i], doc["adc_label"][i] | "", 20);
        a16Cfg.adc_min[i] = doc["adc_min"][i] | 0.0f;
        a16Cfg.adc_max[i] = doc["adc_max"][i] | 10.0f;
    }
    for (int i = 0; i < A16_MAX_OW_SENSORS; i++)
        strlcpy(a16Cfg.ow_label[i], doc["ow_label"][i] | "", 20);
    a16Cfg.poll_interval = doc["poll_interval"] | 1000;
    return true;
}

bool A16Device::saveConfig() {
    JsonDocument doc;
    JsonArray dl = doc["din_label"].to<JsonArray>();
    JsonArray ql = doc["dout_label"].to<JsonArray>();
    JsonArray al = doc["adc_label"].to<JsonArray>();
    JsonArray mn = doc["adc_min"].to<JsonArray>();
    JsonArray mx = doc["adc_max"].to<JsonArray>();
    JsonArray ol = doc["ow_label"].to<JsonArray>();
    for (int i = 0; i < 16; i++) { dl.add(a16Cfg.din_label[i]); ql.add(a16Cfg.dout_label[i]); }
    for (int i = 0; i < 4;  i++) { al.add(a16Cfg.adc_label[i]); mn.add(a16Cfg.adc_min[i]); mx.add(a16Cfg.adc_max[i]); }
    for (int i = 0; i < A16_MAX_OW_SENSORS; i++) ol.add(a16Cfg.ow_label[i]);
    doc["poll_interval"] = a16Cfg.poll_interval;
    return ConfigManager::saveJson("/device.json", doc);
}

#endif // DEVICE_KC868_A16
