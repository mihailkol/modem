#include "EkoNomDevice.h"
#include "../../modules/rs485/Rs485Handler.h"

// ── Глобальные данные ────────────────────────────────────────────────────────
EkoNomData ekoNomData;

// ── Статика ──────────────────────────────────────────────────────────────────
EkoNomDevice::State    EkoNomDevice::_state        = IDLE;
uint32_t               EkoNomDevice::_lastPollMs   = 0;
uint32_t               EkoNomDevice::_sentMs       = 0;
uint8_t                EkoNomDevice::_pollInterval = 30;

// ── Запросы (CRC предрассчитан и проверен на реальном приборе) ───────────────
// R1: 01 03 00 00 00 0C 45 CF  — рег 0x00-0x0B: энергия, темп., дельта, объём
// R2: 01 03 00 0C 00 02 04 08  — рег 0x0C-0x0D: расход
// R3: 01 03 00 0E 00 02 A5 C8  — рег 0x0E-0x0F: мощность
static const uint8_t REQ_R1[] = {0x01,0x03,0x00,0x00,0x00,0x0C,0x45,0xCF};
static const uint8_t REQ_R2[] = {0x01,0x03,0x00,0x0C,0x00,0x02,0x04,0x08};
static const uint8_t REQ_R3[] = {0x01,0x03,0x00,0x0E,0x00,0x02,0xA5,0xC8};

static const uint16_t TIMEOUT_MS = 1000;

// ── Вспомогательные ──────────────────────────────────────────────────────────
uint32_t EkoNomDevice::_reg32(const uint8_t* d, int offset) {
    return ((uint32_t)d[offset]   << 24)
         | ((uint32_t)d[offset+1] << 16)
         | ((uint32_t)d[offset+2] <<  8)
         |  (uint32_t)d[offset+3];
}

// ── Парсинг ──────────────────────────────────────────────────────────────────
// R1 ответ: 01 03 18 [24 байта] CRC CRC
// Раскладка по регистрам (каждый параметр = 2 регистра = 4 байта):
//   offset 3:  0x00-0x01 — энергия       (÷100 кВт·ч)
//   offset 7:  0x02-0x03 — зарезервировано
//   offset 11: 0x04-0x05 — T подачи      (÷100 °C)
//   offset 15: 0x06-0x07 — T обратки     (÷100 °C)
//   offset 19: 0x08-0x09 — ΔT            (÷100 °C)
//   offset 23: 0x0A-0x0B — объём         (÷100 м³)
bool EkoNomDevice::_parseR1(const uint8_t* d, size_t len) {
    if (len < 27) return false;
    if (d[0] != 0x01 || d[1] != 0x03 || d[2] != 0x18) return false;
    ekoNomData.energy_kwh = _reg32(d,  3) / 100.0f;
    ekoNomData.t_supply   = _reg32(d, 11) / 100.0f;
    ekoNomData.t_return   = _reg32(d, 15) / 100.0f;
    ekoNomData.t_delta    = _reg32(d, 19) / 100.0f;
    ekoNomData.volume_m3  = _reg32(d, 23) / 100.0f;
    return true;
}

// R2 ответ: 01 03 04 [4 байта] CRC CRC — расход (÷10000 м³/ч)
bool EkoNomDevice::_parseR2(const uint8_t* d, size_t len) {
    if (len < 9) return false;
    if (d[0] != 0x01 || d[1] != 0x03 || d[2] != 0x04) return false;
    ekoNomData.flow_m3h = _reg32(d, 3) / 10000.0f;
    return true;
}

// R3 ответ: 01 03 04 [4 байта] CRC CRC — мощность (÷100 кВт)
bool EkoNomDevice::_parseR3(const uint8_t* d, size_t len) {
    if (len < 9) return false;
    if (d[0] != 0x01 || d[1] != 0x03 || d[2] != 0x04) return false;
    ekoNomData.power_kw = _reg32(d, 3) / 100.0f;
    return true;
}

// ── Callback от Rs485Handler ─────────────────────────────────────────────────
void EkoNomDevice::_onFrame(const uint8_t* data, size_t len) {
    switch (_state) {
        case WAIT_R1:
            if (_parseR1(data, len)) {
                _state = WAIT_R2;
                Rs485Handler::sendBytes(REQ_R2, sizeof(REQ_R2));
                _sentMs = millis();
            } else {
                Serial.printf("[EKONOM] R1 parse error, len=%d\n", len);
                _state = IDLE;
            }
            break;

        case WAIT_R2:
            if (_parseR2(data, len)) {
                _state = WAIT_R3;
                Rs485Handler::sendBytes(REQ_R3, sizeof(REQ_R3));
                _sentMs = millis();
            } else {
                Serial.printf("[EKONOM] R2 parse error, len=%d\n", len);
                _state = IDLE;
            }
            break;

        case WAIT_R3:
            if (_parseR3(data, len)) {
                ekoNomData.valid      = true;
                ekoNomData.updated_ms = millis();
                Serial.printf("[EKONOM] Ts=%.2f Tr=%.2f dT=%.2f P=%.2f kW F=%.4f m3/h E=%.2f kWh\n",
                    ekoNomData.t_supply, ekoNomData.t_return, ekoNomData.t_delta,
                    ekoNomData.power_kw, ekoNomData.flow_m3h, ekoNomData.energy_kwh);
            } else {
                Serial.printf("[EKONOM] R3 parse error, len=%d\n", len);
            }
            _state = IDLE;
            break;

        default:
            break;
    }
}

// ── INIT ─────────────────────────────────────────────────────────────────────
void EkoNomDevice::init(uint8_t pollIntervalSec) {
    _pollInterval = pollIntervalSec;
    Rs485Handler::onFrame = _onFrame;
    Serial.printf("[EKONOM] Init OK, poll every %ds\n", _pollInterval);
}

// ── LOOP ─────────────────────────────────────────────────────────────────────
void EkoNomDevice::loop() {
    uint32_t now = millis();

    // Таймаут ожидания ответа
    if (_state != IDLE && (now - _sentMs) > TIMEOUT_MS) {
        Serial.printf("[EKONOM] Timeout in state %d\n", (int)_state);
        _state = IDLE;
    }

    // Запуск нового цикла опроса
    if (_state == IDLE && (now - _lastPollMs) >= (uint32_t)_pollInterval * 1000) {
        _lastPollMs = now;
        _state = WAIT_R1;
        Rs485Handler::sendBytes(REQ_R1, sizeof(REQ_R1));
        _sentMs = now;
    }
}