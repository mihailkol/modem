#if defined(DEVICE_KC868_A16) && defined(APP_BOILER)
#include "BoilerAppDevice.h"
#include "../../core/ConfigManager.h"
#include "../../core/WebHandler.h"
#include "../../devices/kc868-a16/A16Types.h"
#include "../../devices/kc868-a16/A16Device.h"
#include <AsyncJson.h>
#include <ArduinoJson.h>

#ifdef MODULE_MQTT
#include "../../modules/mqtt/MqttHandler.h"
#endif
#ifdef MODULE_TELEGRAM
#include "../../modules/telegram/TgHandler.h"
#endif

#ifdef DEVICE_EKONOM
#include "../../devices/ekonom/EkoNomDevice.h"
#endif

BoilerAppConfig boilerAppCfg;
BoilerAppState  boilerAppState;

uint32_t BoilerAppDevice::_lastCalc    = 0;
uint32_t BoilerAppDevice::_lastHistory = 0;
uint32_t BoilerAppDevice::_lastPub     = 0;

// ============================================================
//  HTML — вкладка мониторинга (повторяет макет)
// ============================================================
static const char BOILER_MON_HTML[] PROGMEM = R"html(
<div class="monitor-page">

  <div class="m-card">
    <div class="m-card-label">ПРОИЗВОДИТЕЛЬНОСТЬ</div>
    <div class="m-power-big"><span id="b_power">--</span><span class="unit">кВт</span></div>
    <div class="m-power-sub">Σ <span id="b_energy">--</span> кВт·ч <span class="muted">(сутки)</span></div>
    <div class="m-power-sub">
      Поток 1: <b id="b_flow1">--</b> л/мин &nbsp;|&nbsp;
      Поток 2: <b id="b_flow2">--</b> л/мин
    </div>
  </div>

  <div class="m-card">
    <div class="m-card-label">СИСТЕМА ОТОПЛЕНИЯ</div>
    <div class="m-sensors-row">
      <div class="m-sensor">
        <div class="m-sensor-label">ПОДАЧА</div>
        <div class="m-sensor-val hot" id="b_tsup">--°</div>
        <div class="m-sensor-icon">▲</div>
      </div>
      <div class="m-sensor">
        <div class="m-sensor-label">ОБРАТКА</div>
        <div class="m-sensor-val cool" id="b_tret">--°</div>
        <div class="m-sensor-icon">•</div>
      </div>
      <div class="m-sensor">
        <div class="m-sensor-label">ДЕЛЬТА Т</div>
        <div class="m-sensor-val accent" id="b_tdelta">--°</div>
        <div class="m-sensor-sub muted">расчёт</div>
      </div>
    </div>
  </div>

  <div class="m-card">
    <div class="m-card-label">ДАВЛЕНИЕ В КОНТУРЕ</div>
    <div class="m-sensors-row">
      <div class="m-sensor">
        <div class="m-sensor-label">ПОДАЧА P1</div>
        <div class="m-sensor-val accent" id="b_psup">--</div>
        <div class="m-sensor-sub muted">бар</div>
      </div>
      <div class="m-sensor">
        <div class="m-sensor-label">ОБРАТКА P2</div>
        <div class="m-sensor-val accent" id="b_pret">--</div>
        <div class="m-sensor-sub muted">бар</div>
      </div>
      <div class="m-sensor">
        <div class="m-sensor-label">ДЕЛЬТА Р</div>
        <div class="m-sensor-val accent" id="b_pdelta">--</div>
        <div class="m-sensor-sub muted">бар</div>
      </div>
    </div>
  </div>

  <div class="card" style="margin-top:10px">
  <div class="section-label">АНАЛИТИКА (последний час)</div>

  <div style="display:grid;grid-template-columns:1fr 1fr;gap:8px;margin-bottom:10px">
    <div style="background:#111318;border-radius:8px;padding:10px 8px;text-align:center">
      <div style="font-size:20px;font-weight:800;color:var(--accent)" id="a_avg">—</div>
      <div style="font-size:10px;color:var(--muted);margin-top:2px">Ср. мощность кВт</div>
    </div>
    <div style="background:#111318;border-radius:8px;padding:10px 8px;text-align:center">
      <div style="font-size:20px;font-weight:800;color:var(--accent)" id="a_peak">—</div>
      <div style="font-size:10px;color:var(--muted);margin-top:2px">Пик мощности кВт</div>
    </div>
    <div style="background:#111318;border-radius:8px;padding:10px 8px;text-align:center">
      <div style="font-size:20px;font-weight:800;color:var(--accent)" id="a_dt">—</div>
      <div style="font-size:10px;color:var(--muted);margin-top:2px">Ср. ΔT °C</div>
    </div>
    <div style="background:#111318;border-radius:8px;padding:10px 8px;text-align:center">
      <div style="font-size:20px;font-weight:800;color:var(--accent)" id="a_duty">—</div>
      <div style="font-size:10px;color:var(--muted);margin-top:2px">Duty cycle %</div>
    </div>
  </div>

  <div style="display:grid;grid-template-columns:1fr 1fr 1fr;gap:8px">
    <div style="background:#111318;border-radius:8px;padding:10px 8px;text-align:center">
      <div style="font-size:20px;font-weight:800;color:var(--accent)" id="a_cyc">—</div>
      <div style="font-size:10px;color:var(--muted);margin-top:2px">Включений</div>
    </div>
    <div style="background:#111318;border-radius:8px;padding:10px 8px;text-align:center">
      <div style="font-size:20px;font-weight:800;color:var(--accent)" id="a_run">—</div>
      <div style="font-size:10px;color:var(--muted);margin-top:2px">Работа мин</div>
    </div>
    <div style="background:#111318;border-radius:8px;padding:10px 8px;text-align:center">
      <div style="font-size:20px;font-weight:800;color:var(--accent)" id="a_idle">—</div>
      <div style="font-size:10px;color:var(--muted);margin-top:2px">Простой мин</div>
    </div>
  </div>

  <div style="margin-top:8px;padding:8px 10px;background:#111318;border-radius:8px;
              display:flex;align-items:center;gap:8px;font-size:13px">
    <span id="a_burner_led" style="width:10px;height:10px;border-radius:50%;
          background:var(--muted);flex-shrink:0"></span>
    <span id="a_burner_txt">—</span>
    <span style="margin-left:auto;color:var(--muted);font-size:11px">
      Текущий цикл: <b id="a_cycdur">—</b> мин
    </span>
  </div>
