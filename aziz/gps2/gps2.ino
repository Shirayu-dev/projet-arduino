
#define RX 0; 
#include <SoftwareSerial.h>
SoftwareSerial SoftSerial(7,8);
String message = "";

void setup()
{
   SoftSerial.begin(9600);
   Serial.begin(9600);
}

void loop()
{
  delay(2000);
  //Serial.print("Trame gps :");
  Serial.print(message);
  char carac = 0;
  // boucle qui attends un \n pour valider la trame et la décoder (/!\ Passer l'option en bas à droite du moniteur série en "Nouvelle ligne")
  while (carac != '\n')

  {
    // si un caractère est présent sur la liaison
    if(SoftSerial.available())
    {
      // lecture d'un caractère
      carac = SoftSerial.read();
      // concaténation du caractère au message
      message = message + carac; 
    }
  }
  
  Serial.print (message);
  
  if (message.substring(0,6) == "$GPRMC")
  {
    //Serial.print("Trame GPRMC Recue : "); 
    //Serial.println(message);
    
    // Décodage de la trame

    // latitude
    String latitude;   latitude = latitude + message.substring(20,29);   if(message[30] == 'N')  latitude = latitude + " Nord";   else      latitude = latitude + " Sud";  Serial.println ("Latitude: " + latitude);  
 
    // longitude
    String longitude;   longitude = longitude + message.substring(32,42); if(message[43] == 'W')  longitude = longitude + " Ouest";   else longitude = longitude + " Est"; Serial.println ("Longitude: " + longitude);
       
        
  }
  if (message.substring(0,6) != "$GPRMC"){ Serial.println(""); }
    
  else 
  Serial.println("");
  
 
  message = ""; 
  
}
