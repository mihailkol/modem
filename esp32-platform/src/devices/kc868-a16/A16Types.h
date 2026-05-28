#pragma once
#ifdef DEVICE_KC868_A16

#include <Arduino.h>

// ============================================================
//  ПИНЫ KC868-A16
// ============================================================
// ============================================================
//  ПИНЫ KC868-A16 (проверено по реальному проекту)
// ============================================================

// Ethernet — переопределяем дефолты platformio.ini
#define A16_ETH_ADDR      0
#define A16_ETH_POWER_PIN -1                    // PHY питание не управляется
#define A16_ETH_MDC_PIN   23
#define A16_ETH_MDIO_PIN  18
#define A16_ETH_CLK_MODE  ETH_CLOCK_GPIO17_OUT  // важно для Kincony!

// I2C
#define A16_I2C_SDA     4
#define A16_I2C_SCL     5    // не 16!

// Аналоговые входы
#define A16_ADC_CH1     36   // 4-20мА вход 1
#define A16_ADC_CH2     39   // 4-20мА вход 2
#define A16_ADC_CH3     34   // 0-5В вход 3
#define A16_ADC_CH4     35   // 0-5В вход 4

// 1-Wire (DS18B20)
#define A16_OW_PIN1     33   // HT1
#define A16_OW_PIN2     32   // HT2
#define A16_OW_PIN3     14   // HT3

// RS485
#define A16_RS485_RX    16
#define A16_RS485_TX    13

// PCF8574 адреса
#define A16_PCF_DI_0    0x22   // IN1-IN8  (входы)
#define A16_PCF_DI_1    0x21   // IN9-IN16 (входы)
#define A16_PCF_OUT_0   0x24   // OT1-OT8  (выходы/реле)
#define A16_PCF_OUT_1   0x25   // OT9-OT16 (выходы/реле)

// ============================================================
//  ТИПЫ АНАЛОГОВЫХ ВХОДОВ
// ============================================================
enum class AnalogMode : uint8_t {
    MODE_4_20MA,   // 4-20мА → 0-100%
    MODE_0_5V,     // 0-5В → сырое значение
    MODE_RAW       // 0-4095 АЦП без конвертации
};


// ============================================================
//  КОНФИГ A16 — сохраняется в /device.json
// ============================================================
#define A16_MAX_OW_SENSORS  8

struct A16Config {
    // Метки входов (для отображения в UI)
    char din_label[16][20];
    char dout_label[16][20];
    char adc_label[4][20];
    char ow_label[A16_MAX_OW_SENSORS][20];

    // Режимы аналоговых входов
    AnalogMode adc_mode[4] = {
        AnalogMode::MODE_4_20MA,
        AnalogMode::MODE_4_20MA,
        AnalogMode::MODE_0_5V,
        AnalogMode::MODE_0_5V
    };

    // Масштаб аналоговых входов (min/max физической величины)
    float adc_min[4] = {0, 0, 0, 0};
    float adc_max[4] = {10, 10, 5, 5};   // бар, бар, В, В

    // Интервал опроса (мс)
    uint32_t poll_interval = 1000;
};

// ============================================================
//  СОСТОЯНИЕ A16 — runtime
// ============================================================
struct A16State {
    // Цифровые входы
    bool     din[16]          = {};
    uint32_t din_counter[16]  = {};   // счётчики импульсов
    bool     din_prev[16]     = {};   // для детекции фронта

    // Цифровые выходы
    bool dout[16] = {};

    // Аналоговые входы
    int   adc_raw[4]   = {};
    float adc_ma[4]    = {};   
    float adc_value[4] = {};   

    // Буфер скользящего среднего для 4-20мА (каналы 0 и 1)
    static const uint8_t ADC_MA_BUF = 10;
    int   adc_ma_buf[2][10] = {};
    uint8_t adc_ma_idx[2]   = {};
    bool    adc_ma_full[2]  = {};

    // 1-Wire датчики
    uint8_t  ow_count = 0;
    uint64_t ow_addr[A16_MAX_OW_SENSORS]  = {};   // 64-бит ROM адрес
    float    ow_temp[A16_MAX_OW_SENSORS]  = {};
    bool     ow_ok[A16_MAX_OW_SENSORS]    = {};
};

extern A16Config a16Cfg;
extern A16State  a16State;

#endif // DEVICE_KC868_A16
