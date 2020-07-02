// * HTTPS Secured Client GET Request
// * Copyright (c) 2019, circuits4you.com
// * All rights reserved.
// * https://circuits4you.com 
// * Connects to WiFi HotSpot. */
 
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h> 
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

/* Set these to your desired credentials. */
const char *ssid = "VM3146024";  //ENTER YOUR WIFI SETTINGS
const char *password = "wcs8bgqxTjmv";
 
//Link to read data from https://jsonplaceholder.typicode.com/comments?postId=7
//Web/Server address to read/write from 
const char *host = "servicelayer3c.azure-api.net";
const int httpsPort = 443;  //HTTPS= 443 and HTTP = 80
 
//SHA1 finger print of certificate use web browser to view and copy
const char fingerprint[] PROGMEM = "65 10 73 1D 4A 19 D9 4C 39 DD AF DE F9 3A 7D 15 20 E6 52 0D";
//=======================================================================
//                    Power on setup
//=======================================================================
 
void setup() {
  delay(1000);
  Serial.begin(115200);
  WiFi.mode(WIFI_OFF);        //Prevents reconnection issue (taking too long to connect)
  delay(1000);
  WiFi.mode(WIFI_STA);        //Only Station No AP, This line hides the viewing of ESP as wifi hotspot
  
  WiFi.begin(ssid, password);     //Connect to your WiFi router
  Serial.println("");
 
  Serial.print("Connecting");
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
 
  //If connection successful show IP address in serial monitor
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  //IP address assigned to your ESP
}
 
//=======================================================================
//                    Main Program Loop
//=======================================================================
void loop() {
  WiFiClientSecure httpsClient;    //Declare object of class WiFiClient
 
  Serial.println(host);
 
  Serial.printf("Using fingerprint '%s'\n", fingerprint);
  httpsClient.setFingerprint(fingerprint);
  httpsClient.setTimeout(15000); // 15 Seconds
  delay(1000);
  
  Serial.print("HTTPS Connecting");
  int r=0; //retry counter
  while((!httpsClient.connect(host, httpsPort)) && (r < 30)){
      delay(100);
      Serial.print(".");
      r++;
  }
  if(r==30) {
    Serial.println("Connection failed");
  }
  else {
    Serial.println("Connected to web");
  }
  
  String getData, Link;
 
  //GET Data
  Link = "/wastecalendar/collection/search/200004185513/?numberOfCollections=12&fbclid=IwAR04JcYs5eiVkD-gK3EsKZg3G8qzMxp6_Mb6SJAxBadeTP9CdE8aTyz5UyY";
 
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
    const char* collection_date = collections_0["date"]; // "2020-06-19T00:00:00Z"
    
    const char* collection_types_0 = collections_0["roundTypes"][0]; // "RECYCLE"

    Serial.print("Collection date: ");
    Serial.println(collection_date);

    Serial.print("Collection type: ");
    Serial.println(collection_types_0);
    
    Serial.println(line); //Print response
  }
  Serial.println("==========");
  Serial.println("closing connection");
    
  delay(2000);  //GET Data at every 2 seconds
}
