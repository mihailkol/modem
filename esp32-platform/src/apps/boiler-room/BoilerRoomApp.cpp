#ifdef APP_BOILER_ROOM
#include "BoilerRoomApp.h"
#include "ConfigManager.h"
#include "WebHandler.h"
#include "Bus.h"
#include "Version.h"
#include <ArduinoJson.h>
#include <AsyncJson.h>
#include <WiFi.h>
#include <ETH.h>

BoilerRoomConfig brrCfg;

uint32_t BoilerRoomApp::_lastCalcMs        = 0;
uint32_t BoilerRoomApp::_lastSysMs         = 0;
uint32_t BoilerRoomApp::_lastCounterSaveMs = 0;
uint32_t BoilerRoomApp::_lastWaterPulses   = 0;
uint32_t BoilerRoomApp::_lastGasPulses     = 0;
double   BoilerRoomApp::_waterTotalM3      = 0;
double   BoilerRoomApp::_gasTotalM3        = 0;
bool     BoilerRoomApp::_countersDirty     = false;

uint32_t BoilerRoomApp::_waterWindowStartMs        = 0;
uint32_t BoilerRoomApp::_waterPulsesAtWindowStart  = 0;
uint32_t BoilerRoomApp::_gasWindowStartMs          = 0;
uint32_t BoilerRoomApp::_gasPulsesAtWindowStart    = 0;

static const uint32_t COUNTER_SAVE_INTERVAL_MS = 5UL * 60UL * 1000UL;  // раз в 5 минут

static const char BRR_TAB_HTML[] PROGMEM = R"html(
<style>
.brr-row{display:flex;gap:8px}
.brr-row > div{flex:1;min-width:0}
.brr-sublabel{display:block;font-size:11px;color:var(--muted);margin-bottom:4px}
</style>
<div class="settings-group">
  <h3>🔧 Калибровки котельной</h3>

  <label>Счётчик воды</label>
  <div class="brr-row">
    <div><span class="brr-sublabel">м³/импульс</span><input type="number" step="0.0001" name="water_m3_per_pulse"></div>
    <div><span class="brr-sublabel">Усреднение расхода, сек</span><input type="number" step="1" name="water_rate_avg_sec"></div>
  </div>

  <label>Счётчик газа</label>
  <div class="brr-row">
    <div><span class="brr-sublabel">м³/импульс</span><input type="number" step="0.0001" name="gas_m3_per_pulse"></div>
    <div><span class="brr-sublabel">Усреднение расхода, сек</span><input type="number" step="1" name="gas_rate_avg_sec"></div>
  </div>

  <label>Давление отопления, бар (при 4мА / при 20мА)</label>
  <div class="brr-row">
    <div><input type="number" step="0.1" name="p_heat_min" placeholder="4мА"></div>
    <div><input type="number" step="0.1" name="p_heat_max" placeholder="20мА"></div>
  </div>

  <label>Давление ХВС, бар (при 4мА / при 20мА)</label>
  <div class="brr-row">
    <div><input type="number" step="0.1" name="p_water_min" placeholder="4мА"></div>
    <div><input type="number" step="0.1" name="p_water_max" placeholder="20мА"></div>
  </div>

  <label>Калибровка ADC A16 (общая для обоих 4-20мА входов) — raw-код при 4мА / при 20мА</label>
  <div class="brr-row">
    <div><input type="number" step="1" name="adc_raw_at_4ma" placeholder="4мА"></div>
    <div><input type="number" step="1" name="adc_raw_at_20ma" placeholder="20мА"></div>
  </div>

  <button onclick="saveBrr()">💾 Сохранить калибровки</button>
