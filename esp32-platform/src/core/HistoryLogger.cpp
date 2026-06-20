#ifdef MODULE_HISTORY

#include "HistoryLogger.h"
#include "ConfigManager.h"
#include <ArduinoJson.h>
#include <AsyncJson.h>
#include <time.h>
#include "CrashLog.h"
#include <esp_task_wdt.h>

#ifdef MODULE_TIME
#include "../modules/time/TimeManager.h"
#endif

// ============================================================
//  СТАТИЧЕСКИЕ ЧЛЕНЫ
// ============================================================
std::vector<DataChannel*>             HistoryLogger::_channels;
uint8_t   HistoryLogger::_l0buf[HISTORY_L0_SIZE * HISTORY_L0_EVENT_SIZE] = {};
uint16_t  HistoryLogger::_l0head  = 0;
uint16_t  HistoryLogger::_l0count = 0;
ChannelAgg* HistoryLogger::_l1agg = nullptr;
ChannelAgg* HistoryLogger::_l2agg = nullptr;
std::vector<std::vector<uint8_t>> HistoryLogger::_l1pending;
std::vector<std::vector<uint8_t>> HistoryLogger::_l2pending;
uint32_t  HistoryLogger::_lastSampleMs = 0;
uint32_t  HistoryLogger::_lastL1Ms    = 0;
uint32_t  HistoryLogger::_lastL2Ms    = 0;
uint32_t  HistoryLogger::_lastFlushMs = 0;
bool      HistoryLogger::_ready       = false;

// ============================================================
//  ТЕКУЩИЙ TIMESTAMP
// ============================================================
uint32_t HistoryLogger::_now() {
#ifdef MODULE_TIME
    int32_t offset = TimeManager::tsOffset();
    return (uint32_t)(millis() / 1000) + (uint32_t)offset;
#else
    time_t t = time(nullptr);
    return (t > 1000000) ? (uint32_t)t : (uint32_t)(millis() / 1000);
#endif
}

// ============================================================
//  BEGIN
// ============================================================
void HistoryLogger::begin(AsyncWebServer& server) {
    if (DataChannel::all().empty()) {
        Serial.println("[HIST] No channels registered");
        return;
    }
    if (!LittleFS.exists("/history"))
        LittleFS.mkdir("/history");

    _loadOverrides();
    _buildActiveList();

    if (_channels.empty()) {
        Serial.println("[HIST] No history channels enabled");
        return;
    }

    if (!_checkFingerprint()) {
        Serial.println("[HIST] Fingerprint changed, invalidating");
        _invalidateFiles();
        _saveOverrides();
    }

    uint8_t nCh = _channels.size();
    _l1agg = new ChannelAgg[nCh]();
    _l2agg = new ChannelAgg[nCh]();

    _registerRoutes(server);

    uint32_t now = millis();
    _lastSampleMs = now;
    _lastL1Ms     = now;
    _lastL2Ms     = now;
    _lastFlushMs  = now;
    _ready = true;

    Serial.printf("[HIST] Ready: %u channels, L0=%u events\n",
                  nCh, HISTORY_L0_SIZE);
}

// ============================================================
//  LOOP
// ============================================================
void HistoryLogger::loop() {
    if (!_ready) return;
    uint32_t now = millis();

    if (now - _lastSampleMs >= SAMPLE_INTERVAL_MS) {
        _lastSampleMs = now;
        _tickSample();
    }
    if (now - _lastL1Ms >= (uint32_t)HISTORY_L1_INTERVAL_SEC * 1000u) {
        _lastL1Ms = now;
        _finalizeL1();
    }
    if (now - _lastL2Ms >= (uint32_t)HISTORY_L2_INTERVAL_SEC * 1000u) {
        _lastL2Ms = now;
        _finalizeL2();
    }
    if (now - _lastFlushMs >= HISTORY_FLUSH_INTERVAL_MS) {
        _lastFlushMs = now;
        _flushPending();
    }
}