</div>

  <div class="m-card">
    <div class="m-card-label">ИСТОРИЯ МОЩНОСТИ</div>
    <canvas id="powerChart" height="80"></canvas>
  </div>

</div>

<script>
async function updateBoiler() {
  try {
    const r = await fetch('/api/boiler/data');
    const d = await r.json();
    document.getElementById('b_power').textContent   = d.power_kw.toFixed(1);
    document.getElementById('b_energy').textContent  = d.energy_kwh.toFixed(1);
    document.getElementById('b_flow1').textContent   = d.flow1_lpm.toFixed(1);
    document.getElementById('b_flow2').textContent   = d.flow2_lpm.toFixed(1);
    document.getElementById('b_tsup').textContent    = d.t_supply.toFixed(1)+'°';
    document.getElementById('b_tret').textContent    = d.t_return.toFixed(1)+'°';
    document.getElementById('b_tdelta').textContent  = d.t_delta.toFixed(1)+'°';
    document.getElementById('b_psup').textContent    = d.p_supply.toFixed(2);
    document.getElementById('b_pret').textContent    = d.p_return.toFixed(2);
    document.getElementById('b_pdelta').textContent  = d.p_delta.toFixed(2);
    drawChart(d.history || []);
    
    // Аналитика
    if (d.power_avg_1h !== undefined) {
      document.getElementById('a_avg').textContent    = d.power_avg_1h.toFixed(1);
      document.getElementById('a_peak').textContent   = d.power_peak_1h.toFixed(1);
      document.getElementById('a_dt').textContent     = d.delta_avg_1h.toFixed(1);
      document.getElementById('a_duty').textContent   = d.duty_pct + '%';
      document.getElementById('a_cyc').textContent    = d.cycles_1h;
      document.getElementById('a_run').textContent    = d.runtime_min;
      document.getElementById('a_idle').textContent   = d.idle_min;
      document.getElementById('a_cycdur').textContent = d.cycle_dur_min;
      const led = document.getElementById('a_burner_led');
      led.style.background = d.burner_on ? 'var(--ok)' : 'var(--muted)';
      led.style.boxShadow  = d.burner_on ? '0 0 6px var(--ok)' : 'none';
      document.getElementById('a_burner_txt').textContent =
        d.burner_on ? 'Горелка работает' : 'Горелка выключена';
      }


  } catch(e) {}
}

