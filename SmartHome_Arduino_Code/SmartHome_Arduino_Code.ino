#include "rgb_lcd.h"

#define TOUCH_PIN 2
#define BUTTON_PIN 6
#define LED_PIN 3
#define TEMP_SENSOR A0
#define LIGHT_SENSOR A1
#define SOUND_SENSOR A2
#define BUZZER_PIN 5

#define B 3975
#define LIGHT_THRESHOLD 50
#define SOUND_THRESHOLD 1


bool alarmActive = false;
bool doorOpen = false;

rgb_lcd lcd;



float read_temperature(){

  // Get the (raw) value of the temperature sensor.
  int val = analogRead(TEMP_SENSOR);

  // Determine the current resistance of the thermistor based on the sensor value.
  float resistance = (float)(1023-val)*10000/val;

  // Calculate the temperature based on the resistance value.
  float temperature = 1/(log(resistance/10000)/B+1/298.15)-273.15;

  return temperature;
}

void set_alarm(){
// Impostazione allarme con pulsante
  if (digitalRead(BUTTON_PIN) == HIGH) {
    alarmActive = !alarmActive;
    doorOpen = false;
    delay(500);  // debounce
  }

  // Attivazione automatica allarme se poca luce
  int lightLevel = analogRead(LIGHT_SENSOR);
  if (lightLevel < LIGHT_THRESHOLD) {
    alarmActive = true;
  }

  // Condizioni per attivare LED
  if (alarmActive) {
    digitalWrite(LED_PIN, HIGH);
  } else {
    digitalWrite(LED_PIN, LOW);
  }
}


void setup() {
  pinMode(TOUCH_PIN, INPUT);
  pinMode(BUTTON_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);

  // Inizializzazione della delle colonne e righe del display LCD
  lcd.begin(16, 2);
  // Stampa di un messaggio
  lcd.print("Avvio...");

  Serial.begin(9600);
}

void play_alarm(){
  /*if(1)
  return;*/
  digitalWrite(BUZZER_PIN, HIGH);
  delay(100);
  digitalWrite(BUZZER_PIN, LOW);
}

void print_info(){
  lcd.clear();
  // Lettura sensori
  int soundLevel = analogRead(SOUND_SENSOR);
  float temperatureC = read_temperature();
  lcd.setCursor(0, 0);
  lcd.print("T.:");
  lcd.setCursor(3, 0);
  lcd.print(temperatureC);

  lcd.setCursor(9, 0);
  lcd.print("L.:");
  int lightLevel = analogRead(LIGHT_SENSOR);
  lcd.setCursor(12, 0);
  lcd.print(lightLevel);

  lcd.setCursor(0, 1);
  lcd.print("Snd:");
  lcd.setCursor(4, 1);
  lcd.print(soundLevel);

  lcd.setCursor(8, 1);
  lcd.print("A:");
  lcd.setCursor(10, 1);
  lcd.print(alarmActive);

  lcd.setCursor(12, 1);
  lcd.print("D:");
  lcd.setCursor(14, 1);
  lcd.print(doorOpen);
}

void loop() {
  set_alarm();
  print_info();

  // Controllo apertura porta con touch
  if (digitalRead(TOUCH_PIN) == HIGH) {
    doorOpen = true;
    delay(500);  // debounce
  }

  // Allarme attivo con suono eccessivo
  int soundLevel = analogRead(SOUND_SENSOR);
  if (alarmActive && soundLevel < SOUND_THRESHOLD) {
    play_alarm();
  }

  // Allarme attivo e porta aperta
  if (alarmActive && doorOpen) {
    play_alarm();
  }


  delay(1000);
}
