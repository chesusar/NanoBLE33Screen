#ifndef SCREEN_H
#define SCREEN_H

#include <Arduino.h>

typedef struct
{
    uint32_t sizeX;
    uint32_t sizeY;
    uint8_t dc;
    uint8_t rst;
    uint8_t cs;
    uint32_t spiFrecuency;
} screen_t;

/**
 * @brief Initializes the screen
 * @param screen: Screen struct to initialize
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

#endif