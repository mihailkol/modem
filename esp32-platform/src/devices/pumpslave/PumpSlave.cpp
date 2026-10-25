#ifdef DEVICE_PUMPSLAVE
#include "PumpSlave.h"
#include "Bus.h"
#include "../a16/A16Types.h"   // A16_I2C_SDA/SCL — общая шина I2C с PCF8574
#include "CoreTypes.h"
#include "ConfigManager.h"
#include <ArduinoJson.h>
#include <Wire.h>

uint32_t PumpSlave::_lastPoll = 0;
volatile bool     PumpSlave::_selfWrite   = false;
volatile bool     PumpSlave::_touched     = false;
volatile bool     PumpSlave::_dirty       = false;
volatile uint32_t PumpSlave::_dirtyMs     = 0;
bool              PumpSlave::_hasSaved    = false;
bool              PumpSlave::_restoreDone = false;
int64_t           PumpSlave::_saved[2]    = {100, 100};

static const uint32_t POLL_INTERVAL_MS = 500;
// Сохранённую уставку применяем не сразу, а когда плата прожила столько и
// есть сеть (чтобы после перезагрузки насосы успели постоять на 100%, а не
// остановились из-за старого значения, пока плата недоступна).
static const uint32_t RESTORE_AFTER_MS = 120000;
static const uint32_t SAVE_DEBOUNCE_MS = 5000;
static const char*    PUMPS_FILE       = "/pumps.json";

// "Человеческая" уставка 0-100% пишется в pump1/2_setpoint (0=выкл,
// 100=макс.скорость). Дефолт при старте — 100%: если плата перезагрузилась и
// никто не успел задать уставку (сеть недоступна), насос не должен вставать —
// иначе зимой замёрзнет система отопления.
static const int64_t SAFE_SETPOINT = 100;

void PumpSlave::init() {
    Wire.begin(A16_I2C_SDA, A16_I2C_SCL);  // безопасно вызвать повторно, если A16Device уже начал

    Bus::declare("pump1_setpoint", 1);
    Bus::declare("pump2_setpoint", 1);
    Bus::declare("pump1_manual", 1);
    Bus::declare("pump2_manual", 1);
    Bus::declare("pump1_feedback", 1);
    Bus::declare("pump2_feedback", 1);
    Bus::write("pump1_setpoint", SAFE_SETPOINT);
    Bus::write("pump2_setpoint", SAFE_SETPOINT);
    Bus::write("pump1_manual", 0);
    Bus::write("pump2_manual", 0);

    Bus::declare("raw.ntc0_adc", 1);
    Bus::declare("raw.ntc1_adc", 1);
    Bus::declare("raw.ntc2_adc", 1);
    Bus::declare("raw.ntc3_adc", 1);

    _loadSaved();
    // Подписываемся после собственных записей дефолта выше: любое следующее
    // изменение уставки (веб/MQTT) считаем действием пользователя.
    auto onSet = [](const char*, int64_t) {
        if (_selfWrite) return;
        _touched = true;
        _dirty   = true;
        _dirtyMs = millis();
    };
    Bus::subscribe("pump1_setpoint", onSet);
    Bus::subscribe("pump2_setpoint", onSet);

    Serial.printf("[PUMPSLAVE] Init OK, addr=0x%02X, start setpoint=%d%%, saved=%s\n",
                  PUMPSLAVE_I2C_ADDR, (int)SAFE_SETPOINT, _hasSaved ? "yes" : "no");
}

void PumpSlave::_loadSaved() {
    JsonDocument doc;
    if (!ConfigManager::loadJson(PUMPS_FILE, doc)) return;
    if (!doc["p1"].is<int>() || !doc["p2"].is<int>()) return;
    _saved[0] = constrain((int)doc["p1"], 0, 100);
    _saved[1] = constrain((int)doc["p2"], 0, 100);
    _hasSaved = true;
}

void PumpSlave::_save() {
    JsonDocument doc;
    doc["p1"] = (int)Bus::read("pump1_setpoint");
    doc["p2"] = (int)Bus::read("pump2_setpoint");
    if (ConfigManager::saveJson(PUMPS_FILE, doc)) {
        Serial.printf("[PUMPSLAVE] Setpoints saved: %d%% / %d%%\n",
                      (int)doc["p1"], (int)doc["p2"]);
    }
}

