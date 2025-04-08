
#include <Wire.h>



void setup() {
Wire.begin(18, 19); // SDA = 21, SCL = 22
  Serial.begin(115200);
  while (!Serial); // attende la connessione Serial Monitor
  Serial.println("\nScanner I2C avviato...");
}

void loop() {
  byte error, address;
  int nDevices = 0;

  Serial.println("Ricerca dispositivi I2C...");

  for (address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0) {
      Serial.print("Dispositivo I2C trovato all'indirizzo 0x");
      if (address < 16) Serial.print("0");
      Serial.print(address, HEX);
      Serial.println("  !");
      nDevices++;
    } else if (error == 4) {
      Serial.print("Errore sconosciuto all'indirizzo 0x");
      if (address < 16) Serial.print("0");
      Serial.println(address, HEX);
    }
  }

  if (nDevices == 0)
    Serial.println("Nessun dispositivo I2C trovato.");
  else
    Serial.println("Scansione completata.");

  delay(5000); // nuova scansione ogni 5 secondi
}
