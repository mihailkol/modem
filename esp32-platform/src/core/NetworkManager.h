#pragma once
#include <WiFi.h>
#include <ETH.h>
#include <ESPmDNS.h>
#include <DNSServer.h>
#include "CoreTypes.h"
#include "ConfigManager.h"

class NetworkManager {
public:
    static void begin();
    static void loop();

private:
    static void startAP();
    static void onWiFiEvent(WiFiEvent_t event);

    static DNSServer  _dns;
    static uint32_t   _lastEthCheck;
};
