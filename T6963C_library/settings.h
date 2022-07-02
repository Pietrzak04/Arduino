#pragma once

// arduino port for data transmition
#define LCD_DATA_PORT   PORTA
#define LCD_DATA_PIN    PINA
#define LCD_DATA_DDR    DDRA

// arduino port for control
#define LCD_CTRL_PORT   PORTC
#define LCD_CTRL_PIN    PINC
#define LCD_CTRL_DDR    DDRC

// control port pins
#define LCD_WR          PC0
#define LCD_RD          PC1
#define LCD_CE          PC2
#define LCD_CD          PC3
#define LCD_RESET       PC4

// memory map
#define LCD_GRAPHIC_HOME     0x0000
#define LCD_TEXT_HOME        0x0A00
#define LCD_CG_HOME          0x0C00

/*
  * -------------------------------
  *   graphic memory   |  2048 b
  * -------------------------------
  *   attribute memory |   512 b
  * -------------------------------
  *   text memory      |   512 b
  * -------------------------------
  *   CG memory        |  1024 b
  * -------------------------------

   2nd screen have the same adresses with increased by 0x8000

   *note
   adress 0x8C00 - 0x8FFF are not used

*/

//screen dimensions
#define WIDTH   160
#define HEIGHT  128

// one character width
#define LCD_FONT_WIDTH  8
