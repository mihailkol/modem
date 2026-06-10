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

float*                                 HistoryLogger::_l0buf        = nullptr;
uint32_t                               HistoryLogger::_l0ts[HISTORY_L0_SIZE] = {};
uint16_t                               HistoryLogger::_l0head       = 0;
uint16_t                               HistoryLogger::_l0count      = 0;

ChannelAgg*                            HistoryLogger::_l1agg        = nullptr;
uint8_t                                HistoryLogger::_l1accum      = 0;
ChannelAgg*                            HistoryLogger::_l2agg        = nullptr;
uint8_t                                HistoryLogger::_l2accum      = 0;

std::vector<std::vector<uint8_t>>      HistoryLogger::_l1pending;
std::vector<std::vector<uint8_t>>      HistoryLogger::_l2pending;

uint32_t                               HistoryLogger::_lastL0tick   = 0;
uint32_t                               HistoryLogger::_lastFlush    = 0;
bool                                   HistoryLogger::_ready        = false;

// ============================================================
//  BEGIN
// ============================================================
void HistoryLogger::begin(AsyncWebServer& server) {
    if (DataChannel::all().empty()) {
        Serial.println("[HIST] No channels registered, skipping");
        return;
    }

    // Создаём директорию если нет
    if (!LittleFS.exists("/history"))
        LittleFS.mkdir("/history");

    // Загружаем overrides из JSON, применяем к DataChannel::all()
    _loadOverrides();

    // Фильтруем активные каналы с history=true
    _buildActiveList();

    if (_channels.empty()) {
        Serial.println("[HIST] No history channels enabled, skipping");
        return;
    }

    // Проверяем fingerprint — если каналы изменились, инвалидируем файлы
    if (!_checkFingerprint()) {
        Serial.println("[HIST] Channel fingerprint changed, invalidating history files");
        _invalidateFiles();
        _saveOverrides();  // сохраняем новый fingerprint
    }

    _recordSize = _calcRecordSize();

    uint8_t nCh = _channels.size();

    // Выделяем L0 буфер
    _l0buf = new float[HISTORY_L0_SIZE * nCh]();

    // Выделяем аккумуляторы
    _l1agg = new ChannelAgg[nCh]();
    _l2agg = new ChannelAgg[nCh]();

    _registerRoutes(server);

    _lastL0tick = millis();
    _lastFlush  = millis();
    _ready      = true;

    Serial.printf("[HIST] Ready: %u channels, record=%u bytes\n", nCh, _recordSize);
}

// ============================================================
//  LOOP
// ============================================================
void HistoryLogger::loop() {
    if (!_ready) return;

    uint32_t now = millis();

    // L0 тик
    if (now - _lastL0tick >= (uint32_t)HISTORY_L0_INTERVAL_SEC * 1000u) {
        _lastL0tick = now;
        _tickL0();
    }

    // Сброс pending на диск по таймеру
    if (now - _lastFlush >= HISTORY_FLUSH_INTERVAL_MS) {
        _lastFlush = now;
        _flushPending();
    }
}

// ============================================================
//  L0 ТИК — снимаем геттеры, пишем в кольцевой буфер, пушим в l1agg
// ============================================================
void HistoryLogger::_tickL0() {
    uint8_t  nCh  = _channels.size();
    uint32_t now  = millis();
    uint32_t ts   = (time(nullptr) > 1000000) ? (uint32_t)time(nullptr) : 0;
    uint32_t dtMs = (uint32_t)HISTORY_L0_INTERVAL_SEC * 1000u;

    // Записываем в L0 кольцевой буфер
    float* slot = &_l0buf[_l0head * nCh];
    for (uint8_t i = 0; i < nCh; i++) {
        float val = _channels[i]->getter ? _channels[i]->getter() : 0.0f;
        slot[i] = val;
        _l1agg[i].push(val, dtMs, _channels[i]->type);
    }
    _l0ts[_l0head] = ts;
    _l0head  = (_l0head + 1) % HISTORY_L0_SIZE;
    if (_l0count < HISTORY_L0_SIZE) _l0count++;

    // Проверяем готовность L1 агрегата
    _l1accum++;
    if (_l1accum >= HISTORY_L1_RATIO)
        _finalizeL1();
}

