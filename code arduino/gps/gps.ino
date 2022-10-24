#include <SoftwareSerial.h >
SoftwareSerial gps(7,8);
// test 2
String message = "";

void setup(){
  Serial.begin(9600);
  gps.begin(9600);
}

void loop(){
  Serial.print("boucle");

  message="";

    while(message.indexOf("GPGGA")==-1){
        message="";
        if (gps.available()){
            message=gps.readStringUntil('$');
        }
  }

  Serial.println(message);
  
  if (message.substring(0,5) == "GPGGA"){
    
    String latitude;   
    latitude = latitude + message.substring(17,26);   
    if(message[27] == 'N'){
      latitude = latitude + " Nord";   
    }else{
      latitude = latitude + " Sud";    
    }
    Serial.println ("Latitude: " + latitude);

    String longitude;
    longitude = longitude + message.substring(29,39);
    if(message[40] == 'W'){
      longitude = longitude + " Ouest";
    }else{
      longitude = longitude + " Est"; 
    }
    Serial.println ("Longitude: " + longitude);

    String altitude;
    altitude = altitude + message.substring(51,54);
    altitude = altitude + " MÃ¨tres";
    Serial.println("Altitude: " + altitude);
  }
  Serial.println("");
}
