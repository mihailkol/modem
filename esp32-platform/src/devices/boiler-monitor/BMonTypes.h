#pragma once
#ifdef DEVICE_BOILER_MONITOR

#include <Arduino.h>

// ============================================================
//  ПИНЫ ПЛАТЫ WT32-ETH01 + шильд boiler-monitor
// ============================================================
#define BMON_PIN_SDA        33
#define BMON_PIN_SCL        32
#define BMON_PIN_FLOW1       2   // расходомер 1 (оптопара)
#define BMON_PIN_FLOW2       4   // расходомер 2 (оптопара)
#define BMON_PIN_PRESS1     36   // давление P1 4-20мА (input only)
#define BMON_PIN_PRESS2     39   // давление P2 4-20мА (input only)

// ETH WT32-ETH01
#define BMON_ETH_ADDR        1
#define BMON_ETH_POWER_PIN  16
#define BMON_ETH_MDC_PIN    23
#define BMON_ETH_MDIO_PIN   18

// ============================================================
//  ADS1115
// ============================================================
#define BMON_ADS_ADDR       0x48  // ADDR → GND
#define BMON_ADS_GAIN       GAIN_ONE  // ±4.096V — достаточно для 3.3V

// ============================================================
//  NTC — B-параметрическая модель
//  R0=10k при T0=25°C, Rref=10k (резистор делителя)
// ============================================================
#define BMON_NTC_B          3950.0f
#define BMON_NTC_R0         10000.0f
#define BMON_NTC_T0         298.15f   // 25°C в Кельвинах
#define BMON_NTC_RREF       10000.0f
#define BMON_ADS_COUNTS     32767.0f  // макс. значение ADS1115 при GAIN_ONE

// ============================================================
//  Давление 4-20мА / 150 Ом
//  U_min = 0.004 * 150 = 0.6V
//  U_max = 0.020 * 150 = 3.0V
//  Диапазон датчика: 0–10 бар
// ============================================================
#define BMON_PRESS_U_MIN    0.6f
#define BMON_PRESS_U_MAX    3.0f
#define BMON_PRESS_BAR_MAX  10.0f
#define BMON_ADC_VREF       3.3f
#define BMON_ADC_BITS       4095.0f   // ESP32 ADC 12-bit

// ============================================================
//  КОНФИГ — уставки, сохраняются в /device.json
// ============================================================
struct BMonConfig {
    // Расходомеры: литров на импульс
    float flow1_lpi     = 1.0f;   // л/имп расходомер 1
    float flow2_lpi     = 1.0f;   // л/имп расходомер 2

    // Пороги аварий
    float t_max         = 85.0f;  // аварийное превышение температуры подачи
    float t_alarm       = 80.0f;  // порог оповещения
    float t_min         = 5.0f;   // защита от замерзания (улица)

    // Интервал публикации MQTT (мс)
    uint32_t pub_interval = 10000;

    // Коэффициент B для NTC (можно уточнить под конкретный датчик)
    float ntc_b         = BMON_NTC_B;
};

// ============================================================
//  СОСТОЯНИЕ — runtime, не сохраняется
// ============================================================

#define BMON_HISTORY_SIZE   60   // 60 × 10 сек = 10 минут

struct BMonState {
    // Температуры (°C), NaN = датчик не подключён
    float t_supply      = NAN;   // NTC1 — подача
    float t_return      = NAN;   // NTC2 — обратка
    float t_room        = NAN;   // NTC3 — помещение
    float t_outdoor     = NAN;   // NTC4 — улица
    float t_delta       = 0.0f;  // подача - обратка

    // Давление (бар)
    float p_supply      = 0.0f;  // P1
    float p_return      = 0.0f;  // P2
    float p_delta       = 0.0f;  // P1 - P2

    // Расход и мощность
    float flow1_lpm     = 0.0f;  // л/мин расходомер 1
    float flow2_lpm     = 0.0f;  // л/мин расходомер 2
    float power_kw      = 0.0f;  // тепловая мощность по расходомеру 1
    float energy_kwh    = 0.0f;  // накопленная энергия за сессию

    // Аварии
    bool alarm_high     = false;
    bool alarm_low      = false;  // защита от замерзания
    bool alarm_sensor   = false;  // ошибка чтения ADS1115

    // Счётчики импульсов (volatile — из ISR)
    volatile uint32_t pulse1_count = 0;
    volatile uint32_t pulse2_count = 0;

    // История мощности (кольцевой буфер)
    float    hist_power[BMON_HISTORY_SIZE] = {};
    uint8_t  hist_head  = 0;
    uint8_t  hist_count = 0;

    void pushHistory(float power) {
        hist_power[hist_head] = power;
        hist_head = (hist_head + 1) % BMON_HISTORY_SIZE;
        if (hist_count < BMON_HISTORY_SIZE) hist_count++;
    }
};

extern BMonConfig bmonCfg;
extern BMonState  bmonState;

#endif // DEVICE_BOILER_MONITOR