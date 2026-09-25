#pragma once
#ifdef DEVICE_MASTERGAS

#include <Arduino.h>

// ============================================================
//  ПИНЫ — WT32-ETH01, плата управления котлом (пульт <-> котёл)
//  Порт протокола/схемы проекта OpenMasterGas
//  (https://github.com/uhfband/OpenMasterGas) на ESP32.
//
//  Физическая шина пульт<->котёл — двухпроводная, 4800 8N1,
//  уровень ~21В. ESP врезается в неё четырьмя сигналами:
//   - boilerRead  — приём с общей шины (слышны и запросы котла,
//                   и, в режиме passthrough, ответы штатного пульта)
//   - remoteRead  — отдельный отвод прямо с TX пульта — виден
//                   ответ пульта, даже когда remoteEn его отключил
//   - remoteEn    — управление разрешением выхода пульта на шину
//                   (HIGH = пульт подключен и отвечает сам,
//                    LOW  = пульт отключен, отвечаем мы)
//   - boilerAnswer— передатчик ESP на шину, используется только
//                   когда remoteEn=LOW и котёл прислал 0xF9
//
//  GPIO36/39 — вход-онли пины ESP32, что и требуется для чистого RX.
//
//  ВАЖНО: физически на этой плате провод общей шины (boilerRead) оказался
//  на GPIO36 (пин 21 разъёма), а не на GPIO39 (пин 22), как задумано по
//  исходной схеме — при пайке дважды промахивались на одну ногу. Проверено
//  на живом котле: чтение (сотни валидных пакетов, честные температуры) и
//  override (котёл реально слушается override-уставки, горелка зажигается/
//  гаснет по команде) стабильно работают именно на GPIO36 — перепаивать
//  третий раз смысла нет, проще было один раз поменять назначение в коде.
//  GPIO39 из-под boilerRead освободился и отдан под remoteRead — отдельный
//  провод прямо с TX пульта (виден ответ пульта, даже когда remoteEn его
//  отключил от общей шины) физически ЕЩЁ НЕ разведён, ждёт своей очереди.
// ============================================================
#define MASTERGAS_BOILER_RX_PIN  36   // boilerRead  (вход-онли; физически — пин 21)
#define MASTERGAS_REMOTE_RX_PIN  39   // remoteRead  (вход-онли; пока не подключен)
#define MASTERGAS_ANSWER_TX_PIN  32   // boilerAnswer
#define MASTERGAS_REMOTE_EN_PIN  33   // remoteEn

// Индикация — RGB (3 независимых пина). Предполагается GPIO=HIGH -> LED
// горит (анод к GPIO через резистор, катод на GND) — если на плате наоборот,
// поменять местами HIGH/LOW в _ledTick().
#define MASTERGAS_LED_R_PIN  2    // красный — heartbeat loop() / режим override
#define MASTERGAS_LED_G_PIN  4    // зелёный — состояние шины котла
#define MASTERGAS_LED_B_PIN  12   // синий   — сеть

#define MASTERGAS_BAUD 4800

// ============================================================
//  КОНФИГ — сохраняется в /mastergas.json
// ============================================================
struct MasterGasConfig {
    bool    enabled        = true;
    // true  — штатный пульт отвечает котлу сам, ESP только слушает и парсит
    // false — ESP отключает пульт от шины и отвечает вместо него override-параметрами
    bool    passthrough    = true;
    bool    override_power = true;
    uint8_t override_temp  = 50;    // °C, уставка при override
    uint32_t pub_interval  = 10000; // интервал MQTT публикации, мс
};

// ============================================================
//  СОСТОЯНИЕ — runtime, не сохраняется
// ============================================================
struct MasterGasState {
    // От котла (пакет 0xF9/0x00)
    bool    burner_on    = false;
    uint8_t coolant_temp = 0;
    uint8_t water_temp   = 0;

    // От пульта (пакет 0xF9/0x80) — либо реальные (passthrough),
    // либо последние подсмотренные перед переходом в override
    bool    remote_power       = true;
    uint8_t remote_target_temp = 50;
    uint8_t remote_air_temp    = 0;

    // Диагностика — сырые байты для /api/mastergas/raw
    uint8_t raw_status[4] = {};   // buf[2..5] последнего 0xF9/0x00
    uint8_t raw_ext[24]   = {};   // накопленные offset-блоки 0xFA (0x00-0x14)

    uint32_t last_boiler_rx_ms = 0;
    uint32_t last_remote_rx_ms = 0;
    uint32_t pkt_ok       = 0;
    uint32_t pkt_crc_err  = 0;

    // Те же OK/CRC ошибки boilerRead, но в разрезе "горелка вкл/выкл" на
    // момент приёма (используется последнее известное burner_on — оно
    // обновляется только при успешном 0xF9/0x00, так что для пакетов между
    // успешными разборами берётся последнее известное состояние — горелка
    // штука инерционная, за секунды не мигает). Нужно, чтобы подтвердить/
    // опровергнуть на цифрах гипотезу "горелка шумит на шину".
    uint32_t pkt_ok_burner_on        = 0;
    uint32_t pkt_ok_burner_off       = 0;
    uint32_t pkt_crc_err_burner_on   = 0;
    uint32_t pkt_crc_err_burner_off  = 0;
};

extern MasterGasConfig masterGasCfg;
extern MasterGasState  masterGasState;

#endif // DEVICE_MASTERGAS
