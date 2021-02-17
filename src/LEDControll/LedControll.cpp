#include "LEDControll/LedControll.h"

Adafruit_NeoPixel ledStrip = Adafruit_NeoPixel(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
GTimer rainBowTimer(MS, 100);

void LedSetup(){
    ledStrip.begin();

    for (int i = 0; i < LED_COUNT; i++)
    {
        ledStrip.setPixelColor(i, ledStrip.Color(48, 199, 88));
        ledStrip.show();

        delay(10);
    }

    for (int i = LED_COUNT; i >= 0; i--)
    {
        ledStrip.setPixelColor(i, ledStrip.Color(0, 0, 0));
        ledStrip.show();

        delay(10);
    }
}

void LedSetColor(byte rgb[]){
    for (int i = LED_COUNT; i >= 0; i--)
    {
        ledStrip.setPixelColor(i, ledStrip.Color(rgb[0], rgb[1], rgb[2]));
    }

    ledStrip.show(); 
}

void LedOneSetColor(int indexLed, byte rgb[]){
    ledStrip.setPixelColor(indexLed, ledStrip.Color(rgb[0], rgb[1], rgb[2]));

    ledStrip.show();
}

void LedOff(){
    for (int i = 0; i < LED_COUNT; i++)
    {
        ledStrip.setPixelColor(i, ledStrip.Color(0, 0, 0));
    }

    ledStrip.show();
}

uint32_t Wheel(byte WheelPos) {
    WheelPos = 255 - WheelPos;
    if(WheelPos < 85) {
        return ledStrip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
    } else if(WheelPos < 170) {
        WheelPos -= 85;
        return ledStrip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
    } else {
        WheelPos -= 170;
        return ledStrip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
    }
}

void RainbowLED(){
    if(!rainBowTimer.isEnabled()) rainBowTimer.start();
    if(!rainBowTimer.isReady()) return;

    uint16_t i, j;
    
    for(j=0; j<256*5; j++) {
        for(i=0; i< ledStrip.numPixels(); i++) {
            ledStrip.setPixelColor(i, Wheel(((i * 256 / ledStrip.numPixels()) + j) & 255));
        }
        ledStrip.show();
    }
}