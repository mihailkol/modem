#pragma once
#ifdef MODULE_DATACHANNEL

#include <Arduino.h>
#include <functional>
#include <vector>

// ============================================================
//  ТИП КАНАЛА
// ============================================================
enum ChannelType : uint8_t {
    CH_FLOAT   = 0,   // float: температура, давление, мощность
                      //   → хранится int16_t * scale, агрегируется avg/min/max
    CH_COUNTER = 1,   // uint32: импульсы, счётчики
                      //   → хранится накопленное значение за период
    CH_BOOL    = 2,   // bool: авария, состояние реле
                      //   → хранится % времени active за период (0–100)
};

// ============================================================
//  ДЕСКРИПТОР КАНАЛА
//  Объявляется статически в device/app, саморегистрируется
// ============================================================
struct DataChannel {
    // ── Идентификация ────────────────────────────────────────
    const char* id;            // уникальный id, точка как разделитель
                               //   "bmon.t_supply", "brr.heat.power_kw"
    const char* _labelDefault; // исходный label из кода (flash, не меняется)
    String      _labelOverride;// override из channels.json (пустая = не задан)
    const char* unit;          // единица измерения: "°C", "бар", "кВт", ""

    // Геттер label: возвращает override если задан, иначе default из кода
    const char* label() const {
        return (_labelOverride.length() > 0) ? _labelOverride.c_str() : _labelDefault;
    }
    void setLabel(const char* l) { _labelOverride = (l && strlen(l) > 0) ? l : ""; }

    // ── Тип и масштабирование ────────────────────────────────
    ChannelType type  = CH_FLOAT;
    int16_t     scale = 10;   // для CH_FLOAT: int16 = float * scale
                              // для CH_COUNTER, CH_BOOL: не используется

    // ── Геттер ──────────────────────────────────────────────
    // Возвращает float для всех типов:
    //   CH_FLOAT:   прямое значение
    //   CH_COUNTER: текущий счётчик (монотонно возрастающий uint32, каст к float)
    //   CH_BOOL:    0.0f или 1.0f
    std::function<float()> getter;

    // ── История ─────────────────────────────────────────────
    bool history = true;      // логировать в HistoryLogger

    // ── MQTT ────────────────────────────────────────────────
    bool     mqtt             = true;   // публиковать в MQTT
    uint32_t mqtt_interval    = 0;      // 0 = on change, >0 = интервал в секундах
    float    mqtt_threshold   = 0.1f;   // порог изменения для on change
                                        // для CH_BOOL и CH_COUNTER игнорируется

    // ── Runtime (не задаётся при объявлении) ─────────────────
    bool     _enabled         = true;   // runtime override из channels.json
    float    _lastMqttValue   = NAN;    // последнее опубликованное значение
    uint32_t _lastMqttTime    = 0;      // millis() последней публикации
    float    y_min            = NAN;    // min для нормализации (NAN = авто)
    float    y_max            = NAN;    // max для нормализации (NAN = авто)
    int16_t  _lastLoggedInt   = INT16_MIN; // последнее записанное int16 значение
                                           // INT16_MIN = первая запись всегда проходит

    // ── Саморегистрация ──────────────────────────────────────
    DataChannel() { _register(this); }

    // Вспомогательный конструктор с инициализатором полей
    // (чтобы статические объекты можно было объявлять с {}-инициализацией
    //  и при этом получить саморегистрацию)
    DataChannel(const char* id_, const char* label_, const char* unit_,
                ChannelType type_, int16_t scale_,
                std::function<float()> getter_,
                bool history_ = true,
                bool mqtt_ = true,
                uint32_t mqtt_interval_ = 0,
                float mqtt_threshold_ = 0.1f)
        : id(id_), _labelDefault(label_), unit(unit_),
          type(type_), scale(scale_), getter(getter_),
          history(history_), mqtt(mqtt_),
          mqtt_interval(mqtt_interval_), mqtt_threshold(mqtt_threshold_)
    {
        _register(this);
    }

    // ── Список всех каналов (singleton, избегаем static init order fiasco) ──
    static std::vector<DataChannel*>& all() {
        static std::vector<DataChannel*> _list;
        return _list;
    }

private:
    static void _register(DataChannel* ch) {
        all().push_back(ch);
    }
};

// ============================================================
//  МАКРОС ДЛЯ ОБЪЯВЛЕНИЯ КАНАЛА В МОДУЛЕ
//
//  Использование (в .cpp файле модуля):
//
//  DECLARE_CHANNEL(ch_t_supply,
//      "bmon.t_supply", "Подача", "°C", CH_FLOAT, 10,
//      []() -> float { return bmonState.t_supply; }
//  );
//
//  Опциональные параметры history/mqtt/interval/threshold
//  можно задать через поля после объявления или дополнительными
//  аргументами конструктора.
// ============================================================
#define DECLARE_CHANNEL(varname, ...) \
    static DataChannel varname(__VA_ARGS__)

#endif // MODULE_DATACHANNEL