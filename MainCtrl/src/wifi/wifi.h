#include <WiFi.h>
#ifndef WIFI_H
#define WIFI_H
// #define WIFI_SSID "-"
// #define WIFI_PASS "-"

#define WIFI_SSID "FPT_132"
#define WIFI_PASS "tast6095"

// WiFi and MQTT objects
extern WiFiClient wifiClient;

boolean wifiSetup();
#endif