void PumpSlave::_persistTick() {
    if (_dirty && millis() - _dirtyMs > SAVE_DEBOUNCE_MS) {
        _dirty = false;
        _save();
    }

    if (_restoreDone) return;
    if (_touched) { _restoreDone = true; return; }       // пользователь уже управляет сам
    if (!_hasSaved) { _restoreDone = true; return; }     // нечего восстанавливать — остаёмся на 100%
    if (millis() < RESTORE_AFTER_MS) return;

    xSemaphoreTake(coreMutex, portMAX_DELAY);
    bool netUp = sysState.ethConnected || sysState.wifiConnected;
    xSemaphoreGive(coreMutex);
    if (!netUp) return;                                  // сети нет — держим 100%

    _selfWrite = true;
    Bus::write("pump1_setpoint", _saved[0]);
    Bus::write("pump2_setpoint", _saved[1]);
    _selfWrite = false;
    _restoreDone = true;
    Serial.printf("[PUMPSLAVE] Restored saved setpoints: %d%% / %d%%\n",
                  (int)_saved[0], (int)_saved[1]);
}

// Реальная характеристика насоса (проверено на железе при вводе в
// эксплуатацию, см. чат) — ШИМ инверсная и нелинейная:
//   RAW 10        → максимальная скорость
//   RAW 84-91     → минимальная скорость
//   RAW 95+       → насос останавливается
//   RAW 0         → насос переходит в свой ручной режим (игнорирует ШИМ)
// Прошивку Pro Mini не трогаем (см. PumpSlave.h), поэтому маппинг из
// удобной шкалы 0-100% в этот RAW-диапазон — здесь, на границе перед I2C.
// userPct<=0 → RAW 96 (стоп, чуть выше порога 95 для запаса); userPct 1..100
// линейно от RAW 85 (минимальная скорость) до RAW 10 (максимальная).
uint8_t PumpSlave::_mapSetpointToRaw(int64_t userPct) {
    if (userPct <= 0) return 96;
    if (userPct > 100) userPct = 100;
    long raw = 85 - ((userPct - 1) * 75 + 49) / 99;  // +49 — округление к ближайшему
    if (raw < 10) raw = 10;
    if (raw > 85) raw = 85;
    return (uint8_t)raw;
}

void PumpSlave::loop() {
    _persistTick();
    if (millis() - _lastPoll < POLL_INTERVAL_MS) return;
    _lastPoll = millis();
    _poll();
}

void PumpSlave::_poll() {
    // ── Запись уставок ──────────────────────────────────────────
    // "Ручной" чекбокс имеет приоритет над уставкой — шлём RAW=0, насос
    // переходит в свой встроенный ручной режим (см. _mapSetpointToRaw()).
    uint8_t duty1 = Bus::read("pump1_manual")
        ? 0 : _mapSetpointToRaw(constrain((int)Bus::read("pump1_setpoint"), 0, 100));
    uint8_t duty2 = Bus::read("pump2_manual")
        ? 0 : _mapSetpointToRaw(constrain((int)Bus::read("pump2_setpoint"), 0, 100));

    Wire.beginTransmission(PUMPSLAVE_I2C_ADDR);
    Wire.write(duty1);
    Wire.write(duty2);
    if (Wire.endTransmission() != 0) {
        Serial.println("[PUMPSLAVE] I2C write failed");
        return;
    }

    // ── Чтение feedback + NTC ────────────────────────────────────
    uint8_t rx[10];
    if (Wire.requestFrom(PUMPSLAVE_I2C_ADDR, (uint8_t)10) != 10) {
        Serial.println("[PUMPSLAVE] I2C read failed");
        return;
    }
    for (int i = 0; i < 10; i++) rx[i] = Wire.read();

    uint8_t fb1 = rx[0];
    uint8_t fb2 = rx[1];
    uint16_t ntc[4];
    for (int i = 0; i < 4; i++) {
        ntc[i] = ((uint16_t)rx[2 + i * 2] << 8) | rx[2 + i * 2 + 1];
    }

    // Фидбек (жёлтый провод) — по даташиту Shimge APE это потребляемая
    // мощность, паспортный максимум насоса 70Вт (см. PumpSlave.h). Иногда
    // на Pro Mini проскакивают глюки декодирования (~95-100) — physически
    // невозможные значения, похоже на паразитный фронт от наводки на
    // open-collector линии (см. чат при вводе в эксплуатацию). Чинить
    // на самом Pro Mini больнее (ICSP), поэтому просто отбрасываем
    // заведомо невозможные показания здесь — остаётся предыдущее
    // валидное значение на Bus, а не дёрганый "потолок".
    static const uint8_t PUMP_FB_MAX_W = 75;  // 70 паспортных + небольшой запас
    if (fb1 <= PUMP_FB_MAX_W) Bus::write("pump1_feedback", fb1);
    if (fb2 <= PUMP_FB_MAX_W) Bus::write("pump2_feedback", fb2);
    Bus::write("raw.ntc0_adc", ntc[0]);
    Bus::write("raw.ntc1_adc", ntc[1]);
    Bus::write("raw.ntc2_adc", ntc[2]);
    Bus::write("raw.ntc3_adc", ntc[3]);
}

#endif // DEVICE_PUMPSLAVE
