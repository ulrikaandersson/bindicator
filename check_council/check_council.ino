// Library for WiFi
#include "ESP8266WiFi.h"
// #include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>

const int httpsPort = 443;
const char* host = "https://servicelayer3c.azure-api.net";
String url = "/wastecalendar/collection/search/200004185513/?numberOfCollections=12&fbclid=IwAR04JcYs5eiVkD-gK3EsKZg3G8qzMxp6_Mb6SJAxBadeTP9CdE8aTyz5UyY";
// WiFi parameters to be configured
const char* ssid = "VM3146024";
const char* password = "wcs8bgqxTjmv";



void setup() {
  // put your setup code here, to run once:
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
  if (WiFi.status() == WL_CONNECTED) { //Check WiFi connection status
    Serial.println("wifi connection still there");
    WiFiClientSecure client;
    Serial.println("connecting to council");
    if (!client.connect(host, httpsPort)) {
      Serial.println("connection failed");
      Serial.println(host);
      Serial.println(httpsPort);
      return;
    }
    
  
    Serial.print("requesting URL: ");
    Serial.println(url);
  
    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "User-Agent: Gitlab-Monitor0\r\n" +
                 "Connection: close\r\n\r\n");
  
    Serial.println("request sent");
    while (client.connected()) {
      String line = client.readStringUntil('\n');
      if (line == "\r") {
        Serial.println("headers received");
        break;
      }
    }
    String contents = client.readStringUntil('\n');
  
    char json[contents.length()+1];
    contents.toCharArray(json, contents.length()+1);
    StaticJsonBuffer<1000> jsonBuffer;
    JsonArray& root = jsonBuffer.parseArray(json);
    for (auto dataobj : root){
      Serial.print(dataobj["status"].asString());
      Serial.println(dataobj["status"].asString());
      }
    }
  delay(30000);    //Send a request every 30 seconds
   

}