function drawChart(data) {
  const canvas = document.getElementById('powerChart');
  if (!canvas || !data.length) return;
  const ctx = canvas.getContext('2d');
  canvas.width = canvas.offsetWidth;
  const W = canvas.width, H = canvas.height;
  ctx.clearRect(0,0,W,H);
  const max = Math.max(...data, 1);
  const step = W / (data.length - 1 || 1);
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

updateBoiler();
setInterval(updateBoiler, 2000);
</script>
)html";

// ============================================================
//  HTML — вкладка уставок
// ============================================================
static const char BOILER_CFG_HTML[] PROGMEM = R"html(
<div class="settings-group">
  <h3>🌡 Температурные уставки</h3>
  <label>Макс. температура (аварийное отключение):
    <input type="number" id="t_max" step="0.5"> °C
  </label>
  <label>Порог оповещения:
    <input type="number" id="t_alarm" step="0.5"> °C
  </label>
  <label>Гистерезис:
    <input type="number" id="t_hyst" step="0.1"> °C
  </label>
  <label>Мин. температура (защита от замерзания):
    <input type="number" id="t_min" step="0.5"> °C
  </label>
</div>
<div class="settings-group">
  <h3>💧 Расходомеры</h3>
  <label>Расходомер 1 (сухой контакт) — литров на импульс:
    <input type="number" id="flow1_lpp" step="0.01">
  </label>
  <label>Расходомер 2 (Холл) — литров на импульс:
    <input type="number" id="flow2_lpp" step="0.01">
  </label>
</div>
<div class="settings-group">
  <h3>📊 Давление</h3>
  <label>Датчик давления: 4мА = <input type="number" id="p_min" step="0.1"> бар</label>
  <label>Датчик давления: 20мА = <input type="number" id="p_max" step="0.5"> бар</label>
</div>
<div class="settings-group">
  <h3>📡 MQTT</h3>
  <label>Интервал публикации:
    <input type="number" id="pub_interval" step="1000"> мс
  </label>
</div>
<button onclick="saveBoilerCfg()">💾 Сохранить уставки</button>

<script>
async function loadBoilerCfg() {
  const r = await fetch('/api/boiler/config');
  const d = await r.json();
  document.getElementById('t_max').value        = d.t_max;
  document.getElementById('t_alarm').value      = d.t_alarm;
  document.getElementById('t_hyst').value       = d.t_hyst;
  document.getElementById('t_min').value        = d.t_min;
  document.getElementById('flow1_lpp').value    = d.flow1_liter_per_pulse;
  document.getElementById('flow2_lpp').value    = d.flow2_liter_per_pulse;
  document.getElementById('p_min').value        = d.p_min;
  document.getElementById('p_max').value        = d.p_max;
  document.getElementById('pub_interval').value = d.pub_interval;


  if (d.power_avg_1h  !== undefined) {
    document.getElementById('a_avg').textContent    = d.power_avg_1h.toFixed(1);
    document.getElementById('a_peak').textContent   = d.power_peak_1h.toFixed(1);
    document.getElementById('a_dt').textContent     = d.delta_avg_1h.toFixed(1);
    document.getElementById('a_duty').textContent   = d.duty_pct + '%';
    document.getElementById('a_cyc').textContent    = d.cycles_1h;
    document.getElementById('a_run').textContent    = d.runtime_min;
    document.getElementById('a_idle').textContent   = d.idle_min;
    document.getElementById('a_cycdur').textContent = d.cycle_dur_min;
    const led = document.getElementById('a_burner_led');
    led.style.background = d.burner_on ? 'var(--ok)' : 'var(--muted)';
    led.style.boxShadow  = d.burner_on ? '0 0 6px var(--ok)' : 'none';
    document.getElementById('a_burner_txt').textContent =
      d.burner_on ? 'Горелка работает' : 'Горелка выключена';
  }

  }
