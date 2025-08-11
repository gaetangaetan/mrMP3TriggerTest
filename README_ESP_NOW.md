# mrMP3TriggerTest avec ESP-NOW

Ce projet permet au mrMP3TriggerTest de recevoir des messages ESP-NOW en broadcast et de déclencher des animations MP3 spécifiques.

## Fonctionnalités

- **Réception ESP-NOW** : Le dispositif écoute les messages ESP-NOW en mode broadcast
- **Déclenchement d'animations** : Quand un message de type `AnimationTriggerMessage` est reçu, la piste MP3 correspondante est jouée
- **Contrôle de lecture** : Support pour arrêter la lecture en cours
- **Structure de message** : Format standardisé pour la communication entre émetteur et récepteur

## Structure du message

```cpp
typedef struct {
    uint8_t type;            // Type de message (voir ci-dessous)
    uint8_t animationNumber; // numéro d'animation à déclencher (1-30)
} AnimationTriggerMessage;
```

## Types de messages supportés

- **0xA0** : Déclencher une animation (lecture d'une piste MP3)
- **0xA1** : Arrêter la lecture en cours

## Configuration

### Récepteur (mrMP3TriggerTest)

1. Compilez et téléversez le code sur votre ESP8266
2. Le dispositif se configure automatiquement en mode récepteur ESP-NOW
3. Il écoute les messages broadcast sur le canal WiFi

### Émetteur (pour test)

Vous pouvez utiliser n'importe quel ESP8266 configuré en mode émetteur ESP-NOW qui envoie des messages au format `AnimationTriggerMessage`.

## Utilisation

1. **Démarrage** : Le dispositif s'initialise et joue la première piste
2. **Réception** : Quand un message ESP-NOW arrive, il est traité automatiquement
3. **Lecture** : Si le type est `0xA0`, la piste correspondante est jouée
4. **Arrêt** : Si le type est `0xA1`, la lecture en cours s'arrête
5. **Logs** : Tous les événements sont affichés sur le moniteur série

## Dépendances

- `ESP8266WiFi` : Gestion WiFi et ESP-NOW
- `SparkFun Qwiic MP3 Trigger Arduino Library` : Contrôle du module MP3

## Dépannage

- **ESP-NOW ne s'initialise pas** : Vérifiez que l'ESP8266 est compatible
- **Messages non reçus** : Vérifiez que l'émetteur et le récepteur sont sur le même canal
- **Lectures incorrectes** : Vérifiez que les numéros de piste correspondent à votre SD card

## Structure du projet

```
mrMP3TriggerTest/
├── src/
│   └── main.cpp           # Code principal avec tout le code ESP-NOW intégré
├── platformio.ini         # Configuration PlatformIO avec dépendances
└── README_ESP_NOW.md      # Cette documentation
```

## Code d'exemple pour l'émetteur

Voici un exemple simple pour tester avec un autre ESP8266 :

```cpp
#include <ESP8266WiFi.h>
#include <espnow.h>

typedef struct {
    uint8_t type;
    uint8_t animationNumber;
} AnimationTriggerMessage;

void setup() {
    Serial.begin(115200);
    
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    
    if (esp_now_init() != 0) {
        Serial.println("Erreur ESP-NOW");
        return;
    }
    
    esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
    
    // Adresse de broadcast
    uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
    
    Serial.println("Émetteur prêt");
}

void loop() {
    if (Serial.available()) {
        int num = Serial.parseInt();
        if (num > 0 && num <= 30) {
            AnimationTriggerMessage msg;
            msg.type = 0xA0;  // Animation
            msg.animationNumber = num;
            
            uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
            esp_now_send(broadcastAddress, (uint8_t*)&msg, sizeof(msg));
            Serial.printf("Animation %d envoyée\n", num);
        }
    }
    delay(100);
}
```
