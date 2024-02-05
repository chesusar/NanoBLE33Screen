#include "screen.h"
#include <Arduino.h>
#include <SPI.h>

#define CURRENT_SPI_MODE SPI_MODE3
#define CURRENT_SPI_ENDIANESS MSBFIRST

static color16_t screenBuffer[BUFFER_SIZE_X][BUFFER_SIZE_Y];

inline static void screen_sendCommand(screen_t *screen, uint8_t data);
inline static void screen_sendData(screen_t *screen, uint8_t data);
inline static void screen_sendDataArray(screen_t *screen, uint8_t *data, uint32_t size);

void screen_init(screen_t *screen)
{
    color16_t blackColor = {.color16 = 0x0000};
    for (uint32_t x = 0; x < BUFFER_SIZE_X; x++)
    {
        for (uint32_t y = 0; y < BUFFER_SIZE_Y; y++)
        {
            // blackColor.color16 = (uint16_t)(x + y * BUFFER_SIZE_Y);
            screenBuffer[x][y] = blackColor;
        }
    }

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

    SPI.beginTransaction(SPISettings(screen->spiFrecuency, CURRENT_SPI_ENDIANESS, CURRENT_SPI_MODE));
    digitalWrite(screen->cs, LOW);

    screen_sendCommand(screen, 0x01); // SWRESET
    delay(200);

    screen_sendCommand(screen, 0x11); // SLEEPOUT
    delay(200);

    screen_sendCommand(screen, 0x3A); // COLMOD
    screen_sendData(screen, 0x55);
    delay(100);

    screen_sendCommand(screen, 0x36); // MADCTL
    screen_sendData(screen, 0x00);    // antes 40

    uint16_t sX, eX;
    sX = screen->offsetX;
    eX = sX + screen->sizeX - 1;
    screen_sendCommand(screen, 0x2A); // CASET
    screen_sendData(screen, (uint8_t)((sX >> 8)));
    screen_sendData(screen, (uint8_t)(sX & 0xFF));
    screen_sendData(screen, (uint8_t)((eX >> 8)));
    screen_sendData(screen, (uint8_t)(eX & 0xFF));
    delay(100);

    uint16_t sY, eY;
    sY = screen->offsetY;
    eY = sY + screen->sizeY - 1;
    screen_sendCommand(screen, 0x2B); // RASET
    screen_sendData(screen, (uint8_t)((sY >> 8)));
    screen_sendData(screen, (uint8_t)(sY & 0xFF));
    screen_sendData(screen, (uint8_t)((eY >> 8)));
    screen_sendData(screen, (uint8_t)(eY & 0xFF));
    delay(100);

    screen_sendCommand(screen, 0x13); // NORON
    delay(10);

    screen_sendCommand(screen, 0x21); // INVON

    screen_sendCommand(screen, 0x29); // DISPON
    delay(200);

    digitalWrite(screen->cs, HIGH);
    SPI.endTransaction();
}

void screen_sendBuffer(screen_t *screen, uint8_t *buffer, uint32_t size)
{
    SPI.beginTransaction(SPISettings(screen->spiFrecuency, CURRENT_SPI_ENDIANESS, CURRENT_SPI_MODE));
    digitalWrite(screen->cs, LOW);

    screen_sendCommand(screen, 0x2C); // RAMWR
    screen_sendDataArray(screen, buffer, size);

    digitalWrite(screen->cs, HIGH);
    SPI.endTransaction();
}

void screen_clearWhite(screen_t *screen)
{
    uint32_t screenSize = screen->sizeX * screen->sizeY * 2;

    SPI.beginTransaction(SPISettings(screen->spiFrecuency, CURRENT_SPI_ENDIANESS, CURRENT_SPI_MODE));
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

    SPI.beginTransaction(SPISettings(screen->spiFrecuency, CURRENT_SPI_ENDIANESS, CURRENT_SPI_MODE));
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

void screen_drawPixel(uint8_t posX, uint8_t posY, color16_t color)
{
    screenBuffer[posX][posY] = color;
}

void screen_refresh(screen_t *screen)
{
    SPI.beginTransaction(SPISettings(screen->spiFrecuency, CURRENT_SPI_ENDIANESS, CURRENT_SPI_MODE));
    digitalWrite(screen->cs, LOW);
    screen_sendCommand(screen, 0x2C); // RAMWR

    for (uint32_t y = 0; y < screen->sizeY; y++)
    {
        for (uint32_t x = 0; x < screen->sizeX; x++)
        {
            uint16_t bufferX = x / BUFFER_FACTOR;
            uint16_t bufferY = y / BUFFER_FACTOR;
            color16_t currentPixel = screenBuffer[bufferX][bufferY];
            screen_sendData(screen, currentPixel.color8[1]);
            screen_sendData(screen, currentPixel.color8[0]);
        }
    }

    digitalWrite(screen->cs, HIGH);
    SPI.endTransaction();
}

inline static void screen_sendCommand(screen_t *screen, uint8_t data)
{
    digitalWrite(screen->dc, LOW);
    SPI.transfer(data);
}

inline static void screen_sendData(screen_t *screen, uint8_t data)
{
    digitalWrite(screen->dc, HIGH);
    SPI.transfer(data);
}

inline static void screen_sendDataArray(screen_t *screen, uint8_t *data, uint32_t size)
{
    digitalWrite(screen->dc, HIGH);
    SPI.transfer(data, size);
}