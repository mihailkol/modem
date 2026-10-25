#pragma once
#include <Arduino.h>
#include <freertos/semphr.h>

// ============================================================
//  BASE CONFIG — сетевая часть, общая для всех модулей
//  (порт из esp32-platform/src/core/CoreTypes.h, без изменений)
// ============================================================
struct BaseConfig {
    // Ethernet
    bool eth_dhcp       = true;
    char eth_ip[16]     = "192.168.1.100";
    char eth_mask[16]   = "255.255.255.0";
    char eth_gw[16]     = "192.168.1.1";
    char eth_dns[16]    = "8.8.8.8";

    // WiFi (резервный канал, если Ethernet недоступен)
    char wifi_ssid[32]  = "";
    char wifi_pass[32]  = "";

    // Web auth
    char web_user[16]   = "admin";
    char web_pass[32]   = "";

    // Device name (mDNS, MQTT client ID, AP SSID)
    char device_name[32] = "boiler-room";

    // Показывать вкладки диагностики (RS485-консоль, сырые ADC/DI/DO) в nav.
    // По умолчанию выключено — не мозолит глаза при обычной эксплуатации,
    // включается тумблером в Настройках без перезагрузки (см. WebHandler
    // /api/nav — фильтрует по WebTab::visibleFlag).
    bool show_diagnostics = false;
};

// ============================================================
//  SYSTEM STATE — runtime, не сохраняется
// ============================================================
struct SystemState {
    bool ethConnected   = false;
    bool wifiConnected  = false;
    bool apMode         = false;
    // false — ETH.begin() вернул ошибку (драйвер/PHY не поднялись), см. NetworkManager
    bool    ethDriverOk      = true;
    uint8_t ethInitRestarts  = 0;   // перезапусков из-за этого с последнего включения питания
    // true — линк Ethernet есть, но шлюз перестал отвечать (watchdog связности)
    bool    ethUpstreamLost  = false;

    bool mqttConnected  = false;

    bool     pendingReboot = false;
    uint32_t rebootAt      = 0;
};

// ============================================================
//  WEB TAB — регистрация вкладок веб-интерфейса
// ============================================================
struct WebTab {
    const char* id;
    const char* label;
    const char* icon;
    const char* html;
    // nullptr = всегда видна в /api/nav. Если задан — вкладка попадает в
    // список, только пока *visibleFlag == true (проверяется на каждый
    // запрос nav, без перезагрузки платы). См. BaseConfig::show_diagnostics.
    //
    // ВАЖНО: без default member initializer намеренно — тулчейн собирает
    // в режиме C++11, где структура с NSDMI перестаёт быть агрегатом и
    // brace-init `{...}` на местах регистрации вкладок ломается (см. чат,
    // ошибка сборки). Поэтому во всех registerTab({...}) без явного флага
    // нужно передавать nullptr последним аргументом явно.
    const bool* visibleFlag;
};

// ============================================================
//  GLOBALS
// ============================================================
extern BaseConfig        baseCfg;
extern SystemState       sysState;
extern SemaphoreHandle_t coreMutex;
