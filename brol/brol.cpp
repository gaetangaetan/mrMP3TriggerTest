// Structure pour le message d'animation reçu de l'onirigun
typedef struct {
    uint8_t type;            // 0xA0 = animation trigger
    uint8_t animationNumber; // numéro d'animation à déclencher
  } AnimationTriggerMessage;
  
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
    // En mode récepteur, traiter les paquets DMX ESP-NOW
  
    
    // En mode émetteur, traiter les messages d'animation (comportement original)
    if (len == sizeof(AnimationTriggerMessage)) {
      AnimationTriggerMessage msg;
      memcpy(&msg, incomingData, sizeof(msg));
      
      // Vérification du type pour éviter les données corrompues
      if (msg.type == 0xA0) {
        mp3.playTrack(msg.animationNumber);
      }
    }
  }