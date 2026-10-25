#ifdef MODULE_TIME
#include "TimeManager.h"
#include "ConfigManager.h"
#include "WebHandler.h"
#include "CoreTypes.h"
#ifdef MODULE_MQTT
#include "MqttModule.h"
#endif
#include <ArduinoJson.h>
#include <AsyncJson.h>

#ifdef TIME_SOURCE_NTP
#include <esp_sntp.h>
#endif

TimeConfig timeCfg;

TimeSource TimeManager::_source     = TimeSource::NONE;
bool       TimeManager::_synced     = false;
uint32_t   TimeManager::_lastNtpMs  = 0;
uint32_t   TimeManager::_lastMqttMs = 0;

time_t TimeManager::now() { return time(nullptr); }
bool   TimeManager::isSynced() { return _synced; }
TimeSource TimeManager::source() { return _source; }

String TimeManager::sourceStr() {
    switch (_source) {
        case TimeSource::MQTT: return "mqtt";
        case TimeSource::NTP:  return "ntp";
        default:               return "none";
    }
}

String TimeManager::formatTime(const char* fmt) {
    if (!_synced) return "--:--:--";
    time_t t = now() + (time_t)timeCfg.tz_offset_min * 60L;
    struct tm* tm = gmtime(&t);
    char buf[32];
    strftime(buf, sizeof(buf), fmt, tm);
    return String(buf);
}

String TimeManager::formatDateTime(const char* fmt) {
    if (!_synced) return "не синхронизировано";
    time_t t = now();
    struct tm* tm = localtime(&t);
    char buf[32];
    strftime(buf, sizeof(buf), fmt, tm);
    return String(buf);
}

// ── MQTT источник ────────────────────────────────────────────────────
void TimeManager::_onMqttMessage(const char* topic, const char* payload) {
    (void)topic;
    if (!timeCfg.mqtt_enabled) return;

    String s(payload);
    time_t t = 0;
    if (s.length() == 19 && s[4] == '-') {
        struct tm tm = {};
        tm.tm_year = s.substring(0, 4).toInt() - 1900;
        tm.tm_mon  = s.substring(5, 7).toInt() - 1;
        tm.tm_mday = s.substring(8, 10).toInt();
        tm.tm_hour = s.substring(11, 13).toInt();
        tm.tm_min  = s.substring(14, 16).toInt();
        tm.tm_sec  = s.substring(17, 19).toInt();
        t = mktime(&tm);
    } else {
        t = (time_t)s.toInt();
    }
    if (t < 1700000000UL) return;  // sanity check (после 2023)

    struct timeval tv = { t, 0 };
    settimeofday(&tv, nullptr);
    _lastMqttMs = millis();

    if (_source == TimeSource::NONE || _source == TimeSource::NTP || _source == TimeSource::MQTT) {
        _source = TimeSource::MQTT;
        _synced = true;
        Serial.printf("[TIME] Synced from MQTT: %s\n", formatDateTime().c_str());
    }
}

// ── NTP ──────────────────────────────────────────────────────────────
void TimeManager::_syncNtp() {
#ifdef TIME_SOURCE_NTP
    if (!timeCfg.ntp_enabled) return;

    int h = timeCfg.tz_offset_min / 60;
    int m = abs(timeCfg.tz_offset_min % 60);
    char tz_str[16];
    if (m == 0)
        snprintf(tz_str, sizeof(tz_str), "UTC%+d", -h);
    else
        snprintf(tz_str, sizeof(tz_str), "UTC%+d:%02d", -h, m);
    setenv("TZ", tz_str, 1);
    tzset();

    configTime(0, 0, timeCfg.ntp_server);

    Serial.printf("[TIME] NTP sync: %s TZ=%s\n", timeCfg.ntp_server, tz_str);
    _lastNtpMs = millis();
#endif
}

