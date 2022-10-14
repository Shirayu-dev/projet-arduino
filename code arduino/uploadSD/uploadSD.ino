#include <Arduino.h>
#include <SPI.h> // Pour la communication SPI
#include <SD.h>  // Pour la communication avec la carte SD
#include <RTClib.h> // Pour la gestion de l'horloge RTC
#include <Wire.h> // Pour la communication I2C
#include <TimeLib.h> // Pour la gestion du temps

// #include <DS1307RTC.h> // Pour la gestion de l'horloge RTC

/* Broche CS de la carte SD */
const byte SDCARD_CS_PIN = 4;
RTC_DS1307 rtc;


void uploadSD(){
  if(SD.exists("toto.txt")) {
    Serial.println(F("Fichier toto.txt OK"));
  } else {
    Serial.println(F("Fichier toto.txt introuvable"));
  }
}

void setup() {
  while (!Serial); // wait for serial port to connect. Needed for native USB port only
  Serial.begin(115200);
  Wire.begin();  //sets up the I2C

  pinMode(SDCARD_CS_PIN, OUTPUT);
  Serial.print(F("Init SD card... "));
  if (!SD.begin(SDCARD_CS_PIN)) {
    Serial.println(F("Card FAIL"));
    for(;;); //  appui sur bouton RESET
  }
  Serial.println(F("Card OK"));

  while (! rtc.begin()) {
    Serial.println("Attente du module RTC...");
    delay(5000);
  }
  if (! rtc.isrunning()) {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    Serial.println("Horloge du module RTC mise a jour");
  }
}

void loop() {
  DateTime now = rtc.now();
  Serial.println(now.unixtime());
  // char heure[10];
  // sprintf(heure, "Il est %02d:%02d:%02d", now.hour(), now.minute(), now.second());
  // Serial.println(heure);
  delay(5000);
}