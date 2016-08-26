// 0 is right eye top (between star and data out)
// 16 is left eye top (between star and data out)
// inc clockwise looking out

// apparently you need to keep it under 94% (4,992 bytes) program storage space used or it fails to initialize?

// todo: figure out this uint32_t, etc shit.  can i save more space?

#include <Adafruit_NeoPixel.h>

#ifdef __AVR_ATtiny85__ // Trinket, Gemma, etc.
#include <avr/power.h>
#endif

#define LED_PIN 0
#define LED_LENGTH 32
#define BUTTON1_PIN 3
#define BUTTON2_PIN 2
#define MODE_SWITCH_MILLIS 10000

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(LED_LENGTH, LED_PIN);

int smileyLEDS[] = {1, 15, 5, 6, 7, 8, 9, 10, 11, 17, 31, 21, 22, 23, 24, 25, 26, 27};
int horizontalLEDs[] = { 4,  4,  3,  5,  2,  6,  1,  7,  0,  8, 15,  9, 14, 10, 13, 11, 12, 12,
                         20, 20, 19, 21, 18, 22, 17, 23, 16, 24, 31, 25, 30, 26, 29, 27, 28, 28
                       };
int verticalLEDs[] = { 0,  0, 15,  1, 14,  2, 13,  3, 12,  4, 11,  5, 10,  6,  9,  7,  8,  8,
                       16, 16, 31, 17, 30, 18, 29, 19, 28, 20, 27, 21, 26, 22, 25, 23, 24, 24
                     };
int verticalLEDsProper[] = { 0,  0, 16, 16, 1,  15, 17,  31, 2,  14, 18,  30, 3,  13,  19,  29,  4,  12, 20, 28,
                           5, 11, 21, 27, 6, 10, 22, 26, 7, 9, 22, 26, 8, 8, 23, 25, 24, 24, 24, 24
                         };
int diagonalLEDs[] = { 2,  2,  1,  3,  0,  4, 15,  5, 14,  6, 13,  7, 12,  8, 11,  9, 10, 10,
                       18, 18, 17, 19, 16, 20, 31, 21, 30, 22, 29, 23, 28, 24, 27, 25, 26, 26
                     };

int infinityLEDs[] = { 13, 14, 15, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 20, 19, 18, 17, 16, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 12};

bool button1OldState = HIGH;
bool button2OldState = HIGH;

bool useTimer = true;

// always cycle through the rainbow
int hue = 0;
// keep track of each mode's progress
int modeCounter = 0;
// fall back brightness, default to 1/3
int brightness = 85;

int mode = 0;
uint32_t lastModeChange;
uint32_t randColor;

void setup() {
#ifdef __AVR_ATtiny85__ // Trinket, Gemma, etc.
  if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
#endif
  pinMode(BUTTON1_PIN, INPUT_PULLUP);
  pinMode(BUTTON2_PIN, INPUT_PULLUP);
  pixels.begin();
  pixels.setBrightness(85); // 1/3 brightness
  lastModeChange = millis();
  randomSeed(millis());
  newRandColor();
}

void newRandColor() {
  randColor = pixels.Color(random(255), random(255), random(255));
}