</div>
<div class="settings-group">
  <h3>🌡️ Термисторы контуров (NTC)</h3>
  <div class="info-row" style="margin-bottom:8px">
    Одна калибровка термистора на все 4 точки (считаем термисторы
    одинаковыми). Термисторов физически ещё нет — ниже типовые значения
    для 10к/B3950, поправить по даташиту, когда датчики появятся.
  </div>
  <label>Термистор — R0 (Ом при 25°C) / Beta (K)</label>
  <div class="brr-row">
    <div><input type="number" step="1" name="ntc_r0_ohm" placeholder="R0, Ом"></div>
    <div><input type="number" step="1" name="ntc_beta" placeholder="Beta, K"></div>
  </div>
  <label>Подтягивающий резистор делителя, Ом</label>
  <input type="number" step="1" name="ntc_pullup_ohm">
  <label>Привязка точек — индекс raw.ntc{N}_adc (см. вкладку Мониторинг, группа "Прочее (raw)"). -1 = не подключен.</label>
  <div class="brr-row">
    <div><span class="brr-sublabel">Контур 1, подача</span><input type="number" step="1" name="ntc_c1_supply_index"></div>
    <div><span class="brr-sublabel">Контур 1, обратка</span><input type="number" step="1" name="ntc_c1_return_index"></div>
  </div>
  <div class="brr-row">
    <div><span class="brr-sublabel">Контур 2, подача</span><input type="number" step="1" name="ntc_c2_supply_index"></div>
    <div><span class="brr-sublabel">Контур 2, обратка</span><input type="number" step="1" name="ntc_c2_return_index"></div>
  </div>
  <button onclick="saveBrr()">💾 Сохранить</button>
</div>
<div class="settings-group">
  <h3>🔢 Текущие показания счётчиков</h3>
  <div class="info-row" style="margin-bottom:8px">
    Наращиваются автоматически по импульсам. Задать вручную нужно один раз
    при вводе в эксплуатацию (физический счётчик уже не на нуле) или для
    коррекции. Автосохранение — раз в 5 минут, при перезагрузке в этом окне
    возможна потеря последних минут накопления.
  </div>
  <label>Вода, текущие показания (м³)</label>
  <input type="number" step="0.001" id="water_reading_input">
  <button onclick="setReading('water')">Задать показания воды</button>
  <label>Газ, текущие показания (м³)</label>
  <input type="number" step="0.001" id="gas_reading_input">
  <button onclick="setReading('gas')">Задать показания газа</button>
</div>
<div class="settings-group">
  <h3>🌡 Привязка 1-Wire датчиков</h3>
  <div class="info-row" style="margin-bottom:8px">
    Индекс = порядок обнаружения при сканировании шины A16 (см. вкладку
    Мониторинг — там видно raw.ow0_temp, raw.ow1_temp...). -1 = не подключен.
    Сканирование — один раз при старте; если подключил датчик уже после
    загрузки, жми "Пересканировать" вместо перезагрузки платы.
  </div>
  <button onclick="rescanOw()" class="btn-secondary">🔄 Пересканировать 1-Wire</button>
  <label>Индекс уличного датчика (t_outdoor)</label>
  <input type="number" step="1" name="ow_outdoor_index">
  <label>Индекс комнатного датчика (t_indoor)</label>
  <input type="number" step="1" name="ow_indoor_index">
  <button onclick="saveBrr()">💾 Сохранить</button>
