#include "screen.h"
#include <Arduino.h>
#include <SPI.h>

inline void screen_sendCommand(screen_t *screen, uint8_t data);
inline void screen_sendData(screen_t *screen, uint8_t data);
inline void screen_sendDataArray(screen_t *screen, uint8_t *data, uint32_t size);

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

    SPI.beginTransaction(SPISettings(screen->spiFrecuency, MSBFIRST, SPI_MODE3));
    digitalWrite(screen->cs, LOW);

    screen_sendCommand(screen, 0x01); // SWRESET
    delay(200);

    screen_sendCommand(screen, 0x11); // SLEEPOUT
    delay(200);

    screen_sendCommand(screen, 0x36); // MADCTL
    screen_sendData(screen, 0x00);

    screen_sendCommand(screen, 0x2A); // CASET
    screen_sendData(screen, 0x00);
    screen_sendData(screen, 0x00);
    screen_sendData(screen, (uint8_t)((screen->sizeX & 0xFF00) >> 8));
    screen_sendData(screen, (uint8_t)(screen->sizeX & 0xFF));
    delay(100);

    screen_sendCommand(screen, 0x2B); // RASET
    screen_sendData(screen, 0x00);
    screen_sendData(screen, 0x00);
    screen_sendData(screen, (uint8_t)((screen->sizeY & 0xFF00) >> 8));
    screen_sendData(screen, (uint8_t)(screen->sizeY & 0xFF));
    delay(100);

    screen_sendCommand(screen, 0x3A); // COLMOD
    screen_sendData(screen, 0x05);
    delay(100);

    screen_sendCommand(screen, 0x0C); // RDDCOLMOD
    screen_sendData(screen, 0x55);
    screen_sendData(screen, 0x55);

    screen_sendCommand(screen, 0xB0); // RAMCTRL
    screen_sendData(screen, 0x00);
    screen_sendData(screen, 0xF5);

    screen_sendCommand(screen, 0x34); // TEOFF
    screen_sendCommand(screen, 0x38); // IDMOFF

    screen_sendCommand(screen, 0x13); // NORON
    delay(10);

    screen_sendCommand(screen, 0x21); // INVON
    screen_sendCommand(screen, 0x29); // DISPON
    delay(200);

    screen_sendCommand(screen, 0xB2); // PORCTRL
    screen_sendData(screen, 0x05);

    digitalWrite(screen->cs, HIGH);
    SPI.endTransaction();
}

void screen_sendBuffer(screen_t *screen, uint8_t *buffer, uint32_t size)
{
    SPI.beginTransaction(SPISettings(screen->spiFrecuency, MSBFIRST, SPI_MODE3));
    digitalWrite(screen->cs, LOW);

    screen_sendCommand(screen, 0x2C); // RAMWR
    screen_sendDataArray(screen, buffer, size);

    digitalWrite(screen->cs, HIGH);
    SPI.endTransaction();
}

void screen_clearWhite(screen_t *screen)
{
    uint32_t screenSize = screen->sizeX * screen->sizeY * 2;

    SPI.beginTransaction(SPISettings(screen->spiFrecuency, MSBFIRST, SPI_MODE3));
    digitalWrite(screen->cs, LOW);
    screen_sendCommand(screen, 0x2C); // RAMWR

    for (uint32_t i = 0; i < screenSize; i++)
    {
        if (i % 2 == 0)
        {
            screen_sendData(screen, 0x00);
        }
        else
        {
            screen_sendData(screen, 0x1F);
        }
    }

    digitalWrite(screen->cs, HIGH);
    SPI.endTransaction();
}

void screen_clearColor(screen_t *screen, uint16_t color)
{
    uint32_t screenSize = screen->sizeX * screen->sizeY * 2;

    SPI.beginTransaction(SPISettings(screen->spiFrecuency, MSBFIRST, SPI_MODE3));
    digitalWrite(screen->cs, LOW);
    screen_sendCommand(screen, 0x2C); // RAMWR

    for (uint32_t i = 0; i < screenSize; i++)
    {
        if (i % 2 == 0)
        {
            screen_sendData(screen, (uint8_t)(color >> 8));
        }
        else
        {
            screen_sendData(screen, (uint8_t)color);
        }
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

inline void screen_sendDataArray(screen_t *screen, uint8_t *data, uint32_t size)
{
    digitalWrite(screen->dc, HIGH);
    SPI.transfer(data, size);
}