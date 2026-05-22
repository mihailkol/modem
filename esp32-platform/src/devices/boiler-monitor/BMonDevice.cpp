#ifdef DEVICE_BOILER_MONITOR

#include "BMonDevice.h"
#include "../../core/ConfigManager.h"
#include "../../core/WebHandler.h"

#include <Wire.h>
#include <Adafruit_ADS1X15.h>
#include <AsyncJson.h>
#include <ArduinoJson.h>

#ifdef MODULE_MQTT
#include "../../modules/mqtt/MqttHandler.h"
#endif
#ifdef MODULE_TELEGRAM
#include "../../modules/telegram/TgHandler.h"
#endif

// ============================================================
//  GLOBALS
// ============================================================
BMonConfig bmonCfg;
BMonState  bmonState;

static Adafruit_ADS1115 ads;
static bool adsOk = false;

uint32_t BMonDevice::_lastRead    = 0;
uint32_t BMonDevice::_lastFlow    = 0;
uint32_t BMonDevice::_lastHistory = 0;
uint32_t BMonDevice::_lastPub     = 0;
uint32_t BMonDevice::_pulse1_prev = 0;
uint32_t BMonDevice::_pulse2_prev = 0;

// ============================================================
//  ISR — счётчики расходомеров (IRAM)
// ============================================================
static void IRAM_ATTR onFlow1() { bmonState.pulse1_count++; }
static void IRAM_ATTR onFlow2() { bmonState.pulse2_count++; }

// ============================================================
//  HTML ВКЛАДОК (PROGMEM)
// ============================================================

static const char BMON_MONITOR_HTML[] PROGMEM = R"html(
<div class="monitor-page">

  <div class="m-card">
    <div class="m-card-label">ПРОИЗВОДИТЕЛЬНОСТЬ</div>
    <div class="m-power-big"><span id="bm_power">--</span><span class="unit">кВт</span></div>
    <div class="m-power-sub">Σ <span id="bm_energy">--</span> кВт·ч <span class="muted">(сессия)</span></div>
    <div class="m-power-sub">
      Поток 1: <b id="bm_flow1">--</b> л/мин &nbsp;|&nbsp;
      Поток 2: <b id="bm_flow2">--</b> л/мин
    </div>
  </div>

  <div class="m-card">
    <div class="m-card-label">ТЕМПЕРАТУРЫ</div>
    <div class="m-sensors-row">
      <div class="m-sensor">
        <div class="m-sensor-label">ПОДАЧА</div>
        <div class="m-sensor-val hot" id="bm_tsup">--°</div>
        <div class="m-sensor-icon">▲</div>
      </div>
      <div class="m-sensor">
        <div class="m-sensor-label">ОБРАТКА</div>
        <div class="m-sensor-val cool" id="bm_tret">--°</div>
        <div class="m-sensor-icon cool">▼</div>
      </div>
      <div class="m-sensor">
        <div class="m-sensor-label">ДЕЛЬТА Т</div>
        <div class="m-sensor-val accent" id="bm_tdelta">--°</div>
        <div class="m-sensor-sub muted">расчёт</div>
      </div>
    </div>
    <div class="m-sensors-row two" style="margin-top:8px">
      <div class="m-sensor">
        <div class="m-sensor-label">ПОМЕЩЕНИЕ</div>
        <div class="m-sensor-val warm" id="bm_troom">--°</div>
      </div>
      <div class="m-sensor">
        <div class="m-sensor-label">УЛИЦА</div>
        <div class="m-sensor-val cool" id="bm_tout">--°</div>
      </div>
    </div>
  </div>

  <div class="m-card">
    <div class="m-card-label">ДАВЛЕНИЕ В КОНТУРЕ</div>
    <div class="m-sensors-row">
      <div class="m-sensor">
        <div class="m-sensor-label">ПОДАЧА P1</div>
        <div class="m-sensor-val accent" id="bm_psup">--</div>
        <div class="m-sensor-sub muted">бар</div>
      </div>
      <div class="m-sensor">
        <div class="m-sensor-label">ОБРАТКА P2</div>
        <div class="m-sensor-val accent" id="bm_pret">--</div>
        <div class="m-sensor-sub muted">бар</div>
      </div>
      <div class="m-sensor">
        <div class="m-sensor-label">ДЕЛЬТА Р</div>
        <div class="m-sensor-val accent" id="bm_pdelta">--</div>
        <div class="m-sensor-sub muted">бар</div>
      </div>
    </div>
  </div>

  <div class="m-card">
    <div class="m-card-label">ИСТОРИЯ МОЩНОСТИ (10 мин)</div>
    <canvas id="bmPowerChart" height="80"></canvas>
  </div>

