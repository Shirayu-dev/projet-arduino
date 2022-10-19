#include <EEPROM.h>

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

const char VERSION[6] PROGMEM ="0.0.1";
const char NUM_LOT[11] PROGMEM ="0000000042";

#define settings_length 16
const int16_t DEFAULT_SETTINGS[settings_length] PROGMEM ={10,2048,30,1,255,768,1,-10,60,1,0,50,1,850,1080,17438};

int16_t settingTemp;

int timer=0;

void setup()
{
    Serial.begin(9600);
    checkSettings();
    configMode();
}

void loop()
{
}

void configMode()
{
    String command="";
    int place;
    //char character;
    while(timer<30){
        if(Serial.available()<=0) {
            Serial.println(F("{\"mode\":\"config\"}"));
            delay(200);
        }
        else {  
            delay(200);
            /*while(Serial.available()>0 && character!='=') {
                character=Serial.read();
                if(character!='=') command.concat(character);
            }*/

            command=Serial.readStringUntil('=');

            if(command=="senddata") sendCurrentSettings();
            else if(command=="version") {
                eraseSerial();
                sendVersion();
            }
            else if(command=="reset"){
                eraseSerial();
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
        }
    }
}

void eraseSerial(){
    while(Serial.available()>0) {
        Serial.read();
    }
}

void checkSettings(){
    EEPROM.get(checkSettingsExist,settingTemp);
    if(settingTemp!=DEFAULT_SETTINGS[settings_length-1]){
        reset();
    }
}

void reset(){
    for(int8_t i=0;i<settings_length;i++){
        settingTemp=pgm_read_word_near(DEFAULT_SETTINGS+i);
        EEPROM.put(i*2,settingTemp);
    }
}

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