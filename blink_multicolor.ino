#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif
#define PIN        D8
#define NUMPIXELS 12
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
#define DELAYVAL 500


void setup() {
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif

  pixels.begin();
}

void loop() {
  // solid_color(set_red);
  // blinking(set_green_and_blue);
  // blink thrice
  blink_x_times(2);
  delay(20*DELAYVAL);
}

void turn_off() {
  pixels.clear();
  pixels.show();
  }

void turn_on(void (*func)()) {
  // func sets pixel values
  func();
  pixels.show();
  }

void blinking(void (*func)()) {
  // give the function to turn on. delay and turn off are executed
  turn_on(func);
  delay(DELAYVAL);
  turn_off();
  delay(DELAYVAL);
  }

void solid_color(void (*func)()) {
  turn_on(func);
  }

void blink_x_times(int a) {
  for(int i=0; i<a; i++) {
    blinking(set_red);
  }
  }

void set_green_and_blue() {
  for(int i=0; i<NUMPIXELS; i++) {
    if(tophalf(i)) {
      pixels.setPixelColor(i, pixels.Color(0, 150, 0));
      } else {
        pixels.setPixelColor(i, pixels.Color(0, 0, 150));
      }
  }
  }

void set_red() {
  for(int i=0; i<NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(150, 0, 0));
  }
  }

bool tophalf(int i) {
  if((i < NUMPIXELS / 4) or !(i < (3 * (NUMPIXELS / 4)))) {
    return true;
    }
    else {
      return false;
    }
  }