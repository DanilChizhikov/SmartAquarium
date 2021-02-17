#include <DallasTemperature.h>
#include <OneWire.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <RTClib.h>
#include "GyverButton/GyverButton.h"
#include "LEDControll/LedControll.h"
#include <Arduino.h>

#define TERMO_PIN 3

OneWire oneWire(TERMO_PIN);
DallasTemperature termometr(&oneWire);
LiquidCrystal_I2C display(0x27, 16, 2);
RTC_DS1307 clock;
GButton oneButton(4);
GButton twoButton(6);
GButton threeButton(5);
GTimer informationUpdate(MS, 1000);

enum AquariumState{ Default, Rainbow, Night, ColorSelect};
enum LedState {Red, Green, Blue, Accept};
enum buttonState {Click, Press, Double, Off};

buttonState oneButtonState = Off;
buttonState twoButtonState = Off;
buttonState threeButtonState = Off;

byte RGB[3] = {0, 0, 0};
int oldTemperature;
DateTime nowTime;
uint8_t temp_cel[8] = { B00111, B00101, B00111, B00000, B00000, B00000, B00000 };

AquariumState currentState = Default;
LedState currentLedState = Red;

void DefaultColor(){
  RGB[0] = 255;
  RGB[1] = 255;
  RGB[2] = 88;
  
  LedSetColor(RGB);
}

void LCDSetup(){
  display.init();
  display.backlight();
  display.print("Smart Aquarium");
  display.setCursor(0,1);
  display.print("Version 0.4.4");
  display.createChar(1, temp_cel);
}

void ModuleSetup(){
  termometr.begin();
  Wire.begin();
  clock.begin();
  //clock.adjust(DateTime(__DATE__, __TIME__));
}

int GetTemperature(){
  termometr.requestTemperatures();
  float temperature = termometr.getTempCByIndex(0);

  return round(temperature);
}

void CheckButtons(){
  oneButton.tick();
  twoButton.tick();
  threeButton.tick();

  if(oneButton.isClick()){
    oneButtonState = Click;
  }else if(oneButton.isDouble()){
    oneButtonState = Double;
  }else if(oneButton.isPress()){
    oneButtonState = Press;
  }else oneButtonState = Off;

  if(twoButton.isClick()){
    twoButtonState = Click;
  }else if(twoButton.isDouble()){
    twoButtonState = Double;
  }else if(twoButton.isPress()){
    twoButtonState = Press;
  }else twoButtonState = Off;

  if(threeButton.isClick()){
    threeButtonState = Click;
  }else if(threeButton.isDouble()){
    threeButtonState = Double;
  }else if(threeButton.isPress()){
    threeButtonState = Press;
  }else threeButtonState = Off;
}

void setup() {
  Serial.begin(9600);
  pinMode(A3, INPUT);
  LCDSetup();
  ModuleSetup();
  LedSetup();
}

void TimeControll(){
  nowTime = clock.now();

  if(nowTime.hour() >= 21 && currentState != Default) currentState = Night;
  else if(nowTime.hour()  >= 6 && nowTime.hour() <= 21 && currentState == Night) currentState = Default;
}

void InformationDisplay(){
  if(!informationUpdate.isEnabled()) informationUpdate.start();

  if(!informationUpdate.isReady()) return;

  display.clear();

  display.setCursor(0,0);
  display.print("Dt:");
  display.print(nowTime.day());
  display.print(".");
  display.print(nowTime.month());
  display.print(" Tm:");
  display.print(nowTime.hour());
  display.print(":");
  display.print(nowTime.minute());
  display.setCursor(0, 1);
  display.print("Temp:");
  display.print(GetTemperature());
  display.print("\1C ");

  switch (currentState)
  {
    case Default: display.print("Default");
    break;

    case Night: display.print("Night");
    break;
  }
}

void StateControll(){
  switch (currentState)
  {
    case Default:{
      InformationDisplay();

      switch (oneButtonState)
      {
        case Press: currentState = Rainbow;
        break;

        case Double: DefaultColor();
        break;
      }
      
      LedSetColor(RGB);
    } break;

    case ColorSelect:{
      display.clear();
      display.setCursor(0,0);
      display.print("RED");
      display.setCursor(5,0);
      display.print("GREEN");
      display.setCursor(11,0);
      display.print("BLUE");

      display.setCursor(0,1);
      display.print(RGB[0]);
      display.setCursor(5,1);
      display.print(RGB[1]);
      display.setCursor(11,1);
      display.print(RGB[2]);

      if(threeButtonState == Click){
        switch (currentLedState)
        {
          case Red: currentLedState = Green;
          break;

          case Green: currentLedState = Blue;
          break;

          case Blue: currentLedState = Accept;
          break;

          case Accept:{
            currentLedState = Red;
            currentState = Default;
          }break;
        }
      }

      switch (currentLedState)
      {
        case Red:{
          if(oneButtonState == Click){
            if(RGB[0] > 0) RGB[0]--;
            else RGB[0] = 255;
          } else if (twoButtonState == Click)
          {
            if(RGB[0] < 255) RGB[0]++;
            else RGB[0] = 0;
          }
        }
        break;

        case Green:{
          if(oneButtonState == Click){
            if(RGB[1] > 0) RGB[1]--;
            else RGB[1] = 255;
          } else if (twoButtonState == Click)
          {
            if(RGB[1] < 255) RGB[1]++;
            else RGB[1] = 0;
          }
        }
        break;

        case Blue:{
          if(oneButtonState == Click){
            if(RGB[2] > 0) RGB[2]--;
            else RGB[2] = 255;
          } else if (twoButtonState == Click)
          {
            if(RGB[2] < 255) RGB[2]++;
            else RGB[2] = 0;
          }
        }
        break;
      }

      LedSetColor(RGB);
    } break;

    case Rainbow:{
      if(threeButtonState == Click) currentState = Default;
      InformationDisplay();
      RainbowLED();
    } break;

    case Night: LedOff();
    break;
  }
}

void loop() {
  CheckButtons();
  TimeControll();
  StateControll();
}