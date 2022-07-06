/* definition of the checkOTA() function that checks for an OTA upload attempt at boot
OTABUTTON : pin number of a button, for checking OTA upload only when a button is pressed
            when OTABUTTON == -1, the OTA upload is checked everytime
BUTTONSTATE : state of the button that triggers the OTA check (typically LOW or HIGH)
OTAWAITTIME : wait time in ms for an OTA upload
ssid : network ssid
password : password for that network
hostname : name for the ESP on the network
hotspasword : pasword for the upload tool in the IDE

Don't forget to add the lines in plaformio.ini to switch from COM upload to WIFI (OTA) upload
#upload_protocol = esptool (comment or remove this line)
upload_protocol = espota
upload_port = 192.168.x.x (example of ESP IP)
upload_flags =
  --auth=123 (example of hostpassword)

  exemple d'utilisation : checkOTA(BOUTONBLANC,LOW,600000,"OpenPoulpy2","youhououhou", "mrFrom_gun","123");
*/
#include <ESP8266WiFi.h>
#include <ArduinoOTA.h>

void checkOTA(int OTABUTTON, int BUTTONSTATE, int OTAWAITTIME, const char *ssid,const char *password, const char* hostname, const char* hostpassword )
{
  pinMode(BUILTIN_LED,OUTPUT);
bool ota_flag = true;
Serial.print("test 1");
  if(OTABUTTON!=-1)
  {
      if(digitalRead(OTABUTTON)!=BUTTONSTATE)return;
  }

  Serial.print("test 2");
  //swipeAndStayOn(0,255,0,0,200,1);
  uint16_t time_elapsed = 0;
  uint16_t start_time;

  ota_flag = true;


  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED)
  {
    delay(500);
    ESP.restart();
  }

    Serial.println("Connection established!");  
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());  

  // Hostname
  ArduinoOTA.setHostname(hostname);

  // No authentication by default
  ArduinoOTA.setPassword(hostpassword);
  ArduinoOTA.onStart([]() {});
  ArduinoOTA.onEnd([]() { ESP.reset(); });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {});
  ArduinoOTA.onError([](ota_error_t error) {});
  ArduinoOTA.begin();

  
  if (ota_flag) // pendant un certain temps (ici, 5000 ms), on "écoute" les tentatives d'upload d'un nouveau code en OTA
  {

    start_time = millis();
    while (time_elapsed < OTAWAITTIME)
    {
      ArduinoOTA.handle();
      time_elapsed = millis() - start_time;
      digitalWrite(LED_BUILTIN,HIGH);
      delay(100);
      digitalWrite(LED_BUILTIN,LOW);
      delay(100);
      
    }
    ota_flag = false;
  }
      Serial.println("wifi.disconnect");  
  WiFi.disconnect();
  delay(10);
}
