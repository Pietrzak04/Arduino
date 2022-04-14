/*
 * #######################################################
 * #                        Safe                         #
 * #    safe made with arduino nano, step engine for     #  
 * #    lock, 16x2 lcd display, 4x4 keypad, buzzer       #
 * #    (alarm) and it stores its password on internal   #
 * #    arduino eeprom.                                  #
 * #                                                     #
 * # created by Piotr Mijakowski                         #
 * # created 12 February 2022                            # 
 * #######################################################
 * 
*/

#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>
#include <Stepper.h>

LiquidCrystal_I2C lcd(0x3F, 16, 2);

enum PinOut
{
  openDetector = 0,
  buzzer,
  col1,
  col2,
  col3,
  col4,
  row1,
  row2,
  row3,
  row4,
  m1,
  m2,
  m3,
  m4
};

enum Screen
{
  end = 0,
  changePassword,
  setAlarm,
  changeAlarm,
  NUMBER_OF_SCREENS
};

void close()
{
  closeSafe();
  lcd.clear();
}

void passwdChange()
{

}

void alarm()
{

}

void alarmChange()
{

}

typedef void (*app)();
app appArray[NUMBER_OF_SCREENS] = 
{
  close,
  passwdChange,
  alarm,
  alarmChange
};

void setup() 
{
  Stepper *motor = new Stepper(32 * 64, m1, m3, m2, m4);
  motor->setSpeed(8);
  motor->step(-1);
  motor->step(1);
  delete motor;

  Serial.begin(9600);
  lcd.init();
  lcd.backlight();

  pinMode(PinOut::openDetector, INPUT_PULLUP);
  pinMode(PinOut::buzzer, OUTPUT);
  pinMode(PinOut::row1, OUTPUT);
  pinMode(PinOut::row2, OUTPUT);
  pinMode(PinOut::row3, OUTPUT);
  pinMode(PinOut::row4, OUTPUT);
  pinMode(PinOut::col1, INPUT_PULLUP);
  pinMode(PinOut::col2, INPUT_PULLUP);
  pinMode(PinOut::col3, INPUT_PULLUP);
  pinMode(PinOut::col4, INPUT_PULLUP);
  pinMode(PinOut::m1, OUTPUT);
  pinMode(PinOut::m2, OUTPUT);
  pinMode(PinOut::m3, OUTPUT);
  pinMode(PinOut::m4, OUTPUT);

  if (digitalRead(PinOut::openDetector) == HIGH) menu();
}

void loop() 
{
  password();
  openSafe();
  menu();
}

void inputPassword(uint8_t length, uint8_t printStartPosition, char *password)
{
  lcd.setCursor(printStartPosition, 1);

  for (uint8_t i = 0; i < length; i++) lcd.print("_");
  
  lcd.setCursor(printStartPosition, 1);
  lcd.blink();

  for (uint8_t i = 0; i < length;)
  {
    char key = getKey();
    
    switch (key)
    {
      case '#':
        if (i == 0) break;

        i--;
        lcd.setCursor(printStartPosition + i, 1);
        lcd.print("_");
        lcd.setCursor(printStartPosition + i, 1);

        break;

      case '*':
        i = 0;

        lcd.setCursor(printStartPosition, 1);
        for (uint8_t i = 0; i < length; i++) lcd.print("_");
        lcd.setCursor(printStartPosition, 1);

        break;
      
      default:
        password[i] = key;
        lcd.print(password[i]);
        i++;
    }
  }

  lcd.noBlink();
}

char* readPassword(uint8_t length)
{
  char *password = new char [length];

  for (uint8_t i = 1; i < length + 1; i++)
  {
    password[i - 1] = EEPROM.read(i);
  }

  return password;
}

bool passwdCheck(char *password, char *savedPassword)
{
  uint8_t passwdLength = getPasswdLength();

  for (uint8_t i = 0; i < passwdLength; i++)
  {
    if (password[i] != savedPassword[i]) return false;
  }

  return true;
}

void password()
{
  uint8_t passwdLength = getPasswdLength();
  uint8_t screenPosition = (16 - passwdLength) >> 1;
  char *password = new char[passwdLength];
  char *savedPassword = readPassword(passwdLength);

  lcd.setCursor(4, 0);
  lcd.print("password");

  do{

    inputPassword(passwdLength, screenPosition, password);

  }while (!passwdCheck(password, savedPassword));

  delete[] password;
  delete[] savedPassword;
  lcd.clear();
}

void openSafe()
{
  Stepper *motor = new Stepper(32 * 64, m1, m3, m2, m4);
  motor->setSpeed(8);
  motor->step(-512);
  delete motor;
}

void closeSafe()
{
  Stepper *motor = new Stepper(32 * 64, m1, m3, m2, m4);
  motor->setSpeed(8);
  motor->step(512);
  delete motor;
}

void menu()
{
  uint8_t screenNumber = 0;

  lcd.setCursor(0, 1);
  lcd.print("<4     |5     6>");

  for(;;)
  {
    lcd.setCursor(0, 0);
    switch (screenNumber)
    {
      case Screen::end:
        lcd.print("      exit      ");
        break;
      case Screen::changePassword:
        lcd.print("change  password");
        break;
      case Screen::setAlarm:
        lcd.print("    set alarm   ");
        break;
      case Screen::changeAlarm:
        lcd.print("  change alarm  ");
        break;
    }

    char key = getKey();

    switch (key)
    {
      case '4':
        if (screenNumber == 0) screenNumber = Screen::NUMBER_OF_SCREENS - 1; else screenNumber--;
        break;
      
      case '5':
        appArray[screenNumber]();
        if (screenNumber == 0) return;
        break;

      case '6':
        if (screenNumber == Screen::NUMBER_OF_SCREENS - 1) screenNumber = 0; else screenNumber++;
        break;
    }
  }
}

uint8_t getPasswdLength()
{
  return EEPROM.read(0x00);
}

char getKey()
{
  while (keyPressed() == '\0') {}
  char key = keyPressed();
  while (keyPressed() != '\0') {}

  return key;
}

char keyPressed()
{
  
  digitalWrite(row1, HIGH);
  digitalWrite(row2, HIGH);
  digitalWrite(row3, HIGH);
  digitalWrite(row4, HIGH);

  digitalWrite(row1, LOW);
  if (digitalRead(col1) == LOW) return '1';
  if (digitalRead(col2) == LOW) return '4';
  if (digitalRead(col3) == LOW) return '7';
  if (digitalRead(col4) == LOW) return '*';
  digitalWrite(row1, HIGH);

  digitalWrite(row2, LOW);
  if (digitalRead(col1) == LOW) return '2';
  if (digitalRead(col2) == LOW) return '5';
  if (digitalRead(col3) == LOW) return '8';
  if (digitalRead(col4) == LOW) return '0';
  digitalWrite(row2, HIGH);

  digitalWrite(row3, LOW);
  if (digitalRead(col1) == LOW) return '3';
  if (digitalRead(col2) == LOW) return '6';
  if (digitalRead(col3) == LOW) return '9';
  if (digitalRead(col4) == LOW) return '#';
  digitalWrite(row3, HIGH);

  digitalWrite(row4, LOW);
  if (digitalRead(col1) == LOW) return 'A';
  if (digitalRead(col2) == LOW) return 'B';
  if (digitalRead(col3) == LOW) return 'C';
  if (digitalRead(col4) == LOW) return 'D';
  digitalWrite(row4, HIGH);

  return '\0';
}
