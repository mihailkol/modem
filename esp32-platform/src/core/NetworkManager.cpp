#include "NetworkManager.h"
#ifdef DEVICE_KC868_A16
#include "../devices/kc868-a16/A16Types.h"
#endif
#ifdef DEVICE_BOILER_MONITOR
#include "../devices/boiler-monitor/BMonTypes.h"
#endif

// Есть ли на этой плате Ethernet вообще. DEVICE_KC868_A16 и
// DEVICE_BOILER_MONITOR несут свои ETH-пины сами; остальные платы с
// Ethernet передают их через ETH_ADDR/ETH_POWER_PIN/... в build_flags
// (см. [env:wt32-eth01]). Платы без Ethernet (например обычный esp32dev
// под clockinfo) ничего из этого не определяют — тогда работаем только
// по WiFi.
#if defined(DEVICE_KC868_A16) || defined(DEVICE_BOILER_MONITOR) || defined(ETH_ADDR)
#define NET_HAS_ETH 1
#endif

RTC_DATA_ATTR static uint8_t _ethRebootCount = 0;

// Перезапуски из-за ETH.begin()==false. RTC_DATA_ATTR тут не годится: при
// ESP.restart() он инициализируется заново, а RTC_NOINIT переживает программный
// сброс. Счётчик обнуляем только при включении питания (или если в памяти мусор).
#define ETH_INIT_MAGIC         0xE7E70001u
#define ETH_INIT_MAX_RESTARTS  5
RTC_NOINIT_ATTR static uint32_t _ethInitMagic;
RTC_NOINIT_ATTR static uint8_t  _ethInitRestarts;
static bool _ethDriverFailed = false;

DNSServer  NetworkManager::_dns;
uint32_t   NetworkManager::_lastEthCheck = 0;
// ETH watchdog
static bool     _ethWdArmed   = true;
static uint8_t  _ethWdPhySt   = 0;      // 0=idle, 1=phy low, 2=phy high
static uint32_t _ethWdTimer   = 0;

void NetworkManager::begin() {
    WiFi.onEvent(onWiFiEvent);

    // Статический IP для Ethernet если нужен
    if (!baseCfg.eth_dhcp) {
        IPAddress ip, mask, gw, dns;
        if (ip.fromString(baseCfg.eth_ip) && mask.fromString(baseCfg.eth_mask)) {
            gw.fromString(baseCfg.eth_gw);
            dns.fromString(baseCfg.eth_dns);
            ETH.config(ip, gw, mask, dns);
            Serial.println("[NET] ETH: static IP configured");
        }
    }

    // Режим тактирования ETH зависит от платы — каждый device определяет свой
    #ifdef NET_HAS_ETH
        if (esp_reset_reason() == ESP_RST_POWERON || _ethInitMagic != ETH_INIT_MAGIC) {
            _ethInitMagic    = ETH_INIT_MAGIC;
            _ethInitRestarts = 0;
        }
        sysState.ethInitRestarts = _ethInitRestarts;

        bool ethOk;
        #ifdef DEVICE_KC868_A16
            ethOk = ETH.begin(A16_ETH_ADDR, A16_ETH_POWER_PIN, A16_ETH_MDC_PIN, A16_ETH_MDIO_PIN,
                    ETH_PHY_LAN8720, A16_ETH_CLK_MODE);
        #elif defined(DEVICE_BOILER_MONITOR)
            pinMode(16, OUTPUT);
            digitalWrite(16, HIGH);
            delay(500);
            ethOk = ETH.begin(BMON_ETH_ADDR, BMON_ETH_POWER_PIN, BMON_ETH_MDC_PIN, BMON_ETH_MDIO_PIN, ETH_PHY_LAN8720, ETH_CLOCK_GPIO0_IN);
        #else
            // На WT32-ETH01 этот же пин питает генератор 50 МГц для PHY. Драйвер
            // инициализирует EMAC (ему нужен внешний такт) раньше, чем сам дёрнет
            // пин, поэтому включаем генератор заранее и даём ему запуститься.
            #if ETH_POWER_PIN >= 0
            pinMode(ETH_POWER_PIN, OUTPUT);
            digitalWrite(ETH_POWER_PIN, HIGH);
            delay(500);
            #endif
            ethOk = ETH.begin(ETH_ADDR, ETH_POWER_PIN, ETH_MDC_PIN, ETH_MDIO_PIN,
                    ETH_PHY_LAN8720, ETH_CLOCK_GPIO0_IN);
        #endif

        if (!ethOk) {
            _ethDriverFailed = true;
            sysState.ethDriverOk = false;
            Serial.printf("[NET] ETH.begin() FAILED (restart attempts used: %u/%d)\n",
                          _ethInitRestarts, ETH_INIT_MAX_RESTARTS);
        } else {
            Serial.println("[NET] ETH driver started");
        }
    #else
        // Плата без Ethernet — сразу поднимаем WiFi, ETH-события никогда не придут
        if (strlen(baseCfg.wifi_ssid) > 0) {
            WiFi.mode(WIFI_STA);
            WiFi.begin(baseCfg.wifi_ssid, baseCfg.wifi_pass);
        }
    #endif

    Serial.printf("[NET] Device name: %s\n", baseCfg.device_name);
}

