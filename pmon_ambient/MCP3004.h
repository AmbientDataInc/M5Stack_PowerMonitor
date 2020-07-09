#ifndef MCP3004_H
#define MCP3004_H

#ifdef ARDUINO_M5Stack_Core_ESP32
#include <M5Stack.h>
#endif
#include <SPI.h>

class MCP3004
{
public:
    MCP3004(uint8_t);
    virtual ~MCP3004();

    void begin(void);
    uint16_t read(uint8_t ch);
private:
    uint8_t _ss;
};

#endif // MCP3004_H
