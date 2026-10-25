#pragma once
#ifdef DEVICE_MASTERGAS

#include "MasterGasTypes.h"
#include "../../core/CoreTypes.h"

class MasterGasDevice {
public:
    static void init();
    static void loop();
    static bool loadConfig();
    static bool saveConfig();

private:
    static bool    _waitPacket(HardwareSerial& serial, uint8_t* buf, bool trackBurnerStats = false);
    static void    _parsePacket(const uint8_t* buf);
    static bool    _isFromBoiler(const uint8_t* buf);
    static uint8_t _calcCrc(const uint8_t* buf);
    static void    _sendReply();
    static void    _publishBus();
    static void    _busTick();
    static void    _ledTick();

    static uint32_t _lastPub;

    // Команды из Bus (см. MasterGasDevice.cpp): коллбэк запоминает значение,
    // применяет и сохраняет loop(). -1 = "не менялось".
    static volatile bool    _selfWrite;
    static volatile bool    _cfgPending;
    static volatile int32_t _pendPass;
    static volatile int32_t _pendPower;
    static volatile int32_t _pendTemp;
    static volatile bool    _publishNow;
};

#endif // DEVICE_MASTERGAS
