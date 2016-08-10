// 0 is right eye top
// 16 is left eye top
// inc clockwise looking out

#include <Adafruit_NeoPixel.h>
#ifdef __AVR_ATtiny85__ // Trinket, Gemma, etc.
 #include <avr/power.h>
#endif

#define PIN 0

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(32, PIN);

uint8_t  mode   = 0, // Current animation effect
         offset = 0; // Position of spinny eyes
uint32_t color  = 0xFFE700;
uint32_t prevTime;

void setup() {
#ifdef __AVR_ATtiny85__ // Trinket, Gemma, etc.
  if(F_CPU == 16000000) clock_prescale_set(clock_div_1);
#endif
  pixels.begin();
  pixels.setBrightness(85); // 1/3 brightness
  prevTime = millis();
}

void loop() {
  for (int i=0; i<=255; i++){
    color = Wheel(i);
    pixels.setPixelColor(1, color);
    pixels.setPixelColor(15, color);
  
    pixels.setPixelColor(5, color);
    pixels.setPixelColor(6, color);
    pixels.setPixelColor(7, color);
    pixels.setPixelColor(8, color);
    pixels.setPixelColor(9, color);
    pixels.setPixelColor(10, color);
    pixels.setPixelColor(11, color);
    
    pixels.setPixelColor(17, color);
    pixels.setPixelColor(31, color);
  
    pixels.setPixelColor(21, color);
    pixels.setPixelColor(22, color);
    pixels.setPixelColor(23, color);
    pixels.setPixelColor(24, color);
    pixels.setPixelColor(25, color);
    pixels.setPixelColor(26, color);
    pixels.setPixelColor(27, color);
    
    pixels.show();
  }
}

uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return pixels.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return pixels.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return pixels.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
