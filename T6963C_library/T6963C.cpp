#include "Arduino.h"
#include "T6963C.h"

// initialises the LCD
void T6963C::init()
{
  initInterface();

  LCD_CTRL_PORT &= ~(1 << LCD_RESET);
  delayMicroseconds(2);
  LCD_CTRL_PORT |= (1 << LCD_RESET);

  T6963C::writeCommand(T6963_MODE_SET);

  T6963C::writeData(LCD_GRAPHIC_HOME & 0xFF);
  T6963C::writeData(LCD_GRAPHIC_HOME >> 8);
  T6963C::writeCommand(T6963_SET_GRAPHIC_HOME_ADDRESS);

  T6963C::writeData(LCD_NUMBER_OF_COLUMNS);
  T6963C::writeData(0x00);
  T6963C::writeCommand(T6963_SET_GRAPHIC_AREA);

  T6963C::writeData(LCD_TEXT_HOME & 0xFF);
  T6963C::writeData(LCD_TEXT_HOME >> 8);
  T6963C::writeCommand(T6963_SET_TEXT_HOME_ADDRESS);

  T6963C::writeData(LCD_NUMBER_OF_COLUMNS);
  T6963C::writeData(0x00);
  T6963C::writeCommand(T6963_SET_TEXT_AREA);

  T6963C::writeData(LCD_OFFSET_REGISTER);
  T6963C::writeData(0x00);
  T6963C::writeCommand(T6963_SET_OFFSET_REGISTER);

  T6963C::writeCommand(T6963_DISPLAY_MODE | T6963_GRAPHIC_DISPLAY_ON | T6963_TEXT_DISPLAY_ON);

  T6963C::clearGraphic();
  T6963C::clearText();
  T6963C::clearCharGen();
}

// initialises the interface
void T6963C::initInterface()
{
  LCD_DATA_DDR = OUTPUT_STATE;
  LCD_CTRL_DDR = (_BV(LCD_WR) | _BV(LCD_RD) | _BV(LCD_CE) | _BV(LCD_CD) | _BV( LCD_RESET));
  LCD_CTRL_PORT |= (_BV(LCD_WR) | _BV(LCD_RD) | _BV(LCD_CE) | _BV(LCD_CD) | _BV(LCD_RESET));
}

// check the LCD status
uint8_t T6963C::checkStatus()
{
  uint8_t tmp;
  LCD_DATA_DDR = INPUT_STATE;

  LCD_CTRL_PORT &= ~(_BV(LCD_RD) | _BV(LCD_CE));
  T6963C::shortDelay();
  tmp = LCD_DATA_PIN;

  LCD_DATA_DDR = OUTPUT_STATE;
  LCD_CTRL_PORT |= (_BV(LCD_RD) | _BV(LCD_CE));

  return tmp;
}

// writes a command to LCD
void T6963C::writeCommand(uint8_t command)
{
  while((T6963C::checkStatus() & 0x03) != 0x03);

  LCD_DATA_PORT = command;

  LCD_CTRL_PORT &= ~(_BV(LCD_WR) | _BV(LCD_CE));
  T6963C::shortDelay();
  LCD_CTRL_PORT |= (_BV(LCD_WR) | _BV(LCD_CE));
}

// writes a byte of data to LCD for command
void T6963C::writeData(uint8_t data)
{
  while((T6963C::checkStatus() & 0x03) != 0x03);

  LCD_DATA_PORT = data;

  LCD_CTRL_PORT &= ~(_BV(LCD_WR) | _BV(LCD_CE) | _BV(LCD_CD));
  T6963C::shortDelay();
  LCD_CTRL_PORT |= (_BV(LCD_WR) | _BV(LCD_CE) | _BV(LCD_CD));
}

