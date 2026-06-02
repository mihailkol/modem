#if defined(DEVICE_KC868_A16) && defined(APP_BOILER_ROOM)

#include "BoilerRoomDevice.h"
#include "../../core/ConfigManager.h"
#include "../../core/WebHandler.h"
#include "../../devices/kc868-a16/A16Types.h"
#include <AsyncJson.h>
#include <ArduinoJson.h>

#ifdef MODULE_MQTT
#include "../../modules/mqtt/MqttHandler.h"
#endif

#ifdef DEVICE_EKONOM
#include "../../devices/ekonom/EkoNomDevice.h"
#endif

// ── Глобальные данные ─────────────────────────────────────────────────────────
BoilerRoomConfig brrCfg;
BoilerRoomState  brrState;

uint32_t BoilerRoomDevice::_lastCalc = 0;
uint32_t BoilerRoomDevice::_lastPub  = 0;

// ============================================================
//  HTML — вкладка мониторинга
// ============================================================
static const char BRR_MONITOR_HTML[] PROGMEM = R"html(
<style>
.brr-page { display:flex; flex-direction:column; gap:10px; }

.brr-card {
  background: var(--card);
  border-radius: 10px;
  padding: 16px;
  border: 1px solid var(--border);
}
.brr-label {
  font-size: 10px; font-weight: 700;
  letter-spacing: .12em; text-transform: uppercase;
  color: var(--muted); margin-bottom: 12px;
}

/* Сетка 2 колонки */
.brr-grid2 {
  display: grid;
  grid-template-columns: 1fr 1fr;
  gap: 8px;
}
.brr-cell {
  background: #111318;
  border: 1px solid var(--border);
  border-radius: 8px;
  padding: 10px 12px;
}
.brr-cell-lbl {
  font-size: 10px; color: var(--muted);
  margin-bottom: 4px;
}
.brr-val {
  font-size: 22px; font-weight: 800; color: var(--accent);
  line-height: 1.1;
}
.brr-val.cool  { color: var(--cool); }
.brr-val.hot   { color: var(--hot); }
.brr-val.ok    { color: var(--ok); }
.brr-unit {
  font-size: 11px; color: var(--muted); margin-left: 2px;
}

/* Большой показатель — мощность теплосчётчика */
.brr-big {
  text-align: center; padding: 4px 0 8px;
}
.brr-big-val {
  font-size: 52px; font-weight: 900;
  color: var(--accent); line-height: 1;
}
.brr-big-unit {
  font-size: 14px; color: var(--muted);
}
.brr-big-sub {
  font-size: 12px; color: var(--muted);
  margin-top: 6px;
}

/* Разделительная строка внутри карточки */
.brr-row {
  display: flex; justify-content: space-between;
  align-items: baseline;
  padding: 5px 0;
  border-bottom: 1px solid var(--border);
  font-size: 13px;
}
.brr-row:last-child { border-bottom: none; }
.brr-row-lbl { color: var(--muted); }
.brr-row-val { font-weight: 700; }

/* Статус свежести данных */
.brr-age {
  font-size: 11px; color: var(--muted);
  text-align: right; margin-top: 8px;
}
.brr-age.stale { color: var(--danger); }
</style>