async function saveBoilerCfg() {
  const data = {
    t_max:                +document.getElementById('t_max').value,
    t_alarm:              +document.getElementById('t_alarm').value,
    t_hyst:               +document.getElementById('t_hyst').value,
    t_min:                +document.getElementById('t_min').value,
    flow1_liter_per_pulse:+document.getElementById('flow1_lpp').value,
    flow2_liter_per_pulse:+document.getElementById('flow2_lpp').value,
    p_min:                +document.getElementById('p_min').value,
    p_max:                +document.getElementById('p_max').value,
    pub_interval:         +document.getElementById('pub_interval').value,
  };
  const r = await fetch('/api/boiler/save', {method:'POST',
    headers:{'Content-Type':'application/json'}, body:JSON.stringify(data)});
  if (r.ok) alert('Уставки сохранены');
}
loadBoilerCfg();
</script>
)html";

// ============================================================
//  INIT
// ============================================================
void BoilerAppDevice::init() {
    loadConfig();

    // Регистрируем вкладки — мониторинг первым
    WebHandler::registerTab({"monitor",   "Мониторинг", "🔥", BOILER_MON_HTML});
    WebHandler::registerTab({"boiler_cfg","Уставки",    "⚙️",  BOILER_CFG_HTML});

    extern AsyncWebServer server;

    // Данные мониторинга
    server.on("/api/boiler/data", HTTP_GET, [](AsyncWebServerRequest* req) {
        JsonDocument doc;
        doc["t_supply"]   = boilerAppState.t_supply;
        doc["t_return"]   = boilerAppState.t_return;
        doc["t_delta"]    = boilerAppState.t_delta;
        doc["p_supply"]   = boilerAppState.p_supply;
        doc["p_return"]   = boilerAppState.p_return;
        doc["p_delta"]    = boilerAppState.p_delta;
        doc["flow1_lpm"]  = boilerAppState.flow1_lpm;
        doc["flow2_lpm"]  = boilerAppState.flow2_lpm;
        doc["power_kw"]   = boilerAppState.power_kw;
        doc["energy_kwh"] = boilerAppState.energy_kwh;

        
        // Аналитика
        doc["power_avg_1h"]  = boilerAppState.power_avg_1h;
        doc["power_peak_1h"] = boilerAppState.power_peak_1h;
        doc["delta_avg_1h"]  = boilerAppState.delta_avg_1h;
        doc["cycles_1h"]     = boilerAppState.cycles_1h;
        doc["runtime_min"]   = boilerAppState.runtime_min;
        doc["idle_min"]      = boilerAppState.idle_min;
        doc["duty_pct"]      = boilerAppState.duty_pct;
        doc["cycle_dur_min"] = boilerAppState.cycle_dur_min;
        doc["burner_on"]     = boilerAppState.burner_on;
    
        JsonArray hist = doc["history"].to<JsonArray>();
        uint8_t start = (boilerAppState.hist_head - boilerAppState.hist_count
                         + BOILER_HIST_SIZE) % BOILER_HIST_SIZE;
        for (uint8_t i = 0; i < boilerAppState.hist_count; i++)
            hist.add(boilerAppState.hist_power[(start+i) % BOILER_HIST_SIZE]);

        String out; serializeJson(doc, out);
        req->send(200, "application/json", out);
    });

    // Конфиг
    server.on("/api/boiler/config", HTTP_GET, [](AsyncWebServerRequest* req) {
        JsonDocument doc;
        doc["t_max"]                = boilerAppCfg.t_max;
        doc["t_alarm"]              = boilerAppCfg.t_alarm;
        doc["t_hyst"]               = boilerAppCfg.t_hyst;
        doc["t_min"]                = boilerAppCfg.t_min;
        doc["flow1_liter_per_pulse"]= boilerAppCfg.flow1_liter_per_pulse;
        doc["flow2_liter_per_pulse"]= boilerAppCfg.flow2_liter_per_pulse;
        doc["p_min"]                = boilerAppCfg.p_min;
        doc["p_max"]                = boilerAppCfg.p_max;
        doc["pub_interval"]         = boilerAppCfg.pub_interval;
        String out; serializeJson(doc, out);
        req->send(200, "application/json", out);
    });

    auto* h = new AsyncCallbackJsonWebHandler("/api/boiler/save",
        [](AsyncWebServerRequest* req, JsonVariant& json) {
            JsonObject o = json.as<JsonObject>();
            boilerAppCfg.t_max                 = o["t_max"]                 | 85.0f;
            boilerAppCfg.t_alarm               = o["t_alarm"]               | 80.0f;
            boilerAppCfg.t_hyst                = o["t_hyst"]                | 2.0f;
            boilerAppCfg.t_min                 = o["t_min"]                 | 40.0f;
            boilerAppCfg.flow1_liter_per_pulse = o["flow1_liter_per_pulse"] | 1.0f;
            boilerAppCfg.flow2_liter_per_pulse = o["flow2_liter_per_pulse"] | 1.0f;
            boilerAppCfg.p_min                 = o["p_min"]                 | 0.0f;
            boilerAppCfg.p_max                 = o["p_max"]                 | 10.0f;
            boilerAppCfg.pub_interval          = o["pub_interval"]          | 10000;
            BoilerAppDevice::saveConfig();
            req->send(200, "application/json", "{\"status\":\"ok\"}");
        }
    );
    server.addHandler(h);

#ifdef MODULE_TELEGRAM
    TgHandler::onCommand = [](const String& cmd, const String& chat) {
        if (cmd == "/boiler") {
            String msg = "🔥 *Котёл*\n";
            msg += "Подача: "  + String(boilerAppState.t_supply, 1) + "°C\n";
            msg += "Обратка: " + String(boilerAppState.t_return, 1) + "°C\n";
            msg += "P1/P2: "   + String(boilerAppState.p_supply, 2)
                               + "/" + String(boilerAppState.p_return, 2) + " бар\n";
            msg += "Мощность: "+ String(boilerAppState.power_kw, 1) + " кВт";
            TgHandler::sendMessage(msg, chat);
        }
    };
#endif

    Serial.println("[BOILER] App init OK");
}

