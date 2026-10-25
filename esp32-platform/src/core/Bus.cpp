#include "Bus.h"

Bus::Channel      Bus::_channels[BUS_MAX_CHANNELS];
size_t            Bus::_channelCount = 0;

Bus::Subscription Bus::_subs[BUS_MAX_SUBSCRIPTIONS];
size_t            Bus::_subCount = 0;

// ── Поиск ────────────────────────────────────────────────────────────
Bus::Channel* Bus::_find(const char* id) {
    for (size_t i = 0; i < _channelCount; i++) {
        if (strcmp(_channels[i].id, id) == 0) return &_channels[i];
    }
    return nullptr;
}

Bus::Channel& Bus::_findOrCreate(const char* id, int32_t scale) {
    Channel* ch = _find(id);
    if (ch) return *ch;

    if (_channelCount >= BUS_MAX_CHANNELS) {
        Serial.printf("[Bus] ERROR: channel limit (%d) exceeded, dropping '%s'\n",
                      BUS_MAX_CHANNELS, id);
        // Возвращаем последний слот, чтобы не разыменовывать мусор —
        // данные будут биться с другим каналом, но прошивка не упадёт.
        // Правильный фикс — поднять BUS_MAX_CHANNELS.
        return _channels[BUS_MAX_CHANNELS - 1];
    }

    Channel& created = _channels[_channelCount++];
    created.id       = id;
    created.scale    = scale;
    created.value    = 0;
    created.hasValue = false;
    return created;
}

// ── Публичное API ────────────────────────────────────────────────────
void Bus::declare(const char* id, int32_t scale) {
    Channel& ch = _findOrCreate(id, scale);
    ch.scale = scale;   // повторный declare() может переопределить scale
}

void Bus::write(const char* id, int64_t value) {
    Channel* ch = _find(id);
    if (!ch) {
        Serial.printf("[Bus] WARNING: write('%s') without declare(), auto-registering with scale=1\n", id);
        ch = &_findOrCreate(id, 1);
    }
    ch->value    = value;
    ch->hasValue = true;
    _notify(id, value);
}

int64_t Bus::read(const char* id) {
    Channel* ch = _find(id);
    return ch ? ch->value : 0;
}

bool Bus::hasValue(const char* id) {
    Channel* ch = _find(id);
    return ch ? ch->hasValue : false;
}

int32_t Bus::scaleOf(const char* id) {
    Channel* ch = _find(id);
    return ch ? ch->scale : 1;
}

void Bus::subscribe(const char* id, BusCallback cb) {
    if (_subCount >= BUS_MAX_SUBSCRIPTIONS) {
        Serial.printf("[Bus] ERROR: subscription limit (%d) exceeded, ignoring subscribe('%s')\n",
                      BUS_MAX_SUBSCRIPTIONS, id);
        return;
    }
    Subscription& s = _subs[_subCount++];
    s.id = id;
    s.cb = cb;
}

void Bus::subscribeAll(BusCallback cb) {
    if (_subCount >= BUS_MAX_SUBSCRIPTIONS) {
        Serial.println("[Bus] ERROR: subscription limit exceeded, ignoring subscribeAll()");
        return;
    }
    Subscription& s = _subs[_subCount++];
    s.id = nullptr;
    s.cb = cb;
}

size_t Bus::count() {
    return _channelCount;
}

bool Bus::infoAt(size_t index, ChannelInfo& out) {
    if (index >= _channelCount) return false;
    Channel& ch = _channels[index];
    out.id       = ch.id;
    out.value    = ch.value;
    out.scale    = ch.scale;
    out.hasValue = ch.hasValue;
    return true;
}

// ── Уведомление подписчиков ──────────────────────────────────────────
void Bus::_notify(const char* id, int64_t value) {
    for (size_t i = 0; i < _subCount; i++) {
        Subscription& s = _subs[i];
        if (s.id == nullptr || strcmp(s.id, id) == 0) {
            s.cb(id, value);
        }
    }
}
