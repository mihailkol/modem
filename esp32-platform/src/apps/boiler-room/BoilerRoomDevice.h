#pragma once
#if defined(DEVICE_KC868_A16) && defined(APP_BOILER_ROOM)

#include "BoilerRoomApp.h"

class BoilerRoomDevice {
public:
    static void init();
    static void loop();
    static bool loadConfig();
    static bool saveConfig();

private:
    static void _updateFromA16();       // давление + импульсы
    static void _updateFromEkoNom();    // теплосчётчик
    static void _calcRates(uint32_t dt_ms);        // расходы по счётчикам
    static void _publishMqtt();

    static uint32_t _lastCalc;
    static uint32_t _lastPub;
};

#endif