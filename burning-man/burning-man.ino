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

// always cycle through the rainbow
int hue = 0;
// keep track of each mode's progress
int modeCounter = 0;

int mode = 0;

void setup() {
#ifdef __AVR_ATtiny85__ // Trinket, Gemma, etc.
  if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
#endif
  pinMode(BUTTON1_PIN, INPUT_PULLUP);
  pinMode(BUTTON2_PIN, INPUT_PULLUP);
  pixels.begin();
  pixels.setBrightness(85); // 1/3 brightness
  //randomSeed(millis());
}

void loop() {
  // Initialize all LEDs to off and reset brightness
  setAllPixels(0x000000);
  pixels.setBrightness(85); // 1/3 brightness

  // Update current button state
  bool button1NewState = digitalRead(BUTTON1_PIN);
  bool button2NewState = digitalRead(BUTTON2_PIN);

  // State machine management
  if (button1NewState == LOW && button1OldState == HIGH) {
    if (mode >= 0 || mode <= -3) mode = -1;
    else mode--;
  }
  else if (button2NewState == LOW && button2OldState == HIGH) {
    if (mode < 0 || mode >= 2) mode = 0;
    else mode++;
  }

  // Flashy light stuff time!
  switch (mode) {
    // -3: flashlight
    case -3: pixels.setBrightness(255); setAllPixels(0xFFFFFF); break;
    // -2: winky face
    case -2: setSmileyPixel(0x00FF00); break;
    // -1: smiley face
    case -1: setSmileyPixels(0xFFFF00); break;
    // 0: uniform rainbow cycle
    case 0: setAllPixels(getRainbow()); break;
    // 1: opposite rainbow spinners
    case 1: oppositeSpin(getRainbow()); break;
    case 2: oppositeSpin(0xFF00FF); break;
  }

  // Update the LEDs
  pixels.show();

  // Update button state variables
  button1OldState = button1NewState;
  button2OldState = button2NewState;

  // Rotate through our rainbow always
  hue++;
  if (hue > 255) hue = 0;

  delay(10);
}

//if (millis()%1000 > 700) {
//    // do the wink, maybe?
//  }

// each eye rotates two opposite LED patches
void oppositeSpin(uint32_t color) {
  if (modeCounter >= 16) modeCounter = 0;
  for (int i = 0; i < 16; i++) {
    uint32_t c = 0;
    if (((modeCounter + i) & 7) < 2) c = color; // 3 pixels on
    pixels.setPixelColor(   i, c); // First eye
    pixels.setPixelColor(31 - i, c); // Second eye (flipped)
  }
  modeCounter++;
  hue++;
  delay(40);
}

// sets the smiley pixels on both eyes
// todo: try making this a rainbow mix of colors instead of one?
void setSmileyPixels(uint32_t color) {
  int smileyLEDS[] = {1, 15, 5, 6, 7, 8, 9, 10, 11, 17, 31, 21, 22, 23, 24, 25, 26, 27};
  for (int i = 0; i < 18; i++) {
    pixels.setPixelColor(smileyLEDS[i], color);
  }
}

// sets all the LEDs on both eyes
void setAllPixels(uint32_t color) {
  for (int i = 0; i < LED_LENGTH; i++) {
    pixels.setPixelColor(i, color);
  }
}

// todo: try invalid input.. can we save byte somewhere?
// Input a value 0 to 255 to get a color value.
// this literally saves 40% of my ROM space, so... shamelessly reusing the sample code
uint32_t getRainbow(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return pixels.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    return pixels.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return pixels.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
