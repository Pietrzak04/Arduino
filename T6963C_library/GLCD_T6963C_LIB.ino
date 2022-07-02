#include "T6963C.h"

T6963C lcd;

void setup()
{
  lcd.init();

  for(uint8_t i = 20; i < 70; i++)
  {
    for(uint8_t j = 20; j < 90; j++) lcd.setPixel(i, j, 1);
  }

  lcd.setPixel(100, 110, 1);

}

void loop() {}