// reads a byte of data from LCD
uint8_t T6963C::readData()
{
  uint8_t tmp;

  while((T6963C::checkStatus() & 0x03) != 0x03);

  LCD_DATA_DDR = INPUT_STATE;

  LCD_CTRL_PORT &= ~(_BV(LCD_RD) | _BV(LCD_CE) | _BV(LCD_CD));

  T6963C::shortDelay();
  tmp = LCD_DATA_PIN;

  LCD_CTRL_PORT |= (_BV(LCD_RD) | _BV(LCD_CE) | _BV(LCD_CD));
  LCD_DATA_DDR = OUTPUT_STATE;

  return tmp;
}

// sets internal LCD memory pointer
void T6963C::setAddressPointer(uint16_t address)
{
  T6963C::writeData(address & 0xFF);
  T6963C::writeData(address >> 8);
  T6963C::writeCommand(T6963_SET_ADDRESS_POINTER);
}

// writes a byte of data to LCD
void T6963C::writeDisplayData(uint8_t x)
{
  T6963C::writeData(x);
  T6963C::writeCommand(T6963_DATA_WRITE_AND_INCREMENT);
}

// set one pixel to specific color (0 - disabled, 1 - enabled)
void T6963C::setPixel(uint8_t x, uint8_t y, uint8_t color)
{
  if (x > WIDTH || y > HEIGHT) return;

  uint8_t tmp;
  uint8_t num = 0;
  uint16_t address;

  //address = LCD_GRAPHIC_HOME + (x / LCD_FONT_WIDTH) + (LCD_GRAPHIC_SIZE * y);

  if (y < 64)
  {
    address = LCD_GRAPHIC_HOME + (y * LCD_NUMBER_OF_COLUMNS) + (x / 8); 
  }else{
    address = LCD_ADDRESS_OFFSET + LCD_GRAPHIC_HOME + ((y - HEIGHT / 2) * LCD_NUMBER_OF_COLUMNS) + (x / 8); 
  }

  T6963C::setAddressPointer(address);

  T6963C::writeCommand(T6963_DATA_READ_AND_NONVARIABLE);
  tmp = T6963C::readData();

  if(color)
    tmp |= _BV(LCD_FONT_WIDTH - 1 - (x % LCD_FONT_WIDTH));
  else
    tmp &= ~_BV((LCD_FONT_WIDTH - 1 - (x % LCD_FONT_WIDTH)));

  T6963C::writeDisplayData(tmp);
}

//clears LCD graphical memory
void T6963C::clearGraphic()
{
  //upper half clear
  T6963C::setAddressPointer(LCD_GRAPHIC_HOME);
  for(uint16_t i = 0; i < LCD_GRAPHIC_SIZE; i++)
  {
    T6963C::writeDisplayData(0x00);
  }

  //lower hlaf clear
  T6963C::setAddressPointer(LCD_ADDRESS_OFFSET + LCD_GRAPHIC_HOME);
  for(uint16_t i = 0; i < LCD_GRAPHIC_SIZE; i++)
  {
    T6963C::writeDisplayData(0x00);
  }
}

//clears LCD RAM char memory
void T6963C::clearCharGen()
{
  T6963C::setAddressPointer(LCD_CG_HOME + 0x0200);

  for(uint8_t i = 0; i < 128; i++)
  {
    T6963C::writeDisplayData(0x00);
  }
}

//clears LCD text memory
void T6963C::clearText()
{

  //upper half clear
  T6963C::setAddressPointer(LCD_TEXT_HOME);
  for(uint8_t i = 0; i < LCD_TEXT_SIZE; i++)
  {
    T6963C::writeDisplayData(0);
  }

  //lower hlaf clear
  T6963C::setAddressPointer(LCD_TEXT_HOME + LCD_ADDRESS_OFFSET);
  for(uint8_t i = 0; i < LCD_TEXT_SIZE; i++)
  {
    T6963C::writeDisplayData(0);
  }
}

void T6963C::shortDelay()
{
  for(uint8_t i = 0; i < (F_CPU / LCD_DELAY_COUNT); i++)
  {
    asm("nop");
  }
}