// ============================================================
//  ФИНАЛИЗАЦИЯ L1 — из 6 L0 делаем одну L1 запись
// ============================================================
void HistoryLogger::_finalizeL1() {
    uint8_t  nCh = _channels.size();
    uint32_t ts  = (time(nullptr) > 1000000) ? (uint32_t)time(nullptr) : 0;

    // Извлекаем типы и scale для сериализации
    ChannelType types[nCh];
    int16_t     scales[nCh];
    for (uint8_t i = 0; i < nCh; i++) {
        types[i]  = _channels[i]->type;
        scales[i] = _channels[i]->scale;
    }

    _l1pending.push_back(_serializeRecord(ts, _l1agg, types, scales, nCh));

    // Пушим L1 агрегат в L2 аккумулятор
    uint32_t l1PeriodMs = (uint32_t)HISTORY_L0_INTERVAL_SEC * HISTORY_L1_RATIO * 1000u;
    for (uint8_t i = 0; i < nCh; i++) {
        // Для CH_FLOAT передаём avg как представительное значение L1 периода
        float representative = (_channels[i]->type == CH_FLOAT)
            ? _l1agg[i].avg()
            : (_channels[i]->type == CH_COUNTER)
                ? _l1agg[i].counterLast
                : (_l1agg[i].boolPct() > 50 ? 1.0f : 0.0f);

        _l2agg[i].push(representative, l1PeriodMs, _channels[i]->type);
        _l1agg[i].reset();
    }
    _l1accum = 0;

    // Проверяем готовность L2 агрегата
    _l2accum++;
    if (_l2accum >= HISTORY_L2_RATIO)
        _finalizeL2();
}

// ============================================================
//  ФИНАЛИЗАЦИЯ L2 — из 10 L1 делаем одну L2 запись
// ============================================================
void HistoryLogger::_finalizeL2() {
    uint8_t  nCh = _channels.size();
    uint32_t ts  = (time(nullptr) > 1000000) ? (uint32_t)time(nullptr) : 0;

    ChannelType types[nCh];
    int16_t     scales[nCh];
    for (uint8_t i = 0; i < nCh; i++) {
        types[i]  = _channels[i]->type;
        scales[i] = _channels[i]->scale;
    }

    _l2pending.push_back(_serializeRecord(ts, _l2agg, types, scales, nCh));

    for (uint8_t i = 0; i < nCh; i++)
        _l2agg[i].reset();
    _l2accum = 0;
}

// ============================================================
//  СЕРИАЛИЗАЦИЯ ЗАПИСИ В БАЙТЫ
// ============================================================
std::vector<uint8_t> HistoryLogger::_serializeRecord(
    uint32_t ts, ChannelAgg* aggs,
    ChannelType* types, int16_t* scales, uint8_t nCh)
{
    std::vector<uint8_t> buf;
    buf.reserve(_recordSize);

    // ts
    buf.push_back((ts >> 0)  & 0xFF);
    buf.push_back((ts >> 8)  & 0xFF);
    buf.push_back((ts >> 16) & 0xFF);
    buf.push_back((ts >> 24) & 0xFF);

    for (uint8_t i = 0; i < nCh; i++) {
        if (types[i] == CH_FLOAT) {
            int16_t avg = (int16_t)(aggs[i].avg()      * scales[i]);
            int16_t mn  = (int16_t)(aggs[i].floatMin() * scales[i]);
            int16_t mx  = (int16_t)(aggs[i].floatMax() * scales[i]);
            buf.push_back((avg >> 0) & 0xFF); buf.push_back((avg >> 8) & 0xFF);
            buf.push_back((mn  >> 0) & 0xFF); buf.push_back((mn  >> 8) & 0xFF);
            buf.push_back((mx  >> 0) & 0xFF); buf.push_back((mx  >> 8) & 0xFF);
        } else if (types[i] == CH_COUNTER) {
            float v = aggs[i].counterLast;
            uint8_t* p = (uint8_t*)&v;
            buf.push_back(p[0]); buf.push_back(p[1]);
            buf.push_back(p[2]); buf.push_back(p[3]);
        } else if (types[i] == CH_BOOL) {
            buf.push_back(aggs[i].boolPct());
        }
    }

    return buf;
}

