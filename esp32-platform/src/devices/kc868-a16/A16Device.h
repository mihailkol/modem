#pragma once
#ifdef DEVICE_KC868_A16

#include "A16Types.h"
#include "../../core/CoreTypes.h"

class A16Device {
public:
    static void init();
    static void loop();
    static bool loadConfig();
    static bool saveConfig();

    // Управление выходами — можно вызывать из других модулей
    static bool setOutput(uint8_t ch, bool state);   // ch: 0-15
    static bool toggleOutput(uint8_t ch);
    static void setAllOutputs(uint16_t mask);         // битовая маска

private:
    static void readDigitalInputs();
    static void readAnalog();
    static void readOneWire();
    static void writePCF(uint8_t addr, uint8_t data);
    static uint8_t readPCF(uint8_t addr);
    static void syncOutputs();   // применить a16State.dout → PCF8574
    static void scanOneWire();   // найти датчики на шине

    static uint32_t _lastPoll;
    static uint32_t _lastOwRead;
    static bool     _owScanned;
};

#endif // DEVICE_KC868_A16
