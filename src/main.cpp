#include <Arduino.h>
#include "screen.h"
#include "image.h"

// #define USING_ARDUINO_NANO
#define USING_ESP32

typedef enum state_t
{
  STATE_IMG_1,
  STATE_IMG_2
} state_t;

screen_t screen;

color16_t rgbToColor(uint8_t red, uint8_t green, uint8_t blue);
void screenLoop();

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
      .blk = -1,
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
      .blk = 25,
      .spiFrecuency = 24000000};
  screen_init(&screen);
#endif
  Serial.begin(9600);
}

void loop()
{
  screenLoop();
  delay(1);
}

void screenLoop()
{
  const uint32_t STATE_MAX_TIME = 5000; // 5 s

  static uint32_t timeOld = 0;
  uint32_t timeNew = millis();
  uint32_t timeElapsed = timeNew - timeOld;
  timeOld = timeNew;

  static state_t currentState = STATE_IMG_1;
  static uint32_t stateTime = 0;
  stateTime += timeElapsed;

  screen_setBrightness(&screen, 40);
  screen_clearBuffer();
  if (currentState == STATE_IMG_1)
  {
    screen_drawImage(0, 0, 135, 240, img2);
    if (stateTime >= STATE_MAX_TIME)
    {
      currentState = STATE_IMG_2;
      stateTime = 0;
    }
  }
  else if (currentState == STATE_IMG_2)
  {
    screen_drawImage(0, 0, 135, 240, img3);
    if (stateTime >= STATE_MAX_TIME)
    {
      currentState = STATE_IMG_1;
      stateTime = 0;
    }
  }
  screen_refresh(&screen);
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