
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <esp_now.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C  // Indirizzo I2C più comune per SSD1306
// --- CONFIGURAZIONE ID --- //


#define DEVICE_ID 1  // Cambiare 1..8 per ogni ESP32

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

char localLetter0 = 'H'; // Lettera che questo dispositivo trasmetterà
char localLetter1 = 'C'; // Lettera che questo dispositivo trasmetterà
char receivedLetter = '_'; // Lettera che questo dispositivo trasmetterà

// --- STRUTTURA MESSAGGIO --- //
typedef struct {
  uint8_t senderID;
  char letter;
} LetterMessage;

LetterMessage msg;

esp_now_peer_info_t peerInfo;
//uint8_t broadcastAddress[] = {0xC4, 0xD8, 0xD5, 0x95, 0xEC, 0x38};
// C4:D8:D5:95:EC:38 <-- 2
uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

// Invio broadcast
void broadcastLetter(int idx) {
  msg.senderID = DEVICE_ID;
  msg.letter = idx % 2 ? localLetter0 : localLetter1;
  //uint8_t broadcastAddr[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
  //esp_err_t result = esp_now_send(broadcastAddr, (uint8_t *)&msg, sizeof(msg));

  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &msg, sizeof(msg));
   
  if (result == ESP_OK) {
    Serial.println("Sent with success");
  }
  else {
    Serial.println("Error sending the data");
  }

  Serial.println("Broadcasted....");
}

// Callback ricezione
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  Serial.println("New message received....");

  LetterMessage received;
  memcpy(&received, incomingData, sizeof(received));
  Serial.println(received.letter);
  receivedLetter = received.letter;
}

void setupESPNow() {
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("Errore inizializzazione ESP-NOW");
    return;
  }
  esp_now_register_recv_cb(OnDataRecv);
}


void setup() {
  Wire.begin(18, 19);  // SDA = 21, SCL = 22
  Serial.begin(115200);

  setupESPNow();

  //Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  //Add peer
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }


  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 non trovato!"));
    while (true)
      ;
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("ESP32 Test EP-NOW");
  display.display();
}

int i = 0;
void loop() {
  // Niente da fare qui per ora
  Serial.println(i++);
  i = i > 255 ? 0 : i;

  broadcastLetter(i); // invia la propria lettera


  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("ESP32 Test EP-NOW");
  display.println(i);
  display.println(i % 2 ? localLetter0 : localLetter1);
  display.println(receivedLetter);

  display.display();
  delay(1000);
}
