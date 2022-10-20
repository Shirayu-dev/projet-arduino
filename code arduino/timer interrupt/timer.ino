#include <ChainableLED.h>
#include <Arduino.h>
#include <Adafruit_BME280.h>

#define SEALEVELPRESSURE_HPA (1013.25) // Pression atmosphérique au niveau de la mer.
unsigned long elapsedTime, previousTime; // Variable pour debug, à del
ChainableLED rgbLED(5, 6, 1);
Adafruit_BME280 bme; // Initialisation du BME pour une utilisation via Bus I2C
int LUMIN_LOW = 255;
int LUMIN_HIGH = 768;

void onTimer() { // Fonc pour test l'interval de temps, à del
  elapsedTime=millis()-previousTime;
  Serial.print(F("Time : "));
  Serial.print(elapsedTime);Serial.println(F("ms"));
  previousTime=millis();
}

void setup(){
  Serial.begin(9600);
  cli();           // disable all interrupts
  TCCR2A = (1<<WGM21)|(0<<WGM20); // Mode CTC
  TIMSK2 = (1<<OCIE2A); // Local interruption OCIE2A
  TCCR2B = (0<<WGM22)|(1<<CS22)|(1<<CS21); // Frequency 16Mhz/ 256 = 62500
  OCR2A = 250;       //250*125 = 31250 = 16Mhz/256/2
  sei();          // enable all interrupts
}

// à del on n'en veut pas !
int temperature = 0;
int humidity = 0;
int pressure = 0;
int lumin = 0;

void getData() {
  int lumin = analogRead(A0);
  Serial.println((LUMIN_LOW > lumin ? LUMIN_HIGH < lumin ? "Hight" : "Low" : "Medium")); // Faudra choisir si met valeur ou la texte idice
  temperature = bme.readTemperature();
  humidity = bme.readHumidity();
  pressure = bme.readPressure() / 100.0F;
  // altitude = bme.readAltitude(SEALEVELPRESSURE_HPA); bjr je ne sers à rien x)
}

byte MIN_TEMP_AIR, MAX_TEMP_AIR, MIN_HUMIDITY, MAX_HUMIDITY, PRESSURE_MIN, MAX_PRESSURE, MIN_LUMIN, MAX_LUMIN = 0;

static byte errorTab[2] = {0,0}; // Tableau pour stocker les erreurs
byte doubleError(){
  (MIN_TEMP_AIR < temperature && MAX_TEMP_AIR > temperature) ? errorTab[0] += 1 : errorTab[0] = 0;
  (PRESSURE_MIN < pressure && MAX_PRESSURE > pressure) ? errorTab[2] += 1 : errorTab[2] = 0;
  
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
    onTimer(); // juste pour le debug voir cb de temps s'est écoulé depuis la dernière fois
    
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

void loop(){
  delay(1500);
  Serial.println(F("Exit ..."));
}