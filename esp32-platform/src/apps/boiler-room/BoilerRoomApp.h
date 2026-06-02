#pragma once
#if defined(DEVICE_KC868_A16) && defined(APP_BOILER_ROOM)

#include <Arduino.h>

// ============================================================
//  ПРИВЯЗКА ДАТЧИКОВ К КАНАЛАМ A16
// ============================================================

// Цифровые входы
#define BRR_DIN_WATER       0   // DI1 — счётчик воды (импульсный)
#define BRR_DIN_GAS         1   // DI2 — счётчик газа (импульсный)

// Аналоговые входы (4-20мА)
#define BRR_ADC_P_HEAT      0   // ADC CH1 — давление отопления
#define BRR_ADC_P_WATER     1   // ADC CH2 — давление воды ХВС

// ============================================================
//  КОНФИГ — сохраняется в /boilerroom.json
// ============================================================
struct BoilerRoomConfig {
    // Коэффициенты счётчиков
    float water_m3_per_pulse    = 0.01f;  // м³/импульс
    float gas_m3_per_pulse      = 0.01f;  // м³/импульс

    // Калибровка давления (4-20мА → бар)
    float p_heat_min            = 0.0f;   // 4мА = 0 бар
    float p_heat_max            = 10.0f;  // 20мА = 10 бар
    float p_water_min           = 0.0f;
    float p_water_max           = 10.0f;

    // Интервал публикации MQTT (мс)
    uint32_t pub_interval       = 10000;
};

// ============================================================
//  СОСТОЯНИЕ — runtime
// ============================================================
struct BoilerRoomState {
    // Теплосчётчик (из EkoNomDevice)
    float    heat_t_supply   = 0.0f;   // температура подачи, °C
    float    heat_t_return   = 0.0f;   // температура обратки, °C
    float    heat_t_delta    = 0.0f;   // разность, °C
    float    heat_flow_m3h   = 0.0f;   // расход теплоносителя, м³/ч
    float    heat_power_kw   = 0.0f;   // тепловая мощность, кВт
    float    heat_energy_kwh = 0.0f;   // накопленная энергия, кВт·ч
    float    heat_volume_m3  = 0.0f;   // накопленный объём, м³
    bool     heat_valid      = false;  // данные актуальны
    uint32_t heat_updated_ms = 0;      // millis() последнего обновления

    // Давление отопления
    float    p_heat          = 0.0f;   // бар

    // Давление воды ХВС
    float    p_water         = 0.0f;   // бар

    // Счётчик воды
    uint32_t water_pulses    = 0;      // импульсы (снапшот последнего цикла)
    uint32_t water_pulses_prev = 0;
    float    water_total_m3  = 0.0f;  // накопленный объём, м³
    float    water_rate_lpm  = 0.0f;  // текущий расход, л/мин

    // Счётчик газа
    uint32_t gas_pulses      = 0;
    uint32_t gas_pulses_prev = 0;
    float    gas_total_m3    = 0.0f;  // накопленный объём, м³
    float    gas_rate_m3h    = 0.0f;  // текущий расход, м³/ч
};

extern BoilerRoomConfig brrCfg;
extern BoilerRoomState  brrState;

#endif // DEVICE_KC868_A16 && APP_BOILER_ROOM