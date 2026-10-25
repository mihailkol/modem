#ifdef MODULE_MQTT
#include "MqttModule.h"
#include "ConfigManager.h"
#include "WebHandler.h"
#include "Bus.h"
#include <AsyncJson.h>
#include <ArduinoJson.h>
#include <vector>

MqttConfig mqttCfg;

static WiFiClient   _espClient;
static PubSubClient  _client(_espClient);
static uint32_t      _lastRetry = 0;

struct ExtraSub { String topic; MqttMessageCallback cb; };
static std::vector<ExtraSub> _extraSubs;

static const char MQTT_TAB_HTML[] PROGMEM = R"html(
<div class="settings-group">
  <label class="toggle-row">
    <span>Включить MQTT</span>
    <input type="checkbox" id="mqtt_enabled" onchange="mqttToggle()">
  </label>
  <div id="mqtt_fields">
    <input type="text"     name="mqtt_server" placeholder="Broker (192.168.1.5)">
    <input type="number"   name="mqtt_port"   placeholder="Port" value="1883">
    <input type="text"     name="mqtt_user"   placeholder="User (опционально)">
    <input type="password" name="mqtt_pass"   placeholder="Password (опционально)">
    <button onclick="saveMqtt()">💾 Сохранить MQTT</button>
  </div>
</div>
<script>
async function loadMqtt() {
  const r = await fetch('/api/mqtt/config');
  const d = await r.json();
  document.getElementById('mqtt_enabled').checked = d.enabled;
  Object.keys(d).forEach(k => {
    const el = document.querySelector('[name="mqtt_'+k+'"]');
    if (el) el.value = d[k];
  });
  mqttToggle();
}
function mqttToggle() {
  document.getElementById('mqtt_fields').style.display =
    document.getElementById('mqtt_enabled').checked ? 'block' : 'none';
}
async function saveMqtt() {
  const data = {
    enabled: document.getElementById('mqtt_enabled').checked,
    server:  document.querySelector('[name="mqtt_server"]').value,
    port:    +document.querySelector('[name="mqtt_port"]').value,
    user:    document.querySelector('[name="mqtt_user"]').value,
    pass:    document.querySelector('[name="mqtt_pass"]').value,
  };
  const r = await fetch('/api/mqtt/save', {method:'POST',
    headers:{'Content-Type':'application/json'}, body:JSON.stringify(data)});
  if (r.ok) alert('MQTT сохранён, перезагружаюсь...');
}
loadMqtt();
</script>
)html";

bool MqttModule::loadConfig() {
    JsonDocument doc;
    if (!ConfigManager::loadJson("/mqtt.json", doc)) return false;
    mqttCfg.enabled = doc["enabled"] | true;
    strlcpy(mqttCfg.server, doc["server"] | "", sizeof(mqttCfg.server));
    mqttCfg.port = doc["port"] | 1883;
    strlcpy(mqttCfg.user, doc["user"] | "", sizeof(mqttCfg.user));
    strlcpy(mqttCfg.pass, doc["pass"] | "", sizeof(mqttCfg.pass));
    return true;
}

bool MqttModule::saveConfig() {
    JsonDocument doc;
    doc["enabled"] = mqttCfg.enabled;
    doc["server"]  = mqttCfg.server;
    doc["port"]    = mqttCfg.port;
    doc["user"]    = mqttCfg.user;
    doc["pass"]    = mqttCfg.pass;
    return ConfigManager::saveJson("/mqtt.json", doc);
}

void MqttModule::addSubscription(const char* topic, MqttMessageCallback cb) {
    _extraSubs.push_back({ String(topic), cb });
    if (_client.connected()) _client.subscribe(topic);
}

// Публикует одно значение канала, форматируя число по scale
// (scale=100 -> 2 знака после запятой, scale=1 -> целое).
static void _publishChannelValue(const char* id, int64_t value, int32_t scale) {
    char topic[80];
    snprintf(topic, sizeof(topic), "%s/state/%s", baseCfg.device_name, id);

    char payload[32];
    if (scale <= 1) {
        snprintf(payload, sizeof(payload), "%lld", (long long)value);
    } else {
        double v = (double)value / (double)scale;
        snprintf(payload, sizeof(payload), "%.4f", v);
    }
    _client.publish(topic, payload);
}

void MqttModule::_onBusWrite(const char* id, int64_t value) {
    if (!_client.connected()) return;
    _publishChannelValue(id, value, Bus::scaleOf(id));
}