// ============================================================
//  ОПРОС ГЕТТЕРОВ — пишем событие только при изменении
// ============================================================
void HistoryLogger::_tickSample() {
    uint8_t  nCh  = _channels.size();
    uint32_t ts   = _now();
    uint32_t dtMs = SAMPLE_INTERVAL_MS;

    for (uint8_t i = 0; i < nCh; i++) {
        DataChannel* ch = _channels[i];
        float fval = ch->getter ? ch->getter() : 0.0f;
        if (isnan(fval)) continue;   // нет данных — этот тик просто не учитываем

        // Конвертируем в int16 для сравнения и хранения
        int16_t ival;
        if (ch->type == CH_FLOAT) {
            ival = (int16_t)(fval * ch->scale);
        } else if (ch->type == CH_COUNTER) {
            ival = (int16_t)fval;  // целая часть для сравнения
        } else {
            ival = fval > 0.5f ? 1 : 0;
        }

        // Пушим в аккумулятор L1 всегда
        _l1agg[i].push(fval, dtMs, ch->type);

        // В L0 пишем только если изменилось
        if (ival == ch->_lastLoggedInt) continue;
        ch->_lastLoggedInt = ival;

        // Записываем событие в L0 кольцевой буфер
        uint8_t* slot = &_l0buf[_l0head * HISTORY_L0_EVENT_SIZE];
        slot[0] = (ts >> 0)  & 0xFF;
        slot[1] = (ts >> 8)  & 0xFF;
        slot[2] = (ts >> 16) & 0xFF;
        slot[3] = (ts >> 24) & 0xFF;
        slot[4] = i;  // индекс канала
        slot[5] = (ival >> 0) & 0xFF;
        slot[6] = (ival >> 8) & 0xFF;

        _l0head = (_l0head + 1) % HISTORY_L0_SIZE;
        if (_l0count < HISTORY_L0_SIZE) _l0count++;
    }
}

// ============================================================
//  ФИНАЛИЗАЦИЯ L1 — раз в минуту
// ============================================================
void HistoryLogger::_finalizeL1() {
    uint8_t  nCh = _channels.size();
    uint32_t ts  = _now();
    uint32_t dtMs = (uint32_t)HISTORY_L1_INTERVAL_SEC * 1000u;

    for (uint8_t i = 0; i < nCh; i++) {
        if (_l1agg[i].count == 0) { _l1agg[i].reset(); continue; }

        int16_t avg, mn, mx;
        if (_channels[i]->type == CH_FLOAT) {
            avg = (int16_t)(_l1agg[i].avg()      * _channels[i]->scale);
            mn  = (int16_t)(_l1agg[i].floatMin() * _channels[i]->scale);
            mx  = (int16_t)(_l1agg[i].floatMax() * _channels[i]->scale);
        } else if (_channels[i]->type == CH_COUNTER) {
            avg = (int16_t)_l1agg[i].last;
            mn = mx = 0;
        } else {
            avg = _l1agg[i].boolPct();
            mn = mx = 0;
        }

        _l1pending.push_back(_serializeEvent(ts, i, avg, mn, mx));

        // Пушим в L2 аккумулятор
        float representative = (_channels[i]->type == CH_FLOAT)
            ? _l1agg[i].avg()
            : (_channels[i]->type == CH_COUNTER)
                ? _l1agg[i].last
                : (_l1agg[i].boolPct() > 50 ? 1.0f : 0.0f);
        _l2agg[i].push(representative, dtMs, _channels[i]->type);

        _l1agg[i].reset();
    }
}

// ============================================================
//  ФИНАЛИЗАЦИЯ L2 — раз в 10 минут
// ============================================================
void HistoryLogger::_finalizeL2() {
    uint8_t  nCh = _channels.size();
    uint32_t ts  = _now();

    for (uint8_t i = 0; i < nCh; i++) {
        if (_l2agg[i].count == 0) { _l2agg[i].reset(); continue; }

        int16_t avg, mn, mx;
        if (_channels[i]->type == CH_FLOAT) {
            avg = (int16_t)(_l2agg[i].avg()      * _channels[i]->scale);
            mn  = (int16_t)(_l2agg[i].floatMin() * _channels[i]->scale);
            mx  = (int16_t)(_l2agg[i].floatMax() * _channels[i]->scale);
        } else if (_channels[i]->type == CH_COUNTER) {
            avg = (int16_t)_l2agg[i].last;
            mn = mx = 0;
        } else {
            avg = _l2agg[i].boolPct();
            mn = mx = 0;
        }

        _l2pending.push_back(_serializeEvent(ts, i, avg, mn, mx));
        _l2agg[i].reset();
    }
}