void BoilerAppDevice::_updateAnalytics() {
    auto& s = boilerAppState;
 
    // 1. Записать в часовой буфер
    s.pushHistory1h(s.power_kw, s.t_delta);
 
    // 2. Подсчёт показателей по буферу
    float sum_p = 0, peak_p = 0, sum_dt = 0;
    uint16_t on_ticks = 0;  // тиков (по 10 сек) когда котёл работал
    uint16_t prev_on = 0;
    uint16_t cyc = 0;
    uint16_t n = s.hist1h_count;
 
    for (uint16_t i = 0; i < n; i++) {
        // Идём от старых к новым
        uint16_t idx = (s.hist1h_head - n + i + BOILER_HIST1H_SIZE) % BOILER_HIST1H_SIZE;
        float p  = s.hist1h_power[idx];
        float dt = s.hist1h_delta[idx];
 
        sum_p  += p;
        sum_dt += dt;
        if (p > peak_p) peak_p = p;
 
        bool on = (p >= BOILER_POWER_ON_KW);
        if (on) on_ticks++;
        // Считаем включения: переход 0→1
        if (on && !prev_on) cyc++;
        prev_on = on;
    }
 
    if (n > 0) {
        s.power_avg_1h  = sum_p  / n;
        s.delta_avg_1h  = sum_dt / n;
        s.power_peak_1h = peak_p;
        s.cycles_1h     = cyc;
        // on_ticks × 10 сек → минуты
        s.runtime_min   = (on_ticks * 10) / 60;
        s.idle_min       = ((n - on_ticks) * 10) / 60;
        s.duty_pct      = (uint8_t)((on_ticks * 100) / n);
    }
 
    // 3. Текущий цикл
    bool now_on = (s.power_kw >= BOILER_POWER_ON_KW);
    if (now_on && !s.burner_on) {
        // Включился
        s.burner_on      = true;
        s.cycle_start_ms = millis();
    } else if (!now_on && s.burner_on) {
        // Выключился
        s.burner_on = false;
        s.cycle_start_ms = millis();
    }
    if (s.cycle_start_ms > 0) {
        s.cycle_dur_min = (uint16_t)((millis() - s.cycle_start_ms) / 60000);
    }
}

