#ifdef MODULE_HISTORY

#include "HistoryLogger.h"
#include "ConfigManager.h"
#include <ArduinoJson.h>
#include <AsyncJson.h>
#include <time.h>

// ============================================================
//  СТАТИЧЕСКИЕ ЧЛЕНЫ
// ============================================================
std::vector<DataChannel*>              HistoryLogger::_channels;
uint16_t                               HistoryLogger::_recordSize   = 0;

int16_t*                               HistoryLogger::_l0vals       = nullptr;
uint32_t                               HistoryLogger::_l0ts[HISTORY_L0_SIZE] = {};
uint16_t                               HistoryLogger::_l0head       = 0;
uint16_t                               HistoryLogger::_l0count      = 0;

ChannelAgg*                            HistoryLogger::_l1agg        = nullptr;
ChannelAgg*                            HistoryLogger::_l2agg        = nullptr;

std::vector<std::vector<uint8_t>>      HistoryLogger::_l1pending;
std::vector<std::vector<uint8_t>>      HistoryLogger::_l2pending;

uint32_t                               HistoryLogger::_lastL0tick   = 0;
uint32_t                               HistoryLogger::_lastL1tick   = 0;
uint32_t                               HistoryLogger::_lastL2tick   = 0;
uint32_t                               HistoryLogger::_lastFlush    = 0;
bool                                   HistoryLogger::_ready        = false;

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

    _recordSize = _calcRecordSize();
    uint8_t nCh = _channels.size();

    _l0vals = new int16_t[HISTORY_L0_SIZE * nCh]();
    _l1agg  = new ChannelAgg[nCh]();
    _l2agg  = new ChannelAgg[nCh]();

    _registerRoutes(server);

    uint32_t now = millis();
    _lastL0tick = now;
    _lastL1tick = now;
    _lastL2tick = now;
    _lastFlush  = now;
    _ready = true;

    Serial.printf("[HIST] Ready: %u channels, record=%u bytes\n", nCh, _recordSize);
}

// ============================================================
//  LOOP
// ============================================================
void HistoryLogger::loop() {
    if (!_ready) return;
    uint32_t now = millis();

    if (now - _lastL0tick >= (uint32_t)HISTORY_L0_INTERVAL_SEC * 1000u) {
        _lastL0tick = now;
        _tickL0();
    }
    if (now - _lastL1tick >= (uint32_t)HISTORY_L1_INTERVAL_SEC * 1000u) {
        _lastL1tick = now;
        _finalizeL1();
    }
    if (now - _lastL2tick >= (uint32_t)HISTORY_L2_INTERVAL_SEC * 1000u) {
        _lastL2tick = now;
        _finalizeL2();
    }
    if (now - _lastFlush >= HISTORY_FLUSH_INTERVAL_MS) {
        _lastFlush = now;
        _flushPending();
    }
}

// ============================================================
//  L0 ТИК
//  Опрашиваем геттеры, конвертируем в int16.
//  Пишем в L0 буфер только если значение изменилось.
//  Всегда пушим в l1agg независимо от изменения.
// ============================================================
void HistoryLogger::_tickL0() {
    uint8_t  nCh  = _channels.size();
    uint32_t ts   = (time(nullptr) > 1000000) ? (uint32_t)time(nullptr) : 0;
    uint32_t dtMs = (uint32_t)HISTORY_L0_INTERVAL_SEC * 1000u;

    bool anyChanged = false;
    int16_t newVals[nCh];

    for (uint8_t i = 0; i < nCh; i++) {
        DataChannel* ch = _channels[i];
        float fval = ch->getter ? ch->getter() : 0.0f;

        // Конвертируем в int16 для CH_FLOAT, для остальных храним как есть
        int16_t ival;
        if (ch->type == CH_FLOAT) {
            ival = (int16_t)(fval * ch->scale);
        } else if (ch->type == CH_COUNTER) {
            // counter: храним float-as-int16 с потерей точности не критично для deadband
            ival = (int16_t)(fval); // целая часть для сравнения
        } else {
            ival = fval > 0.5f ? 1 : 0;
        }
        newVals[i] = ival;
        if (ival != ch->_lastLoggedInt) anyChanged = true;

        // Пушим в l1agg всегда
        _l1agg[i].push(fval, dtMs, ch->type);
    }

    // В L0 буфер пишем только если хоть один канал изменился
    if (anyChanged) {
        int16_t* slot = &_l0vals[_l0head * nCh];
        for (uint8_t i = 0; i < nCh; i++) {
            slot[i] = newVals[i];
            if (newVals[i] != _channels[i]->_lastLoggedInt)
                _channels[i]->_lastLoggedInt = newVals[i];
        }
        _l0ts[_l0head] = ts;
        _l0head  = (_l0head + 1) % HISTORY_L0_SIZE;
        if (_l0count < HISTORY_L0_SIZE) _l0count++;
    }
}

