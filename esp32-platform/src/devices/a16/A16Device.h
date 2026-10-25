#pragma once
#ifdef DEVICE_A16
#include "A16Types.h"
#include "CoreTypes.h"

// Порт из esp32-platform/src/devices/kc868-a16/A16Device.*, сужен под
// котельную (см. A16Types.h). Пишет RAW-значения в Bus (raw.* namespace) —
// калибровку в физические единицы делает apps/boiler-room, см. ТЗ п.7.
//
// Публикуемые raw-каналы:
//   raw.water_pulses  (uint32, накопленные импульсы DI1 с момента загрузки)
//   raw.gas_pulses     (uint32, накопленные импульсы DI2 с момента загрузки)
//   raw.p_heat_adc     (int, 0-4095, сырой ADC-код входа CH1)
//   raw.p_water_adc    (int, 0-4095, сырой ADC-код входа CH2)
//   raw.ow0_temp .. raw.ow{N-1}_temp (float×100, °C, порядок = порядок сканирования)
class A16Device {
public:
    static void init();
    static void loop();
    static bool loadConfig();
    static bool saveConfig();

    // Управление реле — не используется котельной сейчас, задел на будущее
    static bool setOutput(uint8_t ch, bool state);
    static bool toggleOutput(uint8_t ch);

    static void scanOneWire();

private:
    static void readDigitalInputs();
    static void readAnalog();
    static void requestOneWire();
    static void finishOneWire();
    static void writePCF(uint8_t addr, uint8_t data);
    static uint8_t readPCF(uint8_t addr);
    static void syncOutputs();

    static uint32_t _lastPoll;
    static uint32_t _lastOwRead;
    static bool     _owScanned;
    static bool     _owConversionPending;
    static uint32_t _owRequestMs;
};
#endif // DEVICE_A16
