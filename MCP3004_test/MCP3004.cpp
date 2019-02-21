#include "MCP3004.h"

MCP3004::MCP3004(uint8_t cs) {
    _cs = cs;
}

MCP3004::~MCP3004() {
}

void MCP3004::begin(void) {
    pinMode(_cs, OUTPUT);
    digitalWrite(_cs, LOW);
    digitalWrite(_cs, HIGH);
}

uint16_t MCP3004::read(uint8_t ch) {
    byte MSB, LSB;

    SPI.beginTransaction(SPISettings(200000, MSBFIRST, SPI_MODE0));
    digitalWrite(_cs, LOW);
    SPI.transfer(0x01);                                 // Start Bit
    MSB = SPI.transfer((0x08 | ch) << 4);  // Send readAddress and receive MSB data
    LSB = SPI.transfer(0x00);                     // Push junk data and get LSB byte return
    digitalWrite(_cs, HIGH);
    SPI.endTransaction();

    return (MSB & 0x03) << 8 | LSB;
}
