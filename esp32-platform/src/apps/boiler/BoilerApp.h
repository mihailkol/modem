#pragma once
#if defined(DEVICE_KC868_A16) && defined(APP_BOILER)

#include <Arduino.h>

// ============================================================
//  ПРИВЯЗКА ДАТЧИКОВ К КАНАЛАМ A16
// ============================================================
#define BOILER_OW_SUPPLY    0   // HT1 (pin 33) — подача
#define BOILER_OW_RETURN    1   // HT2 (pin 32) — обратка

#define BOILER_ADC_P1       0   // ADC CH1 (pin 36) — давление подачи 4-20мА
#define BOILER_ADC_P2       1   // ADC CH2 (pin 39) — давление обратки 4-20мА

#define BOILER_DIN_FLOW1    0   // DIN1 — импульсный расходомер (сухой контакт)
#define BOILER_DIN_FLOW2    1   // DIN2 — расходомер Холла

// ============================================================
//  КОНФИГ КОТЛА — сохраняется в /boiler.json
// ============================================================
struct BoilerAppConfig {
    // Уставки температуры
    float t_max         = 85.0f;   // Аварийное отключение
    float t_alarm       = 80.0f;   // Порог оповещения
    float t_hyst        = 2.0f;    // Гистерезис
    float t_min         = 40.0f;   // Защита от замерзания

    // Калибровка давления (4-20мА → бар)
    float p_min         = 0.0f;    // 4мА = 0 бар
    float p_max         = 10.0f;   // 20мА = 10 бар

    // Расходомеры — импульсов на литр (настраивается после замера)
    float flow1_liter_per_pulse = 1.0f;   // сухой контакт
    float flow2_liter_per_pulse = 1.0f;   // Холл

    // Интервал публикации MQTT (мс)
    uint32_t pub_interval = 10000;
};

// ============================================================
//  СОСТОЯНИЕ КОТЛА — runtime
// ============================================================
#define BOILER_HIST_SIZE 60  // 60 × 10сек = 10 минут
#define BOILER_HIST1H_SIZE 360  // 360 × 10сек = 1 час
#define BOILER_POWER_ON_KW 0.5f // порог "котёл работает", кВт

struct BoilerAppState {
    // Температуры
    float t_supply  = 0.0f;
    float t_return  = 0.0f;
    float t_delta   = 0.0f;

    // Давление
    float p_supply  = 0.0f;
    float p_return  = 0.0f;
    float p_delta   = 0.0f;

    // Расход
    float flow1_lpm = 0.0f;   // л/мин расходомер 1
    float flow2_lpm = 0.0f;   // л/мин расходомер 2
    float flow_lpm  = 0.0f;   // итоговый (среднее или выбранный)

    // Счётчики импульсов (снапшот для расчёта расхода)
    uint32_t flow1_last = 0;
    uint32_t flow2_last = 0;

    // Мощность
    float power_kw  = 0.0f;
    float energy_kwh = 0.0f;  // суточный накопитель

    // Аварии
    bool alarm_high   = false;
    bool alarm_low    = false;
    bool alarm_sensor = false;

    // История мощности
    float    hist_power[BOILER_HIST_SIZE] = {};
    uint8_t  hist_head  = 0;
    uint8_t  hist_count = 0;

    
    // Аналитика за последний час
    float    hist1h_power[BOILER_HIST1H_SIZE] = {};  // мощность, кВт
    float    hist1h_delta[BOILER_HIST1H_SIZE] = {};  // дельта T, °C
    uint16_t hist1h_head  = 0;
    uint16_t hist1h_count = 0;
 
    // Расчётные показатели (обновляются каждые 10 сек)
    float    power_avg_1h    = 0.0f;  // средняя мощность за час, кВт
    float    power_peak_1h   = 0.0f;  // пиковая мощность за час, кВт
    float    delta_avg_1h    = 0.0f;  // средняя дельта T за час, °C
    uint16_t cycles_1h       = 0;     // количество включений за час
    uint16_t runtime_min     = 0;     // время работы за час, мин
    uint16_t idle_min        = 0;     // время простоя за час, мин
    uint8_t  duty_pct        = 0;     // duty cycle, %
    uint16_t cycle_dur_min   = 0;     // длительность текущего цикла, мин
    bool     burner_on       = false; // котёл сейчас работает
    uint32_t cycle_start_ms  = 0;     // millis() начала текущего цикла
 
    void pushHistory1h(float p, float dt) {
        hist1h_power[hist1h_head] = p;
        hist1h_delta[hist1h_head] = dt;
        hist1h_head = (hist1h_head + 1) % BOILER_HIST1H_SIZE;
        if (hist1h_count < BOILER_HIST1H_SIZE) hist1h_count++;
    }

    void pushHistory(float p) {
        hist_power[hist_head] = p;
        hist_head = (hist_head + 1) % BOILER_HIST_SIZE;
        if (hist_count < BOILER_HIST_SIZE) hist_count++;
    }
};

extern BoilerAppConfig boilerAppCfg;
extern BoilerAppState  boilerAppState;

#endif // DEVICE_KC868_A16 && APP_BOILER
