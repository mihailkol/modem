#ifdef DEVICE_BOILER
#include "BoilerDevice.h"
#include "../../core/ConfigManager.h"
#include "../../core/WebHandler.h"
#include <AsyncJson.h>
#include <ArduinoJson.h>

#ifdef MODULE_MQTT
#include "../../modules/mqtt/MqttHandler.h"
#endif
#ifdef MODULE_TELEGRAM
#include "../../modules/telegram/TgHandler.h"
#endif

BoilerConfig boilerCfg;
BoilerState  boilerState;

uint32_t BoilerDevice::_lastRead   = 0;
uint32_t BoilerDevice::_lastPub    = 0;
uint32_t BoilerDevice::_lastEnergy = 0;

// ============================================================
//  HTML ВКЛАДОК (PROGMEM)
// ============================================================

// --- Мониторинг (повторяет макет) ---
static const char BOILER_MONITOR_HTML[] PROGMEM = R"html(
<div class="monitor-page">

  <div class="m-card">
    <div class="m-card-label">ПРОИЗВОДИТЕЛЬНОСТЬ</div>
    <div class="m-power-big"><span id="b_power">--</span><span class="unit">кВт</span></div>
    <div class="m-power-sub">Σ <span id="b_energy">--</span> кВт·ч <span class="muted">(сутки)</span></div>
    <div class="m-power-sub">Поток: <b id="b_flow">--</b> л/мин</div>
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
    <div class="m-card-label">ОКРУЖАЮЩАЯ СРЕДА</div>
    <div class="m-sensors-row two">
      <div class="m-sensor">
        <div class="m-sensor-label">ПОМЕЩЕНИЕ</div>
        <div class="m-sensor-val warm" id="b_troom">--°</div>
        <div class="m-sensor-icon cool">▼</div>
      </div>
      <div class="m-sensor">
        <div class="m-sensor-label">УЛИЦА</div>
        <div class="m-sensor-val cool" id="b_tout">--°</div>
        <div class="m-sensor-icon cool">▼</div>
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
    document.getElementById('b_power').textContent  = d.power_kw.toFixed(1);
    document.getElementById('b_energy').textContent = d.energy_kwh.toFixed(1);
    document.getElementById('b_flow').textContent   = d.flow_lpm.toFixed(1);
    document.getElementById('b_tsup').textContent   = d.t_supply.toFixed(1)+'°';
    document.getElementById('b_tret').textContent   = d.t_return.toFixed(1)+'°';
    document.getElementById('b_tdelta').textContent = d.t_delta.toFixed(1)+'°';
    document.getElementById('b_troom').textContent  = d.t_room.toFixed(1)+'°';
    document.getElementById('b_tout').textContent   = d.t_outdoor.toFixed(1)+'°';
    document.getElementById('b_psup').textContent   = d.p_supply.toFixed(2);
    document.getElementById('b_pret').textContent   = d.p_return.toFixed(2);
    document.getElementById('b_pdelta').textContent = d.p_delta.toFixed(2);
    drawChart(d.history || []);
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

  // Fill under line
  ctx.lineTo(W, H); ctx.lineTo(0, H); ctx.closePath();
  ctx.fillStyle = 'rgba(240,165,0,0.08)';
  ctx.fill();
}

updateBoiler();
setInterval(updateBoiler, 2000);
</script>
)html";

// --- Уставки ---
static const char BOILER_THRESHOLDS_HTML[] PROGMEM = R"html(
<div class="settings-group">
  <h3>Пороги срабатывания</h3>
  <label>Макс. температура (аварийное отключение):
    <input type="number" id="t_max" step="0.5">°C
  </label>
  <label>Порог оповещения:
    <input type="number" id="t_alarm" step="0.5">°C
  </label>
  <label>Гистерезис:
    <input type="number" id="t_hyst" step="0.1">°C
  </label>
  <label>Мин. температура (защита от замерзания):
    <input type="number" id="t_min" step="0.5">°C
  </label>
  <label>Интервал публикации MQTT:
    <input type="number" id="pub_interval" step="1000"> мс
  </label>
  <button onclick="saveThresholds()">💾 Сохранить уставки</button>
</div>
<script>
async function loadThresholds() {
  const r = await fetch('/api/boiler/config');
  const d = await r.json();
  document.getElementById('t_max').value          = d.t_max;
  document.getElementById('t_alarm').value        = d.t_alarm;
  document.getElementById('t_hyst').value         = d.t_hyst;
  document.getElementById('t_min').value          = d.t_min;
  document.getElementById('pub_interval').value   = d.pub_interval;
}
async function saveThresholds() {
  const data = {
    t_max:        +document.getElementById('t_max').value,
    t_alarm:      +document.getElementById('t_alarm').value,
    t_hyst:       +document.getElementById('t_hyst').value,
    t_min:        +document.getElementById('t_min').value,
    pub_interval: +document.getElementById('pub_interval').value,
  };
  const r = await fetch('/api/boiler/save', {method:'POST',
    headers:{'Content-Type':'application/json'}, body:JSON.stringify(data)});
  if (r.ok) alert('Уставки сохранены');
}
loadThresholds();
</script>
)html";

