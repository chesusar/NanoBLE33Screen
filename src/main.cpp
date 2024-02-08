#include <Arduino.h>
#include "screen.h"
#include "image.h"

// #define USING_ARDUINO_NANO
#define USING_ESP32

typedef enum state_t
{
  STATE_IMG_1,
  STATE_IMG_2,
  STATE_IMG_3
} state_t;

typedef struct ball_t
{
  float velX;
  float velY;
  uint8_t posX;
  uint8_t posY;
} ball_t;

screen_t screen;

#define BALLS_NUM 3

ball_t balls[BALLS_NUM];

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
      .dc = A7,
      .rst = A6,
      .cs = 7,
      .blk = 9,
      .spiFrecuency = 24000000};
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

  ball_t ball1 = {
      .velX = -3,
      .velY = 3,
      .posX = BUFFER_SIZE_X / 2 - 12,
      .posY = BUFFER_SIZE_Y / 2 + 12};
  ball_t ball2 = {
      .velX = 3,
      .velY = 3,
      .posX = 80,
      .posY = 150};
  ball_t ball3 = {
      .velX = 3,
      .velY = 3,
      .posX = 40,
      .posY = 50};

  balls[0] = ball1;
  balls[1] = ball2;
  balls[2] = ball3;
}

void loop()
{
  screenLoop();
  delay(1);
}

void screenLoop()
{
  static uint32_t timeOld = 0;
  uint32_t timeNew = millis();
  uint32_t timeElapsed = timeNew - timeOld;
  timeOld = timeNew;

  screen_setBrightness(&screen, 100);
  screen_drawImage(0, 0, 135, 240, imgBackground);

  // static float gravityX = 0.0;
  // static float gravityY = 0.1;

  for (int i = 0; i < BALLS_NUM; i++)
  {

    // balls[i].velX += gravityX;
    // balls[i].velY += gravityY;

    balls[i].posX += balls[i].velX;
    balls[i].posY += balls[i].velY;

    if (balls[i].posX >= BUFFER_SIZE_X - 40)
    {
      balls[i].posX = BUFFER_SIZE_X - 40;
      balls[i].velX = -balls[i].velX;
    }
    else if (balls[i].posX <= 20)
    {
      balls[i].posX = 20;
      balls[i].velX = -balls[i].velX;
    }
    if (balls[i].posY >= BUFFER_SIZE_Y - 40)
    {
      balls[i].posY = BUFFER_SIZE_Y - 40;
      balls[i].velY = -balls[i].velY;
    }
    else if (balls[i].posY <= 20)
    {
      balls[i].posY = 20;
      balls[i].velY = -balls[i].velY;
    }

    // Collision
    for (int j = 0; j < BALLS_NUM; j++)
    {
      if (i == j)
        continue;
      int dist2 = ((int)balls[i].posX - (int)balls[j].posX) *
                      ((int)balls[i].posX - (int)balls[j].posX) +
                  ((int)balls[i].posY - (int)balls[j].posY) *
                      ((int)balls[i].posY - (int)balls[j].posY);
      if (dist2 <= 200)
      {
        balls[i].velX = -balls[i].velX;
        balls[i].velY = -balls[i].velY;
      }
    }

    color16_t alphaColor;
    alphaColor.color16 = 2016;
    screen_drawAlphaImage(balls[i].posX, balls[i].posY, 24, 24, imgBall, alphaColor);
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