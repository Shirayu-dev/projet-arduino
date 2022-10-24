#include <Arduino.h>
#include <SPI.h> // Pour la communication SPI
#include <SdFat.h>  // Pour la communication avec la carte SD
#include <RTClib.h> // Pour la gestion de l'horloge RTC
#include <Wire.h> // Pour la communication I2C
#include <TimeLib.h> // Pour la gestion du temps
#include <ChainableLED.h>

#define redButton 2
#define blueButton 3

byte maintenanceMode = false;
byte ecoMode = false;
unsigned long tempTimeBlue = 0;
unsigned long tempTimeRed = 0;


void toggleEco() {
  if((millis() - tempTimeBlue) >= 5000 && digitalRead(blueButton) < 0.5){
    ecoMode = !ecoMode;
  }
  tempTimeBlue = millis();
}

void toggleMaintenance() {
  if((millis() - tempTimeRed) >= 5000 && digitalRead(redButton) < 0.5){
    maintenanceMode = !maintenanceMode;
  }
  tempTimeRed = millis();
}

void setup() {
  Serial.begin(115200);
  
  // Interruption
  pinMode(redButton, INPUT); // Initialisation bouton rouge
  pinMode(blueButton, INPUT); // Initialisation bouton bleu
  attachInterrupt(digitalPinToInterrupt(redButton),toggleMaintenance,CHANGE);
  attachInterrupt(digitalPinToInterrupt(blueButton),toggleEco,CHANGE);
  
}

void loop() {
  Serial.println("Maintenance : " + String(maintenanceMode) + " Eco : " + String(ecoMode));
  delay(1000);
}