// ============================================================
//  СБРОС PENDING НА ДИСК
// ============================================================
void HistoryLogger::_flushPending() {
    if (!_l1pending.empty()) {
        _flushLevel("/history/l1.bin", "/history/l1.bin.prev",
                    _l1pending, HISTORY_L1_SIZE);
        _l1pending.clear();
    }
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
    if (!f) {
        Serial.printf("[HIST] Failed to open %s\n", path);
        return;
    }
    for (auto& rec : pending)
        f.write(rec.data(), rec.size());

    uint32_t fileRecords = f.size() / _recordSize;
    f.close();

    // Ротация: если файл превысил maxRecords — перекладываем в .prev
    if (fileRecords >= maxRecords)
        _rotateFile(path, prevPath);
}

void HistoryLogger::_rotateFile(const char* path, const char* prevPath) {
    if (LittleFS.exists(prevPath))
        LittleFS.remove(prevPath);
    LittleFS.rename(path, prevPath);
    Serial.printf("[HIST] Rotated %s → %s\n", path, prevPath);
}

// ============================================================
//  ВЫЧИСЛЕНИЕ РАЗМЕРА ЗАПИСИ
// ============================================================
uint16_t HistoryLogger::_calcRecordSize() {
    uint16_t sz = 4; // ts
    for (auto* ch : _channels) {
        if      (ch->type == CH_FLOAT)   sz += 6; // avg + min + max int16
        else if (ch->type == CH_COUNTER) sz += 4; // float
        else if (ch->type == CH_BOOL)    sz += 1; // uint8 pct
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
//  OVERRIDES — загрузка/сохранение channels.json
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
        if (savedLabel && strlen(savedLabel) > 0) ch->label = savedLabel;
        // offset для CH_COUNTER читается в самом device через Preferences,
        // здесь не хранится
    }
}

void HistoryLogger::_saveOverrides() {
    JsonDocument doc;
    doc["fingerprint"] = _buildFingerprint();

    JsonObject chans = doc["channels"].to<JsonObject>();
    for (auto* ch : DataChannel::all()) {
        JsonObject o = chans[ch->id].to<JsonObject>();
        o["enabled"] = ch->_enabled;
        o["label"]   = ch->label;
        o["unit"]    = ch->unit;
        o["type"]    = (uint8_t)ch->type;
        o["history"] = ch->history;
        o["mqtt"]    = ch->mqtt;
    }
    ConfigManager::saveJson("/history/channels.json", doc);
}

// ============================================================
//  ПОСТРОЕНИЕ СПИСКА АКТИВНЫХ КАНАЛОВ
// ============================================================
void HistoryLogger::_buildActiveList() {
    _channels.clear();
    for (auto* ch : DataChannel::all()) {
        if (ch->history && ch->_enabled)
            _channels.push_back(ch);
    }
}

// ============================================================
//  ПУБЛИЧНЫЙ ДОСТУП К КАНАЛАМ
// ============================================================
uint8_t HistoryLogger::channelCount() { return _channels.size(); }
DataChannel* HistoryLogger::channel(uint8_t idx) {
    return (idx < _channels.size()) ? _channels[idx] : nullptr;
}

// ============================================================
//  REST ОТДАЧА
// ============================================================

