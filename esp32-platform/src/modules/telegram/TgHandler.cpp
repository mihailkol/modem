#ifdef MODULE_TELEGRAM
#include "TgHandler.h"
#include "../../core/ConfigManager.h"
#include "../../core/WebHandler.h"
#include <AsyncJson.h>
#include <ArduinoJson.h>

TgConfig tgCfg;
void (*TgHandler::onCommand)(const String&, const String&) = nullptr;

static WiFiClientSecure _secClient;
static UniversalTelegramBot _bot("", _secClient);
static uint32_t _lastCheck = 0;

static const char TG_TAB_HTML[] PROGMEM = R"html(
<div class="settings-group">
  <label class="toggle-row">
    <span>Включить Telegram</span>
    <input type="checkbox" id="tg_enabled" onchange="tgToggle()">
  </label>
  <div id="tg_fields">
    <input type="text" name="tg_token"  placeholder="Bot Token">
    <input type="text" name="tg_group"  placeholder="Group Chat ID (для алертов)">
    <input type="text" name="tg_admin"  placeholder="Admin User ID (для команд)">
    <button onclick="saveTg()">💾 Сохранить</button>
  </div>
  <div class="info-row" id="tg_status">...</div>
</div>
<script>
async function loadTg() {
  const r = await fetch('/api/tg/config');
  const d = await r.json();
  document.getElementById('tg_enabled').checked = d.enabled;
  ['token','group','admin'].forEach(k => {
    const el = document.querySelector('[name="tg_'+k+'"]');
    if (el) el.value = d[k] || '';
  });
  tgToggle();
}
function tgToggle() {
  document.getElementById('tg_fields').style.display =
    document.getElementById('tg_enabled').checked ? 'block' : 'none';
}
async function saveTg() {
  const data = {
    enabled: document.getElementById('tg_enabled').checked,
    token:   document.querySelector('[name="tg_token"]').value,
    group:   document.querySelector('[name="tg_group"]').value,
    admin:   document.querySelector('[name="tg_admin"]').value,
  };
  const r = await fetch('/api/tg/save', {method:'POST',
    headers:{'Content-Type':'application/json'}, body:JSON.stringify(data)});
  if (r.ok) alert('Telegram сохранён. Перезагрузите устройство.');
}
loadTg();
</script>
)html";

bool TgHandler::loadConfig() {
    JsonDocument doc;
    if (!ConfigManager::loadJson("/tg.json", doc)) return false;
    tgCfg.enabled = doc["enabled"] | false;
    strlcpy(tgCfg.token, doc["token"] | "", sizeof(tgCfg.token));
    strlcpy(tgCfg.group, doc["group"] | "", sizeof(tgCfg.group));
    strlcpy(tgCfg.admin, doc["admin"] | "", sizeof(tgCfg.admin));
    return true;
}

bool TgHandler::saveConfig() {
    JsonDocument doc;
    doc["enabled"] = tgCfg.enabled;
    doc["token"]   = tgCfg.token;
    doc["group"]   = tgCfg.group;
    doc["admin"]   = tgCfg.admin;
    return ConfigManager::saveJson("/tg.json", doc);
}

