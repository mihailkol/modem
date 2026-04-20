#pragma once
#ifdef DEVICE_BOILER

#include "BoilerTypes.h"
#include "../../core/CoreTypes.h"

class BoilerDevice {
public:
    static void init();   // Регистрирует вкладки, API, инициализирует датчики
    static void loop();   // Опрос датчиков, логика управления
    static bool loadConfig();
    static bool saveConfig();

private:
    static void readSensors();
    static void checkAlarms();
    static void publishMqtt();

    static uint32_t _lastRead;
    static uint32_t _lastPub;
    static uint32_t _lastEnergy;
};

#endif // DEVICE_BOILER