// L0 из RAM — отдаём последние `last` записей
void HistoryLogger::_sendL0(AsyncWebServerRequest* req, uint16_t last) {
    uint8_t nCh = _channels.size();
    if (nCh == 0 || _l0count == 0) {
        req->send(200, "application/json", "{\"ts\":[],\"channels\":[]}");
        return;
    }

    uint16_t count = min((uint16_t)_l0count, last);

    // Собираем в JSON (L0 небольшой — до 720 × nCh float)
    JsonDocument doc;
    JsonArray tsArr = doc["ts"].to<JsonArray>();
    JsonArray chArr = doc["channels"].to<JsonArray>();

    // Подготавливаем массивы значений
    JsonArray vals[nCh];
    for (uint8_t i = 0; i < nCh; i++) {
        JsonObject chObj = chArr.add<JsonObject>();
        chObj["id"]    = _channels[i]->id;
        chObj["label"] = _channels[i]->label;
        chObj["unit"]  = _channels[i]->unit;
        chObj["type"]  = (uint8_t)_channels[i]->type;
        vals[i] = chObj["values"].to<JsonArray>();
    }

    // Читаем в хронологическом порядке
    // head указывает на следующую запись (самую старую в буфере)
    uint16_t startIdx = (_l0count < HISTORY_L0_SIZE)
        ? 0
        : _l0head;  // буфер полон — oldest = head

    uint16_t skip = (_l0count > count) ? (_l0count - count) : 0;

    for (uint16_t n = 0; n < _l0count; n++) {
        if (n < skip) { startIdx = (startIdx + 1) % HISTORY_L0_SIZE; continue; }
        uint16_t idx = startIdx % HISTORY_L0_SIZE;
        tsArr.add(_l0ts[idx]);
        float* slot = &_l0buf[idx * nCh];
        for (uint8_t i = 0; i < nCh; i++)
            vals[i].add(slot[i]);
        startIdx = (startIdx + 1) % HISTORY_L0_SIZE;
    }

    String out;
    serializeJson(doc, out);
    req->send(200, "application/json", out);
}

