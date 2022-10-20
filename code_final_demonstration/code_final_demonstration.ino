#include <EEPROM.h> // Lire et écrire dans la mémoire EEPROM
#include <ChainableLED.h> // LED
#include <Wire.h> // Pour la communication I2C
#include <Adafruit_BME280.h> // Capteur
#include <SoftwareSerial.h> // Pour le GPS
#include <SPI.h> // Pour la communication SPI
#include <SdFat.h>  // Pour la communication avec la carte SD
#include <RTClib.h> // Pour la gestion de l'horloge RTC
#include <TimeLib.h> // Pour la gestion du temps

//Emplacement dans l'EEPROM de chaque paramètre

#define LOG_INTERVALL 0
#define FILE_MAX_SIZE 2
#define TIMEOUT 4
#define LUMIN 6
#define LUMIN_LOW 8
#define LUMIN_HIGH 10
#define TEMP_AIR 12
#define MIN_TEMP_AIR 14
#define MAX_TEMP_AIR 16
#define HYGR 18
#define HYGR_MINT 20
#define HYGR_MAXT 22
#define PRESSURE 24
#define PRESSURE_MIN 26
#define PRESSURE_MAX 28
#define checkSettingsExist 30

//Numéro de version du programme
const char VERSION[2] PROGMEM ="1";
//Numéro de lot du module météo
const char NUM_LOT[3] PROGMEM ="42";

#define settings_length 16
//Liste des paramètres par défaut
const int16_t DEFAULT_SETTINGS[settings_length] PROGMEM ={10,2048,30,1,255,768,1,-10,60,1,0,50,1,850,1080,17438};
int16_t settingTemp;

//Pin des boutons rouge et bleu
#define redButton 2
#define blueButton 3

unsigned long tempTimeBlue = 0;
unsigned long tempTimeRed = 0;

//Initialisation des différents périphériques

ChainableLED rgbLED(5, 6, 1); //LED
SoftwareSerial gps(7,8);
byte SDCARD_CS_PIN  = 4; // Broche CS de la carte SD
RTC_DS1307 rtc; //Horloge RTC
SdFat SD;
<<<<<<< HEAD
// SdFile myFile;
=======
>>>>>>> c033426d78023fba373758f23b7241d72a2b36e0

//Variables utiles pour les mesures des capteurs

float temperature, humidity, pressure, lumin;
Adafruit_BME280 bme; // Initialisation du BME pour une utilisation via Bus I2C
String gpsMessage = "";
String latitude="";
String longitude="";

//État des différents modes

byte maintenanceMode = false;
byte ecoMode = false;

void setup()
{
    Serial.begin(9600);
    gps.begin(9600);
    bme.begin(0x76);   // 0x76 = I2C adresse si sur 3.3v
    pinMode(redButton, INPUT); // Initialisation bouton rouge
    pinMode(blueButton, INPUT); // Initialisation bouton bleu

    checkSettings();

    if(digitalRead(redButton)==1) config(); //Si bouton rouge appuyé => mode configuration

    attachInterrupt(digitalPinToInterrupt(redButton),toggleMaintenance,CHANGE);
    attachInterrupt(digitalPinToInterrupt(blueButton),toggleEco,CHANGE);

    ledManager();
}

void loop()
{
    ledManager();
    if(maintenanceMode) maintenance();
}

//----------Interruptions----------

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

//----------Gestions des LEDS----------
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
      delay(667);
    }
<<<<<<< HEAD
    while(!bme.begin(0x76)) {
      Serial.println(F("Erreur Capteurs"));
      rgbLED.setColorRGB(0, 50, 0, 0);
      delay(333);
      rgbLED.setColorRGB(0, 0, 50, 0);
      delay(667);
    }
    // while (SD.freeClusterCount() <= 1){
    //   Serial.println(F("SD Full"));
    //   rgbLED.setColorRGB(0, 50, 0, 0);
    //   delay(500);
    //   rgbLED.setColorRGB(0, 50, 50, 50);
    //   delay(500);
    // }
=======
    
>>>>>>> c033426d78023fba373758f23b7241d72a2b36e0
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

//----------Fonctions pour le mode config----------

