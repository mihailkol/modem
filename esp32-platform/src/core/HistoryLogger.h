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
#define HISTORY_L0_INTERVAL_SEC   10      // интервал снятия сырых данных
#define HISTORY_L0_SIZE          720      // L0: 720 × 10с = 2 часа (RAM only)

#define HISTORY_L1_RATIO           6      // L1 = агрегат 6 × L0  → 1 минута
#define HISTORY_L1_SIZE         1440      // L1: 1440 × 1мин = 24 часа

#define HISTORY_L2_RATIO          10      // L2 = агрегат 10 × L1 → 10 минут
#define HISTORY_L2_SIZE         2016      // L2: 2016 × 10мин = 14 суток

#define HISTORY_FLUSH_INTERVAL_MS 600000u // сброс pending на диск каждые 10 минут

// ============================================================
//  ФОРМАТ БИНАРНОЙ ЗАПИСИ (L1 / L2)
//
//  Размер записи фиксирован для данного набора каналов,
//  вычисляется в begin() по _channels и кешируется в _recordSize.
//
//  Layout:
//    [uint32_t ts]               — 4 байта, unix или 0 если нет NTP
//    для каждого канала:
//      CH_FLOAT:   int16 avg, int16 min, int16 max  — 6 байт (value * scale)
//      CH_COUNTER: float value                      — 4 байта (абсолютное значение)
//      CH_BOOL:    uint8 pct                        — 1 байт  (% времени active, 0-100)
//
//  L0 (RAM) хранит только снимок float на канал + ts, без агрегации.
// ============================================================

// ── Агрегат одного канала за период ──────────────────────────
struct ChannelAgg {
    // CH_FLOAT
    float    sum  = 0.0f;
    float    mn   = FLT_MAX;
    float    mx   = -FLT_MAX;
    // CH_COUNTER
    float    counterLast = 0.0f;  // последнее значение за период
    // CH_BOOL
    uint32_t boolActiveMs = 0;
    // общее
    uint32_t count = 0;
    uint32_t periodMs = 0;        // накопленное время периода (для CH_BOOL %)

    void reset() {
        sum = 0.0f; mn = FLT_MAX; mx = -FLT_MAX;
        counterLast = 0.0f;
        boolActiveMs = 0;
        count = 0; periodMs = 0;
    }

    void push(float val, uint32_t dtMs, ChannelType type) {
        count++;
        periodMs += dtMs;
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

    float avg()     const { return (count > 0) ? sum / count : 0.0f; }
    float floatMin() const { return (mn == FLT_MAX) ? 0.0f : mn; }
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
    // Вызывается из main.cpp после инициализации всех устройств.
    // Собирает DataChannel::all(), читает channels.json с overrides,
    // проверяет fingerprint, при несовпадении инвалидирует файлы.
    static void begin(AsyncWebServer& server);

    // Вызывается в основном loop()
    static void loop();

    // ── Публичный доступ к каналам (для внешних модулей) ─────
    static uint8_t      channelCount();
    static DataChannel* channel(uint8_t idx);

private:
    // ── Инициализация ────────────────────────────────────────
    static void    _loadOverrides();       // читает /history/channels.json
    static void    _saveOverrides();       // пишет /history/channels.json
    static void    _buildActiveList();     // DataChannel::all() → _channels (history=true, _enabled=true)
    static bool    _checkFingerprint();    // true = каналы не изменились
    static void    _invalidateFiles();     // удаляет l1/l2 файлы
    static uint16_t _calcRecordSize();     // вычисляет _recordSize по _channels
    static void    _registerRoutes(AsyncWebServer& server);

    // ── Тики (вызываются из loop по таймеру) ─────────────────
    static void _tickL0();   // каждые HISTORY_L0_INTERVAL_SEC: снимает геттеры,
                             // пишет в L0 буфер, пушит в _l1agg

    static void _finalizeL1(); // вызывается когда _l1accum == L1_RATIO:
                               // финализирует _l1agg → pending запись,
                               // сбрасывает _l1agg, пушит агрегат в _l2agg

    static void _finalizeL2(); // вызывается когда _l2accum == L2_RATIO:
                               // финализирует _l2agg → pending запись,
                               // сбрасывает _l2agg

    // ── Сериализация одной записи в байты ────────────────────
    // Возвращает вектор байт готовый для записи в файл
    static std::vector<uint8_t> _serializeRecord(uint32_t ts,
                                                  ChannelAgg* aggs,
                                                  ChannelType* types,
                                                  int16_t* scales,
                                                  uint8_t nCh);

    // ── Сброс pending на диск ─────────────────────────────────
    static void _flushPending();          // сбрасывает оба уровня
    static void _flushLevel(const char* path, const char* prevPath,
                             std::vector<std::vector<uint8_t>>& pending,
                             uint32_t maxRecords);
    static void _rotateFile(const char* path, const char* prevPath);

    // ── REST отдача ───────────────────────────────────────────
    static void _sendL0(AsyncWebServerRequest* req, uint16_t last);
    static void _sendFile(AsyncWebServerRequest* req,
                          const char* path, const char* prevPath,
                          uint16_t last);

    // ── Fingerprint ──────────────────────────────────────────
    static String _buildFingerprint();   // "bmon.t_supply:bmon.t_return:..."

    // ── Данные ───────────────────────────────────────────────
    static std::vector<DataChannel*> _channels;  // каналы с history=true и _enabled=true
    static uint16_t  _recordSize;                // байт на одну L1/L2 запись

    // L0: плоский кольцевой буфер float[L0_SIZE × nChannels] + timestamps
    static float*    _l0buf;
    static uint32_t  _l0ts[HISTORY_L0_SIZE];
    static uint16_t  _l0head;
    static uint16_t  _l0count;

    // Аккумуляторы агрегации
    static ChannelAgg* _l1agg;     // [nChannels], L0 → L1
    static uint8_t     _l1accum;   // сколько L0 уже в текущем L1 периоде
    static ChannelAgg* _l2agg;     // [nChannels], L1 → L2
    static uint8_t     _l2accum;   // сколько L1 уже в текущем L2 периоде

    // Pending буферы — готовые записи ждут сброса на диск
    static std::vector<std::vector<uint8_t>> _l1pending;
    static std::vector<std::vector<uint8_t>> _l2pending;

    // Таймеры
    static uint32_t _lastL0tick;
    static uint32_t _lastFlush;

    static bool _ready;
};

#endif // MODULE_HISTORY