</div>

<script>
function fmtT(v) { return (v === null || v === undefined) ? '--°' : v.toFixed(1) + '°'; }
function fmtP(v) { return (v === null || v === undefined) ? '--'  : v.toFixed(2); }
function fmtF(v) { return (v === null || v === undefined) ? '--'  : v.toFixed(2); }

async function updateBMon() {
  try {
    const r = await fetch('/api/bmon/data');
    const d = await r.json();
    document.getElementById('bm_power').textContent  = d.power_kw.toFixed(1);
    document.getElementById('bm_energy').textContent = d.energy_kwh.toFixed(2);
    document.getElementById('bm_flow1').textContent  = fmtF(d.flow1_lpm);
    document.getElementById('bm_flow2').textContent  = fmtF(d.flow2_lpm);
    document.getElementById('bm_tsup').textContent   = fmtT(d.t_supply);
    document.getElementById('bm_tret').textContent   = fmtT(d.t_return);
    document.getElementById('bm_tdelta').textContent = fmtT(d.t_delta);
    document.getElementById('bm_troom').textContent  = fmtT(d.t_room);
    document.getElementById('bm_tout').textContent   = fmtT(d.t_outdoor);
    document.getElementById('bm_psup').textContent   = fmtP(d.p_supply);
    document.getElementById('bm_pret').textContent   = fmtP(d.p_return);
    document.getElementById('bm_pdelta').textContent = fmtP(d.p_delta);
    drawBMChart(d.history || []);
  } catch(e) {}
}

function drawBMChart(data) {
  const canvas = document.getElementById('bmPowerChart');
  if (!canvas || data.length < 2) return;
  const ctx = canvas.getContext('2d');
  canvas.width = canvas.offsetWidth;
  const W = canvas.width, H = canvas.height;
  ctx.clearRect(0,0,W,H);
  const max = Math.max(...data, 1);
  const step = W / (data.length - 1);
  ctx.beginPath();
  ctx.strokeStyle = '#f0a500';
  ctx.lineWidth = 2;
  data.forEach((v,i) => {
    const x = i * step;
    const y = H - (v / max * H * 0.85) - 4;
    i === 0 ? ctx.moveTo(x,y) : ctx.lineTo(x,y);
  });
  ctx.stroke();
  ctx.lineTo(W,H); ctx.lineTo(0,H); ctx.closePath();
  ctx.fillStyle = 'rgba(240,165,0,0.08)';
  ctx.fill();
}

updateBMon();
setInterval(updateBMon, 2000);
</script>
)html";

// --- Уставки ---
static const char BMON_SETTINGS_HTML[] PROGMEM = R"html(
<div class="settings-group">
  <h3>🌡 Температурные пороги</h3>
  <label>Макс. температура подачи (авария):
    <input type="number" id="bm_t_max" step="0.5">°C
  </label>
  <label>Порог оповещения:
    <input type="number" id="bm_t_alarm" step="0.5">°C
  </label>
  <label>Мин. температура улицы (заморозка):
    <input type="number" id="bm_t_min" step="0.5">°C
  </label>
</div>

