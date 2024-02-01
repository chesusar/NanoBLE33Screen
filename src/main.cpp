#include <Arduino.h>
#include "screen.h"

screen_t screen;

void setup()
{
  screen = {
    .sizeX = 135,
    .sizeY = 240,
    .dc = 9,
    .rst = 9,
    .cs = 9,
    .spiFrecuency = 100000
  };
  screen_init(&screen);
}

void loop()
{
  uint8_t screenBuffer[128];
  screen_sendBuffer(&screen, screenBuffer, 128);
}