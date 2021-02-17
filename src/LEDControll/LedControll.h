#include "Adafruit_NeoPixel.h"
#include "GyverTimer/GyverTimer.h"
#include <math.h>

#define LED_COUNT 30
#define LED_PIN 2

void LedSetup();

void LedSetColor(byte rgb[]);

void LedOneSetColor(int indexLed, byte rgb[]);

void LedOff();

void RainbowLED();