<div class="settings-group">
  <h3>💧 Расходомеры</h3>
  <label>Расходомер 1 — л/имп:
    <input type="number" id="bm_flow1_lpi" step="0.001" min="0.001">
  </label>
  <label>Расходомер 2 — л/имп:
    <input type="number" id="bm_flow2_lpi" step="0.001" min="0.001">
  </label>
</div>

<div class="settings-group">
  <h3>📡 Публикация</h3>
  <label>Интервал MQTT (мс):
    <input type="number" id="bm_pub_interval" step="1000" min="1000">
  </label>
  <label>Коэффициент B датчика NTC:
    <input type="number" id="bm_ntc_b" step="10" min="1000" max="6000">
  </label>
</div>

<button onclick="saveBMonCfg()">💾 Сохранить уставки</button>

<script>
async function loadBMonCfg() {
  const r = await fetch('/api/bmon/config');
  const d = await r.json();
  document.getElementById('bm_t_max').value        = d.t_max;
  document.getElementById('bm_t_alarm').value      = d.t_alarm;
  document.getElementById('bm_t_min').value        = d.t_min;
  document.getElementById('bm_flow1_lpi').value    = d.flow1_lpi;
  document.getElementById('bm_flow2_lpi').value    = d.flow2_lpi;
  document.getElementById('bm_pub_interval').value = d.pub_interval;
  document.getElementById('bm_ntc_b').value        = d.ntc_b;
}
async function saveBMonCfg() {
  const data = {
    t_max:        +document.getElementById('bm_t_max').value,
    t_alarm:      +document.getElementById('bm_t_alarm').value,
    t_min:        +document.getElementById('bm_t_min').value,
    flow1_lpi:    +document.getElementById('bm_flow1_lpi').value,
    flow2_lpi:    +document.getElementById('bm_flow2_lpi').value,
    pub_interval: +document.getElementById('bm_pub_interval').value,
    ntc_b:        +document.getElementById('bm_ntc_b').value,
  };
  const r = await fetch('/api/bmon/save', {
    method:'POST',
    headers:{'Content-Type':'application/json'},
    body: JSON.stringify(data)
  });
  if (r.ok) alert('Уставки сохранены');
}
loadBMonCfg();
</script>
)html";

