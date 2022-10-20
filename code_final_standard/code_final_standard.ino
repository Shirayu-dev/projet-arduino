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

#define settings_length 16
//Liste des paramètres par défaut
const int16_t DEFAULT_SETTINGS[settings_length] PROGMEM ={10,2048,30,1,255,768,1,-10,60,1,0,50,1,850,1080,17438};
int16_t settingTemp;

#define SEALEVELPRESSURE_HPA (1013.25) // Pression atmosphérique au niveau de la mer.

//Pin des boutons rouge et bleu
#define redButton 2
#define blueButton 3

unsigned long tempTimeBlue = 0;
// unsigned long tempTimeRed = 0;

//Initialisation des différents périphériques

ChainableLED rgbLED(5, 6, 1); //LED
SoftwareSerial gps(7,8);
byte SDCARD_CS_PIN  = 4; // Broche CS de la carte SD
RTC_DS1307 rtc; //Horloge RTC
SdFat SD;
SdFile myFile;

//Variables utiles pour les mesures des capteurs

float temperature, humidity, pressure, lumin;
Adafruit_BME280 bme; // Initialisation du BME pour une utilisation via Bus I2C
String gpsMessage = "";
String latitude="";
String longitude="";
unsigned long time; // Stocker le temps depuis la dernière mesure

//État des différents modes

// byte maintenanceMode = false;
byte ecoMode = false;
byte ecoGPS = false;

void setup()
{
    Serial.begin(9600);
    Serial.println("setup");
    gps.begin(9600);
    bme.begin(0x76);   // 0x76 = I2C adresse si sur 3.3v
    Wire.begin();  //sets up the I2C
    pinMode(redButton, INPUT); // Initialisation bouton rouge
    pinMode(blueButton, INPUT); // Initialisation bouton bleu

    cli();           // disable all interrupts
    TCCR2A = (1<<WGM21)|(0<<WGM20); // Mode CTC
    TIMSK2 = (1<<OCIE2A); // Local interruption OCIE2A
    TCCR2B = (0<<WGM22)|(1<<CS22)|(1<<CS21); // Frequency 16Mhz/ 256 = 62500
    OCR2A = 250;       //250*125 = 31250 = 16Mhz/256/2
    sei();          // enable all interrupts

    if (!rtc.isrunning()) {
        rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
        //Serial.println("Horloge du module RTC mise a jour");
    }

    checkSettings();

    // if(digitalRead(redButton)==1) config(); //Si bouton rouge appuyé => mode configuration

    // attachInterrupt(digitalPinToInterrupt(redButton),toggleMaintenance,CHANGE);
    attachInterrupt(digitalPinToInterrupt(blueButton),toggleEco,CHANGE);

    ledManager();
    time=millis();  
}

void loop()
{
    // delay(200);
    Serial.println("loop");
    ledManager();
    // if(maintenanceMode) maintenance();
    // else 
    if(millis()>(time+(getParameter(LOG_INTERVALL)*(1+ecoMode)))){
        standart();
    }
}


// Erreur en paralèlle
static byte errorTab[2] = {0,0}; // Tableau pour stocker les erreurs
byte doubleError(){
  (getParameter(MIN_TEMP_AIR) < temperature && getParameter(MAX_TEMP_AIR) > temperature) ? errorTab[0] += 1 : errorTab[0] = 0;
  (getParameter(PRESSURE_MIN) < pressure && getParameter(PRESSURE_MAX) > pressure) ? errorTab[2] += 1 : errorTab[2] = 0;
  
  if(errorTab[0] > 1 || errorTab[2] > 1){
    return 1;
  }
  return 0;
}

ISR(TIMER2_COMPA_vect){         // timer compare interrupt service routine
  static byte timer2Counter = 0;
  static byte state = 0;

  
  if (++timer2Counter >= 125) { // 125 * 4 ms = 500 ms
    timer2Counter = 0;
    state = (state+1)%2;
    
    if(bme.begin(0x76)){
      state == 1 ? rgbLED.setColorRGB(0, 50, 0, 0) : rgbLED.setColorRGB(0, 0, 50, 0);
    }
  }

  if (++timer2Counter == 83 && doubleError()) { // 83 * 4 ms = 332 ms et un capteur abérant !
    timer2Counter = 0;
    state = (state+1)%3;
    if(bme.begin(0x76)){
      state == 1 ? rgbLED.setColorRGB(0, 50, 0, 0) : rgbLED.setColorRGB(0, 50, 50, 50);
    }
  }
}

