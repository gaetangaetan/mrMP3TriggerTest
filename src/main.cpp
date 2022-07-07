#include <Arduino.h>
// #include <mrOTA.h>

// #include <ESP8266WiFiMulti.h>
// #include <ESP8266WebServer.h>
// #include <ESP8266mDNS.h>
// #include <WiFiUdp.h>
// #include <ESP8266HTTPClient.h>
// #include <WiFiClient.h>

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
int nbsongs = 30;


void setup()
{
  Serial.begin(115200);

  Wire.begin();

  //Check to see if Qwiic MP3 is present on the bus
  if (mp3.begin() == false)
  {
    Serial.println("Qwiic MP3 failed to respond. Please check wiring and possibly the I2C address. Freezing...");
    while (1)
      ;
  }

  if (mp3.hasCard() == false)
  {
    Serial.println("Qwiic MP3 is missing its SD card. Freezing...");
    while (1)
      ;
  }

  mp3.setVolume(30); //Volume can be 0 (off) to 31 (max)

  Serial.print("Song count: ");
  nbsongs=mp3.getSongCount();
  Serial.println(nbsongs);

  Serial.print("Volume level: ");
  Serial.println(mp3.getVolume());

  Serial.print("EQ Setting: ");
  byte eqSetting = mp3.getEQ();
  if (eqSetting == 0)
    Serial.print("Normal");
  else if (eqSetting == 1)
    Serial.print("Pop");
  else if (eqSetting == 2)
    Serial.print("Rock");
  else if (eqSetting == 3)
    Serial.print("Jazz");
  else if (eqSetting == 4)
    Serial.print("Classic");
  else if (eqSetting == 5)
    Serial.print("Bass");
  Serial.println();

  Serial.print("Firmware version: ");
  Serial.println(mp3.getVersion());

  Serial.print("Playing the first track: ");
  //mp3.playTrack(1);
  delay(2000); //Give the WT2003S a bit of time before we check to see which track is playing (allow song to start)
  String songName = mp3.getSongName();
  Serial.println(songName);
}

void loop()
{
  // delay(1000);
  Serial.print("Playing the next track: ");
  mp3.playFile(songnumber++);  
  while (mp3.isPlaying() == true)
  {
    delay(50); // Don't pound the Qwiic MP3 too hard!
  }

  
  // mp3.playNext();
  delay(500); //Give the WT2003S a bit of time before we check to see which track is playing (allow song to start)
  String songName = mp3.getSongName();
  Serial.println(songName);
  if(songnumber>nbsongs)songnumber = 1;
}