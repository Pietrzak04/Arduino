/*
 * #######################################################
 * #                     led driver                      #
 * #        uses arduino uno and color ir remote         #
 * #                                                     #
 * # created by Piotr Mijakowski                         #
 * # created 1 January 2021                              # 
 * #######################################################
 */

#include <IRremote.h>

#define RECEIVER 5
#define ON_OFF_LED 3
#define OUT_PIN_R 9
#define OUT_PIN_G 10
#define OUT_PIN_B 6

enum receiverButtons
{
  brightnessUp = 0xF700FF,
  brightnessDown = 0xF7807F,
  offline = 0xF740BF,
  online = 0xF7C03F,
  red = 0xF720DF,
  green = 0xF7A05F,
  blue = 0xF7609F,
  white = 0xF7E01F,
  lightRed = 0xF110EF,
  lightGreen = 0xF906F,
  lightBlue = 0xF750AF,
  darkOrange = 0xF730CF,
  orange = 0xF708F7,
  yellow = 0xF728D7,
  pink = 0xF76897,
  purple = 0xF748BF,
  darkPurple = 0xF7708F,
  cyan = 0xF78877,
  lightCyan = 0xF7B04F,
  darkCyan = 0xF7A857,

  flash = 0xF7D02F,
  strobe = 0xF7F00F,
  fade = 0xF7C837,
  smooth = 0xF7E817
};

struct RGB {
  byte r;
  byte g;
  byte b;
};

IRrecv irrecv(RECEIVER);
decode_results results;

RGB color = { 0 , 0 , 255 };

bool active = true;
float brightness = 1.0;
uint32_t previous;

