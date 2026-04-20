#pragma once
#ifdef MODULE_MQTT

#include <PubSubClient.h>
#include <WiFi.h>
#include "../core/CoreTypes.h"

// Конфиг MQTT — сохраняется в /mqtt.json
struct MqttConfig {
    char server[64] = "";
    int  port       = 1883;
    char user[32]   = "";
    char pass[32]   = "";
    bool enabled    = true;
};

extern MqttConfig mqttCfg;

class MqttHandler {
public:
    static void init();   // Вызвать после WebHandler::init()
    static void loop();
    static void publish(const char* topic, const char* payload, bool retained = false);
    static bool loadConfig();
    static bool saveConfig();

    // Callback для входящих сообщений — устройство может переопределить
    static void (*onMessage)(const char* topic, const char* payload, unsigned int len);

private:
    static void reconnect();
    static void _defaultCallback(char* topic, byte* payload, unsigned int length);
};

#endif // MODULE_MQTT