</div>
<script>
async function loadBrr() {
  const d = await (await fetch('/api/boilerroom/config')).json();
  Object.keys(d).forEach(k => {
    const el = document.querySelector('[name="'+k+'"]');
    if (el) el.value = d[k];
  });
}
async function saveBrr() {
  const fields = ['water_m3_per_pulse','water_rate_avg_sec','gas_m3_per_pulse','gas_rate_avg_sec',
                   'p_heat_min','p_heat_max','p_water_min','p_water_max',
                   'adc_raw_at_4ma','adc_raw_at_20ma','ow_outdoor_index','ow_indoor_index',
                   'ntc_r0_ohm','ntc_beta','ntc_pullup_ohm',
                   'ntc_c1_supply_index','ntc_c1_return_index','ntc_c2_supply_index','ntc_c2_return_index'];
  const data = {};
  fields.forEach(k => data[k] = parseFloat(document.querySelector('[name="'+k+'"]').value));
  const r = await fetch('/api/boilerroom/save', {method:'POST',
    headers:{'Content-Type':'application/json'}, body: JSON.stringify(data)});
  if (r.ok) alert('Сохранено');
}
async function loadReadings() {
  try {
    const list = await (await fetch('/api/channels')).json();
    const w = list.find(c => c.id === 'water_total_m3');
    const g = list.find(c => c.id === 'gas_total_m3');
    if (w) document.getElementById('water_reading_input').placeholder = 'сейчас: ' + (w.value/w.scale).toFixed(2);
    if (g) document.getElementById('gas_reading_input').placeholder = 'сейчас: ' + (g.value/g.scale).toFixed(2);
  } catch(e) {}
}
async function setReading(kind) {
  const input = document.getElementById(kind + '_reading_input');
  const value = parseFloat(input.value);
  if (isNaN(value)) return alert('Введите число');
  if (!confirm('Заменить текущие показания ' + (kind==='water'?'воды':'газа') + ' на ' + value + ' м³?')) return;
  const r = await fetch('/api/boilerroom/counters/set', {method:'POST',
    headers:{'Content-Type':'application/json'}, body: JSON.stringify({kind, value})});
  if (r.ok) { alert('Показания заданы'); input.value = ''; loadReadings(); }
}
async function rescanOw() {
  try {
    const r = await (await fetch('/api/a16/rescan-ow')).json();
    alert('Найдено датчиков: ' + r.found + '\nСмотри raw.ow0_temp, raw.ow1_temp... на вкладке Мониторинг');
  } catch(e) { alert('Ошибка пересканирования'); }
}
loadBrr();
loadReadings();
</script>
)html";

