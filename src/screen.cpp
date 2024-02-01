#include "screen.h"
#include <Arduino.h>
#include <SPI.h>

inline void screen_sendCommand(screen_t *screen, uint8_t data);
inline void screen_sendData(screen_t *screen, uint8_t data);

void screen_init(screen_t *screen)
{
    SPI.begin();

    pinMode(screen->cs, OUTPUT);
    pinMode(screen->rst, OUTPUT);
    pinMode(screen->dc, OUTPUT);

    // Reset
    digitalWrite(screen->rst, HIGH);
    delayMicroseconds(10);
    digitalWrite(screen->rst, LOW);
    delayMicroseconds(10);
    digitalWrite(screen->rst, HIGH);

    digitalWrite(screen->cs, LOW);
    SPI.beginTransaction(SPISettings(screen->spiFrecuency, MSBFIRST, SPI_MODE0));

    screen_sendCommand(screen, 0x2A); // CASET
    screen_sendData(screen, 0x00);
    screen_sendData(screen, 0x00);
    screen_sendData(screen, 0x00);
    screen_sendData(screen, 0x87);

    screen_sendCommand(screen, 0x2B); // RASET
    screen_sendData(screen, 0x00);
    screen_sendData(screen, 0x00);
    screen_sendData(screen, 0x00);
    screen_sendData(screen, 0xF0);

    screen_sendCommand(screen, 0x3A); // COLMOD
    screen_sendData(screen, 0x05);

    screen_sendCommand(screen, 0x11); // SLEEPOUT

    digitalWrite(screen->cs, HIGH);
    SPI.endTransaction();
}

void screen_sendBuffer(screen_t *screen, uint8_t *buffer, uint32_t size)
{
    digitalWrite(screen->cs, LOW);
    SPI.beginTransaction(SPISettings(100000, MSBFIRST, SPI_MODE0));

    screen_sendCommand(screen, 0x2C); // RAMWR

    for (uint32_t i = 0; i < size; i++)
    {
        screen_sendData(screen, buffer[i]);
    }

    digitalWrite(screen->cs, HIGH);
    SPI.endTransaction();
}

inline void screen_sendCommand(screen_t *screen, uint8_t data)
{
    digitalWrite(screen->dc, LOW);
    SPI.transfer(data);
}

inline void screen_sendData(screen_t *screen, uint8_t data)
{
    digitalWrite(screen->dc, HIGH);
    SPI.transfer(data);
}