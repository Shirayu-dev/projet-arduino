#include <Arduino.h>
#include <SPI.h> // Pour la communication SPI
#include <SdFat.h>  // Pour la communication avec la carte SD
#include <RTClib.h> // Pour la gestion de l'horloge RTC
#include <Wire.h> // Pour la communication I2C
#include <TimeLib.h> // Pour la gestion du temps
#include <ChainableLED.h>

#define redButton 2
#define blueButton 3


byte SDCARD_CS_PIN  = 4; // Broche CS de la carte SD

byte FILE_MAX_SIZE = 2048; // Taille maximale du fichier de log
ChainableLED rgbLED(5, 6, 1);
byte maintenanceMode = false;
byte ecoMode = false;
RTC_DS1307 rtc;
SdFat SD;
SdFile myFile;


void toggleMaintenance(){
  Serial.println("Red button pressed");
  DateTime start_temp = rtc.now();
  do{
    Serial.println("Waiting for red button");
    if(start_temp.secondstime() - rtc.now().secondstime() >= 5){
      maintenanceMode = !maintenanceMode;
      Serial.println("Maintenance mode: " + String(maintenanceMode));
      break;
    }
    Serial.println("Bah non");
    delay(100);
  }while (analogRead(redButton) >.5);
}

void toggleEco(){
  Serial.println("Blue button pressed");
}

void setup() {

  Serial.begin(115200);
  Wire.begin();  //sets up the I2C

  // Interruption
  pinMode(redButton, INPUT); // Initialisation bouton rouge
  pinMode(blueButton, INPUT); // Initialisation bouton bleu
  attachInterrupt(digitalPinToInterrupt(redButton),toggleMaintenance,RISING);
  attachInterrupt(digitalPinToInterrupt(blueButton),toggleEco,RISING);

  // Initialisation de la carte SD
  pinMode(SDCARD_CS_PIN, OUTPUT);
  Serial.print(F("Init SD card... "));
  if (SD.begin(SDCARD_CS_PIN)) {
    Serial.println(F("Card OK"));
  }
  

  if (!rtc.isrunning()) {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    Serial.println("Horloge du module RTC mise a jour");
  }
}

void loop() {
  delay(1000);
}