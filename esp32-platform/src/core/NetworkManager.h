#pragma once
// Порт из esp32-platform/src/core/NetworkManager.h, упрощён под одно
// устройство (KC868-A16) — убрана ветка под другие платы.
#include <WiFi.h>
#include <ETH.h>
#include <ESPmDNS.h>
#include "CoreTypes.h"
#include "ConfigManager.h"

class NetworkManager {
public:
    static void begin();
    static void loop();

private:
    static void startAP();
    static void onWiFiEvent(WiFiEvent_t event);
    static void _ethPhyReset();
    static uint32_t _lastEthCheck;
};
