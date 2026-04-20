#pragma once
#include <LittleFS.h>
#include <ArduinoJson.h>
#include "CoreTypes.h"

class ConfigManager {
public:
    static void begin();

    // Core сеть
    static bool loadBase();
    static bool saveBase();

    // Хелпер: загрузить произвольный JSON-файл в JsonDocument
    static bool loadJson(const char* path, JsonDocument& doc);
    static bool saveJson(const char* path, JsonDocument& doc);
};
