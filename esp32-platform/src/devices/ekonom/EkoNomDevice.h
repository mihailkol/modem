#pragma once
#ifdef DEVICE_EKONOM

#include <Arduino.h>

// ── Данные теплосчётчика ─────────────────────────────────────────────────────
struct EkoNomData {
    float    energy_kwh  = 0;    // накопленная энергия, кВт·ч
    float    t_supply    = 0;    // температура подачи, °C
    float    t_return    = 0;    // температура обратки, °C
    float    t_delta     = 0;    // разность температур, °C
    float    volume_m3   = 0;    // объём, м³
    float    flow_m3h    = 0;    // расход, м³/ч
    float    power_kw    = 0;    // мощность, кВт
    uint32_t updated_ms  = 0;    // millis() последнего успешного опроса
    bool     valid       = false;
};

extern EkoNomData ekoNomData;

// ── Драйвер ──────────────────────────────────────────────────────────────────
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

    static void    _onFrame(const uint8_t* data, size_t len);
    static bool    _parseR1(const uint8_t* data, size_t len);
    static bool    _parseR2(const uint8_t* data, size_t len);
    static bool    _parseR3(const uint8_t* data, size_t len);
    static uint32_t _reg32(const uint8_t* d, int offset);
};

#endif // DEVICE_EKONOM