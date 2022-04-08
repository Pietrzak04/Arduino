/*
 * #######################################################
 * #              A simple snake game                    #
 * #    runs on arduino uno and 128 x 64 graphic lcd     #
 * #                                                     #
 * # created by Piotr Mijakowski                         #
 * # created 23 March 2021                               # 
 * #######################################################
 * 
*/
#include <Arduino.h>
#include <U8g2lib.h>
#include <EEPROM.h>
/*
   Button Map:
        [BUTTON4]
      
  [BUTTON3]    [BUTTON1]
     
        [BUTTON2]
*/
#define BUTTON1 A0
#define BUTTON2 A1
#define BUTTON3 A2
#define BUTTON4 A3
#define BUTTON5 A4

#define SIZE 8
#define MAX_LENGH 128

U8G2_ST7920_128X64_F_8080 u8g2(U8G2_R2, 6, 7, 8, 9, 10, 11, 12, 13, 2, U8X8_PIN_NONE, 4, 5);

struct Vector2
{
  int8_t x;
  int8_t y;
};

Vector2 headPosition = {64,16};
Vector2 snakeDirection = {1,0};
Vector2 fruitPosition;

Vector2 snakePartPosition[MAX_LENGH];

bool gameOver = false;
uint8_t snakeSize = 3;
uint8_t score = 0;
uint8_t inButton = 0;

static const unsigned char fruit[] U8X8_PROGMEM = {
 0x18,0x66,0x81,0x81,0x81,0x81,0x42,0x3c};

void scoreCheck()
{
  uint8_t value = EEPROM.read(0);

  if (value < score)
  {
    EEPROM.update(0, score);
  }
}

Vector2 input()
{
  if (digitalRead(BUTTON2) == LOW)
  {
    if (snakeDirection.x != 0)
    {
      snakeDirection = {0,1};
      inButton++;
    }
  }
  else if (digitalRead(BUTTON1) == LOW)
  {
    if (snakeDirection.y != 0)
    {
      snakeDirection = {1,0};
      inButton++;
    }
  }
  else if (digitalRead(BUTTON4) == LOW)
  {
    if (snakeDirection.x != 0)
    {
      snakeDirection = {0,-1};
      inButton++;
    }
  }
  else if (digitalRead(BUTTON3) == LOW)
  {
    if (snakeDirection.y != 0)
    {
      snakeDirection = {-1,0};
      inButton++;
    }
  }
  return snakeDirection;
}

void drawFruit()
{
   u8g2.drawXBMP( fruitPosition.x, fruitPosition.y, SIZE, SIZE, fruit);
}

Vector2 setFruitPosition()
{
  Vector2 newPosition;
  bool positionTaken = false;
  
  do{
    //positionTaken = false;
    newPosition = getRandomVector();
    newPosition.x = newPosition.x << 3;
    newPosition.y = newPosition.y << 3;

    for (uint8_t i = 1; i < snakeSize; i++)
    {
      if ((snakePartPosition[i].x == newPosition.x) && (snakePartPosition[i].y == newPosition.y))
      {
        positionTaken = true; 
      }
    }
  }
  while(positionTaken);
  
  return newPosition;
}

void checkFruitCollision()
{
  for (uint8_t i = 0; i < snakeSize; i++)
  {
    if ((snakePartPosition[i].x == fruitPosition.x) && (snakePartPosition[i].y == fruitPosition.y))
    {
      snakeSize++;
      score++;
      fruitPosition = setFruitPosition();
    }
  }
}

Vector2 getRandomVector()
{
  Vector2 genPosition;
  genPosition.x = random(16);
  genPosition.y = random(8);
  return genPosition;
}

Vector2 setSnakePsoition()
{
  Vector2 newPosition = getRandomVector();
  newPosition.x = newPosition.x << 3;
  newPosition.y = newPosition.y << 3;

  return newPosition;
}

void edge()
{
  if (headPosition.x == -128) headPosition.x = 0;
  if (headPosition.x == -8) headPosition.x = 120;
  if (headPosition.y == 64) headPosition.y = 0;
  if (headPosition.y == -8) headPosition.y = 64;
}

void drawSnake()
{
  
  for (uint8_t i = snakeSize-1; i > 0; i--)
  {
    snakePartPosition[i] = snakePartPosition[i - 1];
  }
  
  headPosition.x += snakeDirection.x << 3;
  headPosition.y += snakeDirection.y << 3;
  edge();
  snakePartPosition[0] = headPosition;
  
  u8g2.drawBox(headPosition.x, headPosition.y, SIZE, SIZE);

  for (uint8_t i = 1; i < snakeSize; i++)
  {
    checkSnakeCollision(snakePartPosition[i].x, snakePartPosition[i].y);
    u8g2.drawFrame(snakePartPosition[i].x, snakePartPosition[i].y, SIZE, SIZE);
  }
}

void checkSnakeCollision(uint8_t indexX, uint8_t indexY)
{
  if ((headPosition.x == indexX) && (headPosition.y == indexY))
  {
   gameOver = true;
  }
}

void wait(unsigned long time)
{
  unsigned long time_now = millis();
  while(millis() < time_now + time)
  {
     inWaitTime();
  }
}

void inWaitTime()
{
  if (inButton == 0)
  {
    snakeDirection = input();
  }
}

void setup()
{
  u8g2.setBitmapMode(0);
  randomSeed(millis());
  u8g2.begin();
  for(uint8_t i = 14; i < 19; i++)
  {
    pinMode(i, INPUT_PULLUP);
  }
  for (uint8_t i = 0; i <= snakeSize; i++)
  {
    snakePartPosition[i].x = -8;
    snakePartPosition[i].y = -8;
  }
  headPosition = setSnakePsoition();
  fruitPosition = setFruitPosition();
}

void loop() 
{
  if (!gameOver)
  {
    u8g2.clearBuffer();
    drawFruit();
    drawSnake();
    u8g2.sendBuffer();
    u8g2.firstPage();
    checkFruitCollision();
    
  }else{
    scoreCheck();
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_ncenB10_tr);
    u8g2.drawStr(20,36,"Game Over!");
    u8g2.setCursor(32, 16);
    u8g2.print("score = ");
    u8g2.print(score);
    u8g2.setCursor(8, 56);
    u8g2.print("max score = ");
    u8g2.print(EEPROM.read(0));
    u8g2.sendBuffer();
    if (digitalRead(BUTTON5) == LOW)
    {
      gameOver = false;
      snakeSize = 3;
      score = 0;
      for (uint8_t i = 0; i <= snakeSize; i++)
      {
        snakePartPosition[i].x = -8;
        snakePartPosition[i].y = -8;
      }
      headPosition = setSnakePsoition();
      fruitPosition = setFruitPosition();
    }
  }

  wait(200UL);
  inButton = 0;
}
