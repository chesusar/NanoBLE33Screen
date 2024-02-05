#include <Arduino.h>
#include "screen.h"

// #define USING_ARDUINO_NANO
#define USING_ESP32

screen_t screen;

color16_t rgbToColor(uint8_t red, uint8_t green, uint8_t blue);

void setup()
{
#ifdef USING_ARDUINO_NANO
  screen = {
      .sizeX = 135,
      .sizeY = 240,
      .offsetX = 52,
      .offsetY = 40,
      .dc = 8,
      .rst = 5,
      .cs = 7,
      .spiFrecuency = 8000000};
  screen_init(&screen);
#endif
#ifdef USING_ESP32
  screen = {
      .sizeX = 135,
      .sizeY = 240,
      .offsetX = 52,
      .offsetY = 40,
      .dc = 33,
      .rst = 32,
      .cs = 15,
      .spiFrecuency = 8000000};
  screen_init(&screen);
#endif
}

void loop()
{
  color16_t color;
  color.color16 = 0x0880;

  for (uint8_t x = 0; x < BUFFER_SIZE_X; x++)
  {
    for (uint8_t y = 0; y < BUFFER_SIZE_Y; y++)
    {
      color = rgbToColor(0, y * 2, 255 - y * 2);
      screen_drawPixel(x, y, color);
    }
  }

  screen_refresh(&screen);
  delay(2000);

  for (uint8_t x = 0; x < BUFFER_SIZE_X; x++)
  {
    for (uint8_t y = 0; y < BUFFER_SIZE_Y; y++)
    {
      color = rgbToColor(255 - y * 2, 255 - y * 2, y * 2);
      screen_drawPixel(x, y, color);
    }
  }

  screen_refresh(&screen);
  delay(2000);
}

color16_t rgbToColor(uint8_t red, uint8_t green, uint8_t blue)
{
  uint8_t r5 = (red * 31) / 255;
  uint8_t g6 = (green * 63) / 255;
  uint8_t b5 = (blue * 31) / 255;

  color16_t color565;
  color565.color16 = ((r5 & 0x1F) << 11) | ((g6 & 0x3F) << 5) | (b5 & 0x1F);
  return color565;
}