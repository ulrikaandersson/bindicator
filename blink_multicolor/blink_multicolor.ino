// Library for LED pixels
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

// Library for WiFi
#include "ESP8266WiFi.h"

#define PIN        D8
#define NUMPIXELS 12
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
#define DELAYVAL 500

// Define some standard colours for easier reference later
const uint32_t redPixel = pixels.Color(255, 0, 0);
const uint32_t greenPixel = pixels.Color(0, 255, 0);
const uint32_t bluePixel = pixels.Color(0, 0, 255);
const uint32_t whitePixel = pixels.Color(255, 255, 255);
const uint32_t unlitPixel = pixels.Color(0, 0, 0);
const uint32_t dimWhitePixel = pixels.Color(255, 255, 255);

// WiFi parameters to be configured
const char* ssid = "YOUR WIFI NETWORK NAME";n
const char* password = "YOUR WIFI PASSWARD";


void setup() {
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif
  
  // Set up neopixel LEDs
  pixels.begin();

  // Set up WiFi
  Serial.begin(9600);
  // Connect to WiFi
  WiFi.begin(ssid, password);

  // while wifi not connected yet, print '.'
  // then after it connected, get out of the loop
  while (WiFi.status() != WL_CONNECTED) {
     delay(500);
     Serial.print(".");
  }
  //print a new line, then print WiFi connected and the IP address
  Serial.println("");
  Serial.println("WiFi connected");
  // Print the IP address
  Serial.println(WiFi.localIP());
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
      pixels.setPixelColor(i, greenPixel);
      } else {
        pixels.setPixelColor(i, bluePixel);
      }
  }
  }

void set_red() {
  set_colour(redPixel);
  }

void set_blue() {
  set_colour(bluePixel);
  }

void set_colour(uint32_t colourPixel) {
  for(int i=0; i<NUMPIXELS; i++) {
    pixels.setPixelColor(i, colourPixel);
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