// ============================================================
//  ФИНАЛИЗАЦИЯ L1 — по таймеру раз в минуту
// ============================================================
void HistoryLogger::_finalizeL1() {
    uint8_t  nCh = _channels.size();
    uint32_t ts  = (time(nullptr) > 1000000) ? (uint32_t)time(nullptr) : 0;

    // Если за минуту не было ни одного значения — не пишем
    bool hasData = false;
    for (uint8_t i = 0; i < nCh; i++)
        if (_l1agg[i].count > 0) { hasData = true; break; }

    if (hasData) {
        _l1pending.push_back(_serializeRecord(ts, _l1agg, nCh));

        // Пушим в l2agg
        uint32_t dtMs = (uint32_t)HISTORY_L1_INTERVAL_SEC * 1000u;
        for (uint8_t i = 0; i < nCh; i++) {
            float representative = 0.0f;
            if (_channels[i]->type == CH_FLOAT)
                representative = _l1agg[i].avg();
            else if (_channels[i]->type == CH_COUNTER)
                representative = _l1agg[i].counterLast;
            else
                representative = _l1agg[i].boolPct() > 50 ? 1.0f : 0.0f;
            _l2agg[i].push(representative, dtMs, _channels[i]->type);
        }
    }

    for (uint8_t i = 0; i < nCh; i++) _l1agg[i].reset();
}

// ============================================================
//  ФИНАЛИЗАЦИЯ L2 — по таймеру раз в 10 минут
// ============================================================
void HistoryLogger::_finalizeL2() {
    uint8_t  nCh = _channels.size();
    uint32_t ts  = (time(nullptr) > 1000000) ? (uint32_t)time(nullptr) : 0;

    bool hasData = false;
    for (uint8_t i = 0; i < nCh; i++)
        if (_l2agg[i].count > 0) { hasData = true; break; }

    if (hasData)
        _l2pending.push_back(_serializeRecord(ts, _l2agg, nCh));

    for (uint8_t i = 0; i < nCh; i++) _l2agg[i].reset();
}

// ============================================================
//  СЕРИАЛИЗАЦИЯ
// ============================================================
std::vector<uint8_t> HistoryLogger::_serializeRecord(
    uint32_t ts, ChannelAgg* aggs, uint8_t nCh)
{
    std::vector<uint8_t> buf;
    buf.reserve(_recordSize);

    buf.push_back((ts >> 0)  & 0xFF);
    buf.push_back((ts >> 8)  & 0xFF);
    buf.push_back((ts >> 16) & 0xFF);
    buf.push_back((ts >> 24) & 0xFF);

    for (uint8_t i = 0; i < nCh; i++) {
        if (_channels[i]->type == CH_FLOAT) {
            int16_t avg = (int16_t)(aggs[i].avg()      * _channels[i]->scale);
            int16_t mn  = (int16_t)(aggs[i].floatMin() * _channels[i]->scale);
            int16_t mx  = (int16_t)(aggs[i].floatMax() * _channels[i]->scale);
            buf.push_back((avg >> 0) & 0xFF); buf.push_back((avg >> 8) & 0xFF);
            buf.push_back((mn  >> 0) & 0xFF); buf.push_back((mn  >> 8) & 0xFF);
            buf.push_back((mx  >> 0) & 0xFF); buf.push_back((mx  >> 8) & 0xFF);
        } else if (_channels[i]->type == CH_COUNTER) {
            float v = aggs[i].counterLast;
            uint8_t* p = (uint8_t*)&v;
            buf.push_back(p[0]); buf.push_back(p[1]);
            buf.push_back(p[2]); buf.push_back(p[3]);
        } else {
            buf.push_back(aggs[i].boolPct());
        }
    }
    return buf;
}

// ============================================================
//  СБРОС НА ДИСК
// ============================================================
void HistoryLogger::_flushPending() {
    if (!_l1pending.empty()) {
        _flushLevel("/history/l1.bin", "/history/l1.bin.prev",
                    _l1pending, HISTORY_L1_SIZE);
        _l1pending.clear();
    }
    yield();
    if (!_l2pending.empty()) {
        _flushLevel("/history/l2.bin", "/history/l2.bin.prev",
                    _l2pending, HISTORY_L2_SIZE);
        _l2pending.clear();
    }
}

