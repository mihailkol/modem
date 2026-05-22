#pragma once
#ifdef MODULE_RS485

#include <HardwareSerial.h>
#include "../../core/CoreTypes.h"

// ── Конфиг (сохраняется в /rs485.json) ──────────────────────────────────────
struct Rs485Config {
    bool     enabled     = true;
    int      rx_pin      = 16;
    int      tx_pin      = 13;
    int      de_pin      = -1;     // DE/RE пин MAX485, -1 = не используется
    uint32_t baud        = 9600;
    uint8_t  data_bits   = 8;
    uint8_t  stop_bits   = 1;
    uint8_t  parity      = 0;      // 0=none, 1=even, 2=odd
    uint16_t tcp_port    = 8485;   // TCP-мост, 0 = отключён
    uint16_t inter_frame = 4;      // мс тишины = конец фрейма
};

// ── Состояние ────────────────────────────────────────────────────────────────
struct Rs485Status {
    bool     running     = false;
    uint32_t rx_bytes    = 0;
    uint32_t tx_bytes    = 0;
    uint8_t  tcp_clients = 0;
    uint32_t last_rx_ms  = 0;
    String   last_rx_hex;
    String   last_tx_hex;
};

extern Rs485Config rs485Cfg;
extern Rs485Status rs485Status;

class Rs485Handler {
public:
    static void   init();
    static void   loop();
    static bool   loadConfig();
    static bool   saveConfig();

    // Отправить байты в шину (из консоли или другого модуля)
    static void sendBytes(const uint8_t* data, size_t len);
    static void sendHex(const String& hexStr);  // "01 03 00 00 00 02 C4 0B"

    // Callback при получении фрейма из шины
    static void (*onFrame)(const uint8_t* data, size_t len);

private:
    static void _initSerial();
    static void _handleTcpClients();
    static void _readSerial();
    static void _broadcastToTcp(const uint8_t* data, size_t len);
    static String _toHex(const uint8_t* data, size_t len);
    static size_t _fromHex(const String& hex, uint8_t* buf, size_t maxLen);

    static HardwareSerial* _serial;
    static uint32_t        _lastByteMs;
    static uint8_t         _rxBuf[256];
    static size_t          _rxLen;
};

#endif // MODULE_RS485