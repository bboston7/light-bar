#include "LPD8806.h"
#include "SPI.h"
#include "stdlib.h"

#define RESET    0
#define SOLID    1
#define PROGRESS 2
#define RANDOM   3
#define CUSTOM   4


int numberLEDS = 64;

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
  //Serial.write("\n");
}

void loop() {
  uint8_t index = getByte();
  if (index >= strip.numPixels())
    // ERROR, out of sync
    index = getByte();
  uint8_t degree = getByte();

  uint8_t pixel = REVERSE_STRIP ? (strip.numPixels() - 1) - index : index;
  strip.setPixelColor(pixel, degree * 0x010101);

  if (index == strip.numPixels() - 1) strip.show();
}

uint8_t getByte() {
  while (!Serial.available()) {}
  return Serial.read();
}
