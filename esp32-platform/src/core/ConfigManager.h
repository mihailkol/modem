#pragma once
// Порт из esp32-platform/src/core/ConfigManager.h без изменений в логике.
#include <LittleFS.h>
#include <ArduinoJson.h>
#include "CoreTypes.h"

class ConfigManager {
public:
    static void begin();

    // Core сеть
    static bool loadBase();
    static bool saveBase();

    // Хелпер: загрузить/сохранить произвольный JSON-файл (используют
    // и модули со своим конфигом — MQTT, BoilerRoomApp, и т.д.)
    static bool loadJson(const char* path, JsonDocument& doc);
    static bool saveJson(const char* path, JsonDocument& doc);
};