void setup()
{
  pinMode(OUT_PIN_R, OUTPUT);
  pinMode(OUT_PIN_G, OUTPUT);
  pinMode(OUT_PIN_B, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(ON_OFF_LED, OUTPUT);
  irrecv.enableIRIn();
  Serial.begin(9600);
}

void loop()
{
  if (irrecv.decode(&results))
  {
    if (results.value == 0xFFFFFFFF)
    {
      results.value = previous;
    }

    switch (results.value) //on/off switch
    {
      case online:
        active = true;
        digitalWrite(ON_OFF_LED, LOW);
        break;
      case offline:
        active = false;
        digitalWrite(ON_OFF_LED, HIGH);
        break;
    }

    if (active)
    {
      switch (results.value) //other switch
      {
        case brightnessUp:
          if (brightness < 1)
          {
            brightness += 0.05;
          }
          break;
        case brightnessDown:
          if (brightness > 0)
          {
            brightness -= 0.05;
          }
          break;
        case red:
          color = { 255 , 0 , 0 };
          break;
        case green:
          color = { 0 , 255 , 0 };
          break;
        case blue:
          color = { 0 , 0 , 255 };
          break;
        case white:
          color = { 255 , 255 , 255 };
          break;
        case fade:
          fadeing(color);
          break;
        case smooth:
          rainbow();
          break;
        case strobe:
          randomFade();
          break;
        case flash:
          flashing();
          break;

      }
    }
    irrecv.resume();
  }

  if (brightness == 1)
  {
    digitalWrite(LED_BUILTIN, HIGH);
  }
  else
  {
    digitalWrite(LED_BUILTIN, LOW);
  }

  if (active)
  {
    analogWrite(OUT_PIN_R, color.r * brightness);
    analogWrite(OUT_PIN_G, color.g * brightness);
    analogWrite(OUT_PIN_B, color.b * brightness);
  } else {
    analogWrite(OUT_PIN_R, 0);
    analogWrite(OUT_PIN_G, 0);
    analogWrite(OUT_PIN_B, 0);
  }

  previous = results.value;
  delay(200);
}

bool quit()
{
  if (irrecv.decode(&results))
  {
    if (results.value == offline)
    {
      return true;
    }
    irrecv.resume();
  }
  return false;
}

void randomFade()
{
  unsigned long time = 0;
  unsigned long timeDifference = 0;
  unsigned long timeLast = 0;
  
  RGB fadeColor = {random(255), random(255), random(255)};
  
  while(true)
  {
    for (float i = 1; i > 0; i -= 0.01)
    {
      analogWrite(OUT_PIN_R, fadeColor.r * i);
      analogWrite(OUT_PIN_G, fadeColor.g * i);
      analogWrite(OUT_PIN_B, fadeColor.b * i);
      while (true)
      {
        time = millis();
        timeDifference = time - timeLast;
        if (quit() == true)
        {
          return;
        }
        if (timeDifference >= 20UL)
        {
          timeLast = time;
          break;
        }
      }
    }

    fadeColor = {random(100, 255), random(100, 255), random(100, 255)};
    
    for (float i = 0; i < 1; i += 0.01)
    {
      analogWrite(OUT_PIN_R, fadeColor.r * i);
      analogWrite(OUT_PIN_G, fadeColor.g * i);
      analogWrite(OUT_PIN_B, fadeColor.b * i);
      while (true)
      {
        time = millis();
        timeDifference = time - timeLast;
        if (quit() == true)
        {
          return;
        }
        if (timeDifference >= 20UL)
        {
          timeLast = time;
          break;
        }
      }
    }
  }
}

void flashing()
{
  unsigned long time = 0;
  unsigned long timeDifference = 0;
  unsigned long timeLast = 0;
  
  RGB f1 = {255, 0 ,0};
  RGB f2 = {0, 255, 0};
  RGB f3 = {0, 0, 255};
  RGB f4 = {255, 0, 255};
  RGB f5 = {0, 255, 255};
  RGB f6 = {255, 255, 0};
  
  RGB flashColor;
  while(true)
  {
    for (byte i = 1; i <= 6; i++)
    {
      switch(i)
      {
        case 1:
          flashColor = f1;
          break;
        case 2:
          flashColor = f2;
          break;
        case 3:
          flashColor = f3;
          break;
        case 4:
          flashColor = f4;
          break;
        case 5:
          flashColor = f5;
          break;
        case 6:
          flashColor = f6;
          break;
      }
      analogWrite(OUT_PIN_R, flashColor.r); 
      analogWrite(OUT_PIN_G, flashColor.g); 
      analogWrite(OUT_PIN_B, flashColor.b); 
      while (true)
      {
        time = millis();
        timeDifference = time - timeLast;
        if (quit() == true)
        {
          return;
        }
        if (timeDifference >= 1000UL)
        {
          timeLast = time;
          break;
        }
      }
    }
  }
}

void rainbow()
{
  unsigned long time = 0;
  unsigned long timeDifference = 0;
  unsigned long timeLast = 0;
  while (true)
  {
    for (int i = 0; i <= 255; i++)
    {
      analogWrite(OUT_PIN_R, i); 
      analogWrite(OUT_PIN_G, 255 - i); 
      analogWrite(OUT_PIN_B, 0); 
      while (true)
      {
        time = millis();
        timeDifference = time - timeLast;
        if (quit() == true)
        {
          return;
        }
        if (timeDifference >= 20UL)
        {
          timeLast = time;
          break;
        }
      }
    }
    for (int i = 0; i <= 255; i++)
    {
      analogWrite(OUT_PIN_R, 255 - i); 
      analogWrite(OUT_PIN_G, 0); 
      analogWrite(OUT_PIN_B, i); 
      while (true)
      {
        time = millis();
        timeDifference = time - timeLast;
        if (quit() == true)
        {
          return;
        }
        if (timeDifference >= 20UL)
        {
          timeLast = time;
          break;
        }
      }
    }
    for (int i = 0; i <= 255; i++)
    {
      analogWrite(OUT_PIN_R, 0); 
      analogWrite(OUT_PIN_G, i); 
      analogWrite(OUT_PIN_B, 255 - i); 
      while (true)
      {
        time = millis();
        timeDifference = time - timeLast;
        if (quit() == true)
        {
          return;
        }
        if (timeDifference >= 20UL)
        {
          timeLast = time;
          break;
        }
      }
    }
  }
}

void fadeing(struct RGB fadeColor)
{
  fadeColor.r *= brightness;
  fadeColor.g *= brightness;
  fadeColor.b *= brightness;
  unsigned long time = 0;
  unsigned long timeDifference = 0;
  unsigned long timeLast = 0;

  while (true)
  {
    for (float i = 1; i > 0; i -= 0.01)
    {
      analogWrite(OUT_PIN_R, fadeColor.r * i);
      analogWrite(OUT_PIN_G, fadeColor.g * i);
      analogWrite(OUT_PIN_B, fadeColor.b * i);
      while (true)
      {
        time = millis();
        timeDifference = time - timeLast;
        if (quit() == true)
        {
          return;
        }
        if (timeDifference >= 20UL)
        {
          timeLast = time;
          break;
        }
      }
    }
    for (float i = 0; i < 1; i += 0.01)
    {
      analogWrite(OUT_PIN_R, fadeColor.r * i);
      analogWrite(OUT_PIN_G, fadeColor.g * i);
      analogWrite(OUT_PIN_B, fadeColor.b * i);
      while (true)
      {
        time = millis();
        timeDifference = time - timeLast;
        if (quit() == true)
        {
          return;
        }
        if (timeDifference >= 20UL)
        {
          timeLast = time;
          break;
        }
      }
    }
  }
}
