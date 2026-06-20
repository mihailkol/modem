#include "CrashLog.h"
#include "ConfigManager.h"
#include <ArduinoJson.h>

RTC_NOINIT_ATTR CrashBreadcrumb g_crumb;

static const char* _reasonStr(esp_reset_reason_t r) {
    switch (r) {
        case ESP_RST_POWERON:   return "poweron";
        case ESP_RST_SW:        return "sw_restart";
        case ESP_RST_PANIC:     return "panic";
        case ESP_RST_INT_WDT:   return "int_wdt";
        case ESP_RST_TASK_WDT:  return "task_wdt";
        case ESP_RST_WDT:       return "other_wdt";
        case ESP_RST_BROWNOUT:  return "brownout";
        case ESP_RST_DEEPSLEEP: return "deepsleep";
        default:                return "unknown";
    }
}

void CrashLog::begin() {
    esp_reset_reason_t r = esp_reset_reason();
    bool crumbValid = (g_crumb.magic == CRASH_MAGIC);

    JsonDocument doc;
    doc["reason"] = _reasonStr(r);
    doc["raw"]    = (int)r;
    if (crumbValid) {
        doc["last_tag"]      = g_crumb.lastTag;
        doc["uptime_s"]      = g_crumb.uptime_s;
        doc["free_heap"]     = g_crumb.freeHeap;
        doc["min_free_heap"] = g_crumb.minFreeHeap;
        doc["boot_count"]    = g_crumb.bootCount;
    }

    JsonDocument log;
    if (LittleFS.exists("/crashlog.json"))
        ConfigManager::loadJson("/crashlog.json", log);
    JsonArray arr = log["entries"].is<JsonArray>()
        ? log["entries"].as<JsonArray>() : log["entries"].to<JsonArray>();
    if (arr.size() >= 20) arr.remove(0);
    arr.add(doc);
    ConfigManager::saveJson("/crashlog.json", log);

    Serial.printf("[CRASH] prev reset=%s tag=%u uptime=%us heap=%u min=%u\n",
        _reasonStr(r), g_crumb.lastTag, g_crumb.uptime_s, g_crumb.freeHeap, g_crumb.minFreeHeap);

    if (!crumbValid) g_crumb.minFreeHeap = UINT32_MAX;
    g_crumb.magic    = CRASH_MAGIC;
    g_crumb.uptime_s = 0;
    g_crumb.lastTag  = 0;
    g_crumb.bootCount++;
}

void CrashLog::mark(uint16_t tag) {
    g_crumb.lastTag  = tag;
    g_crumb.uptime_s = millis() / 1000;
    uint32_t heap    = ESP.getFreeHeap();
    g_crumb.freeHeap = heap;
    if (heap < g_crumb.minFreeHeap) g_crumb.minFreeHeap = heap;
}