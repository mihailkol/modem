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
    static void    _publishMqtt();
    static void    _ledTick();
    static void    _onMqtt(const char* topic, const char* payload, unsigned int len);

    static uint32_t _lastPub;
};

#endif // DEVICE_MASTERGAS