void loop() {
  // Initialize all LEDs to off and reset brightness
  //setAllPixels(0x000000);
  pixels.setBrightness(brightness);

  // Update current button state
  bool button1NewState = digitalRead(BUTTON1_PIN);
  bool button2NewState = digitalRead(BUTTON2_PIN);

  // Button and State machine management
  if (button1NewState == LOW && button2NewState == LOW &&
      (button1OldState == HIGH || button2OldState == HIGH)) {
    useTimer = !useTimer;
  }
  if (button1NewState == LOW && button1OldState == HIGH) {
    if (mode >= 0 || mode <= -4) mode = -1;
    else mode--;
    lastModeChange = millis();
  }
  else if (button2NewState == LOW && button2OldState == HIGH) {
    if (mode < 0 || mode >= 5) mode = 0;
    else mode++;
    lastModeChange = millis();
  }
  else if (useTimer && mode >= 0 && (millis() > lastModeChange + MODE_SWITCH_MILLIS)) {
    if (mode < 0 || mode >= 5) mode = 0;
    else mode++;
    lastModeChange = millis();
  }

  // Flashy light stuff time!
  switch (mode) {
    // brightness adjusting mode
    case -4: setAllPixels(0x000000); setSun(hue); break;

    // flashlight; force max brightness
    case -3: pixels.setBrightness(255); setAllPixels(0xFFFFFF); break;

    // winky face, using color 0xFFD700
    case -2: setAllPixels(0x000000); setSmileyPixels(0xFFD700); wink(); break;

    // smiley face, cycling rainbow for now!
    case -1: setAllPixels(0x000000); setRainbowSmileyPixels(); break;

    // uniform colors, a rainbow cycling
    case 0: setAllPixels(getRainbow(hue)); break;

    // colorwipe the rainbow cycle... like windshield wipers going around
    case 1: colorWipe(getRainbow(hue)); break;

    // colorwipe with a random color
    case 2: colorWipe(randColor); break;

    // opposite spinners slowly rotating through the rainbow
    case 3: oppositeSpin(getRainbow(hue)); break;

    // rainbow gradient going horizontal
    case 4: rainbowGradient(horizontalLEDs); break;

    // rainbow gradient going diagonal
//    case 5: rainbowGradient(diagonalLEDs); break;

    // rainbow gradient going vertical... except separate eyes
    //case 6: rainbowGradient(verticalLEDs); break;

    //
    case 5: writeRainbowFromArray(infinityLEDs); break;
  }

  // i love rainbowCycle.  implement that rainbowCycle()
  // vertical wheel like horizontal
  // infinity sign?
  // make a rainbow circle on each eye, then rotate it around

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

void writeColorFromArray(int leds[], uint32_t color) {
  for (int i = 0; i < (sizeof(leds) / sizeof(int)); i++) {
    pixels.setPixelColor(leds[i], color);
  }
}

//(sizeof(leds) / sizeof(int)
void writeRainbowFromArray(int leds[]) {
  for (int i = 0; i < 40; i++) {
    pixels.setPixelColor(leds[i], getRainbow((i*(255/40)) + hue));
  }
}


void setSun(int b) {
  brightness = b;
  int numToLight = b / 16;
  for (int i = 0; i < numToLight; i++) {
    pixels.setPixelColor(i, getRainbow(i * 16 + hue));
    pixels.setPixelColor(i + 16, getRainbow(i * 16 + hue));
  }
}

void rainbowCycle() {
  uint16_t i, j;
  for (i = 0; i < pixels.numPixels(); i++) {
    pixels.setPixelColor(i, getRainbow(((i / pixels.numPixels()) ) & 255));
  }
  delay(20);

}

// horizontal gradients seem to work fine
// vertical gradients have some mistakes...
void gradientRainbow(int startingLED) {
  for (int i = 0; i < 18; i++) {
    if (i < 9) {
      int topLED = startingLED - i;
      if (topLED < 0) topLED = 16 + topLED;

      int bottomLED = startingLED + i;
      if (bottomLED > 15) bottomLED = 16 - bottomLED;

      pixels.setPixelColor(topLED, getRainbow(i * 14 + hue));
      pixels.setPixelColor(bottomLED, getRainbow(i * 14 + hue));
    }
    else {
      int topLED = (startingLED + 16) - (i - 9);
      if (topLED < 16) topLED = 32 - (16 - topLED);

      int bottomLED = (startingLED + 16) + (i - 9);
      if (bottomLED > 31) bottomLED = 32 - (bottomLED - 16);

      pixels.setPixelColor(topLED, getRainbow(i * 14 + hue));
      pixels.setPixelColor(bottomLED, getRainbow(i * 14 + hue));
    }
  }
  delay(50);
  hue += 10;
}

// rainbow gradient, switching direction every 5 seconds
void rainbowGradient(int LEDmapping[]) {
  if (millis() % 10000 > 5000) {
    for (int i = 0; i < 18; i++) {
      pixels.setPixelColor(LEDmapping[i * 2], getRainbow(i * 14 + hue));
      pixels.setPixelColor(LEDmapping[i * 2 + 1], getRainbow(i * 14 + hue));
    }
  }
  else {
    for (int i = 0; i < 18; i++) {
      pixels.setPixelColor(LEDmapping[i * 2], getRainbow((18 - i) * 14 + hue));
      pixels.setPixelColor(LEDmapping[i * 2 + 1], getRainbow((18 - i) * 14 + hue));
    }
  }
  delay(30);
  hue += 10;
}

// each eye rotates three opposite LED patches
// todo: randomize the number of LED patches
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
void setSmileyPixels(uint32_t color) {
  for (int i = 0; i < 18; i++) {
    pixels.setPixelColor(smileyLEDS[i], color);
  }
}

void wink() {
  uint32_t winkTime = (millis() - lastModeChange) % 3000; // todo: add some random amount too?
  if (winkTime > 1000 && winkTime < 1600) {
    pixels.setPixelColor(1, 0x000000);
    pixels.setPixelColor(17, 0x000000);
  }
}

void setRainbowSmileyPixels() {
  for (int i = 0; i < 18; i++) {
    pixels.setPixelColor(smileyLEDS[i], getRainbow(i * 14 + hue));
  }
}

// sets all the LEDs on both eyes
void setAllPixels(uint32_t color) {
  for (int i = 0; i < LED_LENGTH; i++) {
    pixels.setPixelColor(i, color);
  }
}

void colorWipe(uint32_t color) {
  if (modeCounter >= pixels.numPixels() / 2) {
    modeCounter = 0;
    newRandColor();
  }
  pixels.setPixelColor(modeCounter, color);
  pixels.setPixelColor(31 - modeCounter, color);
  pixels.show();
  hue += 10; // the delta between this value and 14 is what makes this interesting visually
  delay(50);
  modeCounter++;
}

// Input a value 0 to 255 to get a color value from the color wheel
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