//Mode configuration - Exit après 30min sans commande
void config()
{
    rgbLED.setColorRGB(0,70,50,0);
    String command="";
    int place;
    unsigned long timer=millis();
    while(millis()-timer<1800000){
        if(Serial.available()<=0) {
            Serial.println(F("{\"mode\":\"config\"}"));
            delay(200);
        }
        else {  
            delay(200);

            command=Serial.readStringUntil('=');

            if(command=="senddata") sendCurrentSettings();
            else if(command=="version") {
                sendVersion();
            }
            else if(command=="reset"){
                reset();
                Serial.println(F("{\"mode\":\"config\",\"answer\":\"Reset des paramètres par défaut effectué.\"}"));
            }
            else if(command=="put"){
                place=Serial.readStringUntil(':').toInt();
                settingTemp=Serial.readStringUntil('.').toInt();
                EEPROM.put(place,settingTemp);
                Serial.println(F("{\"mode\":\"config\",\"answer\":\"Valeur du paramètre modifié.\"}"));
            }
            Serial.println(F("{\"mode\":\"config\",\"state\":\"next\"}"));
            timer=millis();
        }
    }
}

//Vérifie si des paramètres sont sauvegardés dans l'EEPROM et sinon enregistre les paramètres par défaut dedans
void checkSettings(){
    EEPROM.get(checkSettingsExist,settingTemp);
    if(settingTemp!=DEFAULT_SETTINGS[settings_length-1]){
        reset();
    }
}

//Reset les paramètres
void reset(){
    for(int8_t i=0;i<settings_length;i++){
        settingTemp=pgm_read_word_near(DEFAULT_SETTINGS+i);
        EEPROM.put(i*2,settingTemp);
    }
}

//Envoyer les paramètres actuels sauvegardés dans l'EEPROM
void sendCurrentSettings(){
    Serial.print(F("{\"mode\":\"config\",\"currentSettings\":\""));
    for(int8_t i=0;i<settings_length;i++){
            EEPROM.get(i*2,settingTemp);
            Serial.print(i*2);
            Serial.print(F("="));
            Serial.print(settingTemp);
            if(i+1<settings_length) Serial.print(F(" "));
        }
    Serial.println(F("\"}"));
}

//Envoyer les infos de version et de numéro de lot
void sendVersion(){
    Serial.print(F("{\"mode\":\"config\",\"answer\":\"Numéro de version du programme : "));
    char myChar;
    for (int8_t k = 0; k < strlen_P(VERSION); k++) {
        myChar = pgm_read_byte_near(VERSION + k);
        Serial.print(myChar);
    }
    Serial.print(F(". Numéro de lot : "));
    for (int8_t k = 0; k < strlen_P(NUM_LOT); k++) {
        myChar = pgm_read_byte_near(NUM_LOT + k);
        Serial.print(myChar);
    }
    Serial.println(F(".\"}"));
}

//----------Fonctions pour le mode maintenance----------

void maintenance(){
    getMeasure();
    Serial.print(F("{\"mode\":\"mntc\""));
    Serial.print(F(",\"light\":\""));
    Serial.print(lumin);
    Serial.print(F(":"));
    Serial.print((getParameter(LUMIN_LOW) > lumin ? getParameter(LUMIN_HIGH) < lumin ? "Hight" : "Low" : "Medium"));
    Serial.print(F("\""));
    printParameter("hydro",humidity);
    printParameter("pression",pressure);
    printParameter("temp",temperature);
    getGPS();
    Serial.print(F(",\"gps\":[\""));
    Serial.print(latitude);
    Serial.print(F("\",\""));
    Serial.print(longitude);
    Serial.print(F("\"]"));

    Serial.println(F("}"));
}

void getGPS(){//structure gps

    if (gps.available()){
        gpsMessage=gps.readStringUntil('$');
    }


    if (gpsMessage.substring(0,5) == "GPGGA"){
        latitude = gpsMessage.substring(17,26);
        if(gpsMessage[27] == 'N'){
            latitude += " Nord";   
        } else {
            latitude += " Sud";    
        }

        longitude = gpsMessage.substring(29,39);
        if(gpsMessage[40] == 'W'){
            longitude += " Ouest";
        } else {
            longitude += " Est"; 
        }
    }
}

void printParameter(char paramName[],float paramValue)
{
    Serial.print(F(",\""));
    Serial.print(paramName);
    Serial.print(F("\":"));
    Serial.print(paramValue);
}

//----------Récupérer les mesures des capteurs----------

void getMeasure(){
    lumin = analogRead(A0);
    temperature = bme.readTemperature();
    humidity = bme.readHumidity();
    pressure = bme.readPressure() / 100.0F;
}

//----------Fonctions globales----------

int16_t getParameter(int id){
    EEPROM.get(id,settingTemp);
    return settingTemp;
}