void BoilerRoomApp::init() {
    loadConfig();
    _loadCounters();

    Bus::declare("p_heat", 100);
    Bus::declare("p_water", 100);
    Bus::declare("water_total_m3", 100);
    Bus::declare("water_rate", 100);
    Bus::declare("water_rate_lph", 100);
    Bus::declare("gas_total_m3", 100);
    Bus::declare("gas_rate", 100);
    Bus::declare("t_outdoor", 100);
    Bus::declare("t_indoor", 100);
    Bus::declare("pump1_t_supply", 100);
    Bus::declare("pump1_t_return", 100);
    Bus::declare("pump1_t_delta", 100);
    Bus::declare("pump2_t_supply", 100);
    Bus::declare("pump2_t_return", 100);
    Bus::declare("pump2_t_delta", 100);
    // Публикуем сразу подгруженные с флэша показания, не дожидаясь первого
    // _recalc() — иначе на дашборде мелькнёт 0 в первую секунду после старта.
    Bus::write("water_total_m3", (int64_t)llround(_waterTotalM3 * 100));
    Bus::write("gas_total_m3",   (int64_t)llround(_gasTotalM3   * 100));

    // Окна усреднения расхода — старт отсчёта с текущего момента и текущих
    // импульсов (обычно 0 на старте, raw.water_pulses ещё не публиковался).
    _waterWindowStartMs       = millis();
    _waterPulsesAtWindowStart = (uint32_t)Bus::read("raw.water_pulses");
    _gasWindowStartMs         = millis();
    _gasPulsesAtWindowStart   = (uint32_t)Bus::read("raw.gas_pulses");

    Bus::declare("uptime", 1);
    Bus::declare("free_heap", 1);
    Bus::declare("rssi", 1);
    Bus::declare("mqtt_status", 1);
    Bus::declare("ip_address", 1);
    Bus::declare("fw_version", 1);
    Bus::write("fw_version", FW_VERSION_MAJOR * 10000 + FW_VERSION_MINOR * 100 + FW_VERSION_PATCH);

    WebHandler::registerTab({"boilerroom", "Котельная", "🏭", BRR_TAB_HTML, nullptr});

    extern AsyncWebServer server;
    server.on("/api/boilerroom/config", HTTP_GET, [](AsyncWebServerRequest* req) {
        JsonDocument doc;
        doc["water_m3_per_pulse"] = brrCfg.water_m3_per_pulse;
        doc["water_rate_avg_sec"] = brrCfg.water_rate_avg_sec;
        doc["gas_m3_per_pulse"]   = brrCfg.gas_m3_per_pulse;
        doc["gas_rate_avg_sec"]   = brrCfg.gas_rate_avg_sec;
        doc["p_heat_min"]         = brrCfg.p_heat_min;
        doc["p_heat_max"]         = brrCfg.p_heat_max;
        doc["p_water_min"]        = brrCfg.p_water_min;
        doc["p_water_max"]        = brrCfg.p_water_max;
        doc["adc_raw_at_4ma"]     = brrCfg.adc_raw_at_4ma;
        doc["adc_raw_at_20ma"]    = brrCfg.adc_raw_at_20ma;
        doc["ow_outdoor_index"]   = brrCfg.ow_outdoor_index;
        doc["ow_indoor_index"]    = brrCfg.ow_indoor_index;
        doc["ntc_r0_ohm"]         = brrCfg.ntc_r0_ohm;
        doc["ntc_beta"]           = brrCfg.ntc_beta;
        doc["ntc_pullup_ohm"]     = brrCfg.ntc_pullup_ohm;
        doc["ntc_c1_supply_index"] = brrCfg.ntc_c1_supply_index;
        doc["ntc_c1_return_index"] = brrCfg.ntc_c1_return_index;
        doc["ntc_c2_supply_index"] = brrCfg.ntc_c2_supply_index;
        doc["ntc_c2_return_index"] = brrCfg.ntc_c2_return_index;
        String out; serializeJson(doc, out);
        req->send(200, "application/json", out);
    });

    auto* h = new AsyncCallbackJsonWebHandler("/api/boilerroom/save",
        [](AsyncWebServerRequest* req, JsonVariant& json) {
            JsonObject o = json.as<JsonObject>();
            brrCfg.water_m3_per_pulse = o["water_m3_per_pulse"] | 0.01f;
            brrCfg.water_rate_avg_sec = o["water_rate_avg_sec"] | 300;
            brrCfg.gas_m3_per_pulse   = o["gas_m3_per_pulse"]   | 0.01f;
            brrCfg.gas_rate_avg_sec   = o["gas_rate_avg_sec"]   | 60;
            brrCfg.p_heat_min         = o["p_heat_min"]         | 0.0f;
            brrCfg.p_heat_max         = o["p_heat_max"]         | 10.0f;
            brrCfg.p_water_min        = o["p_water_min"]        | 0.0f;
            brrCfg.p_water_max        = o["p_water_max"]        | 10.0f;
            brrCfg.adc_raw_at_4ma     = o["adc_raw_at_4ma"]     | 557;
            brrCfg.adc_raw_at_20ma    = o["adc_raw_at_20ma"]    | 4402;
            brrCfg.ow_outdoor_index   = o["ow_outdoor_index"]   | 0;
            brrCfg.ow_indoor_index    = o["ow_indoor_index"]    | 1;
            brrCfg.ntc_r0_ohm         = o["ntc_r0_ohm"]         | 10000.0f;
            brrCfg.ntc_beta           = o["ntc_beta"]           | 3950.0f;
            brrCfg.ntc_pullup_ohm     = o["ntc_pullup_ohm"]     | 10000.0f;
            brrCfg.ntc_c1_supply_index = o["ntc_c1_supply_index"] | 0;
            brrCfg.ntc_c1_return_index = o["ntc_c1_return_index"] | 1;
            brrCfg.ntc_c2_supply_index = o["ntc_c2_supply_index"] | 2;
            brrCfg.ntc_c2_return_index = o["ntc_c2_return_index"] | 3;
            BoilerRoomApp::saveConfig();
            req->send(200, "application/json", "{\"status\":\"ok\"}");
        }
    );
    server.addHandler(h);

    // ── Ручная установка текущих показаний ────────────────────
    auto* readingH = new AsyncCallbackJsonWebHandler("/api/boilerroom/counters/set",
        [](AsyncWebServerRequest* req, JsonVariant& json) {
            JsonObject o = json.as<JsonObject>();
            String kind = o["kind"] | "";
            double value = o["value"] | 0.0;
            if (kind == "water") setWaterReading(value);
            else if (kind == "gas") setGasReading(value);
            else { req->send(400, "application/json", "{\"error\":\"bad kind\"}"); return; }
            req->send(200, "application/json", "{\"status\":\"ok\"}");
        }
    );
    server.addHandler(readingH);

    Serial.println("[BRR] App init OK");
}

