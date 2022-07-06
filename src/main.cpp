#include <Arduino.h>
#include <mrOTA.h>

#include <ESP8266WiFiMulti.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

/*
  Controlling the Qwiic MP3 Trigger with I2C Commands
  By: Nathan Seidle
  SparkFun Electronics
  Date: January 12th, 2019
  License: This code is public domain but you buy me a beer if you use this and we meet someday (Beerware license).

  This example plays the first track on the SD card.

  Feel like supporting open source hardware?
  Buy a board from SparkFun! https://www.sparkfun.com/products/15165

  Hardware Connections:
  Plug in headphones
  Make sure the SD card is in the socket and has some MP3s in the root directory
  Don't have a USB cable connected to the Qwiic MP3 Trigger right now
  If needed, attach a Qwiic Shield to your Arduino/Photon/ESP32 or other
  Plug the Qwiic device onto an available Qwiic port
  Open the serial monitor at 115200 baud
*/

#include <Wire.h> //Needed for I2C to Qwiic MP3 Trigger
#define BUTTON1 D5
#define BUTTON2 D6


#include "SparkFun_Qwiic_MP3_Trigger_Arduino_Library.h" //http://librarymanager/All#SparkFun_MP3_Trigger
MP3TRIGGER mp3;
int songnumber = 1;


void setup()
{
      Serial.begin(115200);
  

  pinMode(BUTTON1,INPUT_PULLUP);
  //pinMode(BUTTON2,INPUT_PULLUP);

  //  WiFi.begin("OpenPoulpy", "youhououhou");  
  //   Serial.print("Connecting to ");
  // Serial.print("OpenPoulpy"); Serial.println(" ...");

  // int i = 0;
  // while (WiFi.status() != WL_CONNECTED) { // Wait for the Wi-Fi to connect
  //   delay(1000);
  //   Serial.print(++i); Serial.print(' ');
  // }

  // Serial.println('\n');
  // Serial.println("Connection established!");  
  // Serial.print("IP address:\t");
  // Serial.println(WiFi.localIP());         // Send the IP address of the ESP8266 to the computer

  checkOTA(BUTTON1,LOW,600000,"OpenPoulpy","youhououhou", "mrMP3Trigger","123");
  

  Wire.begin();

  //Check to see if Qwiic MP3 is present on the bus
  if (mp3.begin() == false)
  {
    Serial.println("Qwiic MP3 failed to respond. Please check wiring and possibly the I2C address. Freezing...");
    while (1)
      ;
  }

  mp3.setVolume(30); //Volume can be 0 (off) to 31 (max)
  
  delay(2000);
      Serial.print("Song count : ");
      byte songcount = mp3.getSongCount();
      Serial.println(songcount);

  
        mp3.playFile(1);
        delay(500);
      Serial.print("Playing the track number : ");
      Serial.print(mp3.getSongCount());
      Serial.print(" | Track name : ");
      Serial.println(mp3.getSongName());

  
}

void checkButtons()
{
  if(!digitalRead(BUTTON1))
  {
    
      mp3.playFile(1);
      delay(500);
      Serial.print("Playing the track number : ");
      Serial.print(mp3.getSongCount());
      Serial.print(" | Track name : ");
      Serial.println(mp3.getSongName());
    
  }
  
}

void loop()
{
     while (mp3.isPlaying())
  {
    delay(50); // Don't pound the Qwiic MP3 too hard!
    checkButtons();
  }
   delay(500);
  mp3.playNext();
  delay(500);
  Serial.print("Playing the track number : ");
  Serial.print(mp3.getSongCount());
  Serial.print(" | Track name : ");
  Serial.println(mp3.getSongName());
  
  // Serial.println(songnumber);
  //   mp3.playFile(songnumber);
  // songnumber++;
  
  delay(500);
  
  

  


  
  
}