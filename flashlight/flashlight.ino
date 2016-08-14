// 0 is right eye top
// 16 is left eye top
// inc clockwise looking out

#include <Adafruit_NeoPixel.h>

#ifdef __AVR_ATtiny85__ // Trinket, Gemma, etc.
#include <avr/power.h>
#endif

#define LED_PIN 0
#define LED_LENGTH 32

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(LED_LENGTH, LED_PIN);

void setup() {
#ifdef __AVR_ATtiny85__ // Trinket, Gemma, etc.
  if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
#endif
  pixels.begin();
  pixels.setBrightness(255); // 1/3 brightness
}

void loop() {
  setAllLEDS(0xFFFFFF);
  delay(100);
}

void setAllLEDS(uint32_t color) {
  for (int i = 0; i < LED_LENGTH; i++) {
      pixels.setPixelColor(i, color);
    }

    pixels.show();
}

// takes int 0-360 and translates from HSV to RGB
// I basically implemented wikipedia: https://en.wikipedia.org/wiki/HSL_and_HSV#/media/File:HSV-RGB-comparison.svg
uint32_t getRGBfromHue(int hue) {
  double r, g, b = 0;
  int i = (int)floor(hue / 60.0); // which section we're in
  double f = hue / 60.0 - i; // how far we are through this section

  switch (i) {
    case 0:
      r = 1;
      g = f;
      b = 0;
      break;
    case 1:
      r = 1 - f;
      g = 1;
      b = 0;
      break;
    case 2:
      r = 0;
      g = 1;
      b = f;
      break;
    case 3:
      r = 0;
      g = 1 - f;
      b = 1;
      break;
    case 4:
      r = f;
      g = 0;
      b = 1;
      break;
    case 5:
      r = 1;
      g = 0;
      b = 1 - f;
      break;
  }
  
  return pixels.Color(r*255, g*255, b*255);
}
