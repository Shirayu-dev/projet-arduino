/* Démo du capteur numérique de lumière V1.0
* Pour commencer, connecter le capteur au port I2C d'Arduino.
*
* Source:http://www.seeedstudio.com

#include <Wire.h>
#include <Digital_Light_TSL2561.h>
void setup()
{
  Wire.begin(); 
  Serial.begin(9600);
  TSL2561.init(); 
}

void loop()
{  
  unsigned long  Lux;
  TSL2561.getLux();
  Serial.print("The Light value is: ");
  Serial.println(TSL2561.calculateLux(0,0,1));
  delay(1000);
  }*/
#include <math.h>

#define LIGHT_SENSOR A0//Grove - Light Sensor is connected to A0 of Arduino
const int ledPin=12;                 //Connect the LED Grove module to Pin12, Digital 12
const int thresholdvalue=10;         //The treshold for which the LED should turn on. Setting it lower will make it go on at more light, higher for more darkness
float Rsensor; //Resistance of sensor in K
void setup() 
{
    Serial.begin(9600);                //Start the Serial connection
    pinMode(ledPin,OUTPUT);            //Set the LED on Digital 12 as an OUTPUT
}
void loop() 
{
  delay(1000);
    int sensorValue = analogRead(LIGHT_SENSOR); 
    Rsensor = (float)sensorValue /80 ;
    
    Serial.println("the analog read data is ");
    Serial.println("the sensor resistance is ");
    Serial.println(Rsensor,DEC);//show the ligth intensity on the serial monitor;
}
