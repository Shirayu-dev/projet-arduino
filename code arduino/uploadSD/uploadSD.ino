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

byte r,g,b; // A supprimer !!!

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
        rgbLED.setColorRGB(0, r, g, b);
        delay(500);
        rgbLED.setColorRGB(0, r, g, b);
    }
    while(!SD.begin(SDCARD_CS_PIN)) {
        Serial.println(F("Erreur SD"));
        rgbLED.setColorRGB(0, r, g, b);
        delay(500);
        rgbLED.setColorRGB(0, r, g, b);
    }
    // while(GPS) {
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
        rgbLED.setColorRGB(0, r, g, b);
    }
    else if(ecoMode){ // en éco
        rgbLED.setColorRGB(0, r, g, b);
    }
    else{ // en standard
        rgbLED.setColorRGB(0, r, g, b);
    }
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
  
  ledManager();

  if (!rtc.isrunning()) {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    Serial.println("Horloge du module RTC mise a jour");
  }
}

void loop() {
  ledManager();
  uploadSD();
  delay(1000);
}