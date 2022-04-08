/*
 * #######################################################
 * #                    Digital clock                    #
 * #    clock uses arduino uno RTC clock ds3231 and      #
 * #              st7735 1.44 TFT display                #
 * #                                                     #
 * # created by Piotr Mijakowski                         #
 * # created 26 July 2021                                # 
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

struct radian
{
  float second;
  float minute;
  float hour;
};

time date;
radian radian;



void setup()
{
  Serial.begin(9600);
  Wire.begin();
  tft.initR(INITR_144GREENTAB);
  tft.fillScreen(ST77XX_BLACK);
  //tft.drawQuadrangle(16,32, 53,48, 37,87, 128,128, ST77XX_GREEN);
  //delay(5000);
  
  tft.drawCircle(64, 64, 63, ST77XX_GREEN);
}

void loop()
{

  /* screenX = cartX + screen_width/2
  screenY = screen_height/2 - cartY */

  //set current values
  date.day = clock.getDate();
  date.month = clock.getMonth(century);
  date.year = clock.getYear();
  date.hour = clock.getHour(h12Flag, pmFlag);
  date.minute = clock.getMinute();
  date.second = clock.getSecond();
  
  //draw second hand of clock
  uint16_t secondAngle = 6 * date.second;
  tft.drawLine(64,64, (sin(radian.second) * 63.0) + 64.0, 64.0 - (cos(radian.second) * 63.0), ST77XX_BLACK);
  radian.second = (secondAngle * 71.0) / 4068.0;
  tft.drawLine(64,64, (sin(radian.second) * 63.0) + 64.0, 64.0 - (cos(radian.second) * 63.0), ST77XX_RED);

  //draw minute hand of clock
  uint16_t minuteAngle = 6 * date.minute;
  tft.drawLine(64,64, (sin(radian.minute) * 50.0) + 64.0, 64.0 - (cos(radian.minute) * 50.0), ST77XX_BLACK);
  radian.minute = (minuteAngle * 71.0) / 4068.0;
  tft.drawLine(64,64, (sin(radian.minute) * 50.0) + 64.0, 64.0 - (cos(radian.minute) * 50.0), ST77XX_YELLOW);

  //draw hour hand of clock
  float minuteRatio = date.minute / 60.0;
  uint16_t hourAngle = (30 * date.hour) + (minuteRatio * 30.0);
  
  tft.drawLine(64,64, (sin(radian.hour) * 40.0) + 64.0, 64.0 - (cos(radian.hour) * 40.0), ST77XX_BLACK);
  radian.hour = (hourAngle * 71.0) / 4068.0;
  tft.drawLine(64,64, (sin(radian.hour) * 40.0) + 64.0, 64.0 - (cos(radian.hour) * 40.0), ST77XX_BLUE);
  
  //draw clock circle
  tft.drawCircle(64, 64, 63, ST77XX_GREEN);

  delay(1000);
  
  
  
  
  //for (float i = 0; i <= 360.0; i += 6)
 // {
  //  radian = (i * 71.0) / 4068.0;
   // tft.drawLine(64,64, (sin(radian) * 63.0) + 64.0, 64.0 - (cos(radian) * 63.0), ST77XX_RED);
   // delay(500);
   // tft.drawLine(64,64, (sin(radian) * 63.0) + 64.0, 64.0 - (cos(radian) * 63.0), ST77XX_BLACK);
 // }
  
}