// ============================================================
//  INIT
// ============================================================
void BMonDevice::init() {
    loadConfig();

    // I2C
    Wire.begin(BMON_PIN_SDA, BMON_PIN_SCL);

    // ADS1115
    adsOk = ads.begin(BMON_ADS_ADDR);
    if (adsOk) {
        ads.setGain(BMON_ADS_GAIN);
        Serial.println("[BMON] ADS1115 OK");
    } else {
        Serial.println("[BMON] ADS1115 NOT FOUND — check wiring!");
        bmonState.alarm_sensor = true;
    }

    // Прерывания расходомеров — FALLING (замыкание контакта / фронт Холла)
    pinMode(BMON_PIN_FLOW1, INPUT_PULLUP);
    pinMode(BMON_PIN_FLOW2, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(BMON_PIN_FLOW1), onFlow1, FALLING);
    attachInterrupt(digitalPinToInterrupt(BMON_PIN_FLOW2), onFlow2, FALLING);
    Serial.println("[BMON] Flow interrupts attached");
    Serial.printf("[BMON] GPIO2=%d GPIO4=%d\n", digitalRead(BMON_PIN_FLOW1), digitalRead(BMON_PIN_FLOW2));

    // Аналоговые входы давления
    pinMode(BMON_PIN_PRESS1, INPUT);
    pinMode(BMON_PIN_PRESS2, INPUT);
    analogReadResolution(12);
    analogSetAttenuation(ADC_11db);  // 0–3.3V диапазон

    // Регистрируем вкладки UI
    WebHandler::registerTab({"monitor",  "Мониторинг", "🔥", BMON_MONITOR_HTML});
    WebHandler::registerTab({"bmon_cfg", "Уставки",    "⚙️",  BMON_SETTINGS_HTML});

    // API
    extern AsyncWebServer server;

    server.on("/api/bmon/data", HTTP_GET, [](AsyncWebServerRequest* req) {
        JsonDocument doc;
        // Температуры: NaN → null в JSON
        if (!isnan(bmonState.t_supply))  doc["t_supply"]  = bmonState.t_supply;
        else                             doc["t_supply"]  = nullptr;
        if (!isnan(bmonState.t_return))  doc["t_return"]  = bmonState.t_return;
        else                             doc["t_return"]  = nullptr;
        if (!isnan(bmonState.t_room))    doc["t_room"]    = bmonState.t_room;
        else                             doc["t_room"]    = nullptr;
        if (!isnan(bmonState.t_outdoor)) doc["t_outdoor"] = bmonState.t_outdoor;
        else                             doc["t_outdoor"] = nullptr;
        doc["t_delta"]    = bmonState.t_delta;
        doc["p_supply"]   = bmonState.p_supply;
        doc["p_return"]   = bmonState.p_return;
        doc["p_delta"]    = bmonState.p_delta;
        doc["flow1_lpm"]  = bmonState.flow1_lpm;
        doc["flow2_lpm"]  = bmonState.flow2_lpm;
        doc["power_kw"]   = bmonState.power_kw;
        doc["energy_kwh"] = bmonState.energy_kwh;
        doc["alarm_high"] = bmonState.alarm_high;
        doc["alarm_low"]  = bmonState.alarm_low;
        doc["alarm_sensor"] = bmonState.alarm_sensor;
        doc["pulse1_count"] = bmonState.pulse1_count;
        doc["pulse2_count"] = bmonState.pulse2_count;

        JsonArray hist = doc["history"].to<JsonArray>();
        uint8_t start = (bmonState.hist_head - bmonState.hist_count + BMON_HISTORY_SIZE) % BMON_HISTORY_SIZE;
        for (uint8_t i = 0; i < bmonState.hist_count; i++)
            hist.add(bmonState.hist_power[(start + i) % BMON_HISTORY_SIZE]);

        String out; serializeJson(doc, out);
        req->send(200, "application/json", out);
    });

    server.on("/api/bmon/config", HTTP_GET, [](AsyncWebServerRequest* req) {
        JsonDocument doc;
        doc["t_max"]        = bmonCfg.t_max;
        doc["t_alarm"]      = bmonCfg.t_alarm;
        doc["t_min"]        = bmonCfg.t_min;
        doc["flow1_lpi"]    = bmonCfg.flow1_lpi;
        doc["flow2_lpi"]    = bmonCfg.flow2_lpi;
        doc["pub_interval"] = bmonCfg.pub_interval;
        doc["ntc_b"]        = bmonCfg.ntc_b;
        String out; serializeJson(doc, out);
        req->send(200, "application/json", out);
    });

    auto* h = new AsyncCallbackJsonWebHandler("/api/bmon/save",
        [](AsyncWebServerRequest* req, JsonVariant& json) {
            JsonObject obj = json.as<JsonObject>();
            bmonCfg.t_max        = obj["t_max"]        | 85.0f;
            bmonCfg.t_alarm      = obj["t_alarm"]      | 80.0f;
            bmonCfg.t_min        = obj["t_min"]        | 5.0f;
            bmonCfg.flow1_lpi    = obj["flow1_lpi"]    | 1.0f;
            bmonCfg.flow2_lpi    = obj["flow2_lpi"]    | 1.0f;
            bmonCfg.pub_interval = obj["pub_interval"] | 10000;
            bmonCfg.ntc_b        = obj["ntc_b"]        | BMON_NTC_B;
            BMonDevice::saveConfig();
            req->send(200, "application/json", "{\"status\":\"ok\"}");
        }
    );
    server.addHandler(h);

#ifdef MODULE_TELEGRAM
    TgHandler::onCommand = [](const String& cmd, const String& chat) {
        if (cmd == "/temp") {
            String msg = "🌡 Котёл:\n";
            if (!isnan(bmonState.t_supply))
                msg += "Подача: " + String(bmonState.t_supply, 1) + "°C\n";
            if (!isnan(bmonState.t_return))
                msg += "Обратка: " + String(bmonState.t_return, 1) + "°C\n";
            msg += "Мощность: " + String(bmonState.power_kw, 1) + " кВт\n";
            msg += "P1: " + String(bmonState.p_supply, 2) + " бар | ";
            msg += "P2: " + String(bmonState.p_return, 2) + " бар";
            TgHandler::sendMessage(msg, chat);
        }
        if (cmd == "/flow") {
            String msg = "💧 Расход:\n";
            msg += "Расходомер 1: " + String(bmonState.flow1_lpm, 2) + " л/мин\n";
            msg += "Расходомер 2: " + String(bmonState.flow2_lpm, 2) + " л/мин";
            TgHandler::sendMessage(msg, chat);
        }
    };
#endif

    Serial.println("[BMON] Device init OK");
}

