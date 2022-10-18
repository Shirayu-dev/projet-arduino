int timer=0;

const char VERSION[6]="0.0.1";
const char NUM_LOT[11]="0000000042";

#define LOG_INTERVALL 0
#define FILE_MAX_SIZE 4
#define TIMEOUT 8
#define LUMIN 12
#define LUMIN_LOW 16
#define LUMIN_HIGH 20
#define TEMP_AIR 24
#define MIN_TEMP_AIR 28
#define MAX_TEMP_AIR 32
#define HYGR 36
#define HYGR_MINT 40
#define HYGR_MAXT 44
#define PRESSURE 48
#define PRESSURE_MIN 52
#define PRESSURE_MAX 56
#define checkSettingsExist 60

const int32_t DEFAULT_SETTINGS[]={10,2048,30,1,255,768,1,-10,60,1,0,50,1,850,1080,740158678};

void setup()
{
    Serial.begin(9600);
    configMode();
}

void loop()
{
}

void configMode()
{
    String command="";
    char character;
    while(timer<30){
        if(Serial.available()<=0) {
            Serial.println(F("{\"mode\":\"config\"}"));
            delay(200);
        }
        else {  
            delay(200);
            while(Serial.available()>0) {
                character=Serial.read();
                command.concat(character);
            }
            Serial.print(F("{\"mode\":\"config\",\"print\":\""));
            Serial.print(command);
            Serial.println(F("\"}"));
        }
    }
}