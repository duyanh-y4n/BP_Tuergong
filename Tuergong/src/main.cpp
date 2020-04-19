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
#define LED_OUT  32

#define MP3_NEXT 36     // next mp3
#define MP3_BACK 39     //  previous mp3

#define MP3_UP   34     //  Volume up mp3
#define MP3_DOWN 35     //  Vomme down mp3

#define MP3_PLAY 25     //  Play down mp3
#define MP3_STOP 26     //  Stop down mp3

// Set these to your desired credentials.
const char *ssid = "TuergongHAW";
const char *password = "AnhTungLong";

WiFiServer server(80);

/*
 ** Function to setup MiniPlayer
 */
void set_up_MiniPlayer();

/*
 ** Function to controll MiniPlayer with UART, Web-Server, GPIO
 */
void controll_MiniPlayer();

// To config Tx and Rx
HardwareSerial mySoftwareSerial(1);
DFRobotDFPlayerMini myDFPlayer;


void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(LED_OUT, OUTPUT);
  pinMode(MP3_NEXT, INPUT);
  pinMode(MP3_BACK, INPUT);
  pinMode(MP3_UP, INPUT);
  pinMode(MP3_DOWN, INPUT);
  pinMode(MP3_PLAY, INPUT);
  pinMode(MP3_STOP, INPUT);

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
  controll_MiniPlayer();
}

void set_up_MiniPlayer()
{
  mySoftwareSerial.begin(9600, SERIAL_8N1, 16, 17);  // speed, type, RX, TX
  Serial.begin(115200);
  
  Serial.println();
  Serial.println(F("DFRobot DFPlayer Mini Demo"));
  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));
  
  // Run MP3
  myDFPlayer.begin(mySoftwareSerial);
  
  // Check MP3 Serial 
  if (!myDFPlayer.begin(mySoftwareSerial)) {  //Use softwareSerial to communicate with mp3.
    Serial.println(myDFPlayer.readType(),HEX);
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while(true);
  }
  Serial.println(F("DFPlayer Mini online."));
  
  myDFPlayer.setTimeOut(500);                   //Set serial communictaion time out 500ms 
  myDFPlayer.volume(1);                         //----Set volume (0 - 30)
  myDFPlayer.EQ(DFPLAYER_EQ_NORMAL);            //----Set different EQ----
  myDFPlayer.outputDevice(DFPLAYER_DEVICE_SD);  //----Set device we use SD as default----

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
 //-----------------------------------------------------------------------------------------//
  // Controll with GPIO
 if(digitalRead(MP3_NEXT)!=0)
 {
    Serial.println(F("next--------------------"));
    myDFPlayer.next();
    Serial.println(myDFPlayer.readCurrentFileNumber()); //read current play file number
 }
 else if (digitalRead(MP3_BACK)!=0)
 {
   Serial.println(F("previous--------------------"));
      myDFPlayer.previous();
      Serial.println(myDFPlayer.readCurrentFileNumber()); //read current play file number
 }
 else if (digitalRead(MP3_UP)!=0)
 {+
    Serial.println(F("up--------------------"));
    myDFPlayer.volumeUp();
    Serial.println(myDFPlayer.readVolume()); //read current volume
    delay(500);
 }
 else if (digitalRead(MP3_DOWN)!=0)
 {
    Serial.println(F("down--------------------"));
    myDFPlayer.volumeDown();
    Serial.println(myDFPlayer.readVolume()); //read current volume
    delay(500);
 }
 else if (digitalRead(MP3_PLAY)!=0)
 {
    Serial.println(F("start--------------------"));
    myDFPlayer.start();
 }
 else if (digitalRead(MP3_STOP)!=0)
 {
    Serial.println(F("pause--------------------"));
    myDFPlayer.pause();
 }
 
  //-----------------------------------------------------------------------------------------//
  // Controll with UART
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
 //-----------------------------------------------------------------------------------------//
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
            client.print("Click <a href=\"/N\">here</a> to next MP3.<br>");
            client.print("Click <a href=\"/B\">here</a> to previous MP3.<br>");
            client.print("Click <a href=\"/U\">here</a> to up Volume sound.<br>");
            client.print("Click <a href=\"/D\">here</a> to down Volume sound.<br>");
            client.print("Click <a href=\"/P\">here</a> to Play sound.<br>");
            client.print("Click <a href=\"/S\">here</a> to stop sound.<br>");
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
        if (currentLine.endsWith("GET /N")) {
          Serial.println(F("next--------------------"));
          myDFPlayer.next();
          Serial.println(myDFPlayer.readCurrentFileNumber()); //read current play file number
        }
        else if (currentLine.endsWith("GET /B")) {
          Serial.println(F("previous--------------------"));
          myDFPlayer.previous();
          Serial.println(myDFPlayer.readCurrentFileNumber()); //read current play file number
        }
        else if (currentLine.endsWith("GET /U")) {
          Serial.println(F("up--------------------"));
          myDFPlayer.volumeUp();
          Serial.println(myDFPlayer.readVolume()); //read current volume              
        }
        else if (currentLine.endsWith("GET /D")) {
          Serial.println(F("down--------------------"));
          myDFPlayer.volumeDown();
          Serial.println(myDFPlayer.readVolume()); //read current volume             
        }
        else if (currentLine.endsWith("GET /P")) {
          Serial.println(F("start--------------------"));
          myDFPlayer.start();         
        }
        else if (currentLine.endsWith("GET /S")) {
          Serial.println(F("pause--------------------"));
          myDFPlayer.pause();
        }
        else if (currentLine.endsWith("GET /bell")) {
              // Controll the MP3 Next or Up-Down the Volume
              /*
              Do something here with controller MP3
              */
        }
      }
    }
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected.");
    
  }
//-----------------------------------------------------------------------------------------//
  // Next when the current MP3 is finish
 if (myDFPlayer.available()) {
 if (myDFPlayer.readType()== DFPlayerPlayFinished) {
    Serial.println(myDFPlayer.read());
    Serial.println(F("next--------------------"));
     myDFPlayer.next();  //Play next mp3 every 3 second.
    Serial.println(F("readCurrentFileNumber--------------------"));
    Serial.println(myDFPlayer.readCurrentFileNumber()); //read current play file number
    delay(500);
  }
 }  
}

