#include <Arduino.h>

#include <WiFi.h>
#include <esp_now.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C
#define BUTTON_PIN 5 

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// CONFIGURAZIONE: Modificare questi valori per ogni ESP32
#define DEVICE_ID 3     // ID scelto ßda 1 a 10
char localLetter = 'A'; // Lettera associata all'ESP32
bool statoPrecedente = HIGH; // Stato precedente del bottone (pull-up)


// Struttura dati inviata via ESP-NOW
typedef struct
{
  uint8_t senderID;
  char letter;
} Message;

Message incomingMsg;

esp_now_peer_info_t peerInfo;
uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

// Stato della rete
char letters[11] = "__________"; // 10 lettere + terminatore
bool activeIDs[11] = {false};    // Tiene traccia degli ID attivi

// Funzione da chiamare periodicamente (es: dentro loop)
void aggiornaLetteraConBottone() {
  bool statoCorrente = digitalRead(BUTTON_PIN);

  // Rilevazione pressione bottone (HIGH -> LOW, con pull-up)
  if (statoPrecedente == HIGH && statoCorrente == LOW) {
    localLetter++;
    if (localLetter > 'Z') {
      localLetter = 'A'; // Torna ad 'A' se superiamo 'Z'
    }
  }
  letters[DEVICE_ID] = localLetter;
  statoPrecedente = statoCorrente;
}

void updateDisplay()
{
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  // Conta peer attivi
  int count = 0;
  for (int i = 1; i <= 10; i++)
  {
    if (activeIDs[i])
      count++;
  }

  display.setCursor(0, 0);
  display.print("ID: ");
  display.print(DEVICE_ID);
  display.print(" - ");
  display.print("Peers: ");
  display.println(count);
  display.println(" ");

  //display.setCursor(0, 15);
  for (int i = 1; i <= 10; i++)
  {
    display.print(letters[i]);
  }

  display.display();
}

// Callback ricezione dati
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len)
{
  memcpy(&incomingMsg, incomingData, sizeof(incomingMsg));

  uint8_t id = incomingMsg.senderID;
  if (id >= 1 && id <= 10)
  {
    letters[id] = incomingMsg.letter;
    activeIDs[id] = true;
    updateDisplay();
  }
}

// Funzione per inviare il proprio stato
void sendState()
{
  Message outgoing;
  outgoing.senderID = DEVICE_ID;
  outgoing.letter = localLetter;

  esp_now_send(broadcastAddress, (uint8_t *)&outgoing, sizeof(outgoing));
}

void setupESPNow()
{
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK)
  {
    Serial.println("Errore inizializzazione ESP-NOW");
    return;
  }
  esp_now_register_recv_cb(OnDataRecv);

  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  // Add peer
  if (esp_now_add_peer(&peerInfo) != ESP_OK)
  {
    Serial.println("Failed to add peer");
    return;
  }
}

void setup()
{
  Wire.begin(18, 19); // SDA = 21, SCL = 22
  Serial.begin(115200);

  // Inizializza Wi-Fi e ESP-NOW
  setupESPNow();

  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
  {
    Serial.println(F("SSD1306 non trovato!"));
    while (true)
      ;
  }

  // Inizializza OLED
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("ESP32");

  updateDisplay();

  activeIDs[DEVICE_ID] = true;
  letters[DEVICE_ID] = localLetter;
  delay(1000); // Attesa di stabilizzazione
  sendState(); // Invio iniziale
}

void loop()
{
  static unsigned long lastSend = 0;
  if (millis() - lastSend > 5000)
  { // Ogni 5 secondi
    sendState();
    //updateDisplay();
    lastSend = millis();
  }

  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate > 100)
  { // Ogni 0.5 secondi
    aggiornaLetteraConBottone();
    updateDisplay();
    lastUpdate = millis();
  }
}