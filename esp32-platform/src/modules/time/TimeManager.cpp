#ifdef MODULE_TIME
#include "TimeManager.h"
#include "../../core/ConfigManager.h"
#include "../../core/WebHandler.h"
#include "../../core/CoreTypes.h"
#ifdef MODULE_MQTT
#include "../mqtt/MqttHandler.h"
#endif
#include <ArduinoJson.h>
#include <AsyncJson.h>

#ifdef TIME_SOURCE_NTP
#include <esp_sntp.h>
#endif

// ── Глобальные данные ────────────────────────────────────────────────────────
TimeConfig timeCfg;


// ── Статика ──────────────────────────────────────────────────────────────────
TimeSource  TimeManager::_source      = TimeSource::NONE;
bool        TimeManager::_synced      = false;
uint32_t    TimeManager::_lastNtpMs   = 0;
uint32_t    TimeManager::_lastMqttMs  = 0;

// ── Основной интерфейс ───────────────────────────────────────────────────────
time_t TimeManager::now() {
    return time(nullptr);
}

bool TimeManager::isSynced() {
    return _synced;
}

TimeSource TimeManager::source() {
    return _source;
}

String TimeManager::sourceStr() {
    switch (_source) {
        case TimeSource::MODEM: return "gsm";
        case TimeSource::MQTT:  return "mqtt";
        case TimeSource::NTP:   return "ntp";
        case TimeSource::RTC:   return "rtc";
        default:                return "none";
    }
}

String TimeManager::formatTime(const char* fmt) {
    if (!_synced) return "--:--:--";
    time_t t = now() + (time_t)timeCfg.tz_offset_min * 60L;  // ← offset вручную
    struct tm* tm = gmtime(&t);  // ← gmtime, не localtime
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

// ── Уведомление от внешнего источника (GSM/RTC) ──────────────────────────────
void TimeManager::notifySynced(TimeSource src) {
    // GSM и RTC имеют приоритет над MQTT и NTP
    if (src == TimeSource::MODEM || src == TimeSource::RTC) {
        _source = src;
        _synced = true;
        Serial.printf("[TIME] Synced from %s: %s\n",
            sourceStr().c_str(), formatDateTime().c_str());
    }
}

// ── MQTT источник ─────────────────────────────────────────────────────────────
void TimeManager::onMqttMessage(const char* topic, const char* payload) {
    if (!timeCfg.mqtt_enabled) return;
    if (strcmp(topic, timeCfg.mqtt_topic) != 0) return;
    // Принимаем unix timestamp (число) или ISO строку "2024-01-15T12:34:56"
    String s = String(payload);
    time_t t = 0;
    if (s.length() == 19 && s[4] == '-') {
        // ISO формат: "2024-01-15T12:34:56"
        struct tm tm = {};
        tm.tm_year = s.substring(0, 4).toInt() - 1900;
        tm.tm_mon  = s.substring(5, 7).toInt() - 1;
        tm.tm_mday = s.substring(8, 10).toInt();
        tm.tm_hour = s.substring(11, 13).toInt();
        tm.tm_min  = s.substring(14, 16).toInt();
        tm.tm_sec  = s.substring(17, 19).toInt();
        t = mktime(&tm);
    } else {
        // Unix timestamp
        t = (time_t)s.toInt();
    }
    if (t < 1700000000UL) return;  // sanity check (после 2023)

    struct timeval tv = { t, 0 };
    settimeofday(&tv, nullptr);
    _lastMqttMs = millis();

    // MQTT источник только если нет GSM/RTC
    if (_source == TimeSource::NONE || _source == TimeSource::NTP
        || _source == TimeSource::MQTT) {
        _source = TimeSource::MQTT;
        _synced = true;
        Serial.printf("[TIME] Synced from MQTT: %s\n", formatDateTime().c_str());
    }
}

// ── NTP ──────────────────────────────────────────────────────────────────────
void TimeManager::_syncNtp() {
#ifdef TIME_SOURCE_NTP
    if (!timeCfg.ntp_enabled) return;
    if (_source == TimeSource::MODEM || _source == TimeSource::RTC) return;

    // Формируем POSIX TZ строку: UTC смещение (знак инвертирован в POSIX!)
    // UTC+3 → "UTC-3", UTC-5 → "UTC+5"
    int h = timeCfg.tz_offset_min / 60;
    int m = abs(timeCfg.tz_offset_min % 60);
    char tz_str[16];
    if (m == 0)
        snprintf(tz_str, sizeof(tz_str), "UTC%+d", -h);
    else
        snprintf(tz_str, sizeof(tz_str), "UTC%+d:%02d", -h, m);
    setenv("TZ", tz_str, 1);
    tzset();

    configTime(0, 0, timeCfg.ntp_server);  // offset=0, timezone через TZ env

    Serial.printf("[TIME] NTP sync: %s TZ=%s\n", timeCfg.ntp_server, tz_str);
    _lastNtpMs = millis();
#endif
}

// ── INIT ─────────────────────────────────────────────────────────────────────
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
        time_t t = now();
        struct tm* ltm = localtime(&t);
        struct tm* gtm = gmtime(&t);
        char local_buf[32], gmt_buf[32];
        strftime(local_buf, sizeof(local_buf), "%H:%M:%S", ltm);
        strftime(gmt_buf,   sizeof(gmt_buf),   "%H:%M:%S", gtm);
        doc["synced"]     = _synced;
        doc["unix"]       = (uint32_t)t;
        doc["source"]     = sourceStr();
        doc["time_str"]   = formatTime();
        doc["date_str"]   = formatDateTime();
        String out; serializeJson(doc, out);
        req->send(200, "application/json", out);
    });

    // server.on("/api/time", HTTP_GET, [](AsyncWebServerRequest* req) {
    //     JsonDocument doc;
    //     doc["synced"]   = _synced;
    //     doc["unix"]     = (uint32_t)now();
    //     doc["source"]   = sourceStr();
    //     doc["time_str"] = formatTime();
    //     doc["date_str"] = formatDateTime();
    //     doc["tz_applied"] = f.tz_applied;
    //     String out; serializeJson(doc, out);
    //     req->send(200, "application/json", out);
    // });


    auto* hSave = new AsyncCallbackJsonWebHandler("/api/time/save",
        [](AsyncWebServerRequest* req, JsonVariant& json) {
            JsonObject o = json.as<JsonObject>();
            timeCfg.tz_offset_min = o["tz_offset_min"] | 180;
            strlcpy(timeCfg.ntp_server,  o["ntp_server"]  | "pool.ntp.org", sizeof(timeCfg.ntp_server));
            strlcpy(timeCfg.mqtt_topic,  o["mqtt_topic"]  | "", sizeof(timeCfg.mqtt_topic));
            timeCfg.ntp_enabled  = o["ntp_enabled"]  | true;
            timeCfg.mqtt_enabled = o["mqtt_enabled"] | true;
            TimeManager::saveConfig();
            TimeManager::applyTimezone();
            req->send(200, "application/json", "{\"status\":\"ok\"}");
        }
    );
    server.addHandler(hSave);

    // Запускаем NTP если нет более приоритетного источника
    if (timeCfg.ntp_enabled) _syncNtp();

    // Подписка на MQTT топик времени
