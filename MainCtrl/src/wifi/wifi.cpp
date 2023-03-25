#include "wifi.h"
WiFiClient wifiClient;
boolean wifiSetup(){
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Wifi Connected!");
    return true;
}