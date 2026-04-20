#ifdef MODULE_MQTT
#include "MqttHandler.h"
#include "../../core/ConfigManager.h"
#include "../../core/WebHandler.h"
#include <AsyncJson.h>
#include <ArduinoJson.h>

MqttConfig mqttCfg;

void (*MqttHandler::onMessage)(const char*, const char*, unsigned int) = nullptr;

static WiFiClient    _espClient;
static PubSubClient  _client(_espClient);
static uint32_t      _lastRetry = 0;

// HTML вкладки настроек MQTT (PROGMEM)
static const char MQTT_TAB_HTML[] PROGMEM = R"html(
<div class="settings-group">
  <label class="toggle-row">
    <span>Включить MQTT</span>
    <input type="checkbox" id="mqtt_enabled" onchange="mqttToggle()">
  </label>
  <div id="mqtt_fields">
    <input type="text"     name="mqtt_server" placeholder="Broker (broker.emqx.io)">
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

bool MqttHandler::loadConfig() {
    JsonDocument doc;
    if (!ConfigManager::loadJson("/mqtt.json", doc)) return false;
    mqttCfg.enabled = doc["enabled"] | true;
    strlcpy(mqttCfg.server, doc["server"] | "", sizeof(mqttCfg.server));
    mqttCfg.port = doc["port"] | 1883;
    strlcpy(mqttCfg.user, doc["user"] | "", sizeof(mqttCfg.user));
    strlcpy(mqttCfg.pass, doc["pass"] | "", sizeof(mqttCfg.pass));
    return true;
}

bool MqttHandler::saveConfig() {
    JsonDocument doc;
    doc["enabled"] = mqttCfg.enabled;
    doc["server"]  = mqttCfg.server;
    doc["port"]    = mqttCfg.port;
    doc["user"]    = mqttCfg.user;
    doc["pass"]    = mqttCfg.pass;
    return ConfigManager::saveJson("/mqtt.json", doc);
}

void MqttHandler::init() {
    loadConfig();

    // Регистрируем вкладку настроек
    WebHandler::registerTab({"mqtt", "MQTT", "📊", MQTT_TAB_HTML});

    // API
    extern AsyncWebServer server; // объявлен в main.cpp
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
            MqttHandler::saveConfig();
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
        _client.setCallback(_defaultCallback);
    }

    Serial.println("[MQTT] Init OK");
}

void MqttHandler::reconnect() {
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
        String sub = String(baseCfg.device_name) + "/set/#";
        _client.subscribe(sub.c_str());
    } else {
        Serial.printf("[MQTT] Failed rc=%d\n", _client.state());
    }
}

void MqttHandler::loop() {
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
        reconnect();
    } else {
        _client.loop();
    }
}

void MqttHandler::publish(const char* topic, const char* payload, bool retained) {
    if (_client.connected()) _client.publish(topic, payload, retained);
}

void MqttHandler::_defaultCallback(char* topic, byte* payload, unsigned int length) {
    if (onMessage) {
        char buf[length + 1];
        memcpy(buf, payload, length);
        buf[length] = '\0';
        onMessage(topic, buf, length);
    }
}

#endif // MODULE_MQTT
