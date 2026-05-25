#pragma once
#ifdef MODULE_TIME

#include <Arduino.h>
#include <time.h>

enum class TimeSource {
    NONE = 0,
    MODEM,    // GSM AT+CCLK (уже реализовано в ModemHandler)
    MQTT,     // топик с unix timestamp
    NTP,      // configTime
    RTC       // внешний RTC модуль (будущее)
};

struct TimeConfig {
    // NTP
    bool     ntp_enabled  = true;
    char     ntp_server[48] = "pool.ntp.org";
    int16_t  tz_offset_min = 180;   // UTC+3 (Москва)

    char tz_applied[16] = "";
    
    // MQTT источник
    bool     mqtt_enabled = true;
    char     mqtt_topic[64] = "homeassistant/sensor/time/state"; // HA time entity
};

extern TimeConfig timeCfg;

class TimeManager {
public:
    static void init();
    static void loop();
    static bool loadConfig();
    static bool saveConfig();

    // Основной интерфейс
    static time_t     now();
    static bool       isSynced();
    static TimeSource source();
    static String     sourceStr();
    static String     formatTime(const char* fmt = "%H:%M:%S");
    static String     formatDateTime(const char* fmt = "%d.%m.%Y %H:%M:%S");

    // Вызывается из ModemHandler когда GSM синхронизировал время (уже есть settimeofday)
    static void notifySynced(TimeSource src);

    // Вызывается из MqttHandler при получении сообщения
    static void onMqttMessage(const char* topic, const char* payload);

    static void applyTimezone();

private:
    static TimeSource  _source;
    static bool        _synced;
    static uint32_t    _lastNtpMs;
    static uint32_t    _lastMqttMs;
    static const uint32_t NTP_INTERVAL_MS  = 3600000UL;  // раз в час
    static const uint32_t MQTT_TIMEOUT_MS  = 120000UL;   // 2 мин без MQTT = не источник

    static void _syncNtp();
};

#endif // MODULE_TIME