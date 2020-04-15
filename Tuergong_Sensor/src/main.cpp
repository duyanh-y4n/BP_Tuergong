/**
 * File            : main.cpp
 * Author          : Duy Anh Pham <duyanh.y4n.pham@gmail.com>
 * Date            : 15.04.2020
 * Last Modified By: Duy Anh Pham <duyanh.y4n.pham@gmail.com>
 */

/*
 * Türgong sensor HTTP Client GET Request
 * */

#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>

/* Set these to your desired credentials. */
const char *ssid = "TuergongHAW";  //ENTER YOUR WIFI SETTINGS
const char *password = "AnhTungLong";

//Web/Server address to read/write from 
const char *host = "192.168.4.1"; 
const char *bell = "http://192.168.4.1/bell"; 
const char *LED_ON = "http://192.168.4.1/H"; 
const char *LED_OFF = "http://192.168.4.1/L"; 

//=======================================================================
//                    Function Prototype
//=======================================================================
String httpGETRequest(const char* serverName, int with_payload);

//=======================================================================
//                    Power on setup
//=======================================================================

void setup() {
  delay(1000);
  Serial.begin(115200);
  WiFi.mode(WIFI_OFF);        //Prevents reconnection issue (taking too long to connect)
  delay(1000);
  WiFi.mode(WIFI_STA);        //This line hides the viewing of ESP as wifi hotspot
  
  WiFi.begin(ssid, password);     //Connect to your WiFi router
  Serial.println("");

  Serial.print("Connecting");
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
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
  /*
   * test
   * */
  //httpGETRequest(LED_ON,0);
  //Serial.println(LED_ON);
  //delay(500);
  //httpGETRequest(LED_OFF,0);
  //Serial.println(LED_OFF);
  //delay(500);  //GET Data at every 5 seconds
  /*
   * trigger bell
   * */
  String response = httpGETRequest(bell,0);
  delay(3000);  //trigger bell every 3 seconds
  Serial.print(response);
}
//=======================================================================
//                    Function Implementation
//=======================================================================

String httpGETRequest(const char* serverName, int with_payload) {
  WiFiClient client;
  HTTPClient http;

  // Your IP address with path or Domain name with URL path
  http.begin(client, serverName);

  // Send HTTP POST request
  int httpResponseCode = http.GET();

  String payload = "--";

  if (httpResponseCode>0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    if(with_payload){
      payload = http.getString(); // this cause delay, use only for debugging
    }
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();

  return payload;
}
