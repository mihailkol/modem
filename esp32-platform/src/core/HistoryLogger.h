#pragma once
#ifdef MODULE_HISTORY

#include <Arduino.h>
#include <cfloat>
#include <LittleFS.h>
#include <ESPAsyncWebServer.h>
#include "DataChannel.h"

// ============================================================
//  ПАРАМЕТРЫ УРОВНЕЙ
// ============================================================
#define HISTORY_L0_INTERVAL_SEC   10      // интервал опроса геттеров
#define HISTORY_L0_SIZE          720      // L0: макс 720 записей (RAM only)

#define HISTORY_L1_INTERVAL_SEC   60      // агрегация L0→L1 каждую минуту
#define HISTORY_L1_SIZE         1440      // L1: макс 1440 записей = 24 часа

#define HISTORY_L2_INTERVAL_SEC  600      // агрегация L1→L2 каждые 10 минут
#define HISTORY_L2_SIZE         2016      // L2: макс 2016 записей = 14 суток

#define HISTORY_FLUSH_INTERVAL_MS 180000u // сброс pending на диск каждые 3 минуты

// ============================================================
//  ФОРМАТ БИНАРНОЙ ЗАПИСИ (L1 / L2)
//
//  Layout:
//    [uint32_t ts]               — 4 байта
//    для каждого CH_FLOAT канала:
//      [int16 avg][int16 min][int16 max]  — 6 байт (value * scale)
//    для каждого CH_COUNTER канала:
//      [float value]                      — 4 байта
//    для каждого CH_BOOL канала:
//      [uint8 pct_active]                 — 1 байт (0-100%)
// ============================================================

// ── L0 запись ────────────────────────────────────────────────
// Хранится в RAM как кольцевой буфер
// Значения уже в int16 (value * scale) для CH_FLOAT,
// float-as-bytes для CH_COUNTER, uint8 для CH_BOOL
struct L0Entry {
    uint32_t ts;
    // данные следуют за структурой в плоском буфере
};

// ── Агрегат одного канала за период ──────────────────────────
struct ChannelAgg {
    float    sum  = 0.0f;
    float    mn   = FLT_MAX;
    float    mx   = -FLT_MAX;
    float    counterLast = 0.0f;
    uint32_t boolActiveMs = 0;
    uint32_t count = 0;
    uint32_t periodMs = 0;

    void reset() {
        sum = 0.0f; mn = FLT_MAX; mx = -FLT_MAX;
        counterLast = 0.0f; boolActiveMs = 0;
        count = 0; periodMs = 0;
    }

    void push(float val, uint32_t dtMs, ChannelType type) {
        count++; periodMs += dtMs;
        if (type == CH_FLOAT) {
            sum += val;
            if (val < mn) mn = val;
            if (val > mx) mx = val;
        } else if (type == CH_COUNTER) {
            counterLast = val;
        } else if (type == CH_BOOL) {
            if (val > 0.5f) boolActiveMs += dtMs;
        }
    }

    float avg()      const { return (count > 0) ? sum / count : 0.0f; }
    float floatMin() const { return (mn == FLT_MAX)  ? 0.0f : mn; }
    float floatMax() const { return (mx == -FLT_MAX) ? 0.0f : mx; }
    uint8_t boolPct() const {
        return (periodMs > 0) ? (uint8_t)(boolActiveMs * 100u / periodMs) : 0;
    }
};

// ============================================================
//  HistoryLogger
// ============================================================
class HistoryLogger {
public:
    static void begin(AsyncWebServer& server);
    static void loop();

    static uint8_t      channelCount();
    static DataChannel* channel(uint8_t idx);

private:
    static void    _loadOverrides();
    static void    _saveOverrides();
    static void    _buildActiveList();
    static bool    _checkFingerprint();
    static void    _invalidateFiles();
    static uint16_t _calcRecordSize();
    static void    _registerRoutes(AsyncWebServer& server);

    // Тики
    static void _tickL0();      // каждые L0_INTERVAL_SEC
    static void _finalizeL1();  // каждые L1_INTERVAL_SEC — агрегат L0→L1
    static void _finalizeL2();  // каждые L2_INTERVAL_SEC — агрегат L1→L2

    // Сериализация
    static std::vector<uint8_t> _serializeRecord(uint32_t ts,
                                                  ChannelAgg* aggs,
                                                  uint8_t nCh);

    // Сброс на диск
    static void _flushPending();
    static void _flushLevel(const char* path, const char* prevPath,
                             std::vector<std::vector<uint8_t>>& pending,
                             uint32_t maxRecords);
    static void _rotateFile(const char* path, const char* prevPath);

    // REST
    static void _sendL0(AsyncWebServerRequest* req, uint16_t last);
    static void _sendFile(AsyncWebServerRequest* req,
                          const char* path, const char* prevPath,
                          uint16_t last);

    // Fingerprint
    static String _buildFingerprint();

    // ── Данные ───────────────────────────────────────────────
    static std::vector<DataChannel*> _channels;
    static uint16_t  _recordSize;

    // L0: кольцевой буфер int16 значений + timestamps
    // Плоский массив: _l0vals[head * nCh + i] = int16 значение канала i
    static int16_t*  _l0vals;
    static uint32_t  _l0ts[HISTORY_L0_SIZE];
    static uint16_t  _l0head;
    static uint16_t  _l0count;

    // Аккумуляторы агрегации
    static ChannelAgg* _l1agg;
    static ChannelAgg* _l2agg;

    // Pending буферы
    static std::vector<std::vector<uint8_t>> _l1pending;
    static std::vector<std::vector<uint8_t>> _l2pending;

    // Таймеры
    static uint32_t _lastL0tick;
    static uint32_t _lastL1tick;
    static uint32_t _lastL2tick;
    static uint32_t _lastFlush;

    static bool _ready;
};

#endif // MODULE_HISTORY