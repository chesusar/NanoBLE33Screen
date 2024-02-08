#include "screen.h"
#include <Arduino.h>
#include <SPI.h>

#define CURRENT_SPI_MODE SPI_MODE3
#define CURRENT_SPI_ENDIANESS MSBFIRST

static color16_t screenBuffer[BUFFER_SIZE_X * BUFFER_SIZE_Y]; // color16 in big-endian

inline static void screen_sendCommand(screen_t *screen, uint8_t data);
inline static void screen_sendData(screen_t *screen, uint8_t data);

void screen_init(screen_t *screen)
{
    color16_t blackColor = {.color16 = 0x0000};
    for (uint32_t x = 0; x < BUFFER_SIZE_X; x++)
    {
        for (uint32_t y = 0; y < BUFFER_SIZE_Y; y++)
        {
            screenBuffer[x + BUFFER_SIZE_X * y] = blackColor;
        }
    }

    SPI.begin();

    pinMode(screen->cs, OUTPUT);
    pinMode(screen->rst, OUTPUT);
    pinMode(screen->dc, OUTPUT);
    if (screen->blk != -1)
    {
        pinMode(screen->blk, OUTPUT);
        digitalWrite(screen->blk, HIGH);
    }
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
    screen_sendData(screen, 0x00);

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

void screen_clearColor(screen_t *screen, color16_t color)
{
    uint32_t screenSize = screen->sizeX * screen->sizeY * 2;

    SPI.beginTransaction(SPISettings(screen->spiFrecuency, CURRENT_SPI_ENDIANESS, CURRENT_SPI_MODE));
    digitalWrite(screen->cs, LOW);
    screen_sendCommand(screen, 0x2C); // RAMWR

    for (uint32_t i = 0; i < screenSize; i++)
    {
        screen_sendData(screen, color.color8[1]);
        screen_sendData(screen, color.color8[0]);
    }

    digitalWrite(screen->cs, HIGH);
    SPI.endTransaction();
}

void screen_drawPixel(uint8_t posX, uint8_t posY, color16_t color)
{
    color16_t pixel; // Buffer in big-endian
    pixel.color8[0] = color.color8[1];
    pixel.color8[1] = color.color8[0];
    screenBuffer[posX + BUFFER_SIZE_X * posY] = pixel;
}

void screen_drawImage(uint8_t posX, uint8_t posY, uint8_t sizeX, uint8_t sizeY, const uint16_t *image)
{
    for (uint8_t x = 0; x < sizeX; x++)
    {
        for (uint8_t y = 0; y < sizeY; y++)
        {
            uint8_t indexX = posX + x;
            uint8_t indexY = posY + y;

            if (indexX >= BUFFER_SIZE_X || indexY >= BUFFER_SIZE_Y)
                continue;

            color16_t pixel;
            uint16_t imageColor = image[x + sizeX * y];
            pixel.color8[0] = (uint8_t)(imageColor >> 8); // transform to big endian
            pixel.color8[1] = (uint8_t)(imageColor & 0x00FF);
            screenBuffer[indexX + BUFFER_SIZE_X * indexY] = pixel;
        }
    }
}

void screen_clearBuffer()
{
    color16_t color = {.color16 = 0x0000};
    for (uint32_t y = 0; y < BUFFER_SIZE_Y; y++)
    {
        for (uint32_t x = 0; x < BUFFER_SIZE_X; x++)
        {

            screenBuffer[x + BUFFER_SIZE_X * y] = color;
        }
    }
}

void screen_refresh(screen_t *screen)
{
    SPI.beginTransaction(SPISettings(screen->spiFrecuency, CURRENT_SPI_ENDIANESS, CURRENT_SPI_MODE));
    digitalWrite(screen->cs, LOW);
    screen_sendCommand(screen, 0x2C); // RAMWR

    digitalWrite(screen->dc, HIGH);

    SPI.transfer(screenBuffer, BUFFER_SIZE_X * BUFFER_SIZE_Y * sizeof(color16_t));

    digitalWrite(screen->cs, HIGH);
    SPI.endTransaction();
}

void screen_sleepIn(screen_t *screen)
{
    SPI.beginTransaction(SPISettings(screen->spiFrecuency, CURRENT_SPI_ENDIANESS, CURRENT_SPI_MODE));
    digitalWrite(screen->cs, LOW);
    screen_sendCommand(screen, 0x10);
    SPI.endTransaction();
    if (screen->blk != -1)
        digitalWrite(screen->blk, LOW);
}

void screen_sleepOut(screen_t *screen)
{
    SPI.beginTransaction(SPISettings(screen->spiFrecuency, CURRENT_SPI_ENDIANESS, CURRENT_SPI_MODE));
    digitalWrite(screen->cs, LOW);
    screen_sendCommand(screen, 0x11);
    SPI.endTransaction();
    if (screen->blk != -1)
        digitalWrite(screen->blk, HIGH);
}

void screen_setBrightness(screen_t *screen, uint8_t brightness)
{
    analogWrite(screen->blk, brightness);
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