// ============================================================
//  LOOP
// ============================================================
void BoilerAppDevice::loop() {
    if (millis() - _lastCalc > 2000) {
        _lastCalc = millis();
        updateFromA16();
        calcPower();
        checkAlarms();
    }

    if (millis() - _lastHistory > 10000) {
        _lastHistory = millis();
        boilerAppState.pushHistory(boilerAppState.power_kw);
        _updateAnalytics(); 
        boilerAppState.energy_kwh += boilerAppState.power_kw * (10.0f / 3600.0f);
    }

    #ifdef MODULE_MQTT
        if (millis() - _lastPub > boilerAppCfg.pub_interval) {
            _lastPub = millis();
            publishMqtt();
        }
    #endif

    #ifdef DEVICE_EKONOM
        updateFromEkoNom();
    #endif
}

// ============================================================
//  ДАННЫЕ ИЗ A16
// ============================================================
void BoilerAppDevice::updateFromA16() {
    // Температуры из 1-Wire (читает A16Device)
    extern A16State a16State;
    if (a16State.ow_count > BOILER_OW_SUPPLY && a16State.ow_ok[BOILER_OW_SUPPLY])
        boilerAppState.t_supply = a16State.ow_temp[BOILER_OW_SUPPLY];
    if (a16State.ow_count > BOILER_OW_RETURN && a16State.ow_ok[BOILER_OW_RETURN])
        boilerAppState.t_return = a16State.ow_temp[BOILER_OW_RETURN];
    boilerAppState.t_delta = boilerAppState.t_supply - boilerAppState.t_return;

    // Давление из ADC (уже сконвертировано A16Device в физические единицы)
    boilerAppState.p_supply = roundf(a16State.adc_value[BOILER_ADC_P1] * 10.0f) / 10.0f;
    boilerAppState.p_return = roundf(a16State.adc_value[BOILER_ADC_P2] * 10.0f) / 10.0f;    
    boilerAppState.p_delta  = boilerAppState.p_supply - boilerAppState.p_return;

    // Расход из счётчиков импульсов DIN
    // Расход (л/мин) = (импульсов за интервал) × (л/имп) / (dt в минутах)
    static uint32_t lastFlowCalc = 0;
    uint32_t now = millis();
    float dt_min = (now - lastFlowCalc) / 60000.0f;
    if (dt_min > 0 && lastFlowCalc > 0) {
        uint32_t d1 = a16State.din_counter[BOILER_DIN_FLOW1] - boilerAppState.flow1_last;
        uint32_t d2 = a16State.din_counter[BOILER_DIN_FLOW2] - boilerAppState.flow2_last;
        boilerAppState.flow1_lpm = (d1 * boilerAppCfg.flow1_liter_per_pulse) / dt_min;
        boilerAppState.flow2_lpm = (d2 * boilerAppCfg.flow2_liter_per_pulse) / dt_min;
    }
    boilerAppState.flow1_last = a16State.din_counter[BOILER_DIN_FLOW1];
    boilerAppState.flow2_last = a16State.din_counter[BOILER_DIN_FLOW2];
    lastFlowCalc = now;

    // Итоговый расход — берём больший из двух
    boilerAppState.flow_lpm = max(boilerAppState.flow1_lpm, boilerAppState.flow2_lpm);
}

#ifdef DEVICE_EKONOM
void BoilerAppDevice::updateFromEkoNom() {
    if (!ekoNomData.valid) return;
    boilerAppState.t_supply  = ekoNomData.t_supply;
    boilerAppState.t_return  = ekoNomData.t_return;
    boilerAppState.t_delta   = ekoNomData.t_delta;
    float lpm = ekoNomData.flow_m3h * 1000.0f / 60.0f;
    boilerAppState.flow1_lpm = lpm;   // отображается в UI
    boilerAppState.flow2_lpm = 0.0f;  // второго расходомера нет
    boilerAppState.flow_lpm  = lpm;   // используется для расчёта мощности 
    boilerAppState.power_kw  = ekoNomData.power_kw;
}
#endif

void BoilerAppDevice::calcPower() {
    // P(кВт) = m_dot(кг/с) × Cp(кДж/кг·К) × ΔT(К)
    // m_dot = flow_lpm / 60 (л/с = кг/с для воды)
    float m_dot = boilerAppState.flow_lpm / 60.0f;
    boilerAppState.power_kw = m_dot * 4.186f * boilerAppState.t_delta;
    if (boilerAppState.power_kw < 0) boilerAppState.power_kw = 0;
}