void BoilerRoomApp::loop() {
    uint32_t now = millis();
    if (now - _lastCalcMs >= 1000) {
        _recalc();
        _lastCalcMs = now;
    }
    if (now - _lastSysMs >= 5000) {
        _publishSystemChannels();
        _lastSysMs = now;
    }
    _saveCountersIfDue();
}

void BoilerRoomApp::_recalc() {
    uint32_t now = millis();

    // ── Давление: raw ADC -> мА (калибровка A16) -> бар ──────────
    int32_t adcSpan = brrCfg.adc_raw_at_20ma - brrCfg.adc_raw_at_4ma;
    auto adcToMa = [adcSpan](int64_t raw) -> float {
        if (adcSpan == 0) return 4.0f;
        float ma = 4.0f + (float)(raw - brrCfg.adc_raw_at_4ma) / (float)adcSpan * 16.0f;
        return constrain(ma, 4.0f, 20.0f);
    };
    float p_heat_ma  = adcToMa(Bus::read("raw.p_heat_adc"));
    float p_water_ma = adcToMa(Bus::read("raw.p_water_adc"));

    float p_heat  = brrCfg.p_heat_min  + (p_heat_ma  - 4.0f) / 16.0f * (brrCfg.p_heat_max  - brrCfg.p_heat_min);
    float p_water = brrCfg.p_water_min + (p_water_ma - 4.0f) / 16.0f * (brrCfg.p_water_max - brrCfg.p_water_min);
    Bus::write("p_heat",  (int64_t)llround(p_heat  * 100));
    Bus::write("p_water", (int64_t)llround(p_water * 100));

    // ── Счётчики воды/газа (импульсы -> показания) ────────────────
    uint32_t waterPulses = (uint32_t)Bus::read("raw.water_pulses");
    uint32_t gasPulses   = (uint32_t)Bus::read("raw.gas_pulses");

    uint32_t dWater = waterPulses - _lastWaterPulses;  // корректно и при переполнении uint32
    uint32_t dGas   = gasPulses   - _lastGasPulses;
    _lastWaterPulses = waterPulses;
    _lastGasPulses   = gasPulses;

    // Показания — НЕ pulses*coeff (raw.water_pulses/raw.gas_pulses обнуляются
    // при каждой перезагрузке A16Device), а накопление поверх подгруженной
    // с флэша базы (см. заголовок файла и _loadCounters/_saveCountersIfDue).
    if (dWater > 0) { _waterTotalM3 += dWater * brrCfg.water_m3_per_pulse; _countersDirty = true; }
    if (dGas   > 0) { _gasTotalM3   += dGas   * brrCfg.gas_m3_per_pulse;   _countersDirty = true; }
    Bus::write("water_total_m3", (int64_t)llround(_waterTotalM3 * 100));
    Bus::write("gas_total_m3",   (int64_t)llround(_gasTotalM3   * 100));

    // ── Расход — усреднённый за настраиваемое окно, не за 1 тик ──
    // (при слабом напоре и дискретности 10 л/имп. мгновенный расход по
    // одному тику скачет между 0 и "как если бы весь литраж протёк за
    // секунду" — см. обсуждение с Михаилом). Значение обновляется только
    // на границе окна, внутри окна остаётся прежним.
    uint32_t waterWindowMs = (uint32_t)brrCfg.water_rate_avg_sec * 1000UL;
    if (waterWindowMs > 0 && (now - _waterWindowStartMs) >= waterWindowMs) {
        uint32_t elapsedMs = now - _waterWindowStartMs;
        uint32_t pulsesInWindow = waterPulses - _waterPulsesAtWindowStart;
        float elapsed_min = elapsedMs / 60000.0f;
        float elapsed_h   = elapsedMs / 3600000.0f;
        float litersInWindow = pulsesInWindow * brrCfg.water_m3_per_pulse * 1000.0f;
        Bus::write("water_rate",     (int64_t)llround((elapsed_min > 0 ? litersInWindow / elapsed_min : 0) * 100));
        Bus::write("water_rate_lph", (int64_t)llround((elapsed_h   > 0 ? litersInWindow / elapsed_h   : 0) * 100));
        _waterWindowStartMs       = now;
        _waterPulsesAtWindowStart = waterPulses;
    }

    uint32_t gasWindowMs = (uint32_t)brrCfg.gas_rate_avg_sec * 1000UL;
    if (gasWindowMs > 0 && (now - _gasWindowStartMs) >= gasWindowMs) {
        uint32_t elapsedMs = now - _gasWindowStartMs;
        uint32_t pulsesInWindow = gasPulses - _gasPulsesAtWindowStart;
        float elapsed_h = elapsedMs / 3600000.0f;
        float m3InWindow = pulsesInWindow * brrCfg.gas_m3_per_pulse;
        Bus::write("gas_rate", (int64_t)llround((elapsed_h > 0 ? m3InWindow / elapsed_h : 0) * 100));
        _gasWindowStartMs       = now;
        _gasPulsesAtWindowStart = gasPulses;
    }

    // ── 1-Wire: raw.ow{index}_temp -> t_outdoor/t_indoor ─────────
    // Индексы задаются в калибровках (см. ТЗ п.9 — привязка не по ROM-адресу,
    // а по порядку сканирования шины в A16Device; -1 = не публиковать).
    auto publishOw = [](int8_t idx, const char* dstId) {
        if (idx < 0) return;
        char rawId[24];
        snprintf(rawId, sizeof(rawId), "raw.ow%d_temp", idx);
        if (Bus::hasValue(rawId)) Bus::write(dstId, Bus::read(rawId));
    };
    publishOw(brrCfg.ow_outdoor_index, "t_outdoor");
    publishOw(brrCfg.ow_indoor_index,  "t_indoor");

    // ── NTC контуров: raw.ntc{index}_adc -> pump{N}_t_supply/return ─
    // Бета-модель термистора (см. заголовок файла): делитель Vcc-Rpullup-
    // ADC-Rterm-GND, 10-битный ADC на Pro Mini (0-1023). raw<=0 или >=1023
    // отбрасываем — это либо обрыв/КЗ цепи, либо (пока термисторов физически
    // нет) не значащий шум на плавающем входе, не считаем.
    auto ntcToCelsius = [](int64_t raw) -> float {
        if (raw <= 0 || raw >= 1023) return NAN;
        float rTerm = brrCfg.ntc_pullup_ohm * (float)raw / (1023.0f - (float)raw);
        float invT = 1.0f / 298.15f + logf(rTerm / brrCfg.ntc_r0_ohm) / brrCfg.ntc_beta;
        return 1.0f / invT - 273.15f;
    };
    auto publishNtc = [&](int8_t idx, const char* dstId) {
        if (idx < 0) return;
        char rawId[24];
        snprintf(rawId, sizeof(rawId), "raw.ntc%d_adc", idx);
        if (!Bus::hasValue(rawId)) return;
        float t = ntcToCelsius(Bus::read(rawId));
        if (isnan(t)) return;
        Bus::write(dstId, (int64_t)llround(t * 100));
    };
    publishNtc(brrCfg.ntc_c1_supply_index, "pump1_t_supply");
    publishNtc(brrCfg.ntc_c1_return_index, "pump1_t_return");
    publishNtc(brrCfg.ntc_c2_supply_index, "pump2_t_supply");
    publishNtc(brrCfg.ntc_c2_return_index, "pump2_t_return");

    // Дельта контура — только когда оба конца реально посчитаны в этом
    // (или предыдущем) цикле, иначе не пишем (см. заголовок файла).
    auto publishDelta = [](const char* supplyId, const char* returnId, const char* deltaId) {
        if (!Bus::hasValue(supplyId) || !Bus::hasValue(returnId)) return;
        Bus::write(deltaId, Bus::read(supplyId) - Bus::read(returnId));
    };
    publishDelta("pump1_t_supply", "pump1_t_return", "pump1_t_delta");
    publishDelta("pump2_t_supply", "pump2_t_return", "pump2_t_delta");
}

