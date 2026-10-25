// ============================================================
//  esp32-boiler-room — main.cpp
//  Сборка модулей по архитектуре из boiler-room-v2-tz.md.
//  Фронтенд — SPA в data/index.html (LittleFS), вкладки отдаются через
//  /api/nav + /api/tab; "Мониторинг" и "Настройки" — см. core/BuiltinTabs.
// ============================================================
#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <ESPmDNS.h>
#include <ArduinoOTA.h>
#include <esp_task_wdt.h>

#include "core/CoreTypes.h"
#include "core/ConfigManager.h"
#include "core/CrashLog.h"
#include "core/NetworkManager.h"
#include "core/WebHandler.h"
#include "core/BuiltinTabs.h"
#include "core/Bus.h"

#ifdef MODULE_TIME
#include "modules/time/TimeManager.h"
#endif
#ifdef MODULE_RS485
#include "modules/rs485/Rs485Handler.h"
#endif
#ifdef MODULE_MQTT
#include "modules/mqtt/MqttModule.h"
#endif

#ifdef DEVICE_A16
#include "devices/a16/A16Device.h"
#endif
#ifdef DEVICE_PUMPSLAVE
#include "devices/pumpslave/PumpSlave.h"
#endif
#ifdef DEVICE_EKONOM
#include "devices/ekonom/EkoNomDevice.h"
#endif
#ifdef DEVICE_MASTERGAS
#include "devices/mastergas/MasterGasDevice.h"
#endif

#ifdef APP_BOILER_ROOM
#include "apps/boiler-room/BoilerRoomApp.h"
#endif

AsyncWebServer server(80);

void setup() {
    Serial.begin(115200);
    Serial.println("\n[BOOT] esp32-boiler-room starting...");

    esp_task_wdt_init(8, true);
    esp_task_wdt_add(NULL);

    // 1. Файловая система и конфиги
    ConfigManager::begin();
    ConfigManager::loadBase();

    CrashLog::begin();

    // 2. Сеть
    NetworkManager::begin();

    // 2a. Устройство, у которого главная вкладка — управление им самим
    //     (газовый котёл MasterGas), регистрируется до "Мониторинга": его
    //     вкладка "Котёл" должна открываться первой.
    #ifdef DEVICE_MASTERGAS
    MasterGasDevice::init();
    #endif

    // 2b. Вкладка "Мониторинг" — регистрируется первой, чтобы быть
    //     дефолтной активной вкладкой в nav (порядок = порядок регистрации)
    BuiltinTabs::registerMonitorTab();
    BuiltinTabs::registerDiagTab();

    // 3. Модули/устройства/приложение — порядок:
    //    Time -> RS485 -> A16 -> PumpSlave -> EkoNom -> BoilerRoomApp -> MQTT
    //    (RS485 до EkoNom — EkoNom сразу шлёт первый запрос через него;
    //     устройства до BoilerRoomApp — приложение читает их raw.* каналы;
    //     MQTT последним из "логики" — Bus::subscribeAll должен успеть
    //     зарегистрироваться до первого loop(), порядок относительно
    //     остальных init() для этого не критичен)

    #ifdef MODULE_TIME
    TimeManager::init();
    #endif

    #ifdef MODULE_RS485
    Rs485Handler::init();
    #endif

    #ifdef DEVICE_A16
    A16Device::init();
    #endif

    #ifdef DEVICE_PUMPSLAVE
    PumpSlave::init();
    #endif

    #ifdef DEVICE_EKONOM
    EkoNomDevice::init(30);   // опрос теплосчётчика каждые 30 сек
    #endif

    #ifdef APP_BOILER_ROOM
    BoilerRoomApp::init();
    #endif

    #ifdef MODULE_MQTT
    MqttModule::init();
    #endif

    // 3b. Вкладка "Настройки" — всегда последняя в nav
    BuiltinTabs::registerSettingsTab();

    // 4. Веб-сервер — регистрирует общие /api/* маршруты и статику.
    //    Вызывается ПОСЛЕ модулей, т.к. они уже добавили свои вкладки/роуты.
    WebHandler::init(server);
    server.begin();

    // 5. OTA
    #ifdef MODULE_OTA
    ArduinoOTA.setHostname(baseCfg.device_name);
    ArduinoOTA.onStart([]() { Serial.println("[OTA] Start"); });
    ArduinoOTA.onError([](ota_error_t e) { Serial.printf("[OTA] Error: %u\n", e); });
    // ВАЖНО: ArduinoOTA.handle() во время реальной передачи данных — это
    // ОДИН блокирующий вызов (_runUpdate() читает весь TCP-поток и пишет
    // во флеш целиком, не отдавая управление обратно в loop()). Наш
    // esp_task_wdt_reset() в loop() стоит СРАЗУ ПОСЛЕ ArduinoOTA.handle(),
    // поэтому весь блокирующий период он не кормит сторож (8с, panic=true,
    // см. setup()) — при заливке дольше 8с (слабый Wi-Fi в котельной)
    // сторож абортит и перезагружает плату посреди прошивки. onProgress()
    // вызывается изнутри этого же блокирующего цикла на каждый
    // записанный кусок — единственное место, откуда можно кормить сторож
    // ВО ВРЕМЯ передачи.
    ArduinoOTA.onProgress([](unsigned int, unsigned int) { esp_task_wdt_reset(); });
    ArduinoOTA.begin();
    #endif

    // 6. mDNS
    MDNS.begin(baseCfg.device_name);
    Serial.printf("[BOOT] Ready at http://%s.local\n", baseCfg.device_name);
}

void loop() {
    #ifdef MODULE_OTA
    ArduinoOTA.handle();
    #endif

    esp_task_wdt_reset();

    CrashLog::mark(TAG_NET);
    NetworkManager::loop();

    #ifdef MODULE_TIME
    CrashLog::mark(TAG_TIME);
    TimeManager::loop();
    #endif

    #ifdef MODULE_RS485
    CrashLog::mark(TAG_RS485);
    Rs485Handler::loop();
    #endif

    #ifdef DEVICE_A16
    CrashLog::mark(TAG_A16);
    A16Device::loop();
    #endif

    #ifdef DEVICE_PUMPSLAVE
    CrashLog::mark(TAG_PUMPSLAVE);
    PumpSlave::loop();
    #endif

    #ifdef DEVICE_EKONOM
    CrashLog::mark(TAG_EKONOM);
    EkoNomDevice::loop();
    #endif

    #ifdef APP_BOILER_ROOM
    CrashLog::mark(TAG_APP);
    BoilerRoomApp::loop();
    #endif

    #ifdef DEVICE_MASTERGAS
    CrashLog::mark(TAG_MASTERGAS);
    MasterGasDevice::loop();
    #endif

    #ifdef MODULE_MQTT
    CrashLog::mark(TAG_MQTT);
    MqttModule::loop();
    #endif

    // Перезагрузка по запросу из веб-интерфейса
    xSemaphoreTake(coreMutex, portMAX_DELAY);
    bool reboot = sysState.pendingReboot && (millis() - sysState.rebootAt > 500);
    xSemaphoreGive(coreMutex);
    if (reboot) ESP.restart();
}
