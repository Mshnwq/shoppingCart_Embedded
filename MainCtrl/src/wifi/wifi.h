#include <WiFi.h>
#ifndef WIFI_H
#define WIFI_H
// #define WIFI_SSID "-"
// #define WIFI_PASS "-"

// #define WIFI_SSID "FPT"
// #define WIFI_PASS "g>66640C"
#define WIFI_SSID "Faisal"
#define WIFI_PASS "123456788"

// WiFi and MQTT objects
extern WiFiClient wifiClient;

boolean wifiSetup();
#endif