// ============================================================
//  СЕРИАЛИЗАЦИЯ СОБЫТИЯ
// ============================================================
std::vector<uint8_t> HistoryLogger::_serializeEvent(
    uint32_t ts, uint8_t ch, int16_t avg, int16_t mn, int16_t mx)
{
    std::vector<uint8_t> buf(HISTORY_FILE_EVENT_SIZE);
    buf[0] = (ts  >> 0)  & 0xFF; buf[1] = (ts  >> 8)  & 0xFF;
    buf[2] = (ts  >> 16) & 0xFF; buf[3] = (ts  >> 24) & 0xFF;
    buf[4] = ch;
    buf[5] = (avg >> 0) & 0xFF;  buf[6] = (avg >> 8) & 0xFF;
    buf[7] = (mn  >> 0) & 0xFF;  buf[8] = (mn  >> 8) & 0xFF;
    buf[9] = (mx  >> 0) & 0xFF;  buf[10]= (mx  >> 8) & 0xFF;
    return buf;
}

// ============================================================
//  СБРОС НА ДИСК
// ============================================================
void HistoryLogger::_flushPending() {
    CrashLog::mark(TAG_HIST_FLUSH);
    uint32_t t0 = millis();
    if (!_l1pending.empty()) {
        _flushLevel("/history/l1.bin", "/history/l1.bin.prev", _l1pending);
        CrashLog::mark(TAG_HIST_ROTATE);
        _rotateIfNeeded("/history/l1.bin", "/history/l1.bin.prev",
                        (uint32_t)HISTORY_L1_HOURS * 3600 / HISTORY_L1_INTERVAL_SEC
                        * _channels.size() * 2);  // с запасом ×2
        _l1pending.clear();
    }
    yield();
    esp_task_wdt_reset();
    if (!_l2pending.empty()) {
        _flushLevel("/history/l2.bin", "/history/l2.bin.prev", _l2pending);
        CrashLog::mark(TAG_HIST_ROTATE);
        _rotateIfNeeded("/history/l2.bin", "/history/l2.bin.prev",
                        (uint32_t)HISTORY_L2_DAYS * 86400 / HISTORY_L2_INTERVAL_SEC
                        * _channels.size() * 2);
        _l2pending.clear();
    }
    uint32_t dt = millis() - t0;
    if (dt > 250) Serial.printf("[HIST] SLOW flush: %u ms\n", dt);
}

void HistoryLogger::_flushLevel(const char* path, const char* prevPath,
                                 std::vector<std::vector<uint8_t>>& pending)
{
    File f = LittleFS.open(path, "a");
    if (!f) { Serial.printf("[HIST] Failed to open %s\n", path); return; }
    for (auto& rec : pending) { f.write(rec.data(), rec.size()); yield(); }
    f.close();
}

 void HistoryLogger::_rotateIfNeeded(const char* path, const char* prevPath,
                                      uint32_t maxEvents)
 {
     File f = LittleFS.open(path, "r");
     if (!f) return;
     uint32_t events = f.size() / HISTORY_FILE_EVENT_SIZE;
     f.close();
     if (events >= maxEvents) {
        if (LittleFS.exists(prevPath)) {
            LittleFS.remove(prevPath);
            yield();
            esp_task_wdt_reset();
        }
         LittleFS.rename(path, prevPath);
        yield();
         Serial.printf("[HIST] Rotated %s (%u events)\n", path, events);
     }
 }