<div class="brr-page">

  <!-- Теплосчётчик: мощность крупно -->
  <div class="brr-card">
    <div class="brr-label">🔥 ТЕПЛОВАЯ ЭНЕРГИЯ</div>
    <div class="brr-big">
      <div>
        <span class="brr-big-val" id="brr_power">--</span>
        <span class="brr-big-unit"> кВт</span>
      </div>
      <div class="brr-big-sub">
        Накоплено: <b id="brr_energy">--</b> кВт·ч
        &nbsp;|&nbsp;
        Объём: <b id="brr_vol">--</b> м³
      </div>
    </div>

    <div class="brr-grid2" style="margin-top:10px">
      <div class="brr-cell">
        <div class="brr-cell-lbl">Подача</div>
        <div><span class="brr-val hot" id="brr_ts">--</span><span class="brr-unit">°C</span></div>
      </div>
      <div class="brr-cell">
        <div class="brr-cell-lbl">Обратка</div>
        <div><span class="brr-val cool" id="brr_tr">--</span><span class="brr-unit">°C</span></div>
      </div>
      <div class="brr-cell">
        <div class="brr-cell-lbl">Δ температур</div>
        <div><span class="brr-val" id="brr_dt">--</span><span class="brr-unit">°C</span></div>
      </div>
      <div class="brr-cell">
        <div class="brr-cell-lbl">Расход тепл.</div>
        <div><span class="brr-val" id="brr_flow">--</span><span class="brr-unit">м³/ч</span></div>
      </div>
    </div>
    <div class="brr-age" id="brr_heat_age">—</div>
  </div>

  <!-- Давление -->
  <div class="brr-card">
    <div class="brr-label">🌡 ДАВЛЕНИЕ</div>
    <div class="brr-grid2">
      <div class="brr-cell">
        <div class="brr-cell-lbl">Отопление</div>
        <div><span class="brr-val ok" id="brr_ph">--</span><span class="brr-unit">бар</span></div>
      </div>
      <div class="brr-cell">
        <div class="brr-cell-lbl">Вода ХВС</div>
        <div><span class="brr-val ok" id="brr_pw">--</span><span class="brr-unit">бар</span></div>
      </div>
    </div>
  </div>

  <!-- Счётчики -->
  <div class="brr-card">
    <div class="brr-label">💧 ВОДОСНАБЖЕНИЕ</div>
    <div class="brr-row">
      <span class="brr-row-lbl">Расход</span>
      <span class="brr-row-val"><span id="brr_wlpm">--</span> л/мин</span>
    </div>
    <div class="brr-row">
      <span class="brr-row-lbl">Накоплено</span>
      <span class="brr-row-val"><span id="brr_wtotal">--</span> м³</span>
    </div>
  </div>

  <div class="brr-card">
    <div class="brr-label">🔵 ГАЗ</div>
    <div class="brr-row">
      <span class="brr-row-lbl">Расход</span>
      <span class="brr-row-val"><span id="brr_gm3h">--</span> м³/ч</span>
    </div>
    <div class="brr-row">
      <span class="brr-row-lbl">Накоплено</span>
      <span class="brr-row-val"><span id="brr_gtotal">--</span> м³</span>
    </div>
  </div>

</div>

<script>
function fmt(v, dec) {
  return (v === null || v === undefined || isNaN(v)) ? '—' : Number(v).toFixed(dec);
}

async function brrUpdate() {
  try {
    const d = await (await fetch('/api/brr/data')).json();

    // Теплосчётчик
    document.getElementById('brr_power').textContent  = fmt(d.heat_power_kw, 1);
    document.getElementById('brr_energy').textContent = fmt(d.heat_energy_kwh, 1);
    document.getElementById('brr_vol').textContent    = fmt(d.heat_volume_m3, 2);
    document.getElementById('brr_ts').textContent     = fmt(d.heat_t_supply, 1);
    document.getElementById('brr_tr').textContent     = fmt(d.heat_t_return, 1);
    document.getElementById('brr_dt').textContent     = fmt(d.heat_t_delta, 1);
    document.getElementById('brr_flow').textContent   = fmt(d.heat_flow_m3h, 3);

    // Возраст данных теплосчётчика
    const ageEl = document.getElementById('brr_heat_age');
    if (d.heat_valid && d.heat_age_sec !== undefined) {
      const age = d.heat_age_sec;
      ageEl.textContent = 'Обновлено ' + age + ' с назад';
      ageEl.className   = 'brr-age' + (age > 120 ? ' stale' : '');
    } else {
      ageEl.textContent = 'Нет данных с теплосчётчика';
      ageEl.className   = 'brr-age stale';
    }

    // Давление
    document.getElementById('brr_ph').textContent = fmt(d.p_heat, 2);
    document.getElementById('brr_pw').textContent = fmt(d.p_water, 2);

    // Вода
    document.getElementById('brr_wlpm').textContent   = fmt(d.water_rate_lpm, 2);
    document.getElementById('brr_wtotal').textContent = fmt(d.water_total_m3, 3);

    // Газ
    document.getElementById('brr_gm3h').textContent   = fmt(d.gas_rate_m3h, 3);
    document.getElementById('brr_gtotal').textContent = fmt(d.gas_total_m3, 3);

  } catch(e) {}
}

