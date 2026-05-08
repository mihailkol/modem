#include "WebHandler.h"
#include "ConfigManager.h"
#include <AsyncJson.h>
#include <ArduinoJson.h>

#ifdef MODULE_MODEM
extern int  _modemCreg;
extern bool _modemEnabled;
#endif

static const char BUILD_TIME[] = __DATE__ " " __TIME__;

std::vector<WebTab> WebHandler::_tabs;

void WebHandler::registerTab(const WebTab& tab) {
    _tabs.push_back(tab);
    Serial.printf("[WEB] Tab registered: %s\n", tab.id);
}

// Генерирует nav из зарегистрированных вкладок
String WebHandler::buildNav() {
    String nav = "";
    for (size_t i = 0; i < _tabs.size(); i++) {
        String cls = (i == 0) ? "tab-btn active" : "tab-btn";
        nav += "<button class=\"" + String(cls) + "\" onclick=\"openTab(event,'";
        nav += _tabs[i].id;
        nav += "')\">";
        if (_tabs[i].icon && strlen(_tabs[i].icon) > 0) {
            nav += String(_tabs[i].icon) + " ";
        }
        nav += _tabs[i].label;
        nav += "</button>";
    }
    return nav;
}

void WebHandler::init(AsyncWebServer& server) {

    // ── /api/nav — список вкладок для фронтенда ──────────────
    server.on("/api/nav", HTTP_GET, [](AsyncWebServerRequest* req) {
        JsonDocument doc;
        JsonArray arr = doc.to<JsonArray>();
        for (size_t i = 0; i < _tabs.size(); i++) {
            JsonObject t = arr.add<JsonObject>();
            t["id"]    = _tabs[i].id;
            t["label"] = _tabs[i].label;
            t["icon"]  = _tabs[i].icon ? _tabs[i].icon : "";
        }
        String out; serializeJson(doc, out);
        req->send(200, "application/json", out);
    });

    // ── /api/tab?id=xxx — HTML-контент вкладки ────────────────
    server.on("/api/tab", HTTP_GET, [](AsyncWebServerRequest* req) {
        if (!req->hasParam("id")) { req->send(400); return; }
        String id = req->getParam("id")->value();
        for (auto& tab : _tabs) {
            if (id == tab.id) {
                req->send(200, "text/html", tab.html);
                return;
            }
        }
        req->send(404, "text/plain", "Tab not found");
    });

    // ── /api/status — состояние системы ──────────────────────
    server.on("/api/status", HTTP_GET, [](AsyncWebServerRequest* req) {
        JsonDocument doc;
        xSemaphoreTake(coreMutex, portMAX_DELAY);
        doc["eth"]   = sysState.ethConnected;
        doc["wifi"]  = sysState.wifiConnected;
        doc["ap"]    = sysState.apMode;
        doc["mqtt"]  = sysState.mqttConnected;
        doc["tg"]    = sysState.tgConnected;
        doc["uptime"] = millis() / 1000;
        doc["build"] = BUILD_TIME;
        xSemaphoreGive(coreMutex);
        #ifdef MODULE_MODEM
        doc["modem_enabled"] = _modemEnabled;
        doc["modem_creg"]    = _modemCreg;
        #endif
        String out; serializeJson(doc, out);
        req->send(200, "application/json", out);


    });

    server.on("/api/syslog", HTTP_GET, [](AsyncWebServerRequest* req) {
        JsonDocument doc;
        if (LittleFS.exists("/syslog.txt")) {
            File f = LittleFS.open("/syslog.txt", "r");
            if (f) { doc["log"] = f.readString(); f.close(); }
        } else { doc["log"] = ""; }
        String out; serializeJson(doc, out);
        req->send(200, "application/json", out);
    });

    server.on("/api/syslog/clear", HTTP_POST, [](AsyncWebServerRequest* req) {
        LittleFS.remove("/syslog.txt");
        req->send(200, "application/json", "{\"status\":\"ok\"}");
    });

    // ── /api/get-config — текущий base конфиг ────────────────
    server.on("/api/get-config", HTTP_GET, [](AsyncWebServerRequest* req) {
        if (!req->authenticate(baseCfg.web_user, baseCfg.web_pass) && strlen(baseCfg.web_pass) > 0)
            return req->requestAuthentication();
        if (LittleFS.exists("/base.json"))
            req->send(LittleFS, "/base.json", "application/json");
        else
            req->send(404, "application/json", "{\"error\":\"no_config\"}");
    });

    // ── /api/save-config — сохранение base конфига ───────────
    auto* cfgHandler = new AsyncCallbackJsonWebHandler(
        "/api/save-config",
        [](AsyncWebServerRequest* req, JsonVariant& json) {
            if (!req->authenticate(baseCfg.web_user, baseCfg.web_pass) && strlen(baseCfg.web_pass) > 0)
                return req->requestAuthentication();

            JsonObject obj = json.as<JsonObject>();
            baseCfg.eth_dhcp = obj["eth_dhcp"] | true;
            strlcpy(baseCfg.eth_ip,      obj["eth_ip"]      | "", sizeof(baseCfg.eth_ip));
            strlcpy(baseCfg.eth_mask,    obj["eth_mask"]    | "", sizeof(baseCfg.eth_mask));
            strlcpy(baseCfg.eth_gw,      obj["eth_gw"]      | "", sizeof(baseCfg.eth_gw));
            strlcpy(baseCfg.eth_dns,     obj["eth_dns"]     | "", sizeof(baseCfg.eth_dns));
            strlcpy(baseCfg.wifi_ssid,   obj["wifi_ssid"]   | "", sizeof(baseCfg.wifi_ssid));
            strlcpy(baseCfg.wifi_pass,   obj["wifi_pass"]   | "", sizeof(baseCfg.wifi_pass));
            strlcpy(baseCfg.web_user,    obj["web_user"]    | "admin", sizeof(baseCfg.web_user));
            strlcpy(baseCfg.web_pass,    obj["web_pass"]    | "", sizeof(baseCfg.web_pass));
            strlcpy(baseCfg.device_name, obj["device_name"] | "esp32-device", sizeof(baseCfg.device_name));

            if (ConfigManager::saveBase())
                req->send(200, "application/json", "{\"status\":\"ok\"}");
            else
                req->send(500, "application/json", "{\"status\":\"error\"}");
        }
    );
    server.addHandler(cfgHandler);

    // ── /api/reboot ───────────────────────────────────────────
    server.on("/api/reboot", HTTP_GET, [](AsyncWebServerRequest* req) {
        req->send(200, "text/plain", "Rebooting...");
        xSemaphoreTake(coreMutex, portMAX_DELAY);
        sysState.pendingReboot = true;
        sysState.rebootAt = millis();
        xSemaphoreGive(coreMutex);
    });

    server.on("/api/download-config", HTTP_GET, [](AsyncWebServerRequest* req) {
        if (!LittleFS.exists("/base.json")) { req->send(404); return; }
        req->send(LittleFS, "/base.json", "application/json");
    });

    // ── /api/backup — все конфиги в одном файле ───────────────
    server.on("/api/backup", HTTP_GET, [](AsyncWebServerRequest* req) {
        JsonDocument doc;
        const char* files[] = {"base", "mqtt", "tg", "boiler", "device"};
        for (auto name : files) {
            String path = String("/") + name + ".json";
            JsonDocument part;
            if (ConfigManager::loadJson(path.c_str(), part))
                doc[name] = part;
        }
        String out; serializeJson(doc, out);
        req->send(200, "application/json", out);
    });

    // ── /api/restore — восстановить из backup.json ────────────
    auto* restoreH = new AsyncCallbackJsonWebHandler("/api/restore",
        [](AsyncWebServerRequest* req, JsonVariant& json) {
            JsonObject obj = json.as<JsonObject>();
            const char* files[] = {"base", "mqtt", "tg", "boiler", "device"};
            for (auto name : files) {
                if (obj[name].is<JsonObject>()) {
                    JsonDocument part;
                    part.set(obj[name]);
                    ConfigManager::saveJson((String("/") + name + ".json").c_str(), part);
                }
            }
            // Перечитываем base сразу
            ConfigManager::loadBase();
            req->send(200, "application/json", "{\"status\":\"ok\"}");
            xSemaphoreTake(coreMutex, portMAX_DELAY);
            sysState.pendingReboot = true;
            sysState.rebootAt = millis() + 1000;
            xSemaphoreGive(coreMutex);
        }
    );
    server.addHandler(restoreH);

    // ── OTA Update ────────────────────────────────────────────
#ifdef MODULE_OTA
    server.on("/update", HTTP_POST,
        [](AsyncWebServerRequest* req) {
            bool fail = Update.hasError();
            auto* resp = req->beginResponse(200, "text/plain", fail ? "FAIL" : "OK");
            resp->addHeader("Connection", "close");
            req->send(resp);
            if (!fail) {
                xSemaphoreTake(coreMutex, portMAX_DELAY);
                sysState.pendingReboot = true;
                sysState.rebootAt = millis();
                xSemaphoreGive(coreMutex);
            }
        },
        handleOtaUpload
    );
#endif

    // ── Captive portal 404 ────────────────────────────────────
    server.onNotFound([](AsyncWebServerRequest* req) {
        bool ap = false;
        xSemaphoreTake(coreMutex, portMAX_DELAY);
        ap = sysState.apMode;
        xSemaphoreGive(coreMutex);

        if (ap) req->redirect("http://192.168.4.1");
        else    req->send(404, "text/plain", "Not Found");
    });

    // ── Статика (index.html + assets) ─────────────────────────
    // Фильтр: отдаём статику только если путь НЕ начинается с /api/
    server.serveStatic("/", LittleFS, "/")
          .setDefaultFile("index.html")
          .setFilter([](AsyncWebServerRequest* req) {
              return !req->url().startsWith("/api/");
          });
}

void WebHandler::handleOtaUpload(AsyncWebServerRequest* req, String filename,
                                  size_t index, uint8_t* data, size_t len, bool final) {
    if (!index) {
        int cmd = (filename.indexOf("littlefs") > -1 || filename.indexOf("spiffs") > -1)
                  ? U_SPIFFS : U_FLASH;
        Serial.printf("[OTA] Start: %s (%s)\n", filename.c_str(),
                      cmd == U_FLASH ? "firmware" : "filesystem");
        if (!Update.begin(UPDATE_SIZE_UNKNOWN, cmd)) Update.printError(Serial);
    }
    if (Update.write(data, len) != len) Update.printError(Serial);
    if (final) {
        if (Update.end(true)) Serial.printf("[OTA] Done: %u bytes\n", index + len);
        else Update.printError(Serial);
    }
}
