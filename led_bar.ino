#include "LPD8806.h"
#include "SPI.h"
#include "stdlib.h"

#define RESET    0
#define SOLID    1
#define PROGRESS 2
#define RANDOM   3
#define CUSTOM   4


int numberLEDS = 64;

int numSeen = 0;

int dataPin  = 2;
int clockPin = 7;

// True if strip is mounted "backwards", but you still wish
// to have the leftmost pixel be considered the 0th pixel
boolean REVERSE_STRIP = true;


LPD8806 strip = LPD8806(numberLEDS, dataPin, clockPin);


void setup() {
  // Start up the LED strip
  strip.begin();
  
  // Update the strip, set all LEDs to off
  strip.show();
  
  // Initialize serial
  Serial.begin(115200);
  Serial.write(1);
}

void loop() {
  int option = getLong();
  if (option == RESET)
    reset();      
  else if (option == SOLID)
    setSolid(getLong());
  else if (option == PROGRESS) {
    int num = getLong();
    int denom = getLong();
    setProgress(num, denom);
  } else if (option == RANDOM)
    setRandom();
  else if (option == CUSTOM) {
    uint32_t colors[strip.numPixels()];
    for (int i = 0; i < strip.numPixels(); i++) {
      colors[i] = getLong();
    }
    setCustom(colors);
  }
}

/*
 * Get the next long.
 * If none is immediately available, will wait until
 * one is.
 */
uint32_t getLong() {
  while (!Serial.available()) {}
  uint32_t num = Serial.parseInt();
  while (Serial.peek() == '\n')
    Serial.read();
  return num;
}

/**
 * Reset the strip to all LEDs off
 */
void reset() {
  setSolid(0);
  strip.show();
}

/**
 * Set the strip to a progress bar based on a fraction
 *
 * @param num the numerator of the fraction
 * @param denom the denominator of the fraction
 */
void setProgress(uint32_t num, uint32_t denom) {
  int color = num * 127 / denom;
  int progress = num * 64 / denom;
  for (int i = 0; i < strip.numPixels(); i++) {
    int pixel = REVERSE_STRIP ? (strip.numPixels() - 1) - i : i;
    if (i < progress) {
      strip.setPixelColor(pixel, strip.Color(127 - color, color, 0));
    } else {
      strip.setPixelColor(pixel, 0);
    }
  }
  strip.show();
}

/**
 * Set the strip to a random pattern
 */
void setRandom() {
  while (!Serial.available()) {
      for(int i = 0; i < 5; i++) {
      int led = random(64);
      uint32_t color;
      color = random(0xFFFFFF);
      strip.setPixelColor(led, color);
    }
    strip.show();
    int delayLength = random(50);
    delay(delayLength);
  }
}

/**
 * Set the entire strip to one color
 *
 * @param color the color to set the strip to
 */
void setSolid(uint32_t color) {
  for (int i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, color);
  }
  strip.show();
}

/**
 * Set the strip to a custom color pattern
 * 
 * @requires colors must be exactly numberLEDs long
 * @param colors an array of colors to set the strip to
 */
void setCustom(uint32_t* colors) {
  for (int i = 0; i < strip.numPixels(); i++) {
    int pixel = REVERSE_STRIP ? (strip.numPixels() - 1) - i : i;
    strip.setPixelColor(pixel, colors[i]);
  }
  strip.show();
}


/**
 * Stress the system
 */
void stress() {
  int i;
  int c = strip.Color(127, 127, 127);
  for (i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
  }
  strip.show();
  delay(600000);
  
  c = strip.Color(0, 127, 0);
  for (i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
  }
  strip.show();
  delay(600000);
}


