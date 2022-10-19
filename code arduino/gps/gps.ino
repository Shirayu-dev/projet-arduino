#include <SoftwareSerial.h >
SoftwareSerial gps(7,8);

String message = "";

void setup(){
  Serial.begin(9600);
  gps.begin(9600);
}

void loop(){
  gps.print(message);
  char carac = 0;
  while (carac != '\n'){
    if (gps.available())
    {
      carac = gps.read();
      message = message + carac;
    }
  }

  gps.print(message);
  
  if (message.substring(0,6) == "$GPGGA"){
    
    String latitude;   
    latitude = latitude + message.substring(18,27);   
    if(message[28] == 'N'){
      latitude = latitude + " Nord";   
    }else{
      latitude = latitude + " Sud";    
    }
    Serial.println ("Latitude: " + latitude);

    String longitude;
    longitude = longitude + message.substring(30,40);
    if(message[41] == 'W'){
      longitude = longitude + " Ouest";
    }else{
      longitude = longitude + " Est"; 
    }
    Serial.println ("Longitude: " + longitude);

    String altitude;
    altitude = altitude + message.substring(52,55);
    altitude = altitude + " MÃ¨tres";
    Serial.println("Altitude: " + altitude);
  }
  Serial.println("");
}
