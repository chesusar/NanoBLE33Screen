#include <Arduino.h>
#include "screen.h"

screen_t screen;

void setup()
{
  screen = {
      .sizeX = 240,
      .sizeY = 320,
      .dc = 7,
      .rst = 8,
      .cs = 5,
      .spiFrecuency = 8000000};
  screen_init(&screen);
}

void loop()
{
  screen_clearColor(&screen, 0xF800);
  delay(2000);
  screen_clearColor(&screen, 0x0780);
  delay(2000);
}