// ============================================================
//  FINGERPRINT
// ============================================================
String HistoryLogger::_buildFingerprint() {
    String fp;
    for (size_t i = 0; i < _channels.size(); i++) {
        if (i > 0) fp += ':';
        fp += _channels[i]->id;
        fp += '/';
        fp += (uint8_t)_channels[i]->type;
    }
    return fp;
}

bool HistoryLogger::_checkFingerprint() {
    JsonDocument doc;
    if (!ConfigManager::loadJson("/history/channels.json", doc)) return false;
    String stored = doc["fingerprint"] | "";
    return stored == _buildFingerprint();
}

void HistoryLogger::_invalidateFiles() {
    const char* files[] = {
        "/history/l1.bin", "/history/l1.bin.prev",
        "/history/l2.bin", "/history/l2.bin.prev"
    };
    for (auto* f : files)
        if (LittleFS.exists(f)) LittleFS.remove(f);
}

// ============================================================
//  OVERRIDES
// ============================================================
void HistoryLogger::_loadOverrides() {
    JsonDocument doc;
    if (!ConfigManager::loadJson("/history/channels.json", doc)) return;
    JsonObject chans = doc["channels"].as<JsonObject>();
    if (chans.isNull()) return;
    for (auto* ch : DataChannel::all()) {
        if (!chans.containsKey(ch->id)) continue;
        JsonObject o = chans[ch->id].as<JsonObject>();
        if (o.containsKey("enabled")) ch->_enabled = o["enabled"].as<bool>();
        const char* lbl = o["label"] | "";
        if (lbl && strlen(lbl) > 0) ch->setLabel(lbl);
        if (o.containsKey("y_min")) ch->y_min = o["y_min"].as<float>();
        if (o.containsKey("y_max")) ch->y_max = o["y_max"].as<float>();
    }
}

void HistoryLogger::_saveOverrides() {
    JsonDocument doc;
    doc["fingerprint"] = _buildFingerprint();
    JsonObject chans = doc["channels"].to<JsonObject>();
    for (auto* ch : DataChannel::all()) {
        JsonObject o = chans[ch->id].to<JsonObject>();
        o["enabled"] = ch->_enabled;
        o["label"]   = ch->label();
        o["unit"]    = ch->unit;
        o["type"]    = (uint8_t)ch->type;
        o["history"] = ch->history;
        o["mqtt"]    = ch->mqtt;
        if (!isnan(ch->y_min)) o["y_min"] = ch->y_min;
        if (!isnan(ch->y_max)) o["y_max"] = ch->y_max;
    }
    ConfigManager::saveJson("/history/channels.json", doc);
}

void HistoryLogger::_buildActiveList() {
    _channels.clear();
    for (auto* ch : DataChannel::all())
        if (ch->history && ch->_enabled)
            _channels.push_back(ch);
}

uint8_t HistoryLogger::channelCount() { return _channels.size(); }
DataChannel* HistoryLogger::channel(uint8_t idx) {
    return (idx < _channels.size()) ? _channels[idx] : nullptr;
}

