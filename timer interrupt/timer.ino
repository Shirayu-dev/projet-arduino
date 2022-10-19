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

void setup() {

  Serial.begin(115200);
  Wire.begin();  //sets up the I2C

}

void loop() {
  delay(1000);
}