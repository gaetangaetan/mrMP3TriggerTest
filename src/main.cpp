#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <espnow.h>
// #include <mrOTA.h>

// #include <ESP8266WiFiMulti.h>
// #include <ESP8266WebServer.h>
// #include <ESP8266mDNS.h>
// #include <WiFiUdp.h>
// #include <ESP8266HTTPClient.h>
// #include <WiFiClient.h>

// Types de messages supportés
#define MSG_TYPE_ANIMATION_TRIGGER 0xA0  // Déclencher une animation
#define MSG_TYPE_STOP_PLAYBACK     0xA1  // Arrêter la lecture

// Structure pour le message d'animation reçu de l'onirigun
typedef struct {
    uint8_t type;            // 0xA0 = animation trigger, 0xA1 = stop
    uint8_t animationNumber; // numéro d'animation à déclencher (1-30)
} AnimationTriggerMessage;

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

//#include "SparkFun_Qwiic_MP3_Trigger_Arduino_Library.h" //http://librarymanager/All#SparkFun_MP3_Trigger
#include <SparkFun_Tsunami_Qwiic.h> //http://librarymanager/All#SparkFun_Tsunami_Super_WAV_Trigger

TsunamiQwiic tsunami;
char versionResponse[36];

//MP3TRIGGER mp3;
int songnumber = 1;
int nbsongs = 30;

// Variables pour la gestion du bouton
bool lastButtonState = HIGH;
bool buttonPressed = false;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50; // Délai anti-rebond en ms

// Fonction de réception des messages ESP-NOW
void OnDataRecv(unsigned char *mac, unsigned char *incomingData, unsigned char len) {
    Serial.println("Message ESP-NOW reçu !");
    
    if (len == sizeof(AnimationTriggerMessage)) {
        AnimationTriggerMessage msg;
        memcpy(&msg, incomingData, sizeof(msg));
        
        Serial.printf("Type: 0x%02X, Animation: %d\n", msg.type, msg.animationNumber);
        
        // Vérification du type pour éviter les données corrompues
        if (msg.type == MSG_TYPE_ANIMATION_TRIGGER) {
            Serial.printf("Déclenchement de l'animation %d\n", msg.animationNumber);
            
            // Vérifier si une lecture est en cours            
            tsunami.trackPlaySolo(msg.animationNumber, 0);
            Serial.printf("Lecture de la track %d\n", msg.animationNumber);
        } else if (msg.type == MSG_TYPE_STOP_PLAYBACK) {
            Serial.println("Arrêt de la lecture demandé");
            tsunami.trackStop(msg.animationNumber);
        } else {
            Serial.printf("Type de message non reconnu: 0x%02X\n", msg.type);
        }
    } else {
        Serial.printf("Taille de message incorrecte: %d (attendu: %d)\n", len, sizeof(AnimationTriggerMessage));
    }
}

// Fonction d'initialisation d'ESP-NOW
bool initESPNow() {
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    
    if (esp_now_init() != 0) {
        Serial.println("Erreur lors de l'initialisation d'ESP-NOW");
        return false;
    }
    
    esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
    esp_now_register_recv_cb(OnDataRecv);
    
    Serial.println("ESP-NOW initialisé en mode récepteur");
    Serial.println("En attente de messages d'animation...");
    Serial.printf("Types supportés: 0x%02X (animation), 0x%02X (stop)\n", 
                  MSG_TYPE_ANIMATION_TRIGGER, MSG_TYPE_STOP_PLAYBACK);
    
    return true;
}

void setup()
{
  Serial.begin(115200);

  // Configuration du bouton
  pinMode(BUTTON1, INPUT_PULLUP);

  Wire.begin();
  
  while(tsunami.begin() == false)
  {
    Serial.println("Tsunami Qwiic failed to respond. Please check wiring and possibly the I2C address. Waiting 1 second...");
    delay(1000);
  }


  Serial.print("Version du firmware: ");
  Serial.println(tsunami.getVersion(versionResponse));

  Serial.println("Reset du module");
  
  // Configuration ESP-NOW en mode récepteur
  while (!initESPNow()) {
    Serial.println("Échec de l'initialisation d'ESP-NOW, waiting 1 second...");
    delay(1000);
  }

  // Jouer le premier fichier au démarrage
  Serial.print("Playing the first track: ");
  tsunami.trackPlaySolo(1, 0);
  delay(2000); //Give the WT2003S a bit of time before we check to see which track is playing (allow song to start)
  
}

void loop()
{
  // Lecture de l'état du bouton avec anti-rebond
 /*
  bool reading = digitalRead(BUTTON1);
  
  // Si l'état a changé, réinitialiser le timer
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }
  
  // Si l'état est stable depuis le délai anti-rebond
  if ((millis() - lastDebounceTime) > debounceDelay) {
    // Si le bouton est pressé (LOW car INPUT_PULLUP) et qu'il n'était pas pressé avant
    if (reading == LOW && !buttonPressed) {
      buttonPressed = true;
      
      // Arrêter la lecture actuelle
      mp3.stop();
      delay(100);
      
      // Passer au fichier suivant
      songnumber++;
      if (songnumber > nbsongs) {
        songnumber = 1; // Retour au premier fichier
      }
      
      // Jouer le nouveau fichier
      Serial.print("Playing track: ");
      
      Serial.println(songnumber);
      mp3.playFile(songnumber);
      
      // Attendre un peu et récupérer le nom du fichier
      delay(1000);
      String songName = mp3.getSongName();
      Serial.print("Song name: ");
      Serial.println(songName);
    }
    
    // Si le bouton est relâché, réinitialiser l'état
    if (reading == HIGH) {
      buttonPressed = false;
    }
  }
  
  // Sauvegarder l'état du bouton
  lastButtonState = reading;
  
  // Vérifier si la lecture est terminée
  if (!mp3.isPlaying()) {
    // Si la lecture est terminée, on peut attendre le prochain appui
    delay(100);
  }
  */
}