// ============================================================
//  REST — L0 из RAM
//  Отдаём события за последние `seconds` секунд
// ============================================================
void HistoryLogger::_sendL0(AsyncWebServerRequest* req, uint32_t seconds) {
    uint8_t nCh = _channels.size();

    if (_l0count == 0) {
        req->send(200, "application/json", "{\"ts\":[],\"channels\":[]}");
        return;
    }

    uint32_t cutoff = (_now() > seconds) ? (_now() - seconds) : 0;

    // Формируем ответ: для каждого канала массив {ts, val}
    JsonDocument doc;
    JsonArray chArr = doc["channels"].to<JsonArray>();
    JsonArray tsPerCh[nCh];  // массивы ts для каждого канала
    JsonArray valPerCh[nCh]; // массивы val

    for (uint8_t i = 0; i < nCh; i++) {
        JsonObject chObj = chArr.add<JsonObject>();
        chObj["id"]    = _channels[i]->id;
        chObj["label"] = _channels[i]->label();
        chObj["unit"]  = _channels[i]->unit;
        chObj["type"]  = (uint8_t)_channels[i]->type;
        tsPerCh[i]  = chObj["ts"].to<JsonArray>();
        valPerCh[i] = chObj["values"].to<JsonArray>();
    }

    // Читаем буфер в хронологическом порядке
    uint16_t startIdx = (_l0count < HISTORY_L0_SIZE) ? 0 : _l0head;
    for (uint16_t n = 0; n < _l0count; n++) {
        uint16_t idx = (startIdx + n) % HISTORY_L0_SIZE;
        uint8_t* slot = &_l0buf[idx * HISTORY_L0_EVENT_SIZE];
        uint32_t ts = (uint32_t)slot[0] | ((uint32_t)slot[1]<<8)
                    | ((uint32_t)slot[2]<<16) | ((uint32_t)slot[3]<<24);
        if (ts < cutoff) continue;
        uint8_t  ch  = slot[4];
        int16_t  val = (int16_t)(slot[5] | (slot[6]<<8));
        if (ch >= nCh) continue;
        tsPerCh[ch].add(ts);
        float fval = (_channels[ch]->type == CH_FLOAT)
            ? (float)val / _channels[ch]->scale
            : (float)val;
        valPerCh[ch].add(fval);
    }

    String out;
    serializeJson(doc, out);
    req->send(200, "application/json", out);
}

// ============================================================
//  REST — L1/L2 из файла
//  Отдаём события за последние `seconds` секунд
// ============================================================
void HistoryLogger::_sendFile(AsyncWebServerRequest* req,
                               const char* path, const char* prevPath,
                               uint32_t seconds)
{
    uint8_t nCh = _channels.size();
    uint32_t cutoff = (_now() > seconds) ? (_now() - seconds) : 0;

    // Формируем ответ
    JsonDocument doc;
    JsonArray chArr = doc["channels"].to<JsonArray>();
    JsonArray tsPerCh[nCh], avgPerCh[nCh], mnPerCh[nCh], mxPerCh[nCh];

    for (uint8_t i = 0; i < nCh; i++) {
        JsonObject chObj = chArr.add<JsonObject>();
        chObj["id"]    = _channels[i]->id;
        chObj["label"] = _channels[i]->label();
        chObj["unit"]  = _channels[i]->unit;
        chObj["type"]  = (uint8_t)_channels[i]->type;
        tsPerCh[i]  = chObj["ts"].to<JsonArray>();
        if (_channels[i]->type == CH_FLOAT) {
            avgPerCh[i] = chObj["avg"].to<JsonArray>();
            mnPerCh[i]  = chObj["min"].to<JsonArray>();
            mxPerCh[i]  = chObj["max"].to<JsonArray>();
        } else {
            avgPerCh[i] = chObj["values"].to<JsonArray>();
        }
    }

    // Читаем файлы: сначала .prev (старые), потом основной
    const char* files[2] = {prevPath, path};
    uint8_t buf[HISTORY_FILE_EVENT_SIZE];

    for (uint8_t fi = 0; fi < 2; fi++) {
        if (!LittleFS.exists(files[fi])) continue;
        File f = LittleFS.open(files[fi], "r");
        if (!f) continue;
        while (f.read(buf, HISTORY_FILE_EVENT_SIZE) == HISTORY_FILE_EVENT_SIZE) {
            uint32_t ts = (uint32_t)buf[0] | ((uint32_t)buf[1]<<8)
                        | ((uint32_t)buf[2]<<16) | ((uint32_t)buf[3]<<24);
            if (ts < cutoff) continue;
            uint8_t  ch  = buf[4];
            int16_t  avg = (int16_t)(buf[5] | (buf[6]<<8));
            int16_t  mn  = (int16_t)(buf[7] | (buf[8]<<8));
            int16_t  mx  = (int16_t)(buf[9] | (buf[10]<<8));
            if (ch >= nCh) continue;
            float sc = _channels[ch]->scale;
            tsPerCh[ch].add(ts);
            if (_channels[ch]->type == CH_FLOAT) {
                avgPerCh[ch].add(avg / sc);
                mnPerCh[ch].add(mn / sc);
                mxPerCh[ch].add(mx / sc);
            } else {
                avgPerCh[ch].add((float)avg);
            }
        }
        f.close();
    }

    String out;
    serializeJson(doc, out);
    req->send(200, "application/json", out);
}

