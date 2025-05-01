#include <Arduino.h>

#define BUTTON_PIN 5  // Collega il bottone Grove a un pin digitale, ad esempio GPIO 4

char letteraCorrente = 'A';  // Iniziamo da 'A'
bool statoPrecedente = HIGH; // Stato precedente del bottone (pull-up)

// Funzione da chiamare periodicamente (es: dentro loop)
void aggiornaLetteraConBottone() {
  bool statoCorrente = digitalRead(BUTTON_PIN);
  Serial.println(statoCorrente);

  // Rilevazione pressione bottone (HIGH -> LOW, con pull-up)
  if (statoPrecedente == HIGH && statoCorrente == LOW) {
    letteraCorrente++;
    if (letteraCorrente > 'Z') {
      letteraCorrente = 'A'; // Torna ad 'A' se superiamo 'Z'
    }

    //Serial.print("Nuova lettera: ");
    //Serial.println(letteraCorrente);

    // Qui puoi inviare letteraCorrente via ESP-NOW
  }

  statoPrecedente = statoCorrente;
  Serial.println(letteraCorrente);

}

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT_PULLUP); // Grove bottone lavora bene con pull-up interno
}

void loop() {
  aggiornaLetteraConBottone();
  delay(50); // Debounce semplice
}