#ifdef MODULE_MQTT
    if (timeCfg.mqtt_enabled && strlen(timeCfg.mqtt_topic) > 0)
        MqttHandler::addSubscription(timeCfg.mqtt_topic);
#endif

    Serial.println("[TIME] Init OK");
}

// ── LOOP ─────────────────────────────────────────────────────────────────────
void TimeManager::loop() {
    uint32_t now_ms = millis();

#ifdef TIME_SOURCE_NTP
    // Проверяем успешность NTP синхронизации
    if (timeCfg.ntp_enabled
        && (_source == TimeSource::NONE || _source == TimeSource::NTP)
        && !_synced) {
        time_t t = time(nullptr);
        if (t > 1700000000UL) {
            _source = TimeSource::NTP;
            _synced = true;
            Serial.printf("[TIME] NTP synced: %s\n", formatDateTime().c_str());
        }
    }
    // Периодическая ресинхронизация NTP
    if (timeCfg.ntp_enabled
        && _source == TimeSource::NTP
        && (now_ms - _lastNtpMs) > NTP_INTERVAL_MS) {
        _syncNtp();
    }
#endif

    // Если MQTT источник давно не присылал — сбрасываем приоритет
    if (_source == TimeSource::MQTT
        && (now_ms - _lastMqttMs) > MQTT_TIMEOUT_MS) {
        Serial.println("[TIME] MQTT time source lost");
        _source = TimeSource::NONE;
        _synced = false;
    }
}

// ── Конфиг ───────────────────────────────────────────────────────────────────
bool TimeManager::loadConfig() {
    JsonDocument doc;
    if (!ConfigManager::loadJson("/time.json", doc)) return false;
    timeCfg.tz_offset_min = doc["tz_offset_min"] | 180;
    timeCfg.ntp_enabled   = doc["ntp_enabled"]   | true;
    timeCfg.mqtt_enabled  = doc["mqtt_enabled"]  | true;
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
    // Сбрасываем TZ env чтобы не мешал
    setenv("TZ", "UTC0", 1);
    tzset();
    configTime(offset_sec, 0, timeCfg.ntp_enabled ? timeCfg.ntp_server : "");
    strlcpy(timeCfg.tz_applied, String(offset_sec).c_str(), sizeof(timeCfg.tz_applied));
    Serial.printf("[TIME] configTime offset=%lds server=%s\n", offset_sec, timeCfg.ntp_server);
}

#endif // MODULE_TIME