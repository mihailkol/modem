#pragma once
#ifdef MODULE_MQTT
// Порт из esp32-platform/src/modules/mqtt/MqttHandler.*, но по новой схеме:
// MqttModule не знает про конкретные устройства/приложения — он только
// подписывается на Bus (Bus::subscribeAll) и публикует наружу, а входящие
// команды пишет обратно в Bus. См. ТЗ п.3.3 (MqttModule).
//
// Топики:
//   <device_name>/state/<channel_id>   — публикуется при каждом Bus::write()
//   <device_name>/set/<channel_id>     — входящая уставка, пишется в Bus
//
// Значения передаются в "человеческом" виде (с учётом scale канала),
// т.е. если pump1_setpoint имеет scale=1, "45" в топике .../set/pump1_setpoint
// означает Bus::write("pump1_setpoint", 45).
#include <PubSubClient.h>
#include <WiFi.h>
#include <functional>
#include "CoreTypes.h"

struct MqttConfig {
    char     server[64] = "";
    int      port       = 1883;
    char     user[32]   = "";
    char     pass[32]   = "";
    bool     enabled    = true;
};

extern MqttConfig mqttCfg;

using MqttMessageCallback = std::function<void(const char* topic, const char* payload)>;

class MqttModule {
public:
    static void init();
    static void loop();
    static bool loadConfig();
    static bool saveConfig();

    // Для модулей, которым нужна отдельная подписка на произвольный топик
    // (например TimeManager — на топик времени из Home Assistant).
    static void addSubscription(const char* topic, MqttMessageCallback cb);

private:
    static void _reconnect();
    static void _onMessage(char* topic, byte* payload, unsigned int length);
    static void _onBusWrite(const char* id, int64_t value);
};
#endif // MODULE_MQTT