brrUpdate();
setInterval(brrUpdate, 3000);
</script>
)html";

// ============================================================
//  HTML — вкладка настроек
// ============================================================
static const char BRR_SETTINGS_HTML[] PROGMEM = R"html(
<style>
.brs-card {
  background: var(--card); border-radius: 10px;
  padding: 16px; border: 1px solid var(--border);
  margin-bottom: 10px;
}
.brs-label {
  font-size: 10px; font-weight: 700;
  letter-spacing: .12em; text-transform: uppercase;
  color: var(--muted); margin-bottom: 12px;
}
.brs-row {
  display: flex; align-items: center;
  justify-content: space-between;
  color: var(--text); font-size: 14px;
  margin-bottom: 10px;
}
.brs-row input {
  width: 110px; background: #111318;
  border: 1px solid var(--border); border-radius: 6px;
  padding: 6px 8px; color: var(--text); font-size: 13px;
  text-align: right;
}
</style>

<div class="brs-card">
  <div class="brs-label">⚙️ КОЭФФИЦИЕНТЫ СЧЁТЧИКОВ</div>
  <div class="brs-row">
    <span>Вода, м³/импульс</span>
    <input type="number" id="water_kf" step="0.001" min="0.001">
  </div>
  <div class="brs-row">
    <span>Газ, м³/импульс</span>
    <input type="number" id="gas_kf" step="0.001" min="0.001">
  </div>
</div>

<div class="brs-card">
  <div class="brs-label">⚙️ ДАВЛЕНИЕ ОТОПЛЕНИЯ (4-20мА)</div>
  <div class="brs-row">
    <span>4мА = 0 бар (min)</span>
    <input type="number" id="ph_min" step="0.1">
  </div>
  <div class="brs-row">
    <span>20мА = N бар (max)</span>
    <input type="number" id="ph_max" step="0.1">
  </div>
</div>

<div class="brs-card">
  <div class="brs-label">⚙️ ДАВЛЕНИЕ ВОДЫ ХВС (4-20мА)</div>
  <div class="brs-row">
    <span>4мА = 0 бар (min)</span>
    <input type="number" id="pw_min" step="0.1">
  </div>
  <div class="brs-row">
    <span>20мА = N бар (max)</span>
    <input type="number" id="pw_max" step="0.1">
  </div>
</div>

<div class="brs-card">
  <div class="brs-label">⚙️ ОБЩИЕ</div>
  <div class="brs-row">
    <span>Интервал MQTT, сек</span>
    <input type="number" id="pub_interval" step="1" min="5">
  </div>
</div>

<button onclick="brsSave()">💾 Сохранить</button>
<div class="info-row" id="brs_msg"></div>

<script>
async function brsLoad() {
  const d = await (await fetch('/api/brr/config')).json();
  document.getElementById('water_kf').value    = d.water_m3_per_pulse;
  document.getElementById('gas_kf').value      = d.gas_m3_per_pulse;
  document.getElementById('ph_min').value      = d.p_heat_min;
  document.getElementById('ph_max').value      = d.p_heat_max;
  document.getElementById('pw_min').value      = d.p_water_min;
  document.getElementById('pw_max').value      = d.p_water_max;
  document.getElementById('pub_interval').value = Math.round(d.pub_interval / 1000);
}

