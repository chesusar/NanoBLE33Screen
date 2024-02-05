#ifndef SCREEN_H
#define SCREEN_H

#include <Arduino.h>

#define SCREEN_MAX_SIZE_X 135
#define SCREEN_MAX_SIZE_Y 240
#define BUFFER_FACTOR 2
#define BUFFER_SIZE_X (SCREEN_MAX_SIZE_X / BUFFER_FACTOR + 1)
#define BUFFER_SIZE_Y (SCREEN_MAX_SIZE_Y / BUFFER_FACTOR + 1)

typedef union color16_t
{
    uint8_t color8[2];
    uint16_t color16;
} color16_t;

typedef struct screen_t
{
    uint16_t sizeX;
    uint16_t sizeY;
    uint16_t offsetX;
    uint16_t offsetY;
    uint8_t dc;
    uint8_t rst;
    uint8_t cs;
    uint32_t spiFrecuency;
} screen_t;

/**
 * @brief Initializes the screen
 * @param sizeX: Size X
 * @param sizeY: Size Y
 * @param dc: DC pin
 * @param rst: RST pin
 * @param cs: SPI CS pin
 * @param spiFrecuency: Frecuency of the SPI
 * @returns pointer to screen struct
 */
void screen_init(screen_t *screen);

/**
 * @brief Sends screen buffer
 * @param screen: Screen struct
 * @param buffer: Screen buffer
 * @param size: Size iin bytes of buffer
 */
void screen_sendBuffer(screen_t *screen, uint8_t *buffer, uint32_t size);

/**
 * @brief Clears the screen to white
 * @param screen: Screen struct
 */
void screen_clearWhite(screen_t *screen);

/**
 * @brief Clears the screen to RGB 5-6-5 color
 * @param screen: Screen struct
 * @param color: color in RGB 5-6-5 format
 */
void screen_clearColor(screen_t *screen, uint16_t color);

void screen_drawPixel(uint8_t posX, uint8_t posY, color16_t color);

void screen_refresh(screen_t *screen);

#endif