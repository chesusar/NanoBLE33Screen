#ifndef SCREEN_H
#define SCREEN_H

#include <stdint.h>

#define SCREEN_MAX_SIZE_X 135
#define SCREEN_MAX_SIZE_Y 240
#define BUFFER_FACTOR 1
#define BUFFER_SIZE_X SCREEN_MAX_SIZE_X // (SCREEN_MAX_SIZE_X / BUFFER_FACTOR + 1)
#define BUFFER_SIZE_Y SCREEN_MAX_SIZE_Y // (SCREEN_MAX_SIZE_Y / BUFFER_FACTOR + 1)

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
    uint8_t blk;
    uint32_t spiFrecuency;
} screen_t;

/**
 * @brief Initializes the screen and buffer
 * @param screen: Screen struct to initialize
 */
void screen_init(screen_t *screen);

/**
 * @brief Clears the screen to RGB 5-6-5 color
 * @param screen: Screen struct
 * @param color: color in RGB 5-6-5 format
 */
void screen_clearColor(screen_t *screen, color16_t color);

/**
 * @brief Draws a pixel in the buffer
 * @param posX: PosX of pixel in buffer coordinates
 * @param posY: PosY of pixel in buffer coordinates
 * @param color: Color of the pixel to draw
 */
void screen_drawPixel(uint8_t posX, uint8_t posY, color16_t color);

/**
 * @brief Draws an image
 * @param posX: PosX of image in buffer coordinates
 * @param posY: PosY of image in buffer coordinates
 * @param sizeX: SizeX of image
 * @param sizeY: SizeY of image
 * @param image: Image to draw
 */
void screen_drawImage(uint8_t posX, uint8_t posY, uint8_t sizeX, uint8_t sizeY, const uint16_t *image);

/**
 * @brief Clears the buffer to black 0x0000
 */
void screen_clearBuffer();

/**
 * @brief Sends the buffer to the screen
 * @param screen: Screen to send buffer to
 */
void screen_refresh(screen_t *screen);

/**
 * @brief Sets screen in sleep mode
 * @param screen: Screen to set in sleep mode
 */
void screen_sleepIn(screen_t *screen);

/**
 * @brief Wakes up the screen
 * @param screen: Screen to wake up
 */
void screen_sleepOut(screen_t *screen);

/**
 * @brief Sets screen brightness
 * @param screen Screen to set brightness to
 * @param brightness brightness from 0 (lowest) to 255 (highest)
 */
void screen_setBrightness(screen_t *screen, uint8_t brightness);

#endif