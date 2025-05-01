
#include <WiFi.h>
#include <esp_now.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// TODO: Modificare questi valori per ogni ESP32
#define DEVICE_ID 1    // ID scelto da 1 a 10
char localLetter = '_'; // Lettera associata all'ESP32

// Struttura dati inviata via ESP-NOW
typedef struct {
  uint8_t senderID;
  char letter;
} Message;

Message incomingMsg;

// Stato della rete
char letters[11] = "__________"; // 10 lettere + terminatore
bool activeIDs[11] = {false};     // Tiene traccia degli ID attivi

void updateData() {
  // TODO: reset screen
  // ... 

  // TODO usa count per contare i peer attivi nell'array activeIDs
  int count = 0;
  // ... 


  // TODO: scorry l'array delle lettere oer stampare ciascuna lettera
  // ... 

}

// Callback ricezione dati
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
  memcpy(&incomingMsg, incomingData, sizeof(incomingMsg));

  uint8_t id = incomingMsg.senderID;
  if (id >= 1 && id <= 10) {
    // TODO: assegna lettera ricevuta e stato true all'indice id
    // ...

    updateData();
  }
}

// Funzione per inviare il proprio stato
void sendState() {
  Message outgoing;
  outgoing.senderID = DEVICE_ID;
  outgoing.letter = localLetter;

  uint8_t broadcastAddress[] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
  esp_now_send(broadcastAddress, (uint8_t *)&outgoing, sizeof(outgoing));
}

void setup() {
  Serial.begin(115200);
  Wire.begin(18, 19); // SDA = 21, SCL = 22

  // TODO: Inizializza OLED
  //...

  // Inizializza Wi-Fi e ESP-NOW
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("Errore inizializzazione ESP-NOW");
    return;
  }
  
  esp_now_register_recv_cb(OnDataRecv);

  delay(1000); // Attesa di stabilizzazione
}

void loop() {
  static unsigned long lastSend = 0;
  if (millis() - lastSend > 5000) { // Ogni 5 secondi
    sendState();
    lastSend = millis();
  }
}
