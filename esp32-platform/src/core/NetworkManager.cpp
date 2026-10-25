#include "NetworkManager.h"

// Пины Ethernet заданы в platformio.ini (ETH_ADDR, ETH_POWER_PIN, ETH_MDC_PIN,
// ETH_MDIO_PIN, ETH_CLK_MODE) — под KC868-A16, единственную плату в этом проекте.

RTC_DATA_ATTR static uint8_t _ethRebootCount = 0;

uint32_t NetworkManager::_lastEthCheck = 0;

// ETH watchdog — если PHY не поднимается 30с, дёргаем power-пин
static bool     _ethWdArmed = true;
static uint8_t  _ethWdPhySt = 0;      // 0=idle, 1=phy low, 2=phy high
static uint32_t _ethWdTimer = 0;

void NetworkManager::begin() {
    WiFi.onEvent(onWiFiEvent);

    if (!baseCfg.eth_dhcp) {
        IPAddress ip, mask, gw, dns;
        if (ip.fromString(baseCfg.eth_ip) && mask.fromString(baseCfg.eth_mask)) {
            gw.fromString(baseCfg.eth_gw);
            dns.fromString(baseCfg.eth_dns);
            ETH.config(ip, gw, mask, dns);
            Serial.println("[NET] ETH: static IP configured");
        }
    }

    ETH.begin(ETH_ADDR, ETH_POWER_PIN, ETH_MDC_PIN, ETH_MDIO_PIN,
              ETH_PHY_LAN8720, ETH_CLK_MODE);

    Serial.printf("[NET] Device name: %s\n", baseCfg.device_name);
}

void NetworkManager::_ethPhyReset() {
    Serial.println("[NET] ETH watchdog: resetting PHY");
    #ifdef ETH_POWER_PIN
    if (ETH_POWER_PIN >= 0) digitalWrite(ETH_POWER_PIN, LOW);
    #endif
}

void NetworkManager::loop() {
    xSemaphoreTake(coreMutex, portMAX_DELAY);
    bool connected = sysState.ethConnected || sysState.wifiConnected;
    bool apActive  = sysState.apMode;
    xSemaphoreGive(coreMutex);

    // ETH watchdog
    if (_ethWdArmed) {
        if (sysState.ethConnected) {
            _ethWdArmed     = false;
            _ethRebootCount = 0;
        } else if (_ethWdPhySt == 0 && millis() - _ethWdTimer > 30000) {
            if (_ethRebootCount < 3) {
                _ethPhyReset();
                _ethWdPhySt = 1;
                _ethWdTimer = millis();
            } else {
                Serial.println("[NET] ETH watchdog: giving up after 3 attempts");
                _ethWdArmed = false;
            }
        } else if (_ethWdPhySt == 1 && millis() - _ethWdTimer > 200) {
            #ifdef ETH_POWER_PIN
            if (ETH_POWER_PIN >= 0) digitalWrite(ETH_POWER_PIN, HIGH);
            #endif
            _ethWdPhySt = 2;
            _ethWdTimer = millis();
        } else if (_ethWdPhySt == 2 && millis() - _ethWdTimer > 30000) {
            _ethRebootCount++;
            _ethWdPhySt = 0;
            _ethWdTimer = millis();
            Serial.printf("[NET] ETH watchdog: PHY reset attempt %d failed\n", _ethRebootCount);
        }
    }

    // Runtime watchdog — если сети нет 5 минут, перезагружаемся
    static uint32_t _netLostTimer    = 0;
    static bool     _netWasConnected = false;

    if (connected) {
        _netWasConnected = true;
        _netLostTimer    = millis();
    } else if (_netWasConnected && millis() - _netLostTimer > 300000) {
        Serial.println("[NET] Network lost for 5 min, rebooting");
        ESP.restart();
    }

    // Поднять AP, если нет сети 30 сек
    static uint32_t lastCheck = 0;
    if (millis() - lastCheck < 2000) return;
    lastCheck = millis();

    if (!connected && !apActive && (millis() - _lastEthCheck > 30000)) {
        startAP();
    }
}

void NetworkManager::startAP() {
    Serial.println("[NET] Starting AP...");
    WiFi.mode(WIFI_AP);
    WiFi.softAP(baseCfg.device_name);

    xSemaphoreTake(coreMutex, portMAX_DELAY);
    sysState.apMode = true;
    xSemaphoreGive(coreMutex);

    Serial.printf("[NET] AP started: %s / %s\n",
                  baseCfg.device_name, WiFi.softAPIP().toString().c_str());
}

void NetworkManager::onWiFiEvent(WiFiEvent_t event) {
    xSemaphoreTake(coreMutex, portMAX_DELAY);

    switch (event) {
        case ARDUINO_EVENT_ETH_GOT_IP:
            Serial.printf("[NET] ETH IP: %s\n", ETH.localIP().toString().c_str());
            sysState.ethConnected = true;
            if (WiFi.getMode() & WIFI_MODE_STA) {
                WiFi.disconnect(true);
                sysState.wifiConnected = false;
            }
            _ethRebootCount = 0;
            _ethWdArmed     = false;
            break;

        case ARDUINO_EVENT_ETH_DISCONNECTED:
            Serial.println("[NET] ETH lost, trying WiFi...");
            sysState.ethConnected = false;
            _lastEthCheck = millis();
            if (strlen(baseCfg.wifi_ssid) > 0)
                WiFi.begin(baseCfg.wifi_ssid, baseCfg.wifi_pass);
            break;

        case ARDUINO_EVENT_WIFI_STA_GOT_IP:
            Serial.printf("[NET] WiFi IP: %s\n", WiFi.localIP().toString().c_str());
            sysState.wifiConnected = true;
            sysState.apMode        = false;
            break;

        case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
            sysState.wifiConnected = false;
            break;

        default: break;
    }

    xSemaphoreGive(coreMutex);
}