// ── INIT ─────────────────────────────────────────────────────────────
void TimeManager::init() {
    loadConfig();
    applyTimezone();

    extern AsyncWebServer server;

    server.on("/api/time/config", HTTP_GET, [](AsyncWebServerRequest* req) {
        JsonDocument doc;
        doc["tz_offset_min"] = timeCfg.tz_offset_min;
        doc["ntp_server"]    = timeCfg.ntp_server;
        doc["mqtt_topic"]    = timeCfg.mqtt_topic;
        doc["ntp_enabled"]   = timeCfg.ntp_enabled;
        doc["mqtt_enabled"]  = timeCfg.mqtt_enabled;
        String out; serializeJson(doc, out);
        req->send(200, "application/json", out);
    });

    server.on("/api/time", HTTP_GET, [](AsyncWebServerRequest* req) {
        JsonDocument doc;
        doc["synced"]   = _synced;
        doc["unix"]     = (uint32_t)now();
        doc["source"]   = sourceStr();
        doc["time_str"] = formatTime();
        doc["date_str"] = formatDateTime();
        String out; serializeJson(doc, out);
        req->send(200, "application/json", out);
    });

    auto* hSave = new AsyncCallbackJsonWebHandler("/api/time/save",
        [](AsyncWebServerRequest* req, JsonVariant& json) {
            JsonObject o = json.as<JsonObject>();
            timeCfg.tz_offset_min = o["tz_offset_min"] | 180;
            strlcpy(timeCfg.ntp_server, o["ntp_server"] | "pool.ntp.org", sizeof(timeCfg.ntp_server));
            strlcpy(timeCfg.mqtt_topic, o["mqtt_topic"] | "", sizeof(timeCfg.mqtt_topic));
            timeCfg.ntp_enabled  = o["ntp_enabled"]  | true;
            timeCfg.mqtt_enabled = o["mqtt_enabled"] | false;
            TimeManager::saveConfig();
            TimeManager::applyTimezone();
            req->send(200, "application/json", "{\"status\":\"ok\"}");
        }
    );
    server.addHandler(hSave);

    if (timeCfg.ntp_enabled) _syncNtp();

#ifdef MODULE_MQTT
    if (timeCfg.mqtt_enabled && strlen(timeCfg.mqtt_topic) > 0)
        MqttModule::addSubscription(timeCfg.mqtt_topic, _onMqttMessage);
#endif

    Serial.println("[TIME] Init OK");
}

// ── LOOP ─────────────────────────────────────────────────────────────
void TimeManager::loop() {
    uint32_t now_ms = millis();

#ifdef TIME_SOURCE_NTP
    if (timeCfg.ntp_enabled && (_source == TimeSource::NONE || _source == TimeSource::NTP) && !_synced) {
        time_t t = time(nullptr);
        if (t > 1700000000UL) {
            _source = TimeSource::NTP;
            _synced = true;
            Serial.printf("[TIME] NTP synced: %s\n", formatDateTime().c_str());
        }
    }
    if (timeCfg.ntp_enabled && _source == TimeSource::NTP && (now_ms - _lastNtpMs) > NTP_INTERVAL_MS) {
        _syncNtp();
    }
#endif

    if (_source == TimeSource::MQTT && (now_ms - _lastMqttMs) > MQTT_TIMEOUT_MS) {
        Serial.println("[TIME] MQTT time source lost");
        _source = TimeSource::NONE;
        _synced = false;
    }
}

// ── Конфиг ───────────────────────────────────────────────────────────
bool TimeManager::loadConfig() {
    JsonDocument doc;
    if (!ConfigManager::loadJson("/time.json", doc)) return false;
    timeCfg.tz_offset_min = doc["tz_offset_min"] | 180;
    timeCfg.ntp_enabled   = doc["ntp_enabled"]   | true;
    timeCfg.mqtt_enabled  = doc["mqtt_enabled"]  | false;
    strlcpy(timeCfg.ntp_server, doc["ntp_server"] | "pool.ntp.org", sizeof(timeCfg.ntp_server));
    strlcpy(timeCfg.mqtt_topic, doc["mqtt_topic"] | "", sizeof(timeCfg.mqtt_topic));
    return true;
}

bool TimeManager::saveConfig() {
    JsonDocument doc;
    doc["tz_offset_min"] = timeCfg.tz_offset_min;
    doc["ntp_enabled"]   = timeCfg.ntp_enabled;
    doc["mqtt_enabled"]  = timeCfg.mqtt_enabled;
    doc["ntp_server"]    = timeCfg.ntp_server;
    doc["mqtt_topic"]    = timeCfg.mqtt_topic;
    return ConfigManager::saveJson("/time.json", doc);
}

void TimeManager::applyTimezone() {
    long offset_sec = timeCfg.tz_offset_min * 60L;
    setenv("TZ", "UTC0", 1);
    tzset();
    configTime(offset_sec, 0, timeCfg.ntp_enabled ? timeCfg.ntp_server : "");
    Serial.printf("[TIME] configTime offset=%lds server=%s\n", offset_sec, timeCfg.ntp_server);
}

#endif // MODULE_TIME
