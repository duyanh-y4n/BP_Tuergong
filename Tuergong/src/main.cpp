/**
 * File            : main.cpp
 * Author          : Duy Anh Pham <duyanh.y4n.pham@gmail.com>
 * Date            : 15.04.2020
 * Last Modified By: Duy Anh Pham <duyanh.y4n.pham@gmail.com>
 */
/*
  Steps:
  1. Connect to the access point "'TuergongHAW'"
  2. Point your web browser to http://192.168.4.1/H to turn the LED on or http://192.168.4.1/L to turn it off
     OR
     Run raw TCP "GET /H" and "GET /L" on PuTTY terminal with 192.168.4.1 as IP address and 80 as port
*/

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
#include "DFRobotDFPlayerMini.h"

#define LED_BUILTIN 2   // Set the GPIO pin where you connected your test LED or comment this line out if your dev board has a built-in LED
#define LED_OUT 32

// Set these to your desired credentials.
const char *ssid = "TuergongHAW";
const char *password = "AnhTungLong";

WiFiServer server(80);
// Function to setup MiniPlayer
void set_up_MiniPlayer();
// Function to controll MiniPlayer with UART
void controll_MiniPlayer();

// To config Tx and Rx
HardwareSerial mySoftwareSerial(1);
DFRobotDFPlayerMini myDFPlayer;


void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(LED_OUT, OUTPUT);

  Serial.begin(115200);
  Serial.println();
  Serial.println("Configuring access point...");

  // You can remove the password parameter if you want the AP to be open.
  WiFi.softAP(ssid, password);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  server.begin();

  Serial.println("Tuergong Server started");

  //set up mini player
  set_up_MiniPlayer();
}



void loop() {
  WiFiClient client = server.available();   // listen for incoming clients

  if (client) {                             // if you get a client,
    Serial.println("New Client.");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            // the content of the HTTP response follows the header:
            client.print("Click <a href=\"/H\">here</a> to turn ON the LED.<br>");
            client.print("Click <a href=\"/L\">here</a> to turn OFF the LED.<br>");
            client.print("Click <a href=\"/bell\">here</a> to trigger sound.<br>");

            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          } else {    // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

        // Check to see if the client request was "GET /H" or "GET /L":
        if (currentLine.endsWith("GET /H")) {
          digitalWrite(LED_BUILTIN, HIGH);  
          digitalWrite(LED_OUT, HIGH);             // GET /H turns the LED on
        }
        if (currentLine.endsWith("GET /L")) {
          digitalWrite(LED_BUILTIN, LOW); 
          digitalWrite(LED_OUT, LOW);                // GET /L turns the LED off
        }
        if (currentLine.endsWith("GET /bell")) {
              // Controll the MP3 Next or Up-Down the Volume
              controll_MiniPlayer();
        }
      }
    }
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected.");
  }
}

void set_up_MiniPlayer()
{
  
  mySoftwareSerial.begin(9600, SERIAL_8N1, 16, 17);  // speed, type, RX, TX
  Serial.begin(115200);
  
  Serial.println();
  Serial.println(F("DFRobot DFPlayer Mini Demo"));
  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));
  Serial.println(myDFPlayer.begin(mySoftwareSerial));
  
  if (!myDFPlayer.begin(mySoftwareSerial)) {  //Use softwareSerial to communicate with mp3.
    
    Serial.println(myDFPlayer.readType(),HEX);
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while(true);
  }
  Serial.println(F("DFPlayer Mini online."));
  
  myDFPlayer.setTimeOut(500); //Set serial communictaion time out 500ms
  
  //----Set volume----
  myDFPlayer.volume(1);  //Set volume value (0~30).
 
  //----Set different EQ----
  myDFPlayer.EQ(DFPLAYER_EQ_NORMAL);
  
  //----Set device we use SD as default----
  myDFPlayer.outputDevice(DFPLAYER_DEVICE_SD);

  //----Mp3 control----

  //----Read imformation----
  Serial.println(F("readState--------------------"));
  Serial.println(myDFPlayer.readState()); //read mp3 state
  Serial.println(F("readVolume--------------------"));
  Serial.println(myDFPlayer.readVolume()); //read current volume
  //Serial.println(F("readEQ--------------------"));
  //Serial.println(myDFPlayer.readEQ()); //read EQ setting
  Serial.println(F("readFileCounts--------------------"));
  Serial.println(myDFPlayer.readFileCounts()); //read all file counts in SD card
  Serial.println(F("readCurrentFileNumber--------------------"));
  Serial.println(myDFPlayer.readCurrentFileNumber()); //read current play file number
  Serial.println(F("readFileCountsInFolder--------------------"));
  Serial.println(myDFPlayer.readFileCountsInFolder(3)); //read fill counts in folder SD:/03
  Serial.println(F("--------------------"));

  Serial.println(F("myDFPlayer.play(1)"));
  myDFPlayer.play(1);  //Play the first mp3
}

void controll_MiniPlayer()
{
  // static unsigned long timer = millis();
  
 if (Serial.available()) {
    String inData = "";
    inData = Serial.readStringUntil('\n');
    if (inData.startsWith("n")) {
      Serial.println(F("next--------------------"));
      myDFPlayer.next();
      Serial.println(myDFPlayer.readCurrentFileNumber()); //read current play file number
    } else if (inData.startsWith("p")) {
      Serial.println(F("previous--------------------"));
      myDFPlayer.previous();
      Serial.println(myDFPlayer.readCurrentFileNumber()); //read current play file number
    } else if (inData.startsWith("+")) {
      Serial.println(F("up--------------------"));
      myDFPlayer.volumeUp();
      Serial.println(myDFPlayer.readVolume()); //read current volume
    } else if (inData.startsWith("-")) {
      Serial.println(F("down--------------------"));
      myDFPlayer.volumeDown();
      Serial.println(myDFPlayer.readVolume()); //read current volume
    } else if (inData.startsWith("*")) {
      Serial.println(F("pause--------------------"));
      myDFPlayer.pause();
    } else if (inData.startsWith(">")) {
      Serial.println(F("start--------------------"));
      myDFPlayer.start();
    }
 }

 if (myDFPlayer.available()) {
  if (myDFPlayer.readType()==DFPlayerPlayFinished) {
    Serial.println(myDFPlayer.read());
    Serial.println(F("next--------------------"));
     myDFPlayer.next();  //Play next mp3 every 3 second.
    Serial.println(F("readCurrentFileNumber--------------------"));
    Serial.println(myDFPlayer.readCurrentFileNumber()); //read current play file number
    delay(500);
  }
 }  
}

