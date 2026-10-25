#pragma once
// Порт из esp32-platform/src/core/CrashLog.h, теги под новый набор модулей.
#include <Arduino.h>
#include <esp_system.h>

#define CRASH_MAGIC 0xC0FFEE42

struct CrashBreadcrumb {
    uint32_t magic;
    uint32_t uptime_s;
    uint16_t lastTag;
    uint32_t freeHeap;
    uint32_t minFreeHeap;
    uint32_t bootCount;
};

extern RTC_NOINIT_ATTR CrashBreadcrumb g_crumb;

enum CrashTag : uint16_t {
    TAG_NET       = 1,
    TAG_OTA       = 2,
    TAG_MQTT      = 3,
    TAG_RS485     = 4,
    TAG_EKONOM    = 5,
    TAG_A16       = 6,
    TAG_TIME      = 7,
    TAG_PUMPSLAVE = 8,
    TAG_EVENTLOG  = 9,
    TAG_APP       = 10,
    TAG_MASTERGAS = 11,
};

namespace CrashLog {
    void begin();
    void mark(uint16_t tag);
}