// ============================================================
//  LOOP
// ============================================================
void BMonDevice::loop() {
    uint32_t now = millis();

    static uint32_t lastDbg = 0;
    if (millis() - lastDbg > 2000) {
        lastDbg = millis();
        Serial.printf("[BMON] GPIO2=%d GPIO4=%d\n", 
            digitalRead(BMON_PIN_FLOW1), digitalRead(BMON_PIN_FLOW2));
    }

    // Опрос температур и давления каждые 2 сек
    if (now - _lastRead > 2000) {
        _lastRead = now;
        readTemperatures();
        readPressure();
        checkAlarms();
    }

    // Расчёт расхода каждые 5 сек (считаем импульсы за интервал)
    if (now - _lastFlow > 5000) {
        calcFlow();
        _lastFlow = now;
    }

    // История мощности каждые 10 сек
    if (now - _lastHistory > 10000) {
        _lastHistory = now;
        bmonState.pushHistory(bmonState.power_kw);
        // Накопление энергии: P(кВт) × dt(ч)
        bmonState.energy_kwh += bmonState.power_kw * (10.0f / 3600.0f);
    }

    // MQTT публикация
#ifdef MODULE_MQTT
    if (now - _lastPub > bmonCfg.pub_interval) {
        _lastPub = now;
        publishMqtt();
    }
#endif

    // Перезагрузка
    xSemaphoreTake(coreMutex, portMAX_DELAY);
    bool reboot = sysState.pendingReboot && (now - sysState.rebootAt > 500);
    xSemaphoreGive(coreMutex);
    if (reboot) ESP.restart();
}

// ============================================================
//  ЧТЕНИЕ NTC ЧЕРЕЗ ADS1115
//  Схема: VCC — Rref — нода — NTC — GND
//  Vin на ADS = напряжение на NTC
//  R_ntc = Rref * Vin / (VCC - Vin)
//  T = 1 / (1/T0 + ln(R/R0)/B) - 273.15
// ============================================================
static float ntcToTemp(int16_t raw, float ntc_b) {
    if (raw <= 0) return NAN;  // обрыв или КЗ
    // ADS1115 с GAIN_ONE: 1 LSB = 0.125 мВ, макс 32767 = 4.096V
    // Но VCC шильда = 3.3V, значит реальный Vin ≤ 3.3V
    float vin  = raw * 0.000125f;           // вольты
    if (vin <= 0.05f || vin >= 3.25f) return NAN; // обрыв/КЗ
    float r_ntc = BMON_NTC_RREF * vin / (3.3f - vin);
    float lnR   = logf(r_ntc / BMON_NTC_R0);
    float tempK = 1.0f / (1.0f / BMON_NTC_T0 + lnR / ntc_b);
    return tempK - 273.15f;
}

