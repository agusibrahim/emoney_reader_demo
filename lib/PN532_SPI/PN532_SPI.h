
#ifndef __PN532_SPI_H__
#define __PN532_SPI_H__

#include <Arduino.h>
#include <SPI.h>
#include "PN532Interface.h"

class PN532_SPI : public PN532Interface {
public:
    PN532_SPI(SPIClass &spi, uint8_t ss);
    
    void begin();
    void wakeup();
    int8_t writeCommand(const uint8_t *header, uint8_t hlen, const uint8_t *body = 0, uint8_t blen = 0);

    int16_t readResponse(uint8_t buf[], uint8_t len, uint16_t timeout);
    
private:
    SPIClass* _spi;
    uint8_t   _ss;
    uint8_t command;
    
    bool isReady();
    void writeFrame(const uint8_t *header, uint8_t hlen, const uint8_t *body = 0, uint8_t blen = 0);
    int8_t readAckFrame();
    
    inline void write(uint8_t data) {
        _spi->beginTransaction(SPISettings(2000000, LSBFIRST, SPI_MODE0));
        _spi->transfer(data);
        _spi->endTransaction();
    };

    inline uint8_t read() {
        _spi->beginTransaction(SPISettings(2000000, LSBFIRST, SPI_MODE0));
        uint8_t data = _spi->transfer(0);
        _spi->endTransaction();
        return data;
    }; 
};

#endif
