#pragma once
#ifdef DEVICE_PUMPSLAVE
// Драйвер I2C для Arduino Pro Mini (pump_slave.ino v1.2, адрес 0x30).
// Протокол — см. ТЗ п.2.1 (boiler-room-v2-tz.md), взят из исходника прошивки
// Pro Mini как есть, прошивка Pro Mini НЕ меняется.
//
// Запись (2 байта): [duty насоса 1 0-100, duty насоса 2 0-100] — RAW-скважность
//                    ШИМ, как её понимает сам насос (см. _mapSetpointToRaw()
//                    в .cpp — характеристика насоса нелинейная и инверсная:
//                    10=макс.скорость, 84-91=мин.скорость, 95+=стоп, 0=ручной
//                    режим насоса. Проверено на реальном насосе при вводе
//                    в эксплуатацию, см. чат).
// Чтение (10 байт):  [fb1_duty, fb2_duty, ntc0_hi,lo, ntc1_hi,lo, ntc2_hi,lo, ntc3_hi,lo]
//
// Bus-каналы:
//   pump1_setpoint, pump2_setpoint  — "человеческая" уставка 0-100% (0=выкл,
//                                     100=макс.скорость), пишутся извне
//                                     (MQTT/веб/app). _poll() маппит их в
//                                     RAW перед отправкой по I2C — см. .cpp.
//   pump1_manual, pump2_manual      — чекбокс "ручной режим насоса": если
//                                     включён, вместо уставки на насос
//                                     уходит RAW=0 (свой ручной режим на
//                                     самом насосе), уставка игнорируется.
//   pump1_feedback, pump2_feedback  — RAW duty (0-100) с жёлтого провода
//                                     насоса. ПО ДАТАШИТУ (Shimge APE,
//                                     см. чат) это НЕ скорость, а
//                                     потребляемая мощность: 0-70Вт,
//                                     ~1Вт/1%ШИМ — то есть RAW-значение
//                                     численно ≈ ватты, отображается на
//                                     дашборде как "Потребление, Вт" без
//                                     доп. пересчёта (MON_META в
//                                     BuiltinTabs.cpp). Значения >70 вне
//                                     спеки насоса — вероятно шум
//                                     декодирования на Pro Mini.
//   raw.ntc0_adc .. raw.ntc3_adc    — сырой ADC (0-1023) с Pro Mini;
//                                     перевод в °C и привязка floor1/2
//                                     supply/return — Phase 2 (нет термисторов
//                                     физически, см. ТЗ п.9)
#include <Arduino.h>

class PumpSlave {
public:
    static void init();
    static void loop();

private:
    static void _poll();
    static uint8_t _mapSetpointToRaw(int64_t userPct);
    static void _persistTick();
    static void _loadSaved();
    static void _save();

    static uint32_t _lastPoll;
    static volatile bool     _selfWrite;   // запись в Bus сделана нами, не пользователем
    static volatile bool     _touched;     // уставку уже меняли с момента старта
    static volatile bool     _dirty;       // есть несохранённое изменение
    static volatile uint32_t _dirtyMs;
    static bool              _hasSaved;
    static bool              _restoreDone;
    static int64_t           _saved[2];
};
#endif // DEVICE_PUMPSLAVE