async function brsSave() {
  const body = {
    water_m3_per_pulse: +document.getElementById('water_kf').value,
    gas_m3_per_pulse:   +document.getElementById('gas_kf').value,
    p_heat_min:         +document.getElementById('ph_min').value,
    p_heat_max:         +document.getElementById('ph_max').value,
    p_water_min:        +document.getElementById('pw_min').value,
    p_water_max:        +document.getElementById('pw_max').value,
    pub_interval:       +document.getElementById('pub_interval').value * 1000,
  };
  const r = await fetch('/api/brr/save', {
    method: 'POST',
    headers: {'Content-Type': 'application/json'},
    body: JSON.stringify(body)
  });
  document.getElementById('brs_msg').textContent = r.ok ? '✅ Сохранено' : '❌ Ошибка';
  setTimeout(() => document.getElementById('brs_msg').textContent = '', 3000);
}

brsLoad();
</script>
)html";

// ============================================================
//  INIT
// ============================================================
void BoilerRoomDevice::init() {
    loadConfig();

    // Регистрируем вкладки
    WebHandler::registerTab({"monitor", "Мониторинг", "🏠", BRR_MONITOR_HTML});
    WebHandler::registerTab({"brr_cfg", "Уставки",    "⚙️",  BRR_SETTINGS_HTML});

    extern AsyncWebServer server;

    // ── /api/brr/data — данные мониторинга ───────────────────
    server.on("/api/brr/data", HTTP_GET, [](AsyncWebServerRequest* req) {
        JsonDocument doc;

        // Теплосчётчик
        doc["heat_t_supply"]   = brrState.heat_t_supply;
        doc["heat_t_return"]   = brrState.heat_t_return;
        doc["heat_t_delta"]    = brrState.heat_t_delta;
        doc["heat_flow_m3h"]   = brrState.heat_flow_m3h;
        doc["heat_power_kw"]   = brrState.heat_power_kw;
        doc["heat_energy_kwh"] = brrState.heat_energy_kwh;
        doc["heat_volume_m3"]  = brrState.heat_volume_m3;
        doc["heat_valid"]      = brrState.heat_valid;
        if (brrState.heat_valid && brrState.heat_updated_ms > 0)
            doc["heat_age_sec"] = (millis() - brrState.heat_updated_ms) / 1000;

        // Давление
        doc["p_heat"]  = brrState.p_heat;
        doc["p_water"] = brrState.p_water;

        // Вода
        doc["water_rate_lpm"]  = brrState.water_rate_lpm;
        doc["water_total_m3"]  = brrState.water_total_m3;

        // Газ
        doc["gas_rate_m3h"]  = brrState.gas_rate_m3h;
        doc["gas_total_m3"]  = brrState.gas_total_m3;

        String out; serializeJson(doc, out);
        req->send(200, "application/json", out);
    });

    // ── /api/brr/config — чтение конфига ─────────────────────
    server.on("/api/brr/config", HTTP_GET, [](AsyncWebServerRequest* req) {
        JsonDocument doc;
        doc["water_m3_per_pulse"] = brrCfg.water_m3_per_pulse;
        doc["gas_m3_per_pulse"]   = brrCfg.gas_m3_per_pulse;
        doc["p_heat_min"]         = brrCfg.p_heat_min;
        doc["p_heat_max"]         = brrCfg.p_heat_max;
        doc["p_water_min"]        = brrCfg.p_water_min;
        doc["p_water_max"]        = brrCfg.p_water_max;
        doc["pub_interval"]       = brrCfg.pub_interval;
        String out; serializeJson(doc, out);
        req->send(200, "application/json", out);
    });

    // ── /api/brr/save — сохранение конфига ───────────────────
    AsyncCallbackJsonWebHandler* saveHandler =
        new AsyncCallbackJsonWebHandler("/api/brr/save",
            [](AsyncWebServerRequest* req, JsonVariant& json) {
                JsonObject obj = json.as<JsonObject>();
                brrCfg.water_m3_per_pulse = obj["water_m3_per_pulse"] | brrCfg.water_m3_per_pulse;
                brrCfg.gas_m3_per_pulse   = obj["gas_m3_per_pulse"]   | brrCfg.gas_m3_per_pulse;
                brrCfg.p_heat_min         = obj["p_heat_min"]         | brrCfg.p_heat_min;
                brrCfg.p_heat_max         = obj["p_heat_max"]         | brrCfg.p_heat_max;
                brrCfg.p_water_min        = obj["p_water_min"]        | brrCfg.p_water_min;
                brrCfg.p_water_max        = obj["p_water_max"]        | brrCfg.p_water_max;
                brrCfg.pub_interval       = obj["pub_interval"]       | brrCfg.pub_interval;
                BoilerRoomDevice::saveConfig();
                req->send(200, "application/json", "{\"ok\":true}");
            });
    server.addHandler(saveHandler);

    Serial.println("[BRR] Init OK");
}

