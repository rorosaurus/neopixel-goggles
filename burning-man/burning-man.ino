// 0 is right eye top (between star and data out)
// 16 is left eye top (between star and data out)
// inc clockwise looking out

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
int horizontalLEDS[] = { 4,  4,  3,  5,  2,  6,  1,  7,  0,  8, 15,  9, 14, 10, 13, 11, 12, 12,
                        20, 20, 19, 21, 18, 22, 17, 23, 16, 24, 31, 25, 30, 26, 29, 27, 28, 28 };

bool button1OldState = HIGH;
bool button2OldState = HIGH;

// always cycle through the rainbow
int hue = 0;
// keep track of each mode's progress
int modeCounter = 0;
// fall back brightness, default to 1/3
int brightness = 85;

int mode = 0;
uint32_t lastModeChange;

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
}

void loop() {
  // Initialize all LEDs to off and reset brightness
  //setAllPixels(0x000000);
  pixels.setBrightness(brightness);

  // Update current button state
  bool button1NewState = digitalRead(BUTTON1_PIN);
  bool button2NewState = digitalRead(BUTTON2_PIN);

  // Button and State machine management
  if (button1NewState == LOW && button2NewState == LOW) {
    brightness = hue;
  }
  if (button1NewState == LOW && button1OldState == HIGH) {
    if (mode >= 0 || mode <= -3) mode = -1;
    else mode--;
    lastModeChange = millis();
  }
  else if (button2NewState == LOW && button2OldState == HIGH) {
    if (mode < 0 || mode >= 4) mode = 0;
    else mode++;
    lastModeChange = millis();
  }
  else if (mode >= 0 && (millis() > lastModeChange + MODE_SWITCH_MILLIS)) {
    if (mode < 0 || mode >= 4) mode = 0;
    else mode++;
    lastModeChange = millis();
  }

  // Flashy light stuff time!
  switch (mode) {
    // -3: flashlight; force max brightness
    case -3: pixels.setBrightness(255); setAllPixels(0xFFFFFF); break;
    // -2: winky face, using color 0xFFD700
    case -2: setSmileyPixels(0xFFD700); wink(); break;
    // -1: smiley face, cycling rainbow for now!
    case -1: setRainbowSmileyPixels(); break;
    // 0: uniform rainbow cycle
    case 0: setAllPixels(getRainbow(hue)); break;
    // 1: colorwipe the rainbow cycle
    case 1: colorWipe(getRainbow(hue)); break;
    // 2: opposite rainbow spinners
    case 2: oppositeSpin(getRainbow(hue)); break;
    // 3: rainbow thing
    case 3: rainbowCycle(); break;
    // 4: do some cool sin and cos shit to get waves of colors moving around your eyes
    // make this mode inaccessible and unaffected by the timer?
    case 4: horizontalWheel(); break;
  }

  // i love rainbowCycle.  implement that
  // vertical wheel like horizontal
  // infinity sign?

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

void rainbowCycle() {
  uint16_t i, j;
  for (i = 0; i < pixels.numPixels(); i++) {
    pixels.setPixelColor(i, getRainbow(((i / pixels.numPixels()) ) & 255));
  }
  pixels.show();
  delay(20);

}

void gradientRainbow(int startingLED) {
  for (int i = 0; i < 18; i++) {
    if (i < 9) {
    }
    else {
    }
  }
  pixels.show();
  delay(50);
  hue += 10;
}

// if you can find a mathematical way to find these addresses.. you can abstract it
// for any starting position, and get vertical/diagonals for free....
void horizontalWheel() {
  for (int i = 0; i < 18; i++) {
    pixels.setPixelColor(horizontalLEDS[i * 2], getRainbow(i * 14 + hue));
    pixels.setPixelColor(horizontalLEDS[i * 2 + 1], getRainbow(i * 14 + hue));
  }
  pixels.show();
  delay(50);
  hue += 10;
}

// each eye rotates two opposite LED patches
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
// todo: try making this a rainbow mix of colors instead of one?
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

// todo: mirror?
void colorWipe(uint32_t color) {
  if (modeCounter >= pixels.numPixels() / 2) modeCounter = 0;
  pixels.setPixelColor(modeCounter, color);
  pixels.setPixelColor(modeCounter + 16, color);
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
