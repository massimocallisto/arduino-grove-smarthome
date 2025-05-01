
/*

Istruzioni collegmento:
- 3.3v su 3v3 della Shield
- GND su GND della Shield
- D5 su D3 della Shield
- Bottone Grove Kit su slot D3 della Shield

Assicurarsi che si illumina Led verde sulla Schield quando l'ESP32 è accesa
*/


#define BUTTON_PIN 5 

bool statoPrecedente = HIGH; // Stato precedente del bottone (pull-up)
char localLetter = 'A'; // Lettera associata all'ESP32
bool statoPrecedente = HIGH; // Stato precedente del bottone (pull-up)


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
  statoPrecedente = statoCorrente;
}

/*
....

*/


void loop()
{

  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate > 100)
  { // Ogni 0.5 secondi
    aggiornaLetteraConBottone();
    // ....
    lastUpdate = millis();
  }
}