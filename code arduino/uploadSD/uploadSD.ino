#include <Arduino.h>
#include <SPI.h> // Pour la communication SPI
#include <SdFat.h>  // Pour la communication avec la carte SD
#include <RTClib.h> // Pour la gestion de l'horloge RTC
#include <Wire.h> // Pour la communication I2C
#include <TimeLib.h> // Pour la gestion du temps
#include <ChainableLED.h>
#include <SoftwareSerial.h >


#define redButton 2
#define blueButton 3

SoftwareSerial gps(7,8);
byte SDCARD_CS_PIN  = 4; // Broche CS de la carte SD

byte FILE_MAX_SIZE = 2048; // Taille maximale du fichier de log
ChainableLED rgbLED(5, 6, 1);
byte maintenanceMode = false;
byte ecoMode = false;
RTC_DS1307 rtc;
SdFat SD;
SdFile myFile;
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

void uploadSD(){
  char fileName[14];
  DateTime now = rtc.now();
  sprintf(fileName, "%02d%02d%02d_0.LOG", now.year()%100, now.month(), now.day());
  myFile.open(fileName, FILE_WRITE);
  if (myFile) {
    while (myFile.available()) { // read from the file until there's nothing else in it:
      Serial.write(myFile.read());
    }
    int sensorValue = 0; // Faudra modif par toute les valeurs des capteurs !
    if(myFile.println(sensorValue)){
      Serial.println("Write OK");
    }
    else{
      Serial.println("SD cart Full"); // Si la carte SD est pleine
      // led
      while (1);
    }

    Serial.println("File Size : " + String(myFile.fileSize())); // Pour le debug uniquement !! A retirer !!
    if(myFile.fileSize() >= FILE_MAX_SIZE){
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

void ledManager(){
    while(!rtc.begin()) {
      Serial.println("Erreur RTC");
      rgbLED.setColorRGB(0, 50, 0, 0);
      delay(500);
      rgbLED.setColorRGB(0, 0, 0, 100);
      delay(500);
    }
    while(!SD.begin(SDCARD_CS_PIN)) {
      Serial.println(F("Erreur SD"));
      rgbLED.setColorRGB(0, 50, 0, 0);
      delay(333);
      rgbLED.setColorRGB(0, 50, 50, 50);
      delay(666);
    }
    while (SD.freeClusterCount() <= 1){
      Serial.println(F("SD Full"));
      rgbLED.setColorRGB(0, 50, 0, 0);
      delay(500);
      rgbLED.setColorRGB(0, 50, 50, 50);
      delay(500);
    }
    
    // while(gps.available()) {
    //   Serial.println(F("Erreur GPS"));
    //   rgbLED.setColorRGB(0, r, g, b);
    //   delay(500);
    //   rgbLED.setColorRGB(0, r, g, b);
    // }
    // while(Accès capteur) { // Faudra aussi faire celle de la donnée incohérente
    //   Serial.println(F("Erreur accès GPS"));
    //   rgbLED.setColorRGB(0, r, g, b);
    //   delay(500);
    //   rgbLED.setColorRGB(0, r, g, b);
    // }

    if(maintenanceMode){ // en maintenance
        rgbLED.setColorRGB(0, 50, 10, 0);
    }
    else if(ecoMode){ // en éco
        rgbLED.setColorRGB(0, 0, 0, 50);
    }
    else{ // en standard
        rgbLED.setColorRGB(0, 0, 50, 0);
    }
}


void setup() {
  gps.begin(115200);
  Serial.begin(115200);
  Wire.begin();  //sets up the I2C

  // Interruption
  pinMode(redButton, INPUT); // Initialisation bouton rouge
  pinMode(blueButton, INPUT); // Initialisation bouton bleu
  attachInterrupt(digitalPinToInterrupt(redButton),toggleMaintenance,CHANGE);
  attachInterrupt(digitalPinToInterrupt(blueButton),toggleEco,CHANGE);
 
  ledManager();

  if (!rtc.isrunning()) {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    Serial.println("Horloge du module RTC mise a jour");
  }
}

void loop() {
  Serial.println("GPS : " + String(gps.available()) + " Capteur : " + String(1));
  Serial.println("" + String(SD.freeClusterCount()));
  Serial.println("Maintenane : " + String(maintenanceMode) + " Eco : " + String(ecoMode));
  ledManager();
  // uploadSD();
  delay(1000);
}