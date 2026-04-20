#include "ConfigManager.h"

BaseConfig   baseCfg;
SystemState  sysState;
SemaphoreHandle_t coreMutex = xSemaphoreCreateMutex();

void ConfigManager::begin() {
    if (!LittleFS.begin(true)) {
        Serial.println("[CFG] LittleFS mount failed!");
        return;
    }
    Serial.println("[CFG] LittleFS OK");
}

bool ConfigManager::loadJson(const char* path, JsonDocument& doc) {
    if (!LittleFS.exists(path)) return false;
    File f = LittleFS.open(path, "r");
    if (!f) return false;
    DeserializationError err = deserializeJson(doc, f);
    f.close();
    return !err;
}

bool ConfigManager::saveJson(const char* path, JsonDocument& doc) {
    File f = LittleFS.open(path, "w");
    if (!f) return false;
    bool ok = serializeJson(doc, f) > 0;
    f.close();
    return ok;
}

bool ConfigManager::loadBase() {
    JsonDocument doc;
    if (!loadJson("/base.json", doc)) return false;

    baseCfg.eth_dhcp = doc["eth_dhcp"] | true;
    strlcpy(baseCfg.eth_ip,       doc["eth_ip"]   | "192.168.1.100", sizeof(baseCfg.eth_ip));
    strlcpy(baseCfg.eth_mask,     doc["eth_mask"] | "255.255.255.0", sizeof(baseCfg.eth_mask));
    strlcpy(baseCfg.eth_gw,       doc["eth_gw"]   | "192.168.1.1",  sizeof(baseCfg.eth_gw));
    strlcpy(baseCfg.eth_dns,      doc["eth_dns"]  | "8.8.8.8",      sizeof(baseCfg.eth_dns));
    strlcpy(baseCfg.wifi_ssid,    doc["wifi_ssid"]| "",              sizeof(baseCfg.wifi_ssid));
    strlcpy(baseCfg.wifi_pass,    doc["wifi_pass"]| "",              sizeof(baseCfg.wifi_pass));
    strlcpy(baseCfg.web_user,     doc["web_user"] | "admin",         sizeof(baseCfg.web_user));
    strlcpy(baseCfg.web_pass,     doc["web_pass"] | "",              sizeof(baseCfg.web_pass));
    strlcpy(baseCfg.device_name,  doc["device_name"] | "esp32-device", sizeof(baseCfg.device_name));
    return true;
}

bool ConfigManager::saveBase() {
    JsonDocument doc;
    doc["eth_dhcp"]     = baseCfg.eth_dhcp;
    doc["eth_ip"]       = baseCfg.eth_ip;
    doc["eth_mask"]     = baseCfg.eth_mask;
    doc["eth_gw"]       = baseCfg.eth_gw;
    doc["eth_dns"]      = baseCfg.eth_dns;
    doc["wifi_ssid"]    = baseCfg.wifi_ssid;
    doc["wifi_pass"]    = baseCfg.wifi_pass;
    doc["web_user"]     = baseCfg.web_user;
    doc["web_pass"]     = baseCfg.web_pass;
    doc["device_name"]  = baseCfg.device_name;
    return saveJson("/base.json", doc);
}
