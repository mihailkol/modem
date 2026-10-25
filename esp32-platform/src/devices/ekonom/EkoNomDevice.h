#pragma once
#ifdef DEVICE_EKONOM
// Порт из esp32-platform/src/devices/ekonom/EkoNomDevice.*.
// Отличие: результат пишется прямо в Bus (t_supply, t_return, t_delta,
// flow_m3h, power_kw, energy_kwh, volume_m3), а не в отдельную структуру
// EkoNomData — её больше нет, читатели берут значения из Bus.
#include <Arduino.h>

class EkoNomDevice {
public:
    static void init(uint8_t pollIntervalSec = 30);
    static void loop();

private:
    enum State { IDLE, WAIT_R1, WAIT_R2, WAIT_R3 };
    static State    _state;
    static uint32_t _lastPollMs;
    static uint32_t _sentMs;
    static uint8_t  _pollInterval;

    static void     _onFrame(const uint8_t* data, size_t len);
    static bool     _parseR1(const uint8_t* data, size_t len);
    static bool     _parseR2(const uint8_t* data, size_t len);
    static bool     _parseR3(const uint8_t* data, size_t len);
    static uint32_t _reg32(const uint8_t* d, int offset);
};
#endif // DEVICE_EKONOM