void BoilerRoomApp::_publishSystemChannels() {
    Bus::write("uptime", millis() / 1000);
    Bus::write("free_heap", ESP.getFreeHeap());

    xSemaphoreTake(coreMutex, portMAX_DELAY);
    bool eth  = sysState.ethConnected;
    bool wifi = sysState.wifiConnected;
    bool mqtt = sysState.mqttConnected;
    xSemaphoreGive(coreMutex);

    Bus::write("rssi", wifi ? WiFi.RSSI() : 0);
    Bus::write("mqtt_status", mqtt ? 1 : 0);

    IPAddress ip = eth ? ETH.localIP() : (wifi ? WiFi.localIP() : IPAddress(0, 0, 0, 0));
    uint32_t packed = ((uint32_t)ip[0] << 24) | ((uint32_t)ip[1] << 16) | ((uint32_t)ip[2] << 8) | ip[3];
    Bus::write("ip_address", packed);
}

// ============================================================
//  Показания счётчиков — персистентность (/counters.json)
//  Отдельно от калибровок (/boilerroom.json): пишется чаще и по другому
//  триггеру (не по действию пользователя, а по таймеру/изменению).
// ============================================================
bool BoilerRoomApp::_loadCounters() {
    JsonDocument doc;
    if (!ConfigManager::loadJson("/counters.json", doc)) return false;
    _waterTotalM3 = doc["water_total_m3"] | 0.0;
    _gasTotalM3   = doc["gas_total_m3"]   | 0.0;
    Serial.printf("[BRR] Counters loaded: water=%.3f m3, gas=%.3f m3\n", _waterTotalM3, _gasTotalM3);
    return true;
}

