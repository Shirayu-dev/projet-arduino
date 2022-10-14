void setup()
{
    Serial.begin(9600);
}

void loop()
{
    maintenanceMode();
}

void maintenanceMode()
{
    Serial.print(F("{\"mode\":\"mntc\""));

    printParameter("light","5");
    printParameter("hydro","10");
    printParameter("pression","1.013");
    printParameter("temp","22");
    printParameter("GPS","[0,0]");

    Serial.println(F("}"));
}

void printParameter(String paramName,String paramValue)
{
    Serial.print(F(",\""));
    Serial.print(paramName);
    Serial.print(F("\":"));
    Serial.print(paramValue);
}