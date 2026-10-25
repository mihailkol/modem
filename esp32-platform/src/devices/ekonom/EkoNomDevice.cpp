#ifdef DEVICE_EKONOM
#include "EkoNomDevice.h"
#include "Rs485Handler.h"
#include "Bus.h"

EkoNomDevice::State EkoNomDevice::_state        = IDLE;
uint32_t             EkoNomDevice::_lastPollMs   = 0;
uint32_t             EkoNomDevice::_sentMs       = 0;
uint8_t              EkoNomDevice::_pollInterval = 30;

// Запросы (CRC предрассчитан и проверен на реальном приборе, см. esp32-platform)
// R1: рег 0x00-0x0B — энергия, темп. подачи/обратки, дельта, объём
// R2: рег 0x0C-0x0D — расход
// R3: рег 0x0E-0x0F — мощность
static const uint8_t REQ_R1[] = {0x01,0x03,0x00,0x00,0x00,0x0C,0x45,0xCF};
static const uint8_t REQ_R2[] = {0x01,0x03,0x00,0x0C,0x00,0x02,0x04,0x08};
static const uint8_t REQ_R3[] = {0x01,0x03,0x00,0x0E,0x00,0x02,0xA5,0xC8};

static const uint16_t TIMEOUT_MS = 1000;

// Промежуточные значения одного цикла опроса (до финальной записи в Bus,
// чтобы не публиковать частично обновлённый набор при ошибке в середине цикла)
static float _energy_kwh, _t_supply, _t_return, _t_delta, _volume_m3, _flow_m3h, _power_kw;

uint32_t EkoNomDevice::_reg32(const uint8_t* d, int offset) {
    return ((uint32_t)d[offset]   << 24)
         | ((uint32_t)d[offset+1] << 16)
         | ((uint32_t)d[offset+2] <<  8)
         |  (uint32_t)d[offset+3];
}

// R1 ответ: 01 03 18 [24 байта] CRC CRC
bool EkoNomDevice::_parseR1(const uint8_t* d, size_t len) {
    if (len < 27) return false;
    if (d[0] != 0x01 || d[1] != 0x03 || d[2] != 0x18) return false;
    _energy_kwh = _reg32(d,  3) / 100.0f;
    _t_supply   = _reg32(d, 11) / 100.0f;
    _t_return   = _reg32(d, 15) / 100.0f;
    _t_delta    = _reg32(d, 19) / 100.0f;
    _volume_m3  = _reg32(d, 23) / 100.0f;
    return true;
}

// R2 ответ: 01 03 04 [4 байта] CRC CRC — расход (÷10000 м³/ч)
bool EkoNomDevice::_parseR2(const uint8_t* d, size_t len) {
    if (len < 9) return false;
    if (d[0] != 0x01 || d[1] != 0x03 || d[2] != 0x04) return false;
    _flow_m3h = _reg32(d, 3) / 10000.0f;
    return true;
}

// R3 ответ: 01 03 04 [4 байта] CRC CRC — мощность (÷100 кВт)
bool EkoNomDevice::_parseR3(const uint8_t* d, size_t len) {
    if (len < 9) return false;
    if (d[0] != 0x01 || d[1] != 0x03 || d[2] != 0x04) return false;
    _power_kw = _reg32(d, 3) / 100.0f;
    return true;
}

void EkoNomDevice::_onFrame(const uint8_t* data, size_t len) {
    switch (_state) {
        case WAIT_R1:
            if (_parseR1(data, len)) {
                _state = WAIT_R2;
                Rs485Handler::sendBytes(REQ_R2, sizeof(REQ_R2));
                _sentMs = millis();
            } else {
                Serial.printf("[EKONOM] R1 parse error, len=%d\n", (int)len);
                _state = IDLE;
            }
            break;

        case WAIT_R2:
            if (_parseR2(data, len)) {
                _state = WAIT_R3;
                Rs485Handler::sendBytes(REQ_R3, sizeof(REQ_R3));
                _sentMs = millis();
            } else {
                Serial.printf("[EKONOM] R2 parse error, len=%d\n", (int)len);
                _state = IDLE;
            }
            break;

        case WAIT_R3:
            if (_parseR3(data, len)) {
                // Весь цикл прошёл успешно — публикуем разом
                Bus::write("t_supply",   (int64_t)llround(_t_supply   * 100));
                Bus::write("t_return",   (int64_t)llround(_t_return   * 100));
                Bus::write("t_delta",    (int64_t)llround(_t_delta    * 100));
                Bus::write("flow_m3h",   (int64_t)llround(_flow_m3h   * 100));
                Bus::write("power_kw",   (int64_t)llround(_power_kw   * 100));
                Bus::write("energy_kwh", (int64_t)llround(_energy_kwh * 100));
                Bus::write("volume_m3",  (int64_t)llround(_volume_m3  * 100));

                Serial.printf("[EKONOM] Ts=%.2f Tr=%.2f dT=%.2f P=%.2f kW F=%.4f m3/h E=%.2f kWh\n",
                    _t_supply, _t_return, _t_delta, _power_kw, _flow_m3h, _energy_kwh);
            } else {
                Serial.printf("[EKONOM] R3 parse error, len=%d\n", (int)len);
            }
            _state = IDLE;
            break;

        default:
            break;
    }
}

void EkoNomDevice::init(uint8_t pollIntervalSec) {
    _pollInterval = pollIntervalSec;

    Bus::declare("t_supply", 100);
    Bus::declare("t_return", 100);
    Bus::declare("t_delta", 100);
    Bus::declare("flow_m3h", 100);
    Bus::declare("power_kw", 100);
    Bus::declare("energy_kwh", 100);
    Bus::declare("volume_m3", 100);

    Rs485Handler::onFrame = _onFrame;
    Serial.printf("[EKONOM] Init OK, poll every %ds\n", _pollInterval);
}

void EkoNomDevice::loop() {
    uint32_t now = millis();

    if (_state != IDLE && (now - _sentMs) > TIMEOUT_MS) {
        Serial.printf("[EKONOM] Timeout in state %d\n", (int)_state);
        _state = IDLE;
    }

    if (_state == IDLE && (now - _lastPollMs) >= (uint32_t)_pollInterval * 1000) {
        _lastPollMs = now;
        _state = WAIT_R1;
        Rs485Handler::sendBytes(REQ_R1, sizeof(REQ_R1));
        _sentMs = now;
    }
}

#endif // DEVICE_EKONOM
