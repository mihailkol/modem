#pragma once
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include <Update.h>
#include <vector>
#include "CoreTypes.h"

class WebHandler {
public:
    // Вызвать ДО init() — модули регистрируют свои вкладки
    static void registerTab(const WebTab& tab);

    static void init(AsyncWebServer& server);

private:
    static std::vector<WebTab> _tabs;

    // Генерация nav-бара из зарегистрированных вкладок
    static String buildNav();

    // OTA upload
    static void handleOtaUpload(AsyncWebServerRequest*, String, size_t, uint8_t*, size_t, bool);
};
