// Library for LED pixels
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

// Library for WiFi
#include "ESP8266WiFi.h"

// Time module
#include <time.h>
#include "timezone.h"
const char *TIME_SERVER = "pool.ntp.org";
int myTimeZone = 0; // change this to your time zone (see in timezone.h)
const int daylightOffset_sec = -3600;  // daylight savings

// parameters for time check
const int delay_between_time_checks = 1000;       // milliseconds
const int scheduled_time_to_check_bin_collection = 17;      // 5pm
int last_date_bin_collections_were_checked = 0;   // Today or yesterday



time_t now;

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
const char* ssid = "VM3146024";
const char* password = "wcs8bgqxTjmv";


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

  // setup time
  configTime(myTimeZone, daylightOffset_sec, TIME_SERVER);
  Serial.println("\nWaiting for time");
  while (now < EPOCH_1_1_2019)
  {
    now = time(nullptr);
    delay(500);
    Serial.print("*");
  }
  Serial.print("\n");
}

void loop() {
  // solid_color(set_red);
  // blinking(set_green_and_blue);
  // blink thrice
  blink_x_times(2);
  delay(delay_between_time_checks);
  retrieve_time();
}


void retrieve_time() {
  struct tm *timeinfo;

  time(&now);
  timeinfo = localtime(&now);

  int year = timeinfo->tm_year + 1900;
  int month = timeinfo->tm_mon + 1;
  int day = timeinfo->tm_mday;
  int hour = timeinfo->tm_hour;
  int mins = timeinfo->tm_min;
  int sec = timeinfo->tm_sec;
  int day_of_week = timeinfo->tm_wday;

  Serial.println("Date = " + toStringAddZero(day) + "/" + toStringAddZero(month) + "/" + String(year));
  Serial.println("Time = " + toStringAddZero(hour) + ":" + toStringAddZero(mins) + ":" + toStringAddZero(sec));
  Serial.print("Day is " + String(DAYS_OF_WEEK[day_of_week]));
  Serial.println(" or " + String(DAYS_OF_WEEK_3[day_of_week]));
  
  bool check_council_time = check_alert(scheduled_time_to_check_bin_collection, last_date_bin_collections_were_checked, mins, sec);
  
  if (check_council_time) {
    Serial.println("check bins with council now");
    last_date_bin_collections_were_checked = mins;
    }
  
  }

bool check_alert(int time_to_check, int last_time_of_check, int current_day, int current_hour) {
  bool time_to_alert = false;
  if ((last_time_of_check != current_day) and (current_hour >= time_to_check)) {
    Serial.println("Check Now! in function");
    time_to_alert = true;
    }
  return time_to_alert;
  }

// toStringAddZero()
// this function adds a zero to a date string if its a single digit
// for example if data = 1, then the function will return 01
// this makes the date and time string's consistant size for display
String toStringAddZero(int data)
{
  String st = "";
  if (data < 10)
  {
    st = "0" + String(data);
  }
  else
  {
    st = String(data);
  }
  return st;
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