// ============================================================
//  INIT
// ============================================================
void BoilerDevice::init() {
    loadConfig();

    // Регистрируем вкладки устройства
    // Мониторинг — первым, чтобы стоял первой вкладкой
    WebHandler::registerTab({"monitor",    "Мониторинг", "🔥", BOILER_MONITOR_HTML});
    WebHandler::registerTab({"thresholds", "Уставки",    "⚙️",  BOILER_THRESHOLDS_HTML});

    // API данных
    extern AsyncWebServer server;
    server.on("/api/boiler/data", HTTP_GET, [](AsyncWebServerRequest* req) {
        JsonDocument doc;
        doc["t_supply"]   = boilerState.t_supply;
        doc["t_return"]   = boilerState.t_return;
        doc["t_delta"]    = boilerState.t_delta;
        doc["t_room"]     = boilerState.t_room;
        doc["t_outdoor"]  = boilerState.t_outdoor;
        doc["p_supply"]   = boilerState.p_supply;
        doc["p_return"]   = boilerState.p_return;
        doc["p_delta"]    = boilerState.p_delta;
        doc["power_kw"]   = boilerState.power_kw;
        doc["energy_kwh"] = boilerState.energy_kwh;
        doc["flow_lpm"]   = boilerState.flow_lpm;

        // История мощности — последние N точек в порядке времени
        JsonArray hist = doc["history"].to<JsonArray>();
        uint8_t start = (boilerState.hist_head - boilerState.hist_count + TEMP_HISTORY_SIZE) % TEMP_HISTORY_SIZE;
        for (uint8_t i = 0; i < boilerState.hist_count; i++) {
            hist.add(boilerState.hist_power[(start + i) % TEMP_HISTORY_SIZE]);
        }

        String out; serializeJson(doc, out);
        req->send(200, "application/json", out);
    });

    server.on("/api/boiler/config", HTTP_GET, [](AsyncWebServerRequest* req) {
        JsonDocument doc;
        doc["t_max"]        = boilerCfg.t_max;
        doc["t_alarm"]      = boilerCfg.t_alarm;
        doc["t_hyst"]       = boilerCfg.t_hyst;
        doc["t_min"]        = boilerCfg.t_min;
        doc["pub_interval"] = boilerCfg.pub_interval;
        String out; serializeJson(doc, out);
        req->send(200, "application/json", out);
    });

    auto* h = new AsyncCallbackJsonWebHandler("/api/boiler/save",
        [](AsyncWebServerRequest* req, JsonVariant& json) {
            JsonObject obj = json.as<JsonObject>();
            boilerCfg.t_max        = obj["t_max"]        | 85.0f;
            boilerCfg.t_alarm      = obj["t_alarm"]      | 80.0f;
            boilerCfg.t_hyst       = obj["t_hyst"]       | 2.0f;
            boilerCfg.t_min        = obj["t_min"]        | 40.0f;
            boilerCfg.pub_interval = obj["pub_interval"] | 10000;
            BoilerDevice::saveConfig();
            req->send(200, "application/json", "{\"status\":\"ok\"}");
        }
    );
    server.addHandler(h);

#ifdef MODULE_TELEGRAM
    // Добавляем команды котла в Telegram
    TgHandler::onCommand = [](const String& cmd, const String& chat) {
        if (cmd == "/temp") {
            String msg = "🌡 Котёл:\n";
            msg += "Подача: " + String(boilerState.t_supply, 1) + "°C\n";
            msg += "Обратка: " + String(boilerState.t_return, 1) + "°C\n";
            msg += "Мощность: " + String(boilerState.power_kw, 1) + " кВт";
            TgHandler::sendMessage(msg, chat);
        }
    };
#endif

    Serial.println("[BOILER] Device init OK");
}

