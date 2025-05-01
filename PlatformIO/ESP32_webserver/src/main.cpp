#include <WiFi.h>

const char* ssid = "NOME_RETE_WIFI";
const char* password = "UNA_PASSWORD";

WiFiServer server(80);

void setup_wifi() {
  // Modalità Access Point
  WiFi.softAP(ssid, password);
  Serial.println("Access Point avviato");
  Serial.print("IP: ");
  Serial.println(WiFi.softAPIP());

  server.begin();
}

#define LED_PIN 2
bool ledState = false;

void setup() {
  Serial.begin(115200);

  setup_wifi();

}

void loop() {
  WiFiClient client = server.available();
  if (client) {
    String request = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        request += c;
        if (c == '\n' && request.endsWith("\r\n\r\n")) break;
      }
    }

    // Analisi richiesta
    if (request.indexOf("/req1") != -1) {
      ledState = true;
      Serial.println("Richiesta pagina 1");
    }
    if (request.indexOf("/req2") != -1) {
      Serial.println("Richiesta pagina 2");

    }

    // Risposta HTML
    client.println("HTTP/1.1 200 OK");
    client.println("Content-type:text/html");
    client.println();
    client.println("<!DOCTYPE html><html><head><title>ESP32 Web Server</title></head><body>");
    client.println("<h1>Hello World</h1>");
    client.println("<p><a href=\"/req1\"><button>Bottone 1</button></a></p>");
    client.println("</body></html>");
    client.stop();
  }
}