void HistoryLogger::_flushLevel(const char* path, const char* prevPath,
                                 std::vector<std::vector<uint8_t>>& pending,
                                 uint32_t maxRecords)
{
    File f = LittleFS.open(path, "a");
    if (!f) { Serial.printf("[HIST] Failed to open %s\n", path); return; }
    for (auto& rec : pending) {
        f.write(rec.data(), rec.size());
        yield();
    }
    uint32_t fileRecords = f.size() / _recordSize;
    f.close();
    if (fileRecords >= maxRecords)
        _rotateFile(path, prevPath);
}

void HistoryLogger::_rotateFile(const char* path, const char* prevPath) {
    if (LittleFS.exists(prevPath)) LittleFS.remove(prevPath);
    LittleFS.rename(path, prevPath);
    Serial.printf("[HIST] Rotated %s\n", path);
}

// ============================================================
//  ВЫЧИСЛЕНИЕ РАЗМЕРА ЗАПИСИ
// ============================================================
uint16_t HistoryLogger::_calcRecordSize() {
    uint16_t sz = 4;
    for (auto* ch : _channels) {
        if      (ch->type == CH_FLOAT)   sz += 6;
        else if (ch->type == CH_COUNTER) sz += 4;
        else if (ch->type == CH_BOOL)    sz += 1;
    }
    return sz;
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
        const char* savedLabel = o["label"] | "";
        if (savedLabel && strlen(savedLabel) > 0) ch->setLabel(savedLabel);
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

// ============================================================
//  ПУБЛИЧНЫЙ ДОСТУП
// ============================================================
uint8_t HistoryLogger::channelCount() { return _channels.size(); }
DataChannel* HistoryLogger::channel(uint8_t idx) {
    return (idx < _channels.size()) ? _channels[idx] : nullptr;
}

// ============================================================
//  REST — L0
// ============================================================
void HistoryLogger::_sendL0(AsyncWebServerRequest* req, uint16_t last) {
    uint8_t nCh = _channels.size();
    if (nCh == 0 || _l0count == 0) {
        req->send(200, "application/json", "{\"ts\":[],\"channels\":[]}");
        return;
    }

    const uint16_t MAX_POINTS = 500;
    uint16_t count  = min((uint16_t)_l0count, last);
    uint16_t stride = max((uint16_t)1, (uint16_t)(count / MAX_POINTS));
    uint16_t outCount = (count + stride - 1) / stride;

    JsonDocument doc;
    JsonArray tsArr = doc["ts"].to<JsonArray>();
    JsonArray chArr = doc["channels"].to<JsonArray>();

    JsonArray avgArr[nCh], minArr[nCh], maxArr[nCh];
    for (uint8_t i = 0; i < nCh; i++) {
        JsonObject chObj = chArr.add<JsonObject>();
        chObj["id"]    = _channels[i]->id;
        chObj["label"] = _channels[i]->label();
        chObj["unit"]  = _channels[i]->unit;
        chObj["type"]  = (uint8_t)_channels[i]->type;
        avgArr[i] = chObj["values"].to<JsonArray>();
        if (_channels[i]->type == CH_FLOAT) {
            minArr[i] = chObj["min"].to<JsonArray>();
            maxArr[i] = chObj["max"].to<JsonArray>();
        }
    }

    uint16_t startIdx = (_l0count < HISTORY_L0_SIZE) ? 0 : _l0head;
    uint16_t skip = (_l0count > count) ? (_l0count - count) : 0;
    for (uint16_t n = 0; n < skip; n++)
        startIdx = (startIdx + 1) % HISTORY_L0_SIZE;

    for (uint16_t b = 0; b < outCount; b++) {
        uint16_t bStart = b * stride;
        uint16_t bEnd   = min((uint16_t)(bStart + stride), count);
        uint32_t bTs = 0;
        float bSum[nCh]; float bMin[nCh]; float bMax[nCh];
        for (uint8_t i = 0; i < nCh; i++) {
            bSum[i] = 0; bMin[i] = FLT_MAX; bMax[i] = -FLT_MAX;
        }
        for (uint16_t r = bStart; r < bEnd; r++) {
            uint16_t idx = startIdx % HISTORY_L0_SIZE;
            if (r == bStart) bTs = _l0ts[idx];
            int16_t* slot = &_l0vals[idx * nCh];
            for (uint8_t i = 0; i < nCh; i++) {
                float fval = (float)slot[i] / _channels[i]->scale;
                bSum[i] += fval;
                if (fval < bMin[i]) bMin[i] = fval;
                if (fval > bMax[i]) bMax[i] = fval;
            }
            startIdx = (startIdx + 1) % HISTORY_L0_SIZE;
        }
        uint16_t bCount = bEnd - bStart;
        tsArr.add(bTs);
        for (uint8_t i = 0; i < nCh; i++) {
            avgArr[i].add(bSum[i] / bCount);
            if (_channels[i]->type == CH_FLOAT) {
                minArr[i].add(bMin[i] == FLT_MAX  ? 0.0f : bMin[i]);
                maxArr[i].add(bMax[i] == -FLT_MAX ? 0.0f : bMax[i]);
            }
        }
    }

    String out;
    serializeJson(doc, out);
    req->send(200, "application/json", out);
}

// ============================================================
//  REST — L1/L2 из файла
// ============================================================
void HistoryLogger::_sendFile(AsyncWebServerRequest* req,
                               const char* path, const char* prevPath,
                               uint16_t last)
{
    uint8_t nCh = _channels.size();

    struct FileInfo { const char* p; uint32_t records; };
    FileInfo files[2]; uint8_t nFiles = 0;
    auto addFile = [&](const char* p) {
        if (LittleFS.exists(p)) {
            File f = LittleFS.open(p, "r");
            if (f) { uint32_t r = f.size() / _recordSize; f.close(); if (r > 0) files[nFiles++] = {p, r}; }
        }
    };
    addFile(prevPath); addFile(path);

    uint32_t total = 0;
    for (uint8_t i = 0; i < nFiles; i++) total += files[i].records;
    if (total == 0) {
        req->send(200, "application/json", "{\"ts\":[],\"channels\":[]}");
        return;
    }

    uint32_t count = min((uint32_t)last, total);
    const uint16_t MAX_POINTS = 100;
    uint32_t stride   = max((uint32_t)1, count / MAX_POINTS);
    uint32_t outCount = (count + stride - 1) / stride;
    uint32_t skip     = total - count;

    JsonDocument doc;
    JsonArray tsArr = doc["ts"].to<JsonArray>();
    JsonArray chArr = doc["channels"].to<JsonArray>();

    JsonArray avgArr[nCh], minArr[nCh], maxArr[nCh];
    for (uint8_t i = 0; i < nCh; i++) {
        JsonObject chObj = chArr.add<JsonObject>();
        chObj["id"]    = _channels[i]->id;
        chObj["label"] = _channels[i]->label();
        chObj["unit"]  = _channels[i]->unit;
        chObj["type"]  = (uint8_t)_channels[i]->type;
        if (_channels[i]->type == CH_FLOAT) {
            avgArr[i] = chObj["avg"].to<JsonArray>();
            minArr[i] = chObj["min"].to<JsonArray>();
            maxArr[i] = chObj["max"].to<JsonArray>();
        } else {
            avgArr[i] = chObj["values"].to<JsonArray>();
        }
    }

    std::vector<uint8_t> recBuf(_recordSize);
    uint32_t globalIdx = 0, readIdx = 0;
    float bSum[nCh]; float bMin[nCh]; float bMax[nCh];
    float bCounter[nCh]; uint32_t bBool[nCh];
    uint32_t bTs = 0, bCount = 0, curBucket = 0;

    auto resetBucket = [&]() {
        for (uint8_t i = 0; i < nCh; i++) {
            bSum[i]=0; bMin[i]=FLT_MAX; bMax[i]=-FLT_MAX;
            bCounter[i]=0; bBool[i]=0;
        }
        bTs=0; bCount=0;
    };
    auto flushBucket = [&]() {
        if (bCount == 0) return;
        tsArr.add(bTs);
        for (uint8_t i = 0; i < nCh; i++) {
            if (_channels[i]->type == CH_FLOAT) {
                avgArr[i].add(bSum[i] / bCount);
                minArr[i].add(bMin[i] == FLT_MAX  ? 0.0f : bMin[i]);
                maxArr[i].add(bMax[i] == -FLT_MAX ? 0.0f : bMax[i]);
            } else if (_channels[i]->type == CH_COUNTER) {
                avgArr[i].add(bCounter[i]);
            } else {
                avgArr[i].add((uint8_t)(bBool[i] * 100 / bCount));
            }
        }
    };
    resetBucket();

    for (uint8_t fi = 0; fi < nFiles; fi++) {
        File f = LittleFS.open(files[fi].p, "r");
        if (!f) continue;
        for (uint32_t r = 0; r < files[fi].records; r++, globalIdx++) {
            f.read(recBuf.data(), _recordSize);
            if (globalIdx < skip) continue;
            uint32_t bucket = readIdx / stride;
            if (bucket != curBucket) { flushBucket(); resetBucket(); curBucket = bucket; }
            uint32_t ts = (uint32_t)recBuf[0] | ((uint32_t)recBuf[1]<<8)
                        | ((uint32_t)recBuf[2]<<16) | ((uint32_t)recBuf[3]<<24);
            if (bCount == 0) bTs = ts;
            uint16_t offset = 4;
            for (uint8_t i = 0; i < nCh; i++) {
                if (_channels[i]->type == CH_FLOAT) {
                    int16_t avg = (int16_t)(recBuf[offset]   | (recBuf[offset+1]<<8));
                    int16_t mn  = (int16_t)(recBuf[offset+2] | (recBuf[offset+3]<<8));
                    int16_t mx  = (int16_t)(recBuf[offset+4] | (recBuf[offset+5]<<8));
                    float sc = _channels[i]->scale;
                    float fAvg = avg/sc, fMn = mn/sc, fMx = mx/sc;
                    bSum[i] += fAvg;
                    if (fMn < bMin[i]) bMin[i] = fMn;
                    if (fMx > bMax[i]) bMax[i] = fMx;
                    offset += 6;
                } else if (_channels[i]->type == CH_COUNTER) {
                    float v; memcpy(&v, &recBuf[offset], 4);
                    bCounter[i] = v; offset += 4;
                } else {
                    bBool[i] += recBuf[offset]; offset += 1;
                }
            }
            bCount++; readIdx++;
        }
        f.close();
    }
    flushBucket();

    String out;
    serializeJson(doc, out);
    req->send(200, "application/json", out);
}

// ============================================================
//  REST МАРШРУТЫ
// ============================================================
void HistoryLogger::_registerRoutes(AsyncWebServer& server) {

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

    server.on("/api/history", HTTP_GET, [](AsyncWebServerRequest* req) {
        uint8_t  level = req->hasParam("level") ? req->getParam("level")->value().toInt() : 1;
        uint16_t last  = req->hasParam("last")  ? req->getParam("last")->value().toInt()  : 100;
        if (level == 0) _sendL0(req, last);
        else if (level == 1) _sendFile(req, "/history/l1.bin", "/history/l1.bin.prev", last);
        else _sendFile(req, "/history/l2.bin", "/history/l2.bin.prev", last);
    });

    server.on("/api/history/reset", HTTP_POST, [](AsyncWebServerRequest* req) {
        _invalidateFiles();
        LittleFS.remove("/history/channels.json");
        // Сбросить L0 буфер
        uint8_t nCh = _channels.size();
        if (_l0vals) memset(_l0vals, 0, HISTORY_L0_SIZE * nCh * sizeof(int16_t));
        _l0head = 0; _l0count = 0;
        // Сбросить lastLoggedInt
        for (auto* ch : _channels) ch->_lastLoggedInt = INT16_MIN;
        req->send(200, "application/json", "{\"ok\":true}");
    });

    auto* saveHandler = new AsyncCallbackJsonWebHandler("/api/history/channels",
        [](AsyncWebServerRequest* req, JsonVariant& json) {
            JsonArray arr = json.as<JsonArray>();
            for (JsonObject o : arr) {
                const char* id = o["id"] | "";
                for (auto* ch : DataChannel::all()) {
                    if (strcmp(ch->id, id) != 0) continue;
                    if (o.containsKey("enabled"))       ch->_enabled      = o["enabled"];
                    if (o.containsKey("label"))         ch->setLabel(o["label"] | "");
                    if (o.containsKey("history"))       ch->history       = o["history"];
                    if (o.containsKey("mqtt"))          ch->mqtt          = o["mqtt"];
                    if (o.containsKey("mqtt_interval")) ch->mqtt_interval = o["mqtt_interval"];
                    if (o.containsKey("mqtt_threshold"))ch->mqtt_threshold= o["mqtt_threshold"];
                    if (o.containsKey("y_min"))         ch->y_min         = o["y_min"].as<float>();
                    if (o.containsKey("y_max"))         ch->y_max         = o["y_max"].as<float>();
                    break;
                }
            }
            _saveOverrides();
            req->send(200, "application/json", "{\"ok\":true}");
        });
    server.addHandler(saveHandler);
}

#endif // MODULE_HISTORY