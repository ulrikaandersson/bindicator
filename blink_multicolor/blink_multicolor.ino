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
const int scheduled_check_time = 17;      // 5pm
const int max_time_to_blink_for = 10;                       // hours to blink for
String last_date_of_check = "";   // Today or yesterday

// button input
const int buttonPin = 4;

time_t now;
time_t tomorrow;

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
    tomorrow = time(nullptr);
    delay(500);
    Serial.print("*");
  }
  Serial.print("\n");

  // setup button input
  pinMode(buttonPin, INPUT);
}

void loop() {
  // solid_color(set_red);
  // blinking(set_green_and_blue);
  // blink thrice
  blink_x_times(2);
  delay(delay_between_time_checks);
  retrieve_time();
}


String get_date_string(tm *t_info) {
  int year = t_info->tm_year + 1900;
  int month = t_info->tm_mon + 1;
  int day = t_info->tm_mday;
  String date_string = String(year) + "-" + toStringAddZero(month) + "-" + toStringAddZero(day);
  
  // int day_of_week = timeinfo->tm_wday;
  // Serial.print("Day is " + String(DAYS_OF_WEEK[day_of_week]));
  // Serial.println(" or " + String(DAYS_OF_WEEK_3[day_of_week]));
  return date_string;
  }
  
  String get_time_string(tm *t_info) {
  int year = t_info->tm_year + 1900;
  int hour = t_info->tm_hour;
  int mins = t_info->tm_min;
  int sec = t_info->tm_sec;
  String time_string = toStringAddZero(hour) + ":" + toStringAddZero(mins) + ":" + toStringAddZero(sec);
  return time_string;
  }

void print_date_and_time(String today_d, String today_t, String tom_d) {
  Serial.print("Today's date: ");
  Serial.println(today_d);
  Serial.print("Today's time: ");
  Serial.println(today_t);
  
  Serial.print("Tomorrow's date: ");
  Serial.println(tom_d);
  }

void retrieve_time() {
  struct tm *timeinfo;
  struct tm *timeinfo_tomorrow;
  
  time(&now);
  timeinfo = localtime(&now);
  String today_date = get_date_string(timeinfo);
  String today_time = get_time_string(timeinfo);
  int current_hour = timeinfo->tm_hour;
  tomorrow = now + 86400;       // add 24 x 60 x 60s
  timeinfo_tomorrow = localtime(&tomorrow);
  String tomorrow_date = get_date_string(timeinfo_tomorrow);
  
  print_date_and_time(today_date, today_time, tomorrow_date);
  
  bool check_council_time = check_alert(scheduled_check_time, last_date_of_check, today_date, current_hour);
  
  if (check_council_time) {
    Serial.println("check bins with council now");
    alert_function();
    last_date_of_check = get_date_string(timeinfo);
    }
  }

bool check_alert(int time_to_check, String last_check_date, String t_date, int hour) {
  bool time_to_alert = false;
  Serial.println("date in func: " + t_date);
  if ((last_check_date != t_date) and (hour >= time_to_check)) {
    // Serial.println("Check Now! in function");
    time_to_alert = true;
    }
  return time_to_alert;
  }

void alert_function() {
  bool button_pressed = 0;
  while (button_pressed == 0) {
    // blink
    blinking(set_green_and_blue);
    button_pressed = digitalRead(buttonPin);  // set to 0 when pressed
    }
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