// L1/L2 из файла — отдаём последние `last` записей
// Читаем с конца файла, не грузим весь файл в память
void HistoryLogger::_sendFile(AsyncWebServerRequest* req,
                               const char* path, const char* prevPath,
                               uint16_t last)
{
    uint8_t nCh = _channels.size();

    // Собираем список файлов: сначала .prev (старые), потом основной
    struct FileInfo { const char* p; uint32_t records; };
    FileInfo files[2];
    uint8_t nFiles = 0;

    auto addFile = [&](const char* p) {
        if (LittleFS.exists(p)) {
            File f = LittleFS.open(p, "r");
            if (f) {
                uint32_t recs = f.size() / _recordSize;
                f.close();
                if (recs > 0) files[nFiles++] = {p, recs};
            }
        }
    };
    addFile(prevPath);
    addFile(path);

    // Считаем сколько всего записей
    uint32_t total = 0;
    for (uint8_t i = 0; i < nFiles; i++) total += files[i].records;
    if (total == 0) {
        req->send(200, "application/json", "{\"ts\":[],\"channels\":[]}");
        return;
    }

    uint32_t count = min((uint32_t)last, total);
    uint32_t skip  = total - count;

    // Формируем JSON
    JsonDocument doc;
    JsonArray tsArr = doc["ts"].to<JsonArray>();
    JsonArray chArr = doc["channels"].to<JsonArray>();

    JsonArray vals[nCh];
    for (uint8_t i = 0; i < nCh; i++) {
        JsonObject chObj = chArr.add<JsonObject>();
        chObj["id"]    = _channels[i]->id;
        chObj["label"] = _channels[i]->label;
        chObj["unit"]  = _channels[i]->unit;
        chObj["type"]  = (uint8_t)_channels[i]->type;
        // Для CH_FLOAT передаём avg; min/max в отдельных массивах
        if (_channels[i]->type == CH_FLOAT) {
            vals[i]       = chObj["avg"].to<JsonArray>();
            chObj["min"].to<JsonArray>();
            chObj["max"].to<JsonArray>();
        } else {
            vals[i] = chObj["values"].to<JsonArray>();
        }
    }

    std::vector<uint8_t> recBuf(_recordSize);
    uint32_t globalIdx = 0;

    for (uint8_t fi = 0; fi < nFiles; fi++) {
        File f = LittleFS.open(files[fi].p, "r");
        if (!f) continue;

        for (uint32_t r = 0; r < files[fi].records; r++, globalIdx++) {
            f.read(recBuf.data(), _recordSize);
            if (globalIdx < skip) continue;

            // Парсим запись
            uint32_t ts = (uint32_t)recBuf[0]
                        | ((uint32_t)recBuf[1] << 8)
                        | ((uint32_t)recBuf[2] << 16)
                        | ((uint32_t)recBuf[3] << 24);
            tsArr.add(ts);

            uint16_t offset = 4;
            for (uint8_t i = 0; i < nCh; i++) {
                if (_channels[i]->type == CH_FLOAT) {
                    int16_t avg = (int16_t)(recBuf[offset] | (recBuf[offset+1] << 8));
                    int16_t mn  = (int16_t)(recBuf[offset+2] | (recBuf[offset+3] << 8));
                    int16_t mx  = (int16_t)(recBuf[offset+4] | (recBuf[offset+5] << 8));
                    float sc = _channels[i]->scale;
                    vals[i].add(avg / sc);
                    doc["channels"][i]["min"].as<JsonArray>().add(mn / sc);
                    doc["channels"][i]["max"].as<JsonArray>().add(mx / sc);
                    offset += 6;
                } else if (_channels[i]->type == CH_COUNTER) {
                    float v;
                    memcpy(&v, &recBuf[offset], 4);
                    vals[i].add(v);
                    offset += 4;
                } else if (_channels[i]->type == CH_BOOL) {
                    vals[i].add(recBuf[offset]);
                    offset += 1;
                }
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

    // GET /api/history/channels — список каналов с метаданными
    // Регистрируется ДО /api/history — иначе широкий маршрут перехватит
    server.on("/api/history/channels", HTTP_GET, [](AsyncWebServerRequest* req) {
        JsonDocument doc;
        JsonArray arr = doc.to<JsonArray>();
        for (auto* ch : DataChannel::all()) {
            JsonObject o = arr.add<JsonObject>();
            o["id"]             = ch->id;
            o["label"]          = ch->label;
            o["unit"]           = ch->unit;
            o["type"]           = (uint8_t)ch->type;
            o["history"]        = ch->history;
            o["mqtt"]           = ch->mqtt;
            o["enabled"]        = ch->_enabled;
            o["mqtt_interval"]  = ch->mqtt_interval;
            o["mqtt_threshold"] = ch->mqtt_threshold;
        }
        String out;
        serializeJson(doc, out);
        req->send(200, "application/json", out);
    });

    // GET /api/history?level=0&last=720
    server.on("/api/history", HTTP_GET, [](AsyncWebServerRequest* req) {
        uint8_t  level = req->hasParam("level") ? req->getParam("level")->value().toInt() : 1;
        uint16_t last  = req->hasParam("last")  ? req->getParam("last")->value().toInt()  : 100;

        if (level == 0) {
            _sendL0(req, last);
        } else if (level == 1) {
            _sendFile(req, "/history/l1.bin", "/history/l1.bin.prev", last);
        } else {
            _sendFile(req, "/history/l2.bin", "/history/l2.bin.prev", last);
        }
    });



    // POST /api/history/reset — удалить файлы истории и channels.json
    server.on("/api/history/reset", HTTP_POST, [](AsyncWebServerRequest* req) {
        _invalidateFiles();
        LittleFS.remove("/history/channels.json");
        req->send(200, "application/json", "{\"ok\":true}");
    });
    auto* saveHandler = new AsyncCallbackJsonWebHandler("/api/history/channels",
        [](AsyncWebServerRequest* req, JsonVariant& json) {
            JsonArray arr = json.as<JsonArray>();
            for (JsonObject o : arr) {
                const char* id = o["id"] | "";
                for (auto* ch : DataChannel::all()) {
                    if (strcmp(ch->id, id) != 0) continue;
                    if (o.containsKey("enabled"))         ch->_enabled        = o["enabled"];
                    if (o.containsKey("label"))            ch->label           = o["label"];
                    if (o.containsKey("history"))          ch->history         = o["history"];
                    if (o.containsKey("mqtt"))             ch->mqtt            = o["mqtt"];
                    if (o.containsKey("mqtt_interval"))    ch->mqtt_interval   = o["mqtt_interval"];
                    if (o.containsKey("mqtt_threshold"))   ch->mqtt_threshold  = o["mqtt_threshold"];
                    break;
                }
            }
            _saveOverrides();
            // Перестроить список активных каналов
            // (изменение enabled вступает в силу после перезагрузки —
            //  буферы и recordSize фиксированы при begin())
            req->send(200, "application/json", "{\"ok\":true,\"note\":\"restart_required\"}");
        });
    server.addHandler(saveHandler);
}

#endif // MODULE_HISTORY