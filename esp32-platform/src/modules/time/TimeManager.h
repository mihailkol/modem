#pragma once
#ifdef MODULE_TIME
// Порт из esp32-platform/src/modules/time/TimeManager.*, упрощён:
// источники только NTP и MQTT (GSM/RTC из старого проекта тут не нужны).
#include <Arduino.h>
#include <time.h>

enum class TimeSource {
    NONE = 0,
    MQTT,     // топик с unix timestamp / ISO-строкой
    NTP,      // configTime
};

struct TimeConfig {
    bool     ntp_enabled    = true;
    char     ntp_server[48] = "pool.ntp.org";
    int16_t  tz_offset_min  = 180;   // UTC+3 (Москва)

    bool     mqtt_enabled   = false;
    char     mqtt_topic[64] = "";
};

extern TimeConfig timeCfg;

class TimeManager {
public:
    static void init();
    static void loop();
    static bool loadConfig();
    static bool saveConfig();

    static time_t     now();
    static bool       isSynced();
    static TimeSource source();
    static String     sourceStr();
    static String     formatTime(const char* fmt = "%H:%M:%S");
    static String     formatDateTime(const char* fmt = "%d.%m.%Y %H:%M:%S");

    static void applyTimezone();

private:
    static TimeSource  _source;
    static bool        _synced;
    static uint32_t    _lastNtpMs;
    static uint32_t    _lastMqttMs;
    static const uint32_t NTP_INTERVAL_MS = 3600000UL;  // раз в час
    static const uint32_t MQTT_TIMEOUT_MS = 120000UL;   // 2 мин без MQTT = не источник

    static void _syncNtp();
    static void _onMqttMessage(const char* topic, const char* payload);
};
#endif // MODULE_TIME
