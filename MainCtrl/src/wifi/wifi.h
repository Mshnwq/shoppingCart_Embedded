#include <WiFi.h>
#ifndef WIFI_H
#define WIFI_H
// #define WIFI_SSID "-"
// #define WIFI_PASS "-"

// #define WIFI_SSID "FPT"
// #define WIFI_PASS "g>66640C"
#define WIFI_SSID "FPT_132"
#define WIFI_PASS "tast6095"

// WiFi and MQTT objects
extern WiFiClient wifiClient;

boolean wifiSetup();
void reconnect(void *pvParameters);
void WiFiStationDisconnected(WiFiEvent_t event, WiFiEventInfo_t info);
#endif