void BoilerAppDevice::checkAlarms() {
    bool wasHigh = boilerAppState.alarm_high;
    boilerAppState.alarm_high   = boilerAppState.t_supply > boilerAppCfg.t_max;
    boilerAppState.alarm_low    = boilerAppState.t_supply < boilerAppCfg.t_min
                                  && boilerAppState.t_supply > 1.0f;
    boilerAppState.alarm_sensor = !a16State.ow_ok[BOILER_OW_SUPPLY]
                                  || !a16State.ow_ok[BOILER_OW_RETURN];

#ifdef MODULE_TELEGRAM
    if (boilerAppState.alarm_high && !wasHigh) {
        TgHandler::sendMessage("🚨 *АВАРИЯ*: Температура подачи "
            + String(boilerAppState.t_supply, 1) + "°C > "
            + String(boilerAppCfg.t_max, 1) + "°C!");
    }
    if (!boilerAppState.alarm_high && wasHigh)
        TgHandler::sendMessage("✅ Температура в норме");
#endif
}

void BoilerAppDevice::publishMqtt() {
#ifdef MODULE_MQTT
    String b = String(baseCfg.device_name) + "/boiler/";
    char buf[16];
    dtostrf(boilerAppState.t_supply,  1, 1, buf); MqttHandler::publish((b+"t_supply").c_str(),  buf);
    dtostrf(boilerAppState.t_return,  1, 1, buf); MqttHandler::publish((b+"t_return").c_str(),  buf);
    dtostrf(boilerAppState.p_supply,  1, 2, buf); MqttHandler::publish((b+"p_supply").c_str(),  buf);
    dtostrf(boilerAppState.p_return,  1, 2, buf); MqttHandler::publish((b+"p_return").c_str(),  buf);
    dtostrf(boilerAppState.flow1_lpm, 1, 1, buf); MqttHandler::publish((b+"flow1_lpm").c_str(), buf);
    dtostrf(boilerAppState.flow2_lpm, 1, 1, buf); MqttHandler::publish((b+"flow2_lpm").c_str(), buf);
    dtostrf(boilerAppState.power_kw,  1, 2, buf); MqttHandler::publish((b+"power_kw").c_str(),  buf);
#endif
}

// ============================================================
//  CONFIG
// ============================================================
bool BoilerAppDevice::loadConfig() {
    JsonDocument doc;
    if (!ConfigManager::loadJson("/boiler.json", doc)) return false;
    boilerAppCfg.t_max                 = doc["t_max"]                 | 85.0f;
    boilerAppCfg.t_alarm               = doc["t_alarm"]               | 80.0f;
    boilerAppCfg.t_hyst                = doc["t_hyst"]                | 2.0f;
    boilerAppCfg.t_min                 = doc["t_min"]                 | 40.0f;
    boilerAppCfg.flow1_liter_per_pulse = doc["flow1_liter_per_pulse"] | 1.0f;
    boilerAppCfg.flow2_liter_per_pulse = doc["flow2_liter_per_pulse"] | 1.0f;
    boilerAppCfg.p_min                 = doc["p_min"]                 | 0.0f;
    boilerAppCfg.p_max                 = doc["p_max"]                 | 10.0f;
    boilerAppCfg.pub_interval          = doc["pub_interval"]          | 10000;
    return true;
}

bool BoilerAppDevice::saveConfig() {
    JsonDocument doc;
    doc["t_max"]                 = boilerAppCfg.t_max;
    doc["t_alarm"]               = boilerAppCfg.t_alarm;
    doc["t_hyst"]                = boilerAppCfg.t_hyst;
    doc["t_min"]                 = boilerAppCfg.t_min;
    doc["flow1_liter_per_pulse"] = boilerAppCfg.flow1_liter_per_pulse;
    doc["flow2_liter_per_pulse"] = boilerAppCfg.flow2_liter_per_pulse;
    doc["p_min"]                 = boilerAppCfg.p_min;
    doc["p_max"]                 = boilerAppCfg.p_max;
    doc["pub_interval"]          = boilerAppCfg.pub_interval;
    return ConfigManager::saveJson("/boiler.json", doc);
}

#endif