void NetworkManager::_ethPhyReset() {
    Serial.println("[NET] ETH watchdog: resetting PHY");
    #ifdef ETH_POWER_PIN
    digitalWrite(ETH_POWER_PIN, LOW);
    #endif
}

void NetworkManager::loop() {
    xSemaphoreTake(coreMutex, portMAX_DELAY);
    bool connected      = sysState.ethConnected || sysState.wifiConnected;
    bool apActive       = sysState.apMode;
    xSemaphoreGive(coreMutex);

    // ETH watchdog (нет смысла на плате без Ethernet — sysState.ethConnected
    // там никогда не станет true, и мы бы просто дёргали несуществующие пины)
    #ifdef NET_HAS_ETH
    // Драйвер Ethernet не поднялся (PHY/генератор не успели после включения
    // питания) — перезапускаемся с нарастающей паузой, до ETH_INIT_MAX_RESTARTS
    // раз за одно включение питания. Дальше остаётся AP-fallback.
    if (_ethDriverFailed && _ethInitRestarts < ETH_INIT_MAX_RESTARTS) {
        uint32_t wait = 5000UL * (_ethInitRestarts + 1);
        if (millis() > wait) {
            _ethInitRestarts++;
            Serial.printf("[NET] ETH init failed, restarting (%u/%d)\n",
                          _ethInitRestarts, ETH_INIT_MAX_RESTARTS);
            Serial.flush();
            ESP.restart();
        }
    }

    if (_ethWdArmed) {
        if (sysState.ethConnected) {
            _ethWdArmed    = false;
            _ethRebootCount = 0;
        } else if (_ethWdPhySt == 0 && millis() - _ethWdTimer > 30000) {
            if (_ethRebootCount < 3) {
                _ethPhyReset();
                _ethWdPhySt = 1;
                _ethWdTimer = millis();
            } else {
                Serial.println("[NET] ETH watchdog: giving up after 3 attempts");
                _ethWdArmed = false;
                // AP поднимется штатно через существующую логику
            }
        } else if (_ethWdPhySt == 1 && millis() - _ethWdTimer > 200) {
            // PHY был LOW 200мс — поднимаем
            #ifdef ETH_POWER_PIN
            digitalWrite(ETH_POWER_PIN, HIGH);
            #endif
            _ethWdPhySt = 2;
            _ethWdTimer = millis();
        } else if (_ethWdPhySt == 2 && millis() - _ethWdTimer > 30000) {
            // Ждали 30 сек после PHY reset — не помогло
            _ethRebootCount++;
            _ethWdPhySt = 0;
            _ethWdTimer = millis();
            Serial.printf("[NET] ETH watchdog: PHY reset attempt %d failed\n", _ethRebootCount);
        }
    }
    #endif // NET_HAS_ETH

    // Runtime watchdog — если сеть пропала надолго, перезагружаемся
    static uint32_t _netLostTimer    = 0;
    static bool     _netWasConnected = false;

    if (connected) {
        _netWasConnected = true;
        _netLostTimer    = millis();
    } else if (_netWasConnected && millis() - _netLostTimer > 300000) {
        Serial.println("[NET] Network lost for 5 min, rebooting");
        ESP.restart();
    }

    // Captive portal DNS
#ifdef MODULE_CAPTIVE_PORTAL
    if (apActive) _dns.processNextRequest();
#endif

    // Запуск AP если нет сети 30 сек
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

    // SSID = device_name
    WiFi.softAP(baseCfg.device_name);

#ifdef MODULE_CAPTIVE_PORTAL
    _dns.start(53, "*", WiFi.softAPIP());
#endif

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
            sysState.apMode = false;
            break;

        case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
            sysState.wifiConnected = false;
            break;

        default: break;
    }

    xSemaphoreGive(coreMutex);
}
