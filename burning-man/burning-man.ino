// 0 is right eye top (between star and data out)
// 16 is left eye top (between star and data out)
// inc clockwise looking out

#include <Adafruit_NeoPixel.h>

#ifdef __AVR_ATtiny85__ // Trinket, Gemma, etc.
#include <avr/power.h>
#endif

#define LED_PIN 0
#define LED_LENGTH 32
#define BUTTON1_PIN 3
#define BUTTON2_PIN 2

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(LED_LENGTH, LED_PIN);

bool button1OldState = HIGH;
bool button2OldState = HIGH;

int hue = 0;

bool inBaseMode = true;
int baseMode = 0;
int utilMode = 0;

void setup() {
#ifdef __AVR_ATtiny85__ // Trinket, Gemma, etc.
  if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
#endif
  pinMode(BUTTON1_PIN, INPUT_PULLUP);
  pinMode(BUTTON2_PIN, INPUT_PULLUP);
  pixels.begin();
  pixels.setBrightness(85); // 1/3 brightness
  randomSeed(millis());
}

void loop() {
  // Initialize all LEDs to off
  setAllPixels(0x000000);

  // Get current button state.
  bool button1NewState = digitalRead(BUTTON1_PIN);
  bool button2NewState = digitalRead(BUTTON2_PIN);

  // State management
  // Check if state changed from high to low (button press) to control util mode
  if (button1NewState == LOW && button1OldState == HIGH) {
    if (inBaseMode) { // switch to utility mode
      inBaseMode = false;
      utilMode = 0;
    }
    else {
      utilMode++;
      if (utilMode >= 3) utilMode = 0;
    }
    // 0 = yellow smiley
    // 1 = winking smiley
    // 2 = flashlight
    //setSmileyPixels(getRainbow());
  }
  // Control base mode
  else if (button2NewState == LOW && button2OldState == HIGH) {
    if (!inBaseMode) {
      inBaseMode = true;
      baseMode = random(5);
    }
    else {
      baseMode++;
      if (baseMode >= 5) baseMode = 0;
    }
    // 0 = rainbow cycle
    // 1 = random flashes
    // 2 = infinity cycle
    // 3 = ????
    // 4 = ????
    //setSmileyPixels(0xFFFF00);
  }
  else {
    setAllPixels(getRainbow());
  }

  // Update the LEDs
  pixels.show();

  // Update button state variables
  button1OldState = button1NewState;
  button2OldState = button2NewState;

  // Rotate through our rainbow always
  hue++;
  if (hue >= 360) hue = 0;

  delay(10);
}

void setSmileyPixels(uint32_t color) {
  int smileyLEDS[] = {1, 15, 5, 6, 7, 8, 9, 10, 11, 17, 31, 21, 22, 23, 24, 25, 26, 27};
  for (int i = 0; i < 18; i++) {
    pixels.setPixelColor(smileyLEDS[i], color);
  }
}

void setAllPixels(uint32_t color) {
  for (int i = 0; i < LED_LENGTH; i++) {
    pixels.setPixelColor(i, color);
  }
}

// takes int 0-360 and translates from HSV to RGB
// I basically implemented wikipedia: https://en.wikipedia.org/wiki/HSL_and_HSV#/media/File:HSV-RGB-comparison.svg
uint32_t getRainbow() {
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

  return pixels.Color(r * 255, g * 255, b * 255);
}