void BMonDevice::readTemperatures() {
    if (!adsOk) {
        bmonState.alarm_sensor = true;
        return;
    }
    bmonState.alarm_sensor = false;

    int16_t raw;
    raw = ads.readADC_SingleEnded(3); bmonState.t_supply  = ntcToTemp(raw, bmonCfg.ntc_b); // NTC1
    raw = ads.readADC_SingleEnded(2); bmonState.t_return  = ntcToTemp(raw, bmonCfg.ntc_b); // NTC2
    raw = ads.readADC_SingleEnded(1); bmonState.t_room    = ntcToTemp(raw, bmonCfg.ntc_b); // NTC3
    raw = ads.readADC_SingleEnded(0); bmonState.t_outdoor = ntcToTemp(raw, bmonCfg.ntc_b); // NTC4
      
    // Дельта только если оба датчика живые
    if (!isnan(bmonState.t_supply) && !isnan(bmonState.t_return))
        bmonState.t_delta = bmonState.t_supply - bmonState.t_return;
    else
        bmonState.t_delta = 0.0f;
}

// ============================================================
//  ЧТЕНИЕ ДАВЛЕНИЯ 4-20мА / 150 Ом
//  U = I × 150; I_min=4мА→0.6V, I_max=20мА→3.0V
//  P = (U - U_min) / (U_max - U_min) × P_max
// ============================================================
static float adcToPressure(int rawAdc) {
    float u = rawAdc / BMON_ADC_BITS * BMON_ADC_VREF;
    if (u < BMON_PRESS_U_MIN) return 0.0f;  // ниже 4мА — обрыв или 0
    float p = (u - BMON_PRESS_U_MIN) / (BMON_PRESS_U_MAX - BMON_PRESS_U_MIN) * BMON_PRESS_BAR_MAX;
    if (p < 0.0f) p = 0.0f;
    if (p > BMON_PRESS_BAR_MAX) p = BMON_PRESS_BAR_MAX;
    return p;
}

void BMonDevice::readPressure() {
    // Среднее из нескольких замеров для фильтрации шума АЦП ESP32
    const int N = 8;
    int32_t sum1 = 0, sum2 = 0;
    for (int i = 0; i < N; i++) {
        sum1 += analogRead(BMON_PIN_PRESS1);
        sum2 += analogRead(BMON_PIN_PRESS2);
    }
    bmonState.p_supply = adcToPressure(sum1 / N);
    bmonState.p_return = adcToPressure(sum2 / N);
    bmonState.p_delta  = bmonState.p_supply - bmonState.p_return;
}

// ============================================================
//  РАСЧЁТ РАСХОДА
//  Вызывается каждые 5 сек, считает импульсы за интервал
// ============================================================
void BMonDevice::calcFlow() {
    const float dt_min = 5.0f / 60.0f;  // 5 сек в минутах

    // Атомарно читаем счётчики (ISR пишет volatile)
    noInterrupts();
    uint32_t p1 = bmonState.pulse1_count;
    uint32_t p2 = bmonState.pulse2_count;
    interrupts();

    uint32_t diff1 = p1 - _pulse1_prev;
    uint32_t diff2 = p2 - _pulse2_prev;
    _pulse1_prev = p1;
    _pulse2_prev = p2;

    // л/мин = (импульсы × л/имп) / dt_мин
    bmonState.flow1_lpm = (diff1 * bmonCfg.flow1_lpi) / dt_min;
    bmonState.flow2_lpm = (diff2 * bmonCfg.flow2_lpi) / dt_min;

    // Тепловая мощность по расходомеру 1:
    // P(кВт) = m_dot(кг/с) × Cp(кДж/кг·К) × ΔT(К)
    // m_dot = flow_lpm / 60 (кг/с при плотности воды 1 кг/л)
    float m_dot = bmonState.flow1_lpm / 60.0f;
    if (!isnan(bmonState.t_delta) && bmonState.t_delta > 0)
        bmonState.power_kw = m_dot * 4.186f * bmonState.t_delta;
    else
        bmonState.power_kw = 0.0f;
}

