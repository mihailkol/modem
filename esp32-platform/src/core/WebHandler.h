#pragma once
// Порт из esp32-platform/src/core/WebHandler.h.
// Отличия: убраны Telegram/Modem-специфичные поля статуса, добавлен
// /api/channels (снимок Bus) — см. boiler-room-v2-tz.md, п.3.3 (WebModule).
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include <Update.h>
#include <vector>
#include "CoreTypes.h"

class WebHandler {
public:
    // Вызвать ДО init() — модули/устройства регистрируют свои вкладки
    static void registerTab(const WebTab& tab);

    static void init(AsyncWebServer& server);

private:
    static std::vector<WebTab> _tabs;

    static void handleOtaUpload(AsyncWebServerRequest*, String, size_t, uint8_t*, size_t, bool);
};
