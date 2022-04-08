/*
 * #######################################################
 * #                    Digital clock                    #
 * #    clock uses arduino uno RTC clock ds3231 and      #
 * #    st7735 1.44 TFT display. This version looks      #
 * #    better and uses quadrangles instead of lines     #
 * #                                                     #
 * # created by Piotr Mijakowski                         #
 * # created 29 July 2021                                # 
 * #######################################################
 * 
*/

#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>
#include <DS3231.h>
#include <Wire.h>

#define TFT_CS        10
#define TFT_RST        9
#define TFT_DC         8

DS3231 clock;
bool h12Flag;
bool pmFlag;
bool century = false;

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

struct time 
{
  uint8_t day;
  uint8_t month;
  uint16_t year;
  uint8_t hour;
  uint8_t minute;
  uint8_t second;
};

time date;
float radian;
uint8_t radius;

uint8_t secondHandPoints[8];
uint8_t minuteHandPoints[8];
uint8_t hourHandPoints[8];

unsigned long timeDifference;
unsigned long timeLast;

void setup()
{
  Wire.begin();
  tft.initR(INITR_144GREENTAB);
  tft.fillScreen(ST77XX_BLACK);
  tft.drawCircle(64, 64, 63, ST77XX_GREEN);
}

void loop()
{

  /* screenX = cartX + screen_width/2
  screenY = screen_height/2 - cartY */

  //set current values while waiting 1 second
  while(true)
  {
    timeDifference = millis() - timeLast;
    
    date.day = clock.getDate();
    date.month = clock.getMonth(century);
    date.year = clock.getYear();
    date.hour = clock.getHour(h12Flag, pmFlag);
    date.minute = clock.getMinute();
    date.second = clock.getSecond();
    
    if (timeDifference >= 1000UL)
    {
      timeLast = millis();
      break;
    }
  }
  
  //draw second hand of clock
  uint16_t secondAngle = 6 * date.second;

  tft.drawQuadrangle(secondHandPoints[0], secondHandPoints[1], secondHandPoints[2], secondHandPoints[3], secondHandPoints[4], secondHandPoints[5], secondHandPoints[6], secondHandPoints[7], ST77XX_BLACK);
  
  for (uint8_t i = 0; i < 8; i += 2)
  {
    if (0 == i)
    {
      radius = 56;
    }else{
      radius = 10;
    }
    radian = ((secondAngle + (i/2 * 90)) * 71.0) / 4068.0;
    secondHandPoints[i] = (sin(radian) * radius) + 64.0;
    secondHandPoints[i + 1] = 64 - (cos(radian) * radius);
  }

  /*secondHandPoints[0] = (sin(radian) * 56.0) + 64.0; 
  secondHandPoints[1] = 64 - (cos(radian) * 56.0);
  
  secondHandPoints[2] = (sin(radian + (PI/2)) * 56.0) + 64.0;
  secondHandPoints[3] = 64 - (cos(radian + (PI/2)) * 56.0);
  
  secondHandPoints[4] = (sin(radian + PI) * 56.0) + 64.0;
  secondHandPoints[5] = 64 - (cos(radian + (PI)) * 56.0);
  
  secondHandPoints[6] = (sin(radian + (1.5 * PI)) * 56.0) + 64.0;
  secondHandPoints[7] = 64 - (cos(radian + (1.5 * PI)) * 56.0);*/
 
  tft.drawQuadrangle(secondHandPoints[0], secondHandPoints[1], secondHandPoints[2], secondHandPoints[3], secondHandPoints[4], secondHandPoints[5], secondHandPoints[6], secondHandPoints[7], ST77XX_RED);
  
  //draw minute hand of clock
  uint16_t minuteAngle = 6 * date.minute;
  
  tft.drawQuadrangle(minuteHandPoints[0], minuteHandPoints[1], minuteHandPoints[2], minuteHandPoints[3], minuteHandPoints[4], minuteHandPoints[5], minuteHandPoints[6], minuteHandPoints[7], ST77XX_BLACK);

  for (uint8_t i = 0; i < 8; i += 2)
  {
    if (0 == i)
    {
      radius = 48;
    }else{
      radius = 7;
    }
    radian = ((minuteAngle + (i/2 * 90)) * 71.0) / 4068.0;
    minuteHandPoints[i] = (sin(radian) * radius) + 64.0;
    minuteHandPoints[i + 1] = 64 - (cos(radian) * radius);
  }

  tft.drawQuadrangle(minuteHandPoints[0], minuteHandPoints[1], minuteHandPoints[2], minuteHandPoints[3], minuteHandPoints[4], minuteHandPoints[5], minuteHandPoints[6], minuteHandPoints[7], ST77XX_YELLOW);

  //draw hour hand of clock
  float minuteRatio = date.minute / 60.0;
  uint16_t hourAngle = (30 * date.hour) + (minuteRatio * 30.0);

  tft.drawQuadrangle(hourHandPoints[0], hourHandPoints[1], hourHandPoints[2], hourHandPoints[3], hourHandPoints[4], hourHandPoints[5], hourHandPoints[6], hourHandPoints[7], ST77XX_BLACK);
  
  for (uint8_t i = 0; i < 8; i += 2)
  {
    if (0 == i)
    {
      radius = 36;
    }else{
      radius = 5;
    }
    radian = ((hourAngle + (i/2 * 90)) * 71.0) / 4068.0;
    hourHandPoints[i] = (sin(radian) * radius) + 64.0;
    hourHandPoints[i + 1] = 64 - (cos(radian) * radius);
  }

  tft.drawQuadrangle(hourHandPoints[0], hourHandPoints[1], hourHandPoints[2], hourHandPoints[3], hourHandPoints[4], hourHandPoints[5], hourHandPoints[6], hourHandPoints[7], ST77XX_BLUE);
  
  //draw clock center
  tft.drawPixel(63, 63, ST77XX_GREEN);

  //draw clock division

  for (uint8_t i = 0; i < 12; i++)
  {
    uint8_t x0, x1, y0, y1;
    radian = (i * 30 * 71.0) / 4068.0;
    x0 = (sin(radian) * 56) + 64.0;
    y0 = 64 - (cos(radian) * 56);
    x1 = (sin(radian) * 62) + 64.0;
    y1 = 64 - (cos(radian) * 62);

    tft.drawLine(x0, y0, x1, y1, ST77XX_GREEN);
  }
}