//----------Interruptions----------

void toggleEco() {
  if((millis() - tempTimeBlue) >= 5000 && digitalRead(blueButton) < 0.5){
    ecoMode = !ecoMode;
  }
  tempTimeBlue = millis();
}

// void toggleMaintenance() {
//   if((millis() - tempTimeRed) >= 5000 && digitalRead(redButton) < 0.5){
//     maintenanceMode = !maintenanceMode;
//   }
//   tempTimeRed = millis();
// }

//----------Gestions des LEDS----------
void ledManager(){
    // while(!rtc.begin()) {
    //   Serial.println("Erreur RTC");
    //   rgbLED.setColorRGB(0, 50, 0, 0);
    //   delay(500);
    //   rgbLED.setColorRGB(0, 0, 0, 100);
    //   delay(500);
    // }
    // while(!SD.begin(SDCARD_CS_PIN)) {
    //   Serial.println(F("Erreur SD"));
    //   rgbLED.setColorRGB(0, 50, 0, 0);
    //   delay(333);
    //   rgbLED.setColorRGB(0, 50, 50, 50);
    //   delay(667);
    // }
    // while (SD.freeClusterCount() <= 1){
    //   Serial.println(F("SD Full"));
    //   rgbLED.setColorRGB(0, 50, 0, 0);
    //   delay(500);
    //   rgbLED.setColorRGB(0, 50, 50, 50);
    //   delay(500);
    // }
    
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

    // if(maintenanceMode){ // en maintenance
    //     rgbLED.setColorRGB(0, 50, 10, 0);
    // }
    // else 
    if(ecoMode){ // en éco
        rgbLED.setColorRGB(0, 0, 0, 50);
    }
    else{ // en standard
        rgbLED.setColorRGB(0, 0, 50, 0);
    }
}

//----------Fonctions pour le mode standart ou eco----------

void standart(){
    getMeasure();
    if(ecoMode&&!ecoGPS) {
        getGPS();
        ecoGPS=true;
    } else ecoGPS=false;
    
    uploadSD();
}

//----------Fonctions pour le mode config----------

//Mode configuration - Exit après 30min sans commande
// void config()
// {
//     rgbLED.setColorRGB(0,70,50,0);
//     delay(10000);
// }

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

//----------Fonctions pour le mode maintenance----------

// void maintenance(){}

void getGPS(){//structure gps
    do
    {
        if (gps.available()){
            gpsMessage=gps.readStringUntil('$');
        }
    } while (gpsMessage.substring(0,5) != "GPGGA"); //!maintenanceMode&&


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

//----------Upload SD----------
void uploadSD(){
  char fileName[14];
  DateTime now = rtc.now();
  sprintf(fileName, "%02d%02d%02d_0.LOG", now.year()%100, now.month(), now.day());
  myFile.open(fileName, FILE_WRITE);
  if (myFile) {
    while (myFile.available()) { // read from the file until there's nothing else in it:
      myFile.read();
    }

    //Afficher l'heure
    myFile.print(now.hour());
    myFile.print(F("h"));
    myFile.print(now.minute());

    if(getParameter(LUMIN)){
        myFile.print(F(" "));
        myFile.print((getParameter(LUMIN_LOW) > lumin ? getParameter(LUMIN_HIGH) < lumin ? "Hight" : "Low" : "Medium"));
    }

    if(getParameter(TEMP_AIR)){
        myFile.print(F(" "));
        myFile.print(temperature);
    }

    if(getParameter(HYGR)&&(temperature>getParameter(HYGR_MINT)&&temperature<getParameter(HYGR_MAXT))){
        myFile.print(F(" "));
        myFile.print(humidity);
    }

    if(getParameter(PRESSURE)){
        myFile.print(F(" "));
        myFile.print(pressure);
    }

    myFile.print(latitude);
    myFile.println(longitude);

    if(myFile.fileSize() >= getParameter(FILE_MAX_SIZE)){
      byte i = 0;
      do
      {
        i++;
        now = rtc.now();
        sprintf(fileName, "%02d%02d%02d_%d.LOG", now.year()%100, now.month(), now.day(),i);
      } while(SD.exists(fileName));

      myFile.rename(fileName);
    }
    myFile.close();
  }
}