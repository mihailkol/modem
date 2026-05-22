#pragma once
#if defined(DEVICE_KC868_A16) && defined(APP_BOILER)

#include "BoilerApp.h"

class BoilerAppDevice {
public:
    static void init();
    static void loop();
    static bool loadConfig();
    static bool saveConfig();

private:
    static void updateFromA16();   // читает данные из a16State
    static void calcPower();
    static void checkAlarms();
    static void publishMqtt();

    static uint32_t _lastCalc;
    static uint32_t _lastHistory;
    static uint32_t _lastPub;

    static void _updateAnalytics();

    #ifdef DEVICE_EKONOM
        static void updateFromEkoNom();
    #endif
};

#endif