bool BoilerRoomApp::_saveCounters() {
    JsonDocument doc;
    doc["water_total_m3"] = _waterTotalM3;
    doc["gas_total_m3"]   = _gasTotalM3;
    bool ok = ConfigManager::saveJson("/counters.json", doc);
    if (ok) _countersDirty = false;
    return ok;
}

void BoilerRoomApp::_saveCountersIfDue() {
    uint32_t now = millis();
    if (!_countersDirty) return;
    if (now - _lastCounterSaveMs < COUNTER_SAVE_INTERVAL_MS) return;
    _lastCounterSaveMs = now;
    _saveCounters();
}

void BoilerRoomApp::setWaterReading(double m3) {
    _waterTotalM3 = m3;
    Bus::write("water_total_m3", (int64_t)llround(_waterTotalM3 * 100));
    _saveCounters();  // ручная правка — сохраняем сразу, не ждём таймера
    Serial.printf("[BRR] Water reading set manually: %.3f m3\n", m3);
}

void BoilerRoomApp::setGasReading(double m3) {
    _gasTotalM3 = m3;
    Bus::write("gas_total_m3", (int64_t)llround(_gasTotalM3 * 100));
    _saveCounters();
    Serial.printf("[BRR] Gas reading set manually: %.3f m3\n", m3);
}

