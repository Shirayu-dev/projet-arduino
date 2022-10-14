#include <Arduino.h>
#include <SPI.h> // Pour la communication SPI
#include <SD.h>  // Pour la communication avec la carte SD

/* Broche CS de la carte SD */
const byte SDCARD_CS_PIN = 4;

void uploadSD(){
  if(SD.exists("toto.txt")) {
    Serial.println(F("Fichier toto.txt OK"));
  } else {
    Serial.println(F("Fichier toto.txt introuvable"));
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(SDCARD_CS_PIN, OUTPUT);
  Serial.print(F("Init SD card... "));
  if (!SD.begin(SDCARD_CS_PIN)) {
    Serial.println(F("FAIL"));
    for(;;); //  appui sur bouton RESET
  }
  Serial.println(F("OK"));
}

void loop() {
  Serial.println(F("Test"));
  delay(5000);
}