// Library for LED pixels
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

// Library for WiFi
#include "ESP8266WiFi.h"

// web reading
#include <WiFiClientSecure.h> 
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

const char *host = "servicelayer3c.azure-api.net";
const int httpsPort = 443;  //HTTPS= 443 and HTTP = 80

//SHA1 finger print of certificate use web browser to view and copy
const char fingerprint[] PROGMEM = "DD 49 B8 6F E6 C2 CB 9B E2 AC 9A 73 FD 30 75 C1 01 19 35 A4";
// "53 04 CC A4 A4 55 1E 0A 05 C2 6B 97 2C D3 08 A5 DF 33 78 2C";
// "65 10 73 1D 4A 19 D9 4C 39 DD AF DE F9 3A 7D 15 20 E6 52 0D";


// Time module
#include <time.h>
#include "timezone.h"
const char *TIME_SERVER = "pool.ntp.org";
int myTimeZone = 0; // change this to your time zone (see in timezone.h)
const int daylightOffset_sec = 0; //-3600;  // daylight savings buggy so ignoring - doesn't matter if 1 hour off

// parameters for time check
const int delay_between_time_checks = 15 * 60 * 1000;       // milliseconds
const int scheduled_check_time = 15;      // 3pm
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
const uint32_t yellowPixel = pixels.Color(247, 214, 0);
const uint32_t whitePixel = pixels.Color(255, 255, 255);
const uint32_t unlitPixel = pixels.Color(0, 0, 0);
const uint32_t dimWhitePixel = pixels.Color(155, 155, 155);

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
  WiFi.mode(WIFI_OFF);        //Prevents reconnection issue (taking too long to connect)
  delay(1000);
  WiFi.mode(WIFI_STA);
  // Connect to WiFi
  WiFi.begin(ssid, password);

  // while wifi not connected yet, print '.'
  // then after it connected, get out of the loop
  Serial.print("Connecting");
  
  while (WiFi.status() != WL_CONNECTED) {
     delay(500);
     Serial.print(".");
  }
  //print a new line, then print WiFi connected and the IP address
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  // Print the IP address
  Serial.println(WiFi.localIP());
  blink_x_times(1, greenPixel, greenPixel);

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
  blink_x_times(3, greenPixel, greenPixel);
  Serial.println("Time found");

  // setup button input
  pinMode(buttonPin, INPUT);
}

void loop() {
  // solid_color(set_red);
  // blinking(set_green_and_blue);
  // blink thrice
  blink_x_times(2, whitePixel, whitePixel);
  retrieve_time();
  delay(delay_between_time_checks);
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
  bool bins_checked_and_alerted_successfully;
  if (check_council_time) {
    Serial.println("check bins with council now");
    bins_checked_and_alerted_successfully = alert_function(tomorrow_date);
    if (bins_checked_and_alerted_successfully) {
      last_date_of_check = today_date;
    }
    }
  else {
    Serial.println("not time to check bins");
    }
  }

bool check_alert(int time_to_check, String last_check_date, String t_date, int hour) {
  bool time_to_alert = false;
  // Serial.println("date in func: " + t_date);
  if ((last_check_date != t_date) and (hour >= time_to_check)) {
    // Serial.println("Check Now! in function");
    time_to_alert = true;
    }
  return time_to_alert;
  }

