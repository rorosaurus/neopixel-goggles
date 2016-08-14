// 0 is right eye top
// 16 is left eye top
// inc clockwise looking out

#include <Adafruit_NeoPixel.h>

#ifdef __AVR_ATtiny85__ // Trinket, Gemma, etc.
#include <avr/power.h>
#endif

#define LED_PIN 0
#define LED_LENGTH 32
#define BUTTON1_PIN 1
#define BUTTON2_PIN 2

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(LED_LENGTH, LED_PIN);

bool button2OldState = HIGH;
bool button1OldState = HIGH;
int degree = 0;

int smileyLEDS[] = {1, 15, 5, 6, 7, 8, 9, 10, 11, 17, 31, 21, 22, 23, 24, 25, 26, 27};

void setup() {
#ifdef __AVR_ATtiny85__ // Trinket, Gemma, etc.
  if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
#endif
  pinMode(BUTTON1_PIN, INPUT_PULLUP);
  pinMode(BUTTON2_PIN, INPUT_PULLUP);
  pixels.begin();
  pixels.setBrightness(85); // 1/3 brightness
}

void loop() {
  // Get current button state.
  bool button1NewState = digitalRead(BUTTON1_PIN);
  bool button2NewState = digitalRead(BUTTON2_PIN);
  
  // Check if state changed from high to low (button press).
  if (button1NewState == LOW) {
    uint32_t color = getRGBfromHue(degree);

    for (int i = 0; i < 18; i++) {
      pixels.setPixelColor(smileyLEDS[i], color);
    }

    pixels.show();
  }
  else if (button2NewState == LOW){
    uint32_t color = 0xFFFF00;

    for (int i = 0; i < 18; i++) {
      pixels.setPixelColor(smileyLEDS[i], color);
    }

    pixels.show();
  }
  else {
    setAllLEDS(0x000000);
  }

  // rotate through our rainbow always
  degree++;
  if (degree >= 360) degree = 0;
  
  delay(10);
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
