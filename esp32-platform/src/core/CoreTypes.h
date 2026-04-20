#pragma once
#include <Arduino.h>
#include <freertos/semphr.h>

// ============================================================
//  BASE CONFIG — сетевая часть, общая для всех устройств
// ============================================================
struct BaseConfig {
    // Ethernet
    bool eth_dhcp       = true;
    char eth_ip[16]     = "192.168.1.100";
    char eth_mask[16]   = "255.255.255.0";
    char eth_gw[16]     = "192.168.1.1";
    char eth_dns[16]    = "8.8.8.8";

    // WiFi
    char wifi_ssid[32]  = "";
    char wifi_pass[32]  = "";

    // Web auth
    char web_user[16]   = "admin";
    char web_pass[32]   = "";

    // Device name (используется в mDNS, MQTT client ID, AP SSID)
    char device_name[32] = "esp32-device";
};

// ============================================================
//  SYSTEM STATE — runtime, не сохраняется
// ============================================================
struct SystemState {
    // Сеть
    bool ethConnected   = false;
    bool wifiConnected  = false;
    bool apMode         = false;

    // Модули
    bool mqttConnected  = false;
    bool tgConnected    = false;

    // Управление
    bool pendingReboot  = false;
    uint32_t rebootAt   = 0;
};

// ============================================================
//  WEB TAB — регистрация вкладок модулями/устройствами
// ============================================================
struct WebTab {
    const char* id;       // "telegram", "boiler" — уникальный ID
    const char* label;    // Отображаемое имя
    const char* icon;     // Эмодзи или пусто
    const char* html;     // HTML-контент (PROGMEM или статическая строка)
    // API-маршруты регистрирует сам модуль в своём init()
};

// ============================================================
//  GLOBALS
// ============================================================
extern BaseConfig       baseCfg;
extern SystemState      sysState;
extern SemaphoreHandle_t coreMutex;