void MqttModule::init() {
    loadConfig();

    WebHandler::registerTab({"mqtt", "MQTT", "📊", MQTT_TAB_HTML, nullptr});

    extern AsyncWebServer server;
    server.on("/api/mqtt/config", HTTP_GET, [](AsyncWebServerRequest* req) {
        JsonDocument doc;
        doc["enabled"] = mqttCfg.enabled;
        doc["server"]  = mqttCfg.server;
        doc["port"]    = mqttCfg.port;
        doc["user"]    = mqttCfg.user;
        String out; serializeJson(doc, out);
        req->send(200, "application/json", out);
    });

    auto* h = new AsyncCallbackJsonWebHandler("/api/mqtt/save",
        [](AsyncWebServerRequest* req, JsonVariant& json) {
            JsonObject obj = json.as<JsonObject>();
            mqttCfg.enabled = obj["enabled"] | true;
            strlcpy(mqttCfg.server, obj["server"] | "", sizeof(mqttCfg.server));
            mqttCfg.port = obj["port"] | 1883;
            strlcpy(mqttCfg.user, obj["user"] | "", sizeof(mqttCfg.user));
            strlcpy(mqttCfg.pass, obj["pass"] | "", sizeof(mqttCfg.pass));
            MqttModule::saveConfig();
            req->send(200, "application/json", "{\"status\":\"ok\"}");
            xSemaphoreTake(coreMutex, portMAX_DELAY);
            sysState.pendingReboot = true;
            sysState.rebootAt = millis();
            xSemaphoreGive(coreMutex);
        }
    );
    server.addHandler(h);

    if (strlen(mqttCfg.server) > 0 && mqttCfg.enabled) {
        _client.setServer(mqttCfg.server, mqttCfg.port);
        _client.setCallback(_onMessage);
    }

    // Публикуем наружу КАЖДОЕ изменение любого канала Bus.
    // NB: публикация идёт синхронно внутри callback'а — PubSubClient::publish()
    // сам по себе не блокирует надолго, но если каналов/частота записи вырастут
    // и это станет заметно в loop-тайминге — вынести в очередь + отправку в loop().
    Bus::subscribeAll(_onBusWrite);

    Serial.println("[MQTT] Init OK");
}

void MqttModule::_reconnect() {
    if (millis() - _lastRetry < 5000) return;
    _lastRetry = millis();

    String clientId = String(baseCfg.device_name) + "-" + String(WiFi.macAddress());
    bool ok = (strlen(mqttCfg.user) > 0)
              ? _client.connect(clientId.c_str(), mqttCfg.user, mqttCfg.pass)
              : _client.connect(clientId.c_str());

    xSemaphoreTake(coreMutex, portMAX_DELAY);
    sysState.mqttConnected = ok;
    xSemaphoreGive(coreMutex);

    if (ok) {
        Serial.println("[MQTT] Connected");
        String setSub = String(baseCfg.device_name) + "/set/#";
        _client.subscribe(setSub.c_str());
        for (auto& s : _extraSubs) _client.subscribe(s.topic.c_str());
    } else {
        Serial.printf("[MQTT] Failed rc=%d\n", _client.state());
    }
}

void MqttModule::loop() {
    if (!mqttCfg.enabled || strlen(mqttCfg.server) == 0) return;

    bool hasNet = false;
    xSemaphoreTake(coreMutex, portMAX_DELAY);
    hasNet = sysState.ethConnected || sysState.wifiConnected;
    xSemaphoreGive(coreMutex);
    if (!hasNet) return;

    if (!_client.connected()) {
        xSemaphoreTake(coreMutex, portMAX_DELAY);
        sysState.mqttConnected = false;
        xSemaphoreGive(coreMutex);
        _reconnect();
    } else {
        _client.loop();
    }
}

void MqttModule::_onMessage(char* topic, byte* payload, unsigned int length) {
    char buf[length + 1];
    memcpy(buf, payload, length);
    buf[length] = '\0';

    String t(topic);

    // <device_name>/set/<channel_id> -> Bus::write(channel_id, value * scale)
    String setPrefix = String(baseCfg.device_name) + "/set/";
    if (t.startsWith(setPrefix)) {
        String id = t.substring(setPrefix.length());
        double v = atof(buf);
        int32_t scale = Bus::scaleOf(id.c_str());
        int64_t scaled = (int64_t)llround(v * scale);
        Bus::write(id.c_str(), scaled);
        return;
    }

    // Прочие подписки (например TimeManager на топик времени)
    for (auto& s : _extraSubs) {
        if (s.topic == t) s.cb(topic, buf);
    }
}

#endif // MODULE_MQTT
