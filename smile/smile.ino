// 0 is right eye top
// 16 is left eye top
// inc clockwise looking out

#include <Adafruit_NeoPixel.h>
#ifdef __AVR_ATtiny85__ // Trinket, Gemma, etc.
 #include <avr/power.h>
#endif

#define PIN 0

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(32, PIN);

uint32_t color  = 0x000000;

void setup() {
#ifdef __AVR_ATtiny85__ // Trinket, Gemma, etc.
  if(F_CPU == 16000000) clock_prescale_set(clock_div_1);
#endif
  pixels.begin();
  pixels.setBrightness(85); // 1/3 brightness
}

void loop() {
  for (int i=0; i<360; i++){
    color = getRGBfromHue(i);
    
    // :
    pixels.setPixelColor(1, color);
    pixels.setPixelColor(15, color);

    // )
    pixels.setPixelColor(5, color);
    pixels.setPixelColor(6, color);
    pixels.setPixelColor(7, color);
    pixels.setPixelColor(8, color);
    pixels.setPixelColor(9, color);
    pixels.setPixelColor(10, color);
    pixels.setPixelColor(11, color);

    // :
    pixels.setPixelColor(17, color);
    pixels.setPixelColor(31, color);

    // )
    pixels.setPixelColor(21, color);
    pixels.setPixelColor(22, color);
    pixels.setPixelColor(23, color);
    pixels.setPixelColor(24, color);
    pixels.setPixelColor(25, color);
    pixels.setPixelColor(26, color);
    pixels.setPixelColor(27, color);
    
    pixels.show();

    delay(50);
  }
}

// takes int 0-360 and translates from HSV to RGB
// I basically implemented wikipedia: https://en.wikipedia.org/wiki/HSL_and_HSV#/media/File:HSV-RGB-comparison.svg
uint32_t getRGBfromHue(int hue) {
  double r, g, b = 0;
  int i=(int)floor(hue/60.0); // which section we're in
  double f = hue/60.0 - i; // how far we are through this section

  switch (i) {
    case 0:
      r = 1;
      g = f;
      b = 0;
      break;
    case 1:
      r = 1-f;
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
      g = 1-f;
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
      b = 1-f;
      break;
  }

  //set each component to a integer value between 0 and 255
  int red = constrain((int)255*r,0,255);
  int green = constrain((int)255*g,0,255);
  int blue = constrain((int)255*b,0,255);

  return pixels.Color(red, green, blue);
}
