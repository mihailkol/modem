#pragma once
#include <Arduino.h>
#include <functional>

// ============================================================
//  BUS — центральная шина данных (publish/subscribe, singleton)
//
//  Правила (см. boiler-room-v2-tz.md, п.3.1):
//   - Значения: int64_t + scale на канал. Никаких float на шине.
//     Пример: 23.45°C при scale=100 -> хранится как 2345.
//   - Хранится только ПОСЛЕДНЕЕ значение канала, без очереди/истории.
//   - write() синхронно вызывает подписчиков. Подписчик обязан быть
//     дешёвым (обновить переменную / выставить флаг). Тяжёлые операции
//     (запись в файл, сетевой вызов) — делать в своём loop() по флагу,
//     а не прямо в callback'е.
//   - Каналы объявляются через declare() — обычно в init() модуля,
//     который владеет данными. Повторный declare(id) с тем же id —
//     не ошибка (например, объявление и в device, и переопределение
//     scale в app), просто обновляет scale.
// ============================================================

// Было 48 — хватало исторически, но добавление raw.din0-15/raw.dout0-15
// в A16Device (32 канала для вкладки "Диагностика") столкнуло реальный
// счёт каналов за лимит: порядок init() в main.cpp — Time->RS485->A16->
// PumpSlave->EkoNom->BoilerRoomApp->MQTT — и A16(~40, включая 4 динамических
// raw.ow{i}_temp)+PumpSlave(8) уже съедали все 48 слотов ДО того, как
// EkoNom/BoilerRoomApp успевали объявить свои каналы. Переполнение не
// крашит прошивку (см. Bus::_findOrCreate — молча алиасит на последний
// слот и пишет "[Bus] ERROR: channel limit (N) exceeded" в serial), но
// визуально выглядит как "часть данных пропала" — ровно то, что видел
// Михаил на дашборде (t_supply/t_return/t_delta/flow_m3h ещё влезли,
// power_kw/energy_kwh/volume_m3 и весь BoilerRoomApp — уже нет). Реально
// нужно ~74 канала на сегодня; взято с запасом на будущее.
#define BUS_MAX_CHANNELS     128
#define BUS_MAX_SUBSCRIPTIONS 64

using BusCallback = std::function<void(const char* id, int64_t value)>;

class Bus {
public:
    // Регистрирует канал с заданным scale (идемпотентно).
    static void declare(const char* id, int32_t scale = 1);

    // Записывает значение канала. Если канал не был declare() —
    // регистрирует его автоматически со scale=1 (и печатает предупреждение
    // в Serial, т.к. это обычно означает забытый declare()).
    static void write(const char* id, int64_t value);

    // Последнее записанное значение. 0, если канала нет или ещё не было write().
    static int64_t read(const char* id);

    // Было ли хоть одно write() по каналу (полезно отличать "0" от "нет данных").
    static bool hasValue(const char* id);

    // Scale канала. 1, если канал не найден.
    static int32_t scaleOf(const char* id);

    // Подписка на конкретный канал — callback вызывается при каждом write() по нему.
    static void subscribe(const char* id, BusCallback cb);

    // Подписка на любую запись в любой канал (для MqttModule/WebModule).
    static void subscribeAll(BusCallback cb);

    // Количество зарегистрированных каналов — для диагностики/веб-снимка.
    static size_t count();

    // Снимок канала по индексу (0..count()-1) — для WebModule (/api/channels).
    struct ChannelInfo {
        const char* id;
        int64_t     value;
        int32_t     scale;
        bool        hasValue;
    };
    static bool infoAt(size_t index, ChannelInfo& out);

private:
    struct Channel {
        const char* id = nullptr;
        int32_t     scale = 1;
        int64_t     value = 0;
        bool        hasValue = false;
    };
    struct Subscription {
        const char* id = nullptr;  // nullptr = subscribeAll
        BusCallback cb;
    };

    static Channel*       _find(const char* id);
    static Channel&       _findOrCreate(const char* id, int32_t scale);
    static void           _notify(const char* id, int64_t value);

    static Channel      _channels[BUS_MAX_CHANNELS];
    static size_t       _channelCount;

    static Subscription _subs[BUS_MAX_SUBSCRIPTIONS];
    static size_t       _subCount;
};
