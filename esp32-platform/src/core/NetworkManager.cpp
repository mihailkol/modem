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

#ifdef NET_HAS_ETH
// ── Watchdog связности ───────────────────────────────────────────────────────
// Линк до свитча/роутера может быть жив, а сеть за ним — нет (завис коммутатор,
// пропал аплинк): событий ETH_DISCONNECTED при этом нет, и без проверки шлюза
// плата сидит на мёртвом Ethernet. Раз в GW_PING_INTERVAL_MS пингуем шлюз по
// Ethernet; сработает только если он хотя бы раз ответил после получения IP
// (так не ловим ложные срабатывания на сетях, где шлюз не отвечает на ICMP).
#include "esp_netif.h"
#include "ping/ping_sock.h"

#define GW_PING_INTERVAL_MS  15000
#define GW_FAIL_LIMIT        4        // подряд без ответа (~1 мин) => шлюз потерян
#define GW_RESTART_AFTER_MS  300000   // без Wi-Fi-запаса — перезапуск через 5 мин
#define GW_MAX_RESTARTS      3        // за одно включение питания

RTC_NOINIT_ATTR static uint8_t _gwRestarts;
static esp_ping_handle_t _gwPing        = nullptr;
static volatile uint32_t _gwFailStreak  = 0;
static volatile uint32_t _gwOkCount     = 0;
static bool     _gwArmed        = false;
static bool     _gwLost         = false;
static bool     _gwFallbackWifi = false;
static uint32_t _gwLostSince    = 0;
static uint32_t _gwTargetIp     = 0;

static void _gwOnSuccess(esp_ping_handle_t, void*) { _gwFailStreak = 0; _gwOkCount = _gwOkCount + 1; }
static void _gwOnTimeout(esp_ping_handle_t, void*) { _gwFailStreak = _gwFailStreak + 1; }

static void _gwStop() {
    if (_gwPing) {
        esp_ping_stop(_gwPing);
        esp_ping_delete_session(_gwPing);
        _gwPing = nullptr;
    }
    _gwArmed = false;
    _gwTargetIp = 0;
}

static void _gwStart(IPAddress gw) {
    _gwFailStreak = 0;
    _gwOkCount    = 0;
    _gwArmed      = false;

    esp_ping_config_t cfg = ESP_PING_DEFAULT_CONFIG();
    cfg.count       = ESP_PING_COUNT_INFINITE;
    cfg.interval_ms = GW_PING_INTERVAL_MS;
    cfg.timeout_ms  = 2000;
    cfg.data_size   = 32;
    IP_ADDR4(&cfg.target_addr, gw[0], gw[1], gw[2], gw[3]);
    esp_netif_t* eth = esp_netif_get_handle_from_ifkey("ETH_DEF");
    cfg.interface = eth ? esp_netif_get_netif_impl_index(eth) : 0;

    esp_ping_callbacks_t cbs = {};
    cbs.on_ping_success = _gwOnSuccess;
    cbs.on_ping_timeout = _gwOnTimeout;

    if (esp_ping_new_session(&cfg, &cbs, &_gwPing) == ESP_OK) {
        esp_ping_start(_gwPing);
        _gwTargetIp = (uint32_t)gw;
        Serial.printf("[NET] Gateway watchdog: pinging %s via ETH\n", gw.toString().c_str());
    } else {
        _gwPing = nullptr;
    }
}

static void _gwTick() {
    bool ethUp = sysState.ethConnected;
    uint32_t gwRaw = (uint32_t)ETH.gatewayIP();

    if (ethUp && gwRaw != 0 && (!_gwPing || gwRaw != _gwTargetIp)) {
        _gwStop();
        _gwStart(ETH.gatewayIP());
    } else if (!ethUp && _gwPing) {
        _gwStop();
    }

    if (!_gwPing) {
        // Ethernet пропал сам (линк) — дальше работает штатная логика ETH_DISCONNECTED
        _gwLost = false;
        _gwFallbackWifi = false;
        sysState.ethUpstreamLost = false;
        return;
    }

    if (_gwOkCount > 0) {
        _gwArmed = true;
        _gwRestarts = 0;
    }

    bool dead = _gwArmed && _gwFailStreak >= GW_FAIL_LIMIT;

    if (dead && !_gwLost) {
        _gwLost = true;
        _gwLostSince = millis();
        sysState.ethUpstreamLost = true;
        Serial.println("[NET] Gateway watchdog: gateway unreachable via ETH");
        if (strlen(baseCfg.wifi_ssid) > 0) {
            _gwFallbackWifi = true;
            Serial.println("[NET] Gateway watchdog: bringing up WiFi as fallback");
            WiFi.begin(baseCfg.wifi_ssid, baseCfg.wifi_pass);
        }
    } else if (!dead && _gwLost) {
        _gwLost = false;
        sysState.ethUpstreamLost = false;
        Serial.println("[NET] Gateway watchdog: gateway reachable again");
        if (_gwFallbackWifi) {
            _gwFallbackWifi = false;
            WiFi.disconnect(true);
            xSemaphoreTake(coreMutex, portMAX_DELAY);
            sysState.wifiConnected = false;
            xSemaphoreGive(coreMutex);
        }
    }

    if (_gwLost && !sysState.wifiConnected
        && millis() - _gwLostSince > GW_RESTART_AFTER_MS
        && _gwRestarts < GW_MAX_RESTARTS) {
        _gwRestarts++;
        Serial.printf("[NET] Gateway watchdog: restarting (%u/%d)\n", _gwRestarts, GW_MAX_RESTARTS);
        Serial.flush();
        ESP.restart();
    }
}
#endif // NET_HAS_ETH

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
            _gwRestarts      = 0;
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

    _gwTick();

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
