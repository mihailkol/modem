#ifdef DEVICE_A16
#include "A16Device.h"
#include "ConfigManager.h"
#include "WebHandler.h"
#include "Bus.h"
#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>

A16Config a16Cfg;
A16State  a16State;

uint32_t A16Device::_lastPoll           = 0;
uint32_t A16Device::_lastOwRead         = 0;
bool     A16Device::_owScanned          = false;
bool     A16Device::_owConversionPending = false;
uint32_t A16Device::_owRequestMs        = 0;

static OneWire           ow1(A16_OW_PIN1);
static OneWire           ow2(A16_OW_PIN2);
static OneWire           ow3(A16_OW_PIN3);
static DallasTemperature dt1(&ow1);
static DallasTemperature dt2(&ow2);
static DallasTemperature dt3(&ow3);

// ============================================================
//  INIT
// ============================================================
void A16Device::init() {
    loadConfig();

    Wire.begin(A16_I2C_SDA, A16_I2C_SCL);

    // Реле выключены при старте (лог.1 = выключено на PCF8574)
    writePCF(A16_PCF_OUT_0, 0xFF);
    writePCF(A16_PCF_OUT_1, 0xFF);

    dt1.begin(); dt2.begin(); dt3.begin();
    dt1.setWaitForConversion(false);
    dt2.setWaitForConversion(false);
    dt3.setWaitForConversion(false);

    Bus::declare("raw.water_pulses", 1);
    Bus::declare("raw.gas_pulses", 1);
    Bus::declare("raw.p_heat_adc", 1);
    Bus::declare("raw.p_water_adc", 1);
    // raw.ow{i}_temp объявляются динамически после первого scanOneWire()

    // Полное состояние всех 16 DI/DO — для вкладки "Диагностика" (не только
    // вода/газ, которые уже считаются отдельно как raw.water_pulses/
    // raw.gas_pulses). Пригодится при вводе в эксплуатацию новых входов/
    // проверке проводки без serial-монитора.
    for (int i = 0; i < 16; i++) {
        char id[16];
        snprintf(id, sizeof(id), "raw.din%d", i);
        Bus::declare(strdup(id), 1);
        snprintf(id, sizeof(id), "raw.dout%d", i);
        Bus::declare(strdup(id), 1);
        Bus::write(id, 0);  // dout стартует выключенным, см. init() writePCF(0xFF) выше
    }

    // Автосканирование 1-Wire — один раз на первом loop() (см. _owScanned).
    // Если датчик подключили уже после старта — без ребута его не увидит,
    // поэтому даём ручной пересканинг с веба (пригодится при монтаже на
    // месте, чтобы не дёргать питание каждый раз).
    extern AsyncWebServer server;
    server.on("/api/a16/rescan-ow", HTTP_GET, [](AsyncWebServerRequest* req) {
        A16Device::scanOneWire();
        req->send(200, "application/json", "{\"status\":\"ok\",\"found\":" + String(a16State.ow_count) + "}");
    });

    Serial.println("[A16] Device init OK");
}

// ============================================================
//  LOOP
// ============================================================
void A16Device::loop() {
    if (millis() - _lastPoll > a16Cfg.poll_interval) {
        _lastPoll = millis();
        readDigitalInputs();
        readAnalog();
    }

    if (!_owScanned) { scanOneWire(); _owScanned = true; }

    // Неблокирующий опрос DS18B20: запрос конверсии и чтение результата
    // разнесены по времени через millis() вместо delay(100) в одном месте
    // цикла — блокирующая пауза здесь периодически (раз в 5с) не давала
    // ArduinoOTA.handle() выполняться вовремя и срывала заливку прошивки
    // по воздуху на случайном проценте (см. чат при вводе в эксплуатацию).
    if (!_owConversionPending && millis() - _lastOwRead > 5000) {
        _lastOwRead = millis();
        requestOneWire();
    } else if (_owConversionPending && millis() - _owRequestMs >= 750) {
        finishOneWire();
    }
}

