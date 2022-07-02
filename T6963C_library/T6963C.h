#pragma once

#include "Arduino.h" 
#include "settings.h" 

#define OUTPUT_STATE 0xFF
#define INPUT_STATE 0x00

#define LCD_NUMBER_OF_COLUMNS  (WIDTH / LCD_FONT_WIDTH)
#define LCD_GRAPHIC_SIZE			 ((WIDTH / LCD_FONT_WIDTH) * (HEIGHT / 2))
#define LCD_TEXT_SIZE				   ((WIDTH / LCD_FONT_WIDTH) * (HEIGHT / 16))
#define LCD_OFFSET_REGISTER    0x01
#define LCD_ADDRESS_OFFSET     0x8000
#define LCD_DELAY_COUNT        2000000

#define T6963_SET_CURSOR_POINTER     0x21
#define T6963_SET_OFFSET_REGISTER    0x22
#define T6963_SET_ADDRESS_POINTER    0x24

#define T6963_SET_TEXT_HOME_ADDRESS     0x40
#define T6963_SET_TEXT_AREA             0x41
#define T6963_SET_GRAPHIC_HOME_ADDRESS  0x42
#define T6963_SET_GRAPHIC_AREA          0x43

#define T6963_MODE_SET    0x80

#define T6963_DISPLAY_MODE          0x90
#define T6963_CURSOR_BLINK_ON       0x01
#define T6963_CURSOR_DISPLAY_ON     0x02
#define T6963_TEXT_DISPLAY_ON       0x04
#define T6963_GRAPHIC_DISPLAY_ON    0x08				

#define T6963_SET_DATA_AUTO_WRITE     0xB0
#define T6963_SET_DATA_AUTO_READ      0xB1
#define T6963_AUTO_RESET              0xB2

#define T6963_DATA_WRITE_AND_INCREMENT    0xC0
#define T6963_DATA_READ_AND_INCREMENT     0xC1
#define T6963_DATA_WRITE_AND_DECREMENT    0xC2
#define T6963_DATA_READ_AND_DECREMENT     0xC3
#define T6963_DATA_WRITE_AND_NONVARIALBE  0xC4
#define T6963_DATA_READ_AND_NONVARIABLE   0xC5

#define T6963_SCREEN_PEEK    0xE0
#define T6963_SCREEN_COPY    0xE8

class T6963C
{
public:
  void init();
  void clearGraphic();
  void clearText();
  void clearCharGen();
  void setPixel(uint8_t, uint8_t, uint8_t);
private:
  void initInterface();
  uint8_t checkStatus();
  void writeCommand(uint8_t);
  void writeData(uint8_t);
  uint8_t readData();
  void setAddressPointer(uint16_t);
  void writeDisplayData(uint8_t);
  void shortDelay();
};