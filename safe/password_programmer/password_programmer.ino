/*
 * #######################################################
 * #                Password programer                   #
 * #    Program to set password for the first time       #
 * #                                                     #
 * # created by Piotr Mijakowski                         #
 * # created 13 February 2022                            # 
 * #######################################################
 * 
*/

#include <EEPROM.h>

//password written to eeprom initially
char password[] = {'1','2','3','4'};
uint8_t passwdLenght = 4;

void setup() 
{
  Serial.begin(9600);
  
  EEPROM.update(0x00, passwdLenght);

  for (uint8_t i = 1; i < passwdLenght + 1; i++)
  {
    EEPROM.update(i, password[i - 1]);
  }

  for (uint8_t i = 0; i < passwdLenght + 1; i++)
  {
    Serial.println(EEPROM.read(i));
  }
}

void loop() {}
