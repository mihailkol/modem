#pragma once
#ifdef DEVICE_BOILER

#include <Arduino.h>

// ============================================================
//  КОНФИГ КОТЛА — уставки, сохраняются в /device.json
// ============================================================
struct BoilerConfig {
    float t_max         = 85.0f;   // Аварийное отключение
    float t_alarm       = 80.0f;   // Порог оповещения
    float t_hyst        = 2.0f;    // Гистерезис
    float t_min         = 40.0f;   // Защита от замерзания
    uint32_t pub_interval = 10000; // Интервал MQTT публикации (мс)
};

// ============================================================
//  СОСТОЯНИЕ КОТЛА — runtime, не сохраняется
// ============================================================

#define TEMP_HISTORY_SIZE 60  // 60 × 10сек = 10 минут

struct BoilerState {
    // Температуры
    float t_supply      = 0.0f;   // Подача
    float t_return      = 0.0f;   // Обратка
    float t_room        = 0.0f;   // Помещение
    float t_outdoor     = 0.0f;   // Улица
    float t_delta       = 0.0f;   // Дельта (расчёт)

    // Давление
    float p_supply      = 0.0f;   // Давление подача (бар)
    float p_return      = 0.0f;   // Давление обратка (бар)
    float p_delta       = 0.0f;   // Дельта давления

    // Производительность
    float power_kw      = 0.0f;   // Текущая мощность кВт
    float energy_kwh    = 0.0f;   // Накопленная энергия за сутки кВт·ч
    float flow_lpm      = 0.0f;   // Поток л/мин

    // Аварии
    bool alarm_high     = false;
    bool alarm_low      = false;
    bool alarm_sensor   = false;

    // История мощности (кольцевой буфер)
    float   hist_power[TEMP_HISTORY_SIZE] = {};
    uint32_t hist_ts[TEMP_HISTORY_SIZE]   = {};
    uint8_t  hist_head  = 0;
    uint8_t  hist_count = 0;

    void pushHistory(float power, uint32_t ts) {
        hist_power[hist_head] = power;
        hist_ts[hist_head]    = ts;
        hist_head = (hist_head + 1) % TEMP_HISTORY_SIZE;
        if (hist_count < TEMP_HISTORY_SIZE) hist_count++;
    }
};

extern BoilerConfig boilerCfg;
extern BoilerState  boilerState;

#endif // DEVICE_BOILER
