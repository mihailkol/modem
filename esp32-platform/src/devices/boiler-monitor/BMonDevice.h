#pragma once
#ifdef DEVICE_BOILER_MONITOR

#include "BMonTypes.h"
#include "../../core/CoreTypes.h"

class BMonDevice {
public:
    static void init();
    static void loop();
    static bool loadConfig();
    static bool saveConfig();

private:
    static void readTemperatures();   // ADS1115 → NTC → °C
    static void readPressure();       // GPIO36/39 ADC → бар
    static void calcFlow();           // импульсы → л/мин, мощность
    static void checkAlarms();
    static void publishMqtt();

    static uint32_t _lastRead;        // опрос датчиков
    static uint32_t _lastFlow;        // расчёт расхода
    static uint32_t _lastHistory;     // запись в историю
    static uint32_t _lastPub;         // MQTT публикация

    // Снимок счётчиков для расчёта л/мин
    static uint32_t _pulse1_prev;
    static uint32_t _pulse2_prev;
};

#endif // DEVICE_BOILER_MONITOR