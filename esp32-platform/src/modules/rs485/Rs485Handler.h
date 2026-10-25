#pragma once
#ifdef MODULE_RS485
// Порт из esp32-platform/src/modules/rs485/Rs485Handler.*, БЕЗ TCP-моста
// (это была отдельная фича под будущие модбас-устройства, которых не
// планируется — см. ТЗ п.6). Hex-консоль для ручной диагностики —
// НЕ убрана: понадобилась при вводе EkoNom в эксплуатацию (см. чат),
// без неё нет способа удалённо понять, отвечает ли прибор вообще.
#include <HardwareSerial.h>
#include "CoreTypes.h"

// Дефолты — РЕАЛЬНЫЕ рабочие параметры EkoNom (найдены в старом чате при
// вводе счётчика в эксплуатацию: 2400 бод, 8E1, inter_frame=50мс). Это
// НЕ заводские дефолты Modbus/либы — те были 9600 8N1 и из-за них счётчик
// молчал на новой прошивке (см. SCAFFOLD.md, "Известные компромиссы").
struct Rs485Config {
    bool     enabled     = true;
    int      rx_pin      = RS485_RX_PIN;
    int      tx_pin      = RS485_TX_PIN;
    int      de_pin      = -1;     // DE/RE пин MAX485, -1 = не используется
    uint32_t baud        = 2400;
    uint8_t  data_bits   = 8;
    uint8_t  stop_bits   = 1;
    uint8_t  parity      = 1;      // 0=none, 1=even, 2=odd — EkoNom: 8E1
    uint16_t inter_frame = 50;     // мс тишины = конец фрейма
};

// Диагностика для hex-консоли — последний TX/RX и счётчики байт.
// last_rx_hex обновляется на КАЖДЫЙ принятый фрейм, включая ответы на
// автоматические запросы EkoNomDevice — так что консоль полезна даже
// не отправляя ничего руками: видно, приходит ли вообще что-то с шины.
struct Rs485Status {
    uint32_t rx_bytes = 0;
    uint32_t tx_bytes = 0;
    String   last_rx_hex;
    String   last_tx_hex;
};

extern Rs485Config rs485Cfg;
extern Rs485Status rs485Status;

class Rs485Handler {
public:
    static void init();
    static void loop();
    static bool loadConfig();
    static bool saveConfig();

    // Отправить байты в шину (например, из EkoNomDevice — запрос R1/R2/R3)
    static void sendBytes(const uint8_t* data, size_t len);

    // Ручная отправка из hex-консоли, например "01 03 00 00 00 0C 45 CF"
    static void sendHex(const String& hexStr);

    // Callback при получении фрейма (устанавливается устройством-потребителем)
    static void (*onFrame)(const uint8_t* data, size_t len);

private:
    static void _initSerial();
    static void _readSerial();
    static String _toHex(const uint8_t* data, size_t len);
    static size_t _fromHex(const String& hex, uint8_t* buf, size_t maxLen);

    static HardwareSerial* _serial;
    static uint32_t         _lastByteMs;
    static uint8_t          _rxBuf[256];
    static size_t           _rxLen;
};
#endif // MODULE_RS485