// ============================================================
//  АВАРИИ
// ============================================================
void BMonDevice::checkAlarms() {
    bool wasHigh = bmonState.alarm_high;
    bool wasLow  = bmonState.alarm_low;

    bmonState.alarm_high = !isnan(bmonState.t_supply) &&
                            bmonState.t_supply > bmonCfg.t_max;

    bmonState.alarm_low  = !isnan(bmonState.t_outdoor) &&
                            bmonState.t_outdoor < bmonCfg.t_min;

#ifdef MODULE_TELEGRAM
    if (bmonState.alarm_high && !wasHigh) {
        TgHandler::sendMessage(
            "🚨 *АВАРИЯ*: Температура подачи " +
            String(bmonState.t_supply, 1) + "°C > " +
            String(bmonCfg.t_max, 1) + "°C!"
        );
    }
    if (!bmonState.alarm_high && wasHigh) {
        TgHandler::sendMessage("✅ Температура подачи в норме");
    }
    if (bmonState.alarm_low && !wasLow) {
        TgHandler::sendMessage(
            "❄️ *ВНИМАНИЕ*: Температура улицы " +
            String(bmonState.t_outdoor, 1) + "°C — риск замерзания!"
        );
    }
    if (bmonState.alarm_sensor && !wasHigh) {  // используем флаг для дебаунса
        TgHandler::sendMessage("⚠️ Ошибка ADS1115 — проверьте датчики");
    }
#endif
}

// ============================================================
//  MQTT
// ============================================================
void BMonDevice::publishMqtt() {
#ifdef MODULE_MQTT
    String base = String(baseCfg.device_name) + "/";
    char buf[12];

    auto pub = [&](const char* topic, float val, int dec = 1) {
        if (!isnan(val)) {
            dtostrf(val, 1, dec, buf);
            MqttHandler::publish((base + topic).c_str(), buf);
        }
    };

    pub("t_supply",  bmonState.t_supply);
    pub("t_return",  bmonState.t_return);
    pub("t_room",    bmonState.t_room);
    pub("t_outdoor", bmonState.t_outdoor);
    pub("t_delta",   bmonState.t_delta);
    pub("p_supply",  bmonState.p_supply, 2);
    pub("p_return",  bmonState.p_return, 2);
    pub("flow1_lpm", bmonState.flow1_lpm, 2);
    pub("flow2_lpm", bmonState.flow2_lpm, 2);
    pub("power_kw",  bmonState.power_kw, 2);
#endif
}

// ============================================================
//  КОНФИГ
// ============================================================
bool BMonDevice::loadConfig() {
    JsonDocument doc;
    if (!ConfigManager::loadJson("/device.json", doc)) return false;
    bmonCfg.t_max        = doc["t_max"]        | 85.0f;
    bmonCfg.t_alarm      = doc["t_alarm"]      | 80.0f;
    bmonCfg.t_min        = doc["t_min"]        | 5.0f;
    bmonCfg.flow1_lpi    = doc["flow1_lpi"]    | 1.0f;
    bmonCfg.flow2_lpi    = doc["flow2_lpi"]    | 1.0f;
    bmonCfg.pub_interval = doc["pub_interval"] | 10000;
    bmonCfg.ntc_b        = doc["ntc_b"]        | BMON_NTC_B;
    return true;
}

bool BMonDevice::saveConfig() {
    JsonDocument doc;
    doc["t_max"]        = bmonCfg.t_max;
    doc["t_alarm"]      = bmonCfg.t_alarm;
    doc["t_min"]        = bmonCfg.t_min;
    doc["flow1_lpi"]    = bmonCfg.flow1_lpi;
    doc["flow2_lpi"]    = bmonCfg.flow2_lpi;
    doc["pub_interval"] = bmonCfg.pub_interval;
    doc["ntc_b"]        = bmonCfg.ntc_b;
    return ConfigManager::saveJson("/device.json", doc);
}

#endif // DEVICE_BOILER_MONITOR