// ============================================================
//  ЦИФРОВЫЕ ВХОДЫ (счётчики воды/газа)
// ============================================================
void A16Device::readDigitalInputs() {
    uint8_t byte0 = readPCF(A16_PCF_DI_0);  // IN1-IN8
    uint8_t byte1 = readPCF(A16_PCF_DI_1);  // IN9-IN16

    // Временная диагностика (ввод в эксплуатацию, счётчики воды/газа):
    // печатаем сырые байты, только когда они меняются, чтобы видно было
    // живьём в serial-мониторе, доходит ли вообще замыкание на GND до
    // прошивки, независимо от логики счётчика/эджей ниже.
    static uint8_t lastByte0 = 0xFF, lastByte1 = 0xFF;
    if (byte0 != lastByte0 || byte1 != lastByte1) {
        Serial.printf("[A16] DI raw: byte0=0x%02X byte1=0x%02X\n", byte0, byte1);
        lastByte0 = byte0; lastByte1 = byte1;
    }

    for (int i = 0; i < 8; i++) {
        bool state = !((byte0 >> i) & 1);  // PCF8574: лог.0 = вход активен
        if (state && !a16State.din_prev[i]) a16State.din_counter[i]++;
        a16State.din_prev[i] = state;
        a16State.din[i] = state;
    }
    for (int i = 0; i < 8; i++) {
        bool state = !((byte1 >> i) & 1);
        if (state && !a16State.din_prev[i + 8]) a16State.din_counter[i + 8]++;
        a16State.din_prev[i + 8] = state;
        a16State.din[i + 8] = state;
    }

    Bus::write("raw.water_pulses", a16State.din_counter[A16_DIN_WATER]);
    Bus::write("raw.gas_pulses",   a16State.din_counter[A16_DIN_GAS]);

    // Полное состояние всех 16 входов — для вкладки "Диагностика".
    for (int i = 0; i < 16; i++) {
        char id[16];
        snprintf(id, sizeof(id), "raw.din%d", i);
        Bus::write(id, a16State.din[i] ? 1 : 0);
    }
}

// ============================================================
//  АНАЛОГОВЫЕ ВХОДЫ (давление, 4-20мА)
// ============================================================
// Никакой калибровки здесь больше нет — A16Device отдаёт чистый raw ADC
// (0-4095, среднее по 4 выборкам для стабильности). Перевод raw->мА->бар —
// в apps/boiler-room/BoilerRoomApp, параметры настраиваются через веб
// (см. ТЗ и BoilerRoomConfig: adc_raw_at_4ma/adc_raw_at_20ma).
void A16Device::readAnalog() {
    const uint8_t pins[2] = {A16_ADC_P_HEAT_PIN, A16_ADC_P_WATER_PIN};
    for (int i = 0; i < 2; i++) {
        int32_t sum = 0;
        for (int j = 0; j < 4; j++) sum += analogRead(pins[i]);
        a16State.adc_raw[i] = sum / 4;
    }
    Bus::write("raw.p_heat_adc",  a16State.adc_raw[0]);
    Bus::write("raw.p_water_adc", a16State.adc_raw[1]);
}

// ============================================================
//  1-WIRE (t_outdoor / t_indoor)
// ============================================================
// TODO (см. ТЗ, "отложено на Phase 2"): физическая привязка "какой из
// найденных датчиков — уличный, какой — комнатный" сейчас определяется
// просто порядком сканирования шин ow1->ow2->ow3. Если окажется, что
// порядок не совпадает с реальной разводкой — нужно либо зафиксировать
// ROM-адреса в конфиге, либо развести по трём отдельным Bus-каналам
// raw.ow0_temp.. и решать привязку в apps/boiler-room.
void A16Device::scanOneWire() {
    a16State.ow_count = 0;
    uint8_t addr[8];

    auto scan = [&](OneWire& ow) {
        ow.reset_search();
        while (ow.search(addr) && a16State.ow_count < A16_MAX_OW_SENSORS) {
            if (OneWire::crc8(addr, 7) != addr[7]) continue;
            uint64_t id = 0;
            for (int i = 0; i < 8; i++) id = (id << 8) | addr[i];
            a16State.ow_addr[a16State.ow_count++] = id;
        }
    };

    scan(ow1); scan(ow2); scan(ow3);
    Serial.printf("[A16] 1-Wire: found %d sensors\n", a16State.ow_count);

    for (uint8_t i = 0; i < a16State.ow_count; i++) {
        char id[24];
        snprintf(id, sizeof(id), "raw.ow%u_temp", i);
        Bus::declare(strdup(id), 100);  // strdup: id должен жить весь аптайм
    }
}