// ============================================================
//  LOOP
// ============================================================
void BoilerRoomDevice::loop() {
    uint32_t now = millis();

    // Каждые 2 секунды: опрос A16 + пересчёт расходов
    if (now - _lastCalc >= 2000) {
        uint32_t dt_ms = now - _lastCalc;
        _lastCalc = now;
        _updateFromA16();
        _updateFromEkoNom();
        _calcRates(dt_ms);
    }

    // MQTT публикация
#ifdef MODULE_MQTT
    if (now - _lastPub >= brrCfg.pub_interval) {
        _lastPub = now;
        _publishMqtt();
    }
#endif
}

// ============================================================
//  ДАННЫЕ ИЗ A16
// ============================================================
void BoilerRoomDevice::_updateFromA16() {
    extern A16State a16State;
    extern A16Config a16Cfg;

    // ── Давление (4-20мА → бар) ──────────────────────────────
    // adc_value уже масштабирован A16Device через adc_min/adc_max из конфига.
    // Но у нас свои названия диапазонов — берём напрямую из adc_value,
    // предполагая что в A16Config выставлены верные adc_min/adc_max.
    brrState.p_heat  = a16State.adc_value[BRR_ADC_P_HEAT];
    brrState.p_water = a16State.adc_value[BRR_ADC_P_WATER];

    // ── Счётчики импульсов ────────────────────────────────────
    brrState.water_pulses = a16State.din_counter[BRR_DIN_WATER];
    brrState.gas_pulses   = a16State.din_counter[BRR_DIN_GAS];
}

// ============================================================
//  ДАННЫЕ ИЗ ТЕПЛОСЧЁТЧИКА
// ============================================================
void BoilerRoomDevice::_updateFromEkoNom() {
#ifdef DEVICE_EKONOM
    extern EkoNomData ekoNomData;
    if (!ekoNomData.valid) return;

    brrState.heat_t_supply   = ekoNomData.t_supply;
    brrState.heat_t_return   = ekoNomData.t_return;
    brrState.heat_t_delta    = ekoNomData.t_delta;
    brrState.heat_flow_m3h   = ekoNomData.flow_m3h;
    brrState.heat_power_kw   = ekoNomData.power_kw;
    brrState.heat_energy_kwh = ekoNomData.energy_kwh;
    brrState.heat_volume_m3  = ekoNomData.volume_m3;
    brrState.heat_valid      = true;
    brrState.heat_updated_ms = ekoNomData.updated_ms;
#endif
}

// ============================================================
//  РАСЧЁТ РАСХОДОВ ПО СЧЁТЧИКАМ
// ============================================================
void BoilerRoomDevice::_calcRates(uint32_t dt_ms) {
    float dt_min = dt_ms / 60000.0f;
    float dt_h   = dt_ms / 3600000.0f;

    // Вода
    uint32_t w_delta = brrState.water_pulses - brrState.water_pulses_prev;
    brrState.water_pulses_prev = brrState.water_pulses;
    brrState.water_total_m3   += w_delta * brrCfg.water_m3_per_pulse;
    brrState.water_rate_lpm    = (dt_min > 0)
        ? (w_delta * brrCfg.water_m3_per_pulse * 1000.0f) / dt_min
        : 0.0f;

    // Газ
    uint32_t g_delta = brrState.gas_pulses - brrState.gas_pulses_prev;
    brrState.gas_pulses_prev = brrState.gas_pulses;
    brrState.gas_total_m3   += g_delta * brrCfg.gas_m3_per_pulse;
    brrState.gas_rate_m3h    = (dt_h > 0)
        ? (g_delta * brrCfg.gas_m3_per_pulse) / dt_h
        : 0.0f;
}