bool BoilerRoomApp::loadConfig() {
    JsonDocument doc;
    if (!ConfigManager::loadJson("/boilerroom.json", doc)) return false;
    brrCfg.water_m3_per_pulse = doc["water_m3_per_pulse"] | 0.01f;
    brrCfg.water_rate_avg_sec = doc["water_rate_avg_sec"] | 300;
    brrCfg.gas_m3_per_pulse   = doc["gas_m3_per_pulse"]   | 0.01f;
    brrCfg.gas_rate_avg_sec   = doc["gas_rate_avg_sec"]   | 60;
    brrCfg.p_heat_min         = doc["p_heat_min"]         | 0.0f;
    brrCfg.p_heat_max         = doc["p_heat_max"]         | 10.0f;
    brrCfg.p_water_min        = doc["p_water_min"]        | 0.0f;
    brrCfg.p_water_max        = doc["p_water_max"]        | 10.0f;
    brrCfg.adc_raw_at_4ma     = doc["adc_raw_at_4ma"]      | 557;
    brrCfg.adc_raw_at_20ma    = doc["adc_raw_at_20ma"]     | 4402;
    brrCfg.ow_outdoor_index   = doc["ow_outdoor_index"]    | 0;
    brrCfg.ow_indoor_index    = doc["ow_indoor_index"]     | 1;
    brrCfg.ntc_r0_ohm         = doc["ntc_r0_ohm"]          | 10000.0f;
    brrCfg.ntc_beta           = doc["ntc_beta"]            | 3950.0f;
    brrCfg.ntc_pullup_ohm     = doc["ntc_pullup_ohm"]      | 10000.0f;
    brrCfg.ntc_c1_supply_index = doc["ntc_c1_supply_index"] | 0;
    brrCfg.ntc_c1_return_index = doc["ntc_c1_return_index"] | 1;
    brrCfg.ntc_c2_supply_index = doc["ntc_c2_supply_index"] | 2;
    brrCfg.ntc_c2_return_index = doc["ntc_c2_return_index"] | 3;
    return true;
}

bool BoilerRoomApp::saveConfig() {
    JsonDocument doc;
    doc["water_m3_per_pulse"] = brrCfg.water_m3_per_pulse;
    doc["water_rate_avg_sec"] = brrCfg.water_rate_avg_sec;
    doc["gas_m3_per_pulse"]   = brrCfg.gas_m3_per_pulse;
    doc["gas_rate_avg_sec"]   = brrCfg.gas_rate_avg_sec;
    doc["p_heat_min"]         = brrCfg.p_heat_min;
    doc["p_heat_max"]         = brrCfg.p_heat_max;
    doc["p_water_min"]        = brrCfg.p_water_min;
    doc["p_water_max"]        = brrCfg.p_water_max;
    doc["adc_raw_at_4ma"]     = brrCfg.adc_raw_at_4ma;
    doc["adc_raw_at_20ma"]    = brrCfg.adc_raw_at_20ma;
    doc["ow_outdoor_index"]   = brrCfg.ow_outdoor_index;
    doc["ow_indoor_index"]    = brrCfg.ow_indoor_index;
    doc["ntc_r0_ohm"]         = brrCfg.ntc_r0_ohm;
    doc["ntc_beta"]           = brrCfg.ntc_beta;
    doc["ntc_pullup_ohm"]     = brrCfg.ntc_pullup_ohm;
    doc["ntc_c1_supply_index"] = brrCfg.ntc_c1_supply_index;
    doc["ntc_c1_return_index"] = brrCfg.ntc_c1_return_index;
    doc["ntc_c2_supply_index"] = brrCfg.ntc_c2_supply_index;
    doc["ntc_c2_return_index"] = brrCfg.ntc_c2_return_index;
    return ConfigManager::saveJson("/boilerroom.json", doc);
}

#endif // APP_BOILER_ROOM
