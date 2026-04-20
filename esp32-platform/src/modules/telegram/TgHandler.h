#pragma once
#ifdef MODULE_TELEGRAM

#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include "../core/CoreTypes.h"

struct TgConfig {
    char token[64]  = "";
    char group[32]  = "";   // Группа/канал для алертов
    char admin[32]  = "";   // ID администратора (управление командами)
    bool enabled    = false;
};

extern TgConfig tgCfg;

class TgHandler {
public:
    static void init();
    static void loop();
    static void sendMessage(const String& msg, const String& chatId = "");
    static bool loadConfig();
    static bool saveConfig();

    // Устройство может добавить свои команды
    static void (*onCommand)(const String& cmd, const String& chatId);

private:
    static void handleMessages(int count);
};

#endif // MODULE_TELEGRAM
