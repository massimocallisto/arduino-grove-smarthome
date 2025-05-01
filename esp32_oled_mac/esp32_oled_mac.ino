
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C  // Indirizzo I2C più comune per SSD1306

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

String MAC;

String get_MAC() {
  uint64_t mac = ESP.getEfuseMac();
  uint8_t macArray[6];
  for (int i = 0; i < 6; i++) {
    macArray[5 - i] = (mac >> ((5 - i) * 8)) & 0xFF;
  }

  char macStr[18];
  snprintf(macStr, 18, "%02x:%02x:%02x:%02x:%02x:%02x", macArray[0], macArray[1], macArray[2], macArray[3], macArray[4], macArray[5]);
  String mystring(macStr);
  Serial.println(mystring);
  mystring.toUpperCase();
  return mystring;
}


void setup() {
  Wire.begin(18, 19);  // SDA = 21, SCL = 22

  Serial.begin(115200);

  Serial.println("Get MAC address...");
  MAC = get_MAC();

  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 non trovato!"));
    while (true)
      ;
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("ESP32");
  display.println(MAC);
  Serial.println("....");
  
  display.display();
}

void loop() {
  // Niente da fare qui per ora
}
