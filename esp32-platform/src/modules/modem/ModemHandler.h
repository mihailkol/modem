#pragma once
#ifdef MODULE_MODEM

#include <HardwareSerial.h>
#include "../../core/CoreTypes.h"

// ── Конфиг (сохраняется в /modem.json) ──────────────────────────────────────
struct ModemConfig {
    bool    enabled        = true;
    int     rx_pin         = 4;
    int     tx_pin         = 2;
    int     rst_pin        = 33;   // RST модема, -1 = не используется
    int     ri_pin         = -1;   // RI,  -1 = не используется
    int     baud           = 9600;
    int     dtmf_timeout   = 10;
    int     poll_interval  = 60;
};

extern ModemConfig modemCfg;

// ── Последнее известное состояние модема ─────────────────────────────────────
struct ModemStatus {
    int     creg     = -1;
    int     csq      = -1;
    int     vbat     = -1;
    char    oper[32] = "";
};

extern ModemStatus modemStatus;

class ModemHandler {
public:
    static void init();
    static void loop();
    static void sendAT(const char* cmd);
    static void (*onDTMFResult)(const char* callerNum, const char* dtmf);
    static bool loadConfig();
    static bool saveConfig();

private:
    // ── Стейт-машина инициализации ────────────────────────────────────────────
    static void _initTick();

    // ── Обработка звонка ──────────────────────────────────────────────────────
    static void _processLine(const String& line);
    static void _onClip(const String& line);
    static void _onDTMF(char digit);
    static void _onNoCarrier();
    static void _checkTimeout();
    static void _checkAnswerDelay();    // неблокирующая задержка после ATA
    static void _notifyResult();

    // ── Опрос статуса ─────────────────────────────────────────────────────────
    static void _pollTick();
    static void _wdTick();
    static void _wdHandleCreg(int stat);
    static void _wdRestart();
    static void _parsePollResponse(const String& line);
    static void _publishStatusIfChanged();

    // ── Вспомогательные ──────────────────────────────────────────────────────
    static void _addLog(const String& msg);
    static void _sysLog(const String& msg);
    static String _normalizePhone(const String& phone);
};

#endif // MODULE_MODEM