void A16Device::requestOneWire() {
    if (a16State.ow_count == 0) return;  // нечего опрашивать — не ждём зря

    dt1.requestTemperatures();
    dt2.requestTemperatures();
    dt3.requestTemperatures();
    _owRequestMs        = millis();
    _owConversionPending = true;
    // Результат читаем позже, в finishOneWire(), когда конверсия точно
    // завершится (750мс для 12-бит DS18B20 — см. loop()). Раньше здесь
    // был delay(100) сразу после запроса: и слишком короткий для честного
    // 12-битного результата, и блокирующий (см. комментарий в loop()).
}

void A16Device::finishOneWire() {
    _owConversionPending = false;
    if (a16State.ow_count == 0) return;

    for (int i = 0; i < a16State.ow_count; i++) {
        uint8_t addr[8];
        for (int b = 7; b >= 0; b--) addr[7 - b] = (a16State.ow_addr[i] >> (b * 8)) & 0xFF;

        float t = DEVICE_DISCONNECTED_C;
        DeviceAddress da; memcpy(da, addr, 8);
        if (t == DEVICE_DISCONNECTED_C) t = dt1.getTempC(da);
        if (t == DEVICE_DISCONNECTED_C) t = dt2.getTempC(da);
        if (t == DEVICE_DISCONNECTED_C) t = dt3.getTempC(da);

        a16State.ow_ok[i]   = (t != DEVICE_DISCONNECTED_C);
        a16State.ow_temp[i] = a16State.ow_ok[i] ? t : 0.0f;

        if (a16State.ow_ok[i]) {
            char id[24];
            snprintf(id, sizeof(id), "raw.ow%d_temp", i);
            Bus::write(id, (int64_t)llround(a16State.ow_temp[i] * 100));
        }
    }
}

// ============================================================
//  PCF8574 I/O
// ============================================================
void A16Device::writePCF(uint8_t addr, uint8_t data) {
    Wire.beginTransmission(addr);
    Wire.write(data);
    Wire.endTransmission();
}

uint8_t A16Device::readPCF(uint8_t addr) {
    uint8_t got = Wire.requestFrom(addr, (uint8_t)1);
    if (got != 1) {
        // Раньше здесь молча возвращался 0xFF ("все входы неактивны") —
        // из-за этого сбой I2C-обмена с конкретным PCF8574 выглядел на
        // дашборде так же, как "жду замыкания на GND", и диагностировать
        // проблему со счётчиками (см. чат при вводе в эксплуатацию) было
        // невозможно без этого лога.
        Serial.printf("[A16] PCF 0x%02X: I2C read failed (got %u bytes)\n", addr, got);
        return 0xFF;
    }
    return Wire.read();
}

void A16Device::syncOutputs() {
    uint8_t byte0 = 0xFF, byte1 = 0xFF;
    for (int i = 0; i < 8; i++) if (a16State.dout[i])     byte0 &= ~(1 << i);
    for (int i = 0; i < 8; i++) if (a16State.dout[i + 8]) byte1 &= ~(1 << i);
    writePCF(A16_PCF_OUT_0, byte0);
    writePCF(A16_PCF_OUT_1, byte1);

    for (int i = 0; i < 16; i++) {
        char id[16];
        snprintf(id, sizeof(id), "raw.dout%d", i);
        Bus::write(id, a16State.dout[i] ? 1 : 0);
    }
}

bool A16Device::setOutput(uint8_t ch, bool state) {
    if (ch >= 16) return false;
    a16State.dout[ch] = state;
    syncOutputs();
    return true;
}

bool A16Device::toggleOutput(uint8_t ch) {
    return setOutput(ch, !a16State.dout[ch]);
}

// ============================================================
//  CONFIG
// ============================================================
bool A16Device::loadConfig() {
    JsonDocument doc;
    if (!ConfigManager::loadJson("/device.json", doc)) return false;
    a16Cfg.poll_interval = doc["poll_interval"] | 1000;
    return true;
}

bool A16Device::saveConfig() {
    JsonDocument doc;
    doc["poll_interval"] = a16Cfg.poll_interval;
    return ConfigManager::saveJson("/device.json", doc);
}

#endif // DEVICE_A16