// ============================================================
//  MQTT ПУБЛИКАЦИЯ
// ============================================================
void BoilerRoomDevice::_publishMqtt() {
#ifdef MODULE_MQTT
    String b = String(baseCfg.device_name) + "/brr/";
    char buf[16];

    // Теплосчётчик
    if (brrState.heat_valid) {
        dtostrf(brrState.heat_t_supply,   1, 1, buf); MqttHandler::publish((b+"heat/t_supply").c_str(),   buf);
        dtostrf(brrState.heat_t_return,   1, 1, buf); MqttHandler::publish((b+"heat/t_return").c_str(),   buf);
        dtostrf(brrState.heat_t_delta,    1, 1, buf); MqttHandler::publish((b+"heat/t_delta").c_str(),    buf);
        dtostrf(brrState.heat_flow_m3h,   1, 4, buf); MqttHandler::publish((b+"heat/flow_m3h").c_str(),   buf);
        dtostrf(brrState.heat_power_kw,   1, 2, buf); MqttHandler::publish((b+"heat/power_kw").c_str(),   buf);
        dtostrf(brrState.heat_energy_kwh, 1, 1, buf); MqttHandler::publish((b+"heat/energy_kwh").c_str(), buf);
        dtostrf(brrState.heat_volume_m3,  1, 2, buf); MqttHandler::publish((b+"heat/volume_m3").c_str(),  buf);
    }

    // Давление
    dtostrf(brrState.p_heat,  1, 2, buf); MqttHandler::publish((b+"p_heat").c_str(),  buf);
    dtostrf(brrState.p_water, 1, 2, buf); MqttHandler::publish((b+"p_water").c_str(), buf);

    // Вода
    dtostrf(brrState.water_rate_lpm, 1, 2, buf); MqttHandler::publish((b+"water/rate_lpm").c_str(),  buf);
    dtostrf(brrState.water_total_m3, 1, 3, buf); MqttHandler::publish((b+"water/total_m3").c_str(),  buf);

    // Газ
    dtostrf(brrState.gas_rate_m3h, 1, 3, buf); MqttHandler::publish((b+"gas/rate_m3h").c_str(),  buf);
    dtostrf(brrState.gas_total_m3, 1, 3, buf); MqttHandler::publish((b+"gas/total_m3").c_str(),   buf);
#endif
}

// ============================================================
//  CONFIG
// ============================================================
bool BoilerRoomDevice::loadConfig() {
    JsonDocument doc;
    if (!ConfigManager::loadJson("/boilerroom.json", doc)) return false;
    brrCfg.water_m3_per_pulse = doc["water_m3_per_pulse"] | 0.01f;
    brrCfg.gas_m3_per_pulse   = doc["gas_m3_per_pulse"]   | 0.01f;
    brrCfg.p_heat_min         = doc["p_heat_min"]         | 0.0f;
    brrCfg.p_heat_max         = doc["p_heat_max"]         | 10.0f;
    brrCfg.p_water_min        = doc["p_water_min"]        | 0.0f;
    brrCfg.p_water_max        = doc["p_water_max"]        | 10.0f;
    brrCfg.pub_interval       = doc["pub_interval"]       | 10000;
    return true;
}

bool BoilerRoomDevice::saveConfig() {
    JsonDocument doc;
    doc["water_m3_per_pulse"] = brrCfg.water_m3_per_pulse;
    doc["gas_m3_per_pulse"]   = brrCfg.gas_m3_per_pulse;
    doc["p_heat_min"]         = brrCfg.p_heat_min;
    doc["p_heat_max"]         = brrCfg.p_heat_max;
    doc["p_water_min"]        = brrCfg.p_water_min;
    doc["p_water_max"]        = brrCfg.p_water_max;
    doc["pub_interval"]       = brrCfg.pub_interval;
    return ConfigManager::saveJson("/boilerroom.json", doc);
}

#endif // DEVICE_KC868_A16 && APP_BOILER_ROOM