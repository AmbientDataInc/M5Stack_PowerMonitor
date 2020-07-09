#ifndef MCP3004_H
#define MCP3004_H

#ifdef ARDUINO_M5Stack_Core_ESP32
#include <M5Stack.h>
#endif
#include <SPI.h>

#define ADC_SINGLE_CH0 (0x08)     // ADC Single Channel 0
#define ADC_SINGLE_CH1 (0x09)     // ADC Single Channel 1
#define ADC_SINGLE_CH2 (0x0A)     // ADC Single Channel 2
#define ADC_SINGLE_CH3 (0x0B)     // ADC Single Channel 3
#define ADC_SINGLE_CH4 (0x0C)     // ADC Single Channel 4
#define ADC_SINGLE_CH5 (0x0D)     // ADC Single Channel 5
#define ADC_SINGLE_CH6 (0x0E)     // ADC Single Channel 6
#define ADC_SINGLE_CH7 (0x0F)     // ADC Single Channel 7

class MCP3004
{
public:
    MCP3004(uint8_t);
    virtual ~MCP3004();

    void begin(void);
    uint16_t read(uint8_t ch);
private:
    uint8_t _cs;
};

#endif // MCP3004_H
