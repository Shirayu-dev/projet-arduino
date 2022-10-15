#include <Arduino.h>
#include <SPI.h> // Pour la communication SPI
#include <SdFat.h>  // Pour la communication avec la carte SD
#include <RTClib.h> // Pour la gestion de l'horloge RTC
#include <Wire.h> // Pour la communication I2C
#include <TimeLib.h> // Pour la gestion du temps

// #include <DS1307RTC.h> // Pour la gestion de l'horloge RTC

/* Broche CS de la carte SD */
const byte SDCARD_CS_PIN = 4;
RTC_DS1307 rtc;
SdFat SD;
SdFile myFile;

void uploadSD(){
  char fileName[14];
  DateTime now = rtc.now();
  sprintf(fileName, "%02d%02d%02d_0.LOG", now.year()%100, now.month(), now.day());
  myFile.open(fileName, FILE_WRITE);
  if (myFile) {
    while (myFile.available()) { // read from the file until there's nothing else in it:
      Serial.write(myFile.read());
    }
    if(myFile.println("testing 1, 2, 3. -----------------------------------------")){// Faudra modif par toute les valeurs des capteurs !
      Serial.println("Write OK");
    }
    else{
      Serial.println("Write ERROR");
    }

    Serial.println("File Size : " + String(myFile.fileSize())); // Pour le debug uniquement !! A retirer !!
    if(myFile.fileSize() >= 2048){
      byte i = 0;
      do
      {
        i++;
        now = rtc.now();
        sprintf(fileName, "%02d%02d%02d_%d.LOG", now.year()%100, now.month(), now.day(),i);
      }while(SD.exists(fileName));
      Serial.println("Create : " + String(fileName)); // Pour le debug uniquement !! A retirer !!
      myFile.rename(fileName);
    }
    myFile.close();
  } else {
    // if the file didn't open, print an error:
    Serial.println("Error opening file");
  }
}


void setup() {
  Serial.begin(115200);
  Wire.begin();  //sets up the I2C


  // Initialisation de la carte SD
  pinMode(SDCARD_CS_PIN, OUTPUT);
  Serial.print(F("Init SD card... "));
  if (!SD.begin(SDCARD_CS_PIN)) {
    Serial.println(F("Card FAIL"));
  }
  Serial.println(F("Card OK"));


  // Initialisation de l'horloge RTC
  while (!rtc.begin()) {
    Serial.println("Attente du module RTC...");
    delay(5000);
  }
  if (!rtc.isrunning()) {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    Serial.println("Horloge du module RTC mise a jour");
  }
}

void loop() {
  if(!rtc.begin()) { // Serial.println(rtc.readSqwPinMode()); Permet de voir si horloge débranché ?
    Serial.println("Erreur RTC");
  }
  if (!SD.begin(SDCARD_CS_PIN)) {
    Serial.println(F("Erreur FAIL"));
  }
  
  uploadSD();
  delay(1000);
}