void TgHandler::init() {
    loadConfig();

    WebHandler::registerTab({"telegram", "Telegram", "🤖", TG_TAB_HTML});

    extern AsyncWebServer server;
    server.on("/api/tg/config", HTTP_GET, [](AsyncWebServerRequest* req) {
        JsonDocument doc;
        doc["enabled"] = tgCfg.enabled;
        doc["token"]   = tgCfg.token;
        doc["group"]   = tgCfg.group;
        doc["admin"]   = tgCfg.admin;
        String out; serializeJson(doc, out);
        req->send(200, "application/json", out);
    });

    auto* h = new AsyncCallbackJsonWebHandler("/api/tg/save",
        [](AsyncWebServerRequest* req, JsonVariant& json) {
            JsonObject obj = json.as<JsonObject>();
            tgCfg.enabled = obj["enabled"] | false;
            strlcpy(tgCfg.token, obj["token"] | "", sizeof(tgCfg.token));
            strlcpy(tgCfg.group, obj["group"] | "", sizeof(tgCfg.group));
            strlcpy(tgCfg.admin, obj["admin"] | "", sizeof(tgCfg.admin));
            TgHandler::saveConfig();
            req->send(200, "application/json", "{\"status\":\"ok\"}");
        }
    );
    server.addHandler(h);

    if (!tgCfg.enabled || strlen(tgCfg.token) == 0) {
        Serial.println("[TG] Disabled or no token");
        return;
    }

    _secClient.setInsecure();
    _bot.updateToken(tgCfg.token);
    _secClient.setHandshakeTimeout(10000);
    _bot.getUpdates(_bot.last_message_received + 1);
    Serial.println("[TG] Init OK");
}

void TgHandler::sendMessage(const String& msg, const String& chatId) {
    if (!tgCfg.enabled || strlen(tgCfg.token) == 0) return;
    String target = (chatId == "") ? String(tgCfg.group) : chatId;
    if (target.length() > 0) _bot.sendMessage(target, msg, "");
}

void TgHandler::handleMessages(int count) {
    static long lastId = 0;
    for (int i = 0; i < count; i++) {
        long uid = _bot.messages[i].update_id;
        if (uid <= lastId && lastId != 0) continue;

        String chat = _bot.messages[i].chat_id;
        String text = _bot.messages[i].text;

        // Проверка прав
        if (chat != String(tgCfg.admin)) {
            _bot.sendMessage(chat, "⛔ Доступ запрещён. Ваш ID: " + chat, "");
            lastId = uid;
            continue;
        }

        // Базовые команды платформы
        if (text == "/start" || text == "/help") {
            String help = "📟 *" + String(baseCfg.device_name) + "*\n";
            help += "/status — состояние системы\n";
            help += "/reboot — перезагрузка\n";
            _bot.sendMessage(chat, help, "Markdown");
        } else if (text == "/status") {
            String s = "📟 *" + String(baseCfg.device_name) + "*\n";
            s += "Сеть: " + String(sysState.ethConnected ? "ETH ✅" : sysState.wifiConnected ? "WiFi ✅" : "❌") + "\n";
            s += "MQTT: " + String(sysState.mqttConnected ? "✅" : "❌") + "\n";
            s += "Uptime: " + String(millis() / 1000) + "с";
            _bot.sendMessage(chat, s, "Markdown");
        } else if (text == "/reboot") {
            _bot.sendMessage(chat, "🔄 Перезагружаюсь...", "");
            xSemaphoreTake(coreMutex, portMAX_DELAY);
            sysState.pendingReboot = true;
            sysState.rebootAt = millis();
            xSemaphoreGive(coreMutex);
        } else if (onCommand) {
            // Передаём устройству неизвестные команды
            onCommand(text, chat);
        }

        lastId = uid;
        _bot.last_message_received = uid;
    }
}

void TgHandler::loop() {
    if (!tgCfg.enabled || strlen(tgCfg.token) < 10) return;

    bool hasNet = false;
    xSemaphoreTake(coreMutex, portMAX_DELAY);
    hasNet = sysState.ethConnected || sysState.wifiConnected;
    xSemaphoreGive(coreMutex);
    if (!hasNet) return;

    if (millis() - _lastCheck > 5000) {
        int n = _bot.getUpdates(_bot.last_message_received + 1);
        if (n > 0) handleMessages(n);
        _lastCheck = millis();
        _secClient.stop(); // освобождаем TCP соединение
    }

    xSemaphoreTake(coreMutex, portMAX_DELAY);
    sysState.tgConnected = tgCfg.enabled;
    xSemaphoreGive(coreMutex);
}

#endif // MODULE_TELEGRAM