// ============================================================
//  LOOP
// ============================================================
void BoilerDevice::loop() {
    // Опрос датчиков каждые 2 сек
    if (millis() - _lastRead > 2000) {
        _lastRead = millis();
        readSensors();
        checkAlarms();
    }

    // История мощности каждые 10 сек
    if (millis() - _lastEnergy > 10000) {
        _lastEnergy = millis();
        boilerState.pushHistory(boilerState.power_kw, millis() / 1000);
        // Накопление энергии: P(кВт) × dt(ч)
        boilerState.energy_kwh += boilerState.power_kw * (10.0f / 3600.0f);
    }

    // MQTT публикация
#ifdef MODULE_MQTT
    if (millis() - _lastPub > boilerCfg.pub_interval) {
        _lastPub = millis();
        publishMqtt();
    }
#endif

    // safe reboot
    xSemaphoreTake(coreMutex, portMAX_DELAY);
    bool reboot = sysState.pendingReboot && (millis() - sysState.rebootAt > 500);
    xSemaphoreGive(coreMutex);
    if (reboot) ESP.restart();
}

void BoilerDevice::readSensors() {
    // TODO: заменить на реальное чтение датчиков
    // Пример для DS18B20 на OneWire, NTC, ADS1115 и т.д.
    // boilerState.t_supply  = ds.getTempCByIndex(0);
    // boilerState.t_return  = ds.getTempCByIndex(1);

    // Расчёт производных
    boilerState.t_delta = boilerState.t_supply - boilerState.t_return;
    boilerState.p_delta = boilerState.p_supply - boilerState.p_return;

    // Расчёт мощности: P = m_dot × Cp × ΔT
    // m_dot (кг/с) = flow_lpm / 60 * плотность воды (~1 кг/л)
    // Cp воды = 4.186 кДж/(кг·К)
    float m_dot = boilerState.flow_lpm / 60.0f;
    boilerState.power_kw = m_dot * 4.186f * boilerState.t_delta;
    if (boilerState.power_kw < 0) boilerState.power_kw = 0;
}

void BoilerDevice::checkAlarms() {
    bool wasAlarmHigh = boilerState.alarm_high;

    boilerState.alarm_high = boilerState.t_supply > boilerCfg.t_max;
    boilerState.alarm_low  = boilerState.t_supply < boilerCfg.t_min
                             && boilerState.t_supply > 1.0f; // 0 = датчик не подключён

    // Оповещение при новой аварии
#ifdef MODULE_TELEGRAM
    if (boilerState.alarm_high && !wasAlarmHigh) {
        TgHandler::sendMessage(
            "🚨 *АВАРИЯ*: Температура подачи " +
            String(boilerState.t_supply, 1) + "°C превысила " +
            String(boilerCfg.t_max, 1) + "°C!"
        );
    }
    if (!boilerState.alarm_high && wasAlarmHigh) {
        TgHandler::sendMessage("✅ Температура в норме");
    }
#endif
}

void BoilerDevice::publishMqtt() {
#ifdef MODULE_MQTT
    String base = String(baseCfg.device_name) + "/";
    char buf[12];

    dtostrf(boilerState.t_supply,  1, 1, buf); MqttHandler::publish((base+"t_supply").c_str(),  buf);
    dtostrf(boilerState.t_return,  1, 1, buf); MqttHandler::publish((base+"t_return").c_str(),  buf);
    dtostrf(boilerState.t_room,    1, 1, buf); MqttHandler::publish((base+"t_room").c_str(),    buf);
    dtostrf(boilerState.t_outdoor, 1, 1, buf); MqttHandler::publish((base+"t_outdoor").c_str(), buf);
    dtostrf(boilerState.power_kw,  1, 2, buf); MqttHandler::publish((base+"power_kw").c_str(),  buf);
    dtostrf(boilerState.p_supply,  1, 2, buf); MqttHandler::publish((base+"p_supply").c_str(),  buf);
    dtostrf(boilerState.p_return,  1, 2, buf); MqttHandler::publish((base+"p_return").c_str(),  buf);
#endif
}

bool BoilerDevice::loadConfig() {
    JsonDocument doc;
    if (!ConfigManager::loadJson("/device.json", doc)) return false;
    boilerCfg.t_max        = doc["t_max"]        | 85.0f;
    boilerCfg.t_alarm      = doc["t_alarm"]      | 80.0f;
    boilerCfg.t_hyst       = doc["t_hyst"]       | 2.0f;
    boilerCfg.t_min        = doc["t_min"]        | 40.0f;
    boilerCfg.pub_interval = doc["pub_interval"] | 10000;
    return true;
}

bool BoilerDevice::saveConfig() {
    JsonDocument doc;
    doc["t_max"]        = boilerCfg.t_max;
    doc["t_alarm"]      = boilerCfg.t_alarm;
    doc["t_hyst"]       = boilerCfg.t_hyst;
    doc["t_min"]        = boilerCfg.t_min;
    doc["pub_interval"] = boilerCfg.pub_interval;
    return ConfigManager::saveJson("/device.json", doc);
}

#endif // DEVICE_BOILER