bool alert_function(String tomorrow_date) {
  bool button_pressed = 0;
  WiFiClientSecure httpsClient;    //Declare object of class WiFiClient
 
  Serial.println(host);
 
  Serial.printf("Using fingerprint '%s'\n", fingerprint);
  httpsClient.setFingerprint(fingerprint);
  httpsClient.setTimeout(15000); // 15 Seconds
  delay(1000);
  
  Serial.println("HTTPS Connecting");
  int r=0; //retry counter
  while((!httpsClient.connect(host, httpsPort)) && (r < 30)){
      delay(100);
      Serial.print(".");
      r++;
  }
  if(r==30) {
    Serial.println("Connection failed");
    blink_x_times(5, redPixel, redPixel);
  }
  else {
    Serial.println("Connected to web");
    blink_x_times(2, greenPixel, greenPixel);
  }
  
  String getData, Link;
 
  //GET Data
  Link = "/wastecalendar/collection/search/200004185513/?numberOfCollections=12";
  // &fbclid=IwAR04JcYs5eiVkD-gK3EsKZg3G8qzMxp6_Mb6SJAxBadeTP9CdE8aTyz5UyY";
 
  Serial.print("requesting URL: ");
  Serial.println(host+Link);
 
  httpsClient.print(String("GET ") + Link + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +               
               "Connection: close\r\n\r\n");
 
  Serial.println("request sent");
                  
  while (httpsClient.connected()) {
    String line = httpsClient.readStringUntil('\n');
    if (line == "\r") {
      Serial.println("headers received");
      break;
    }
  }
 
  Serial.println("reply was:");
  Serial.println("==========");
  String line;
  while(httpsClient.available()){        
    line = httpsClient.readStringUntil('\n');  //Read Line by Line
    Serial.println("Starting to parse:");
    
    size_t capacity = 9*JSON_ARRAY_SIZE(1) + 3*JSON_ARRAY_SIZE(2) + JSON_ARRAY_SIZE(3) + JSON_ARRAY_SIZE(12) + JSON_OBJECT_SIZE(2) + 12*JSON_OBJECT_SIZE(3) + 930;
    DynamicJsonDocument doc(capacity);
    
    
    deserializeJson(doc, line);
    
    JsonArray collections = doc["collections"];
    
    JsonObject collections_0 = collections[0];
    const char* collection_date_char = collections_0["date"];
    char collection_date[11] = "";
    strncpy(collection_date, collection_date_char, 10);
    String collection_date_string = String(collection_date);
    
    const char* collection_types_0 = collections_0["roundTypes"][0]; // "RECYCLE"
    const char* collection_types_1 = collections_0["roundTypes"][1];
    
    Serial.print("Collection date: ");
    Serial.println(collection_date_string);

    Serial.print("Tomorrow's date: ");
    Serial.println(tomorrow_date);

    Serial.print("Collection type 1: ");
    Serial.println(collection_types_0);

    Serial.print("Collection type 2: ");
    Serial.println(collection_types_1);

    if (tomorrow_date == collection_date_string) {
      Serial.println("Collection tomorrow!");
      uint32_t c1;
      uint32_t c2;
      c1 = get_colour_from_string(collection_types_0);
      
      // Serial.println("some error in this if statement because if empty string?");
      if (collection_types_1 == NULL || *collection_types_1 == 0) {
        c2 = c1;
        Serial.println("c1 = c2");
        }
      else {
        Serial.println("getting new c2");
        c2 = get_colour_from_string(collection_types_1);
        }
      Serial.println("colours retrieved");
      
      while (button_pressed == 0) {
        // blink
        
        blinking(c1, c2);
        // Serial.println("before button read");
        button_pressed = digitalRead(buttonPin);  // set to 0 when pressed
        
        // Serial.println("after button read");
        }
      Serial.println("button has been pressed");
      button_pressed = 1;
      }
    else {
      // Serial.println("no collection tomorrow or no connection. returning not pressed.");
      if ((collection_date_string != NULL) && (collection_date_string != 0)) {
        Serial.println("date is found but no collection tomorrow");
        button_pressed = 1;
        }
      else {
        Serial.println("no date found, check unsuccessfull");
        }
      }

    
    // Serial.println(line); //Print response
  }
  Serial.println("==========");
  Serial.println("closing connection");
  Serial.print("button pressed returned = ");
  Serial.println(button_pressed);
  return button_pressed;
  }


uint32_t get_colour_from_string(const char* bin_type) {
  uint32_t c;
  if (strcmp(bin_type, "RECYCLE") == 0) {
            Serial.println("setting blue");
            c = bluePixel;}
  if (strcmp(bin_type, "DOMESTIC") == 0) {
    Serial.println("setting yellow");
            c = yellowPixel;}
  if (strcmp(bin_type, "ORGANIC") == 0) {
    Serial.println("setting green");
            c = greenPixel;}
  return c;
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

void blinking(uint32_t colour_1, uint32_t colour_2) {
  // give the function to turn on. delay and turn off are executed
  set_half_and_half(colour_1, colour_2);
  pixels.show();
  delay(DELAYVAL);
  turn_off();
  delay(DELAYVAL);
  }

void solid_color(void (*func)()) {
  turn_on(func);
  }

void blink_x_times(int a, uint32_t c1, uint32_t c2) {
  for(int i=0; i<a; i++) {
    blinking(c1, c2);
  }
  }

void set_half_and_half(uint32_t colour_1, uint32_t colour_2) {
  for(int i=0; i<NUMPIXELS; i++) {
    if(tophalf(i)) {
      pixels.setPixelColor(i, colour_1);
      } else {
        pixels.setPixelColor(i, colour_2);
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