// ============================================================
//  REST МАРШРУТЫ
// ============================================================
void HistoryLogger::_registerRoutes(AsyncWebServer& server) {

    // GET /api/history/channels
    server.on("/api/history/channels", HTTP_GET, [](AsyncWebServerRequest* req) {
        JsonDocument doc;
        JsonArray arr = doc.to<JsonArray>();
        for (auto* ch : DataChannel::all()) {
            JsonObject o = arr.add<JsonObject>();
            o["id"]             = ch->id;
            o["label"]          = ch->label();
            o["unit"]           = ch->unit;
            o["type"]           = (uint8_t)ch->type;
            o["history"]        = ch->history;
            o["mqtt"]           = ch->mqtt;
            o["enabled"]        = ch->_enabled;
            o["mqtt_interval"]  = ch->mqtt_interval;
            o["mqtt_threshold"] = ch->mqtt_threshold;
            if (!isnan(ch->y_min)) o["y_min"] = ch->y_min;
            if (!isnan(ch->y_max)) o["y_max"] = ch->y_max;
        }
        String out; serializeJson(doc, out);
        req->send(200, "application/json", out);
    });

    // GET /api/history?level=0&hours=2
    //                  level=1&hours=24
    //                  level=2&days=14
    server.on("/api/history", HTTP_GET, [](AsyncWebServerRequest* req) {
        uint8_t level = req->hasParam("level") ? req->getParam("level")->value().toInt() : 1;
        uint32_t seconds = 7200;  // default 2 часа

        if (req->hasParam("hours"))
            seconds = req->getParam("hours")->value().toInt() * 3600u;
        else if (req->hasParam("days"))
            seconds = req->getParam("days")->value().toInt() * 86400u;

        if (level == 0)
            _sendL0(req, seconds);
        else if (level == 1)
            _sendFile(req, "/history/l1.bin", "/history/l1.bin.prev", seconds);
        else
            _sendFile(req, "/history/l2.bin", "/history/l2.bin.prev", seconds);
    });

    // POST /api/history/reset
    server.on("/api/history/reset", HTTP_POST, [](AsyncWebServerRequest* req) {
        _invalidateFiles();
        LittleFS.remove("/history/channels.json");
        memset(_l0buf, 0, sizeof(_l0buf));
        _l0head = 0; _l0count = 0;
        for (auto* ch : _channels) ch->_lastLoggedInt = INT16_MIN;
        req->send(200, "application/json", "{\"ok\":true}");
    });

    // POST /api/history/channels — сохранить overrides
    auto* saveHandler = new AsyncCallbackJsonWebHandler("/api/history/channels",
        [](AsyncWebServerRequest* req, JsonVariant& json) {
            JsonArray arr = json.as<JsonArray>();
            for (JsonObject o : arr) {
                const char* id = o["id"] | "";
                for (auto* ch : DataChannel::all()) {
                    if (strcmp(ch->id, id) != 0) continue;
                    if (o.containsKey("enabled"))        ch->_enabled      = o["enabled"];
                    if (o.containsKey("label"))          ch->setLabel(o["label"] | "");
                    if (o.containsKey("history"))        ch->history       = o["history"];
                    if (o.containsKey("mqtt"))           ch->mqtt          = o["mqtt"];
                    if (o.containsKey("mqtt_interval"))  ch->mqtt_interval = o["mqtt_interval"];
                    if (o.containsKey("mqtt_threshold")) ch->mqtt_threshold= o["mqtt_threshold"];
                    if (o.containsKey("y_min"))          ch->y_min         = o["y_min"].as<float>();
                    if (o.containsKey("y_max"))          ch->y_max         = o["y_max"].as<float>();
                    break;
                }
            }
            _saveOverrides();
            req->send(200, "application/json", "{\"ok\":true}");
        });
    server.addHandler(saveHandler);
}

#endif // MODULE_HISTORY