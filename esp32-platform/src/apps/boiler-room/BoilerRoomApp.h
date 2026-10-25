#pragma once
#ifdef APP_BOILER_ROOM
// Приложение котельной — "склеивает" raw-данные устройств (A16, PumpSlave)
// в итоговые каналы Bus по ТЗ (п.4), используя калибровки, которыми владеет
// само приложение (см. ТЗ п.8 — отдельный конфиг, редактируется через веб).
//
// Читает:  raw.p_heat_adc, raw.p_water_adc, raw.water_pulses, raw.gas_pulses,
//          raw.ow0_temp..raw.ow{N-1}_temp (1-Wire с A16, см. ТЗ п.9 — привязка
//          "какой индекс это какой физический датчик" не гарантирована
//          порядком сканирования шины, поэтому настраивается здесь),
//          raw.ntc0_adc..raw.ntc3_adc (термисторы на Pro Mini, см. PumpSlave —
//          привязка "какой индекс это какая точка контура" тоже настраивается
//          здесь, а не зашита порядком)
// Пишет:   p_heat, p_water, water_total_m3, water_rate, water_rate_lph,
//          gas_total_m3, gas_rate, t_outdoor, t_indoor,
//          pump1_t_supply, pump1_t_return, pump1_t_delta,
//          pump2_t_supply, pump2_t_return, pump2_t_delta,
//          uptime, free_heap, rssi, mqtt_status, ip_address, fw_version
//
// pump1_t_supply/return, pump2_t_supply/return — температуры подачи/обратки
// двух контуров отопления, считаются из raw.ntc{index}_adc по бета-модели
// термистора (см. ntc_r0_ohm/ntc_beta/ntc_pullup_ohm ниже): делитель
// Vcc-Rpullup-ADC-Rterm-GND, Rterm = Rpullup*raw/(1023-raw), дальше
// 1/T = 1/T0 + (1/Beta)*ln(Rterm/R0), T0=25°C. Одна и та же калибровка
// термистора на все 4 точки (считаем термисторы одинаковыми). Термисторов
// физически ещё нет — коэффициенты дефолтные (типовые 10к/B=3950), проверить
// на реальном железе когда датчики появятся (см. SCAFFOLD.md).
//
// pump{N}_t_delta = pump{N}_t_supply - pump{N}_t_return — диагностика
// контура: близкая к нулю дельта при работающем насосе (feedback>0) обычно
// значит завоздушенность/закрытый клапан/проблему с датчиком, а не глазами
// сравнивать два числа. Публикуется только когда оба конца контура уже
// посчитаны (см. _recalc).
//
// water_total_m3/gas_total_m3 — НЕ производная от raw.water_pulses/raw.gas_pulses
// (те живут только в RAM устройства A16 и обнуляются при каждой перезагрузке).
// Это накопительное "показание счётчика": при старте подгружается из
// /counters.json, дальше наращивается на Δимпульсов на каждом цикле, и
// периодически сохраняется обратно (см. _saveCountersIfDue). Начальное
// значение при вводе в эксплуатацию (когда физический счётчик уже не на
// нуле) задаётся вручную через веб — см. /api/boilerroom/counters/set.
//
// water_rate/gas_rate — усреднённые за настраиваемое окно (не за 1 тик
// _recalc): при слабом напоре и дискретности счётчика 10л/имп. мгновенная
// скорость по одному тику скачет между 0 и "как если бы весь литраж
// протёк за секунду". Значение обновляется только на границе окна,
// внутри окна держится последнее.
#include <Arduino.h>

struct BoilerRoomConfig {
    // Счётчики — коэффициент и окно усреднения расхода
    float    water_m3_per_pulse   = 0.01f;  // м³/импульс (10 л/имп по умолчанию)
    uint16_t water_rate_avg_sec   = 300;    // окно усреднения расхода воды, сек
    float    gas_m3_per_pulse     = 0.01f;  // м³/импульс
    uint16_t gas_rate_avg_sec     = 60;     // окно усреднения расхода газа, сек

    // Калибровка давления (4-20мА -> бар)
    float p_heat_min  = 0.0f;
    float p_heat_max  = 10.0f;
    float p_water_min = 0.0f;
    float p_water_max = 10.0f;

    // Калибровка ADC A16 (raw-код -> мА), общая для обоих 4-20мА входов —
    // это характеристика одного и того же входного каскада платы, не датчика.
    // Дефолты — как было хардкодом в A16Device (esp32-platform), см. SCAFFOLD.md.
    int16_t adc_raw_at_4ma  = 557;
    int16_t adc_raw_at_20ma = 4402;

    // Привязка 1-Wire датчиков (индекс = порядок сканирования шины в A16Device,
    // -1 = датчик не подключен/не публикуется)
    int8_t ow_outdoor_index = 0;
    int8_t ow_indoor_index  = 1;

    // Калибровка термисторов (NTC) на Pro Mini — бета-модель, общая для
    // всех 4 точек (см. заголовок файла). Дефолты — типовой NTC 10к/B3950.
    float ntc_pullup_ohm = 10000.0f;  // подтягивающий резистор делителя, Ом
    float ntc_r0_ohm     = 10000.0f;  // сопротивление термистора при 25°C, Ом
    float ntc_beta       = 3950.0f;   // бета-коэффициент термистора, K

    // Привязка raw.ntc{index}_adc к точкам контуров (-1 = не подключен/
    // не публиковать), аналогично приводке 1-Wire выше.
    int8_t ntc_c1_supply_index = 0;
    int8_t ntc_c1_return_index = 1;
    int8_t ntc_c2_supply_index = 2;
    int8_t ntc_c2_return_index = 3;
};

extern BoilerRoomConfig brrCfg;

class BoilerRoomApp {
public:
    static void init();
    static void loop();
    static bool loadConfig();
    static bool saveConfig();

    // Ручная установка текущих показаний (например при вводе в эксплуатацию,
    // когда физический счётчик уже не на нуле, или для коррекции дрейфа).
    static void setWaterReading(double m3);
    static void setGasReading(double m3);

private:
    static void _recalc();               // давление + счётчики, из raw.*
    static void _publishSystemChannels(); // uptime/free_heap/rssi/mqtt/ip/fw
    static bool _loadCounters();
    static bool _saveCounters();
    static void _saveCountersIfDue();

    static uint32_t _lastCalcMs;
    static uint32_t _lastSysMs;
    static uint32_t _lastCounterSaveMs;
    static uint32_t _lastWaterPulses;
    static uint32_t _lastGasPulses;

    // Накопленные показания — авторитетное значение, живёт поверх raw-импульсов
    static double _waterTotalM3;
    static double _gasTotalM3;
    static bool   _countersDirty;  // выставляется при изменении, сбрасывается после сохранения

    // Окна усреднения расхода (см. заголовок файла)
    static uint32_t _waterWindowStartMs;
    static uint32_t _waterPulsesAtWindowStart;
    static uint32_t _gasWindowStartMs;
    static uint32_t _gasPulsesAtWindowStart;
};
#endif // APP_BOILER_ROOM
