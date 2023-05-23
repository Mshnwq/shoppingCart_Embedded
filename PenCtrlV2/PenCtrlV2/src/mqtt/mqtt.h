#include <PubSubClient.h> // Library for mqtt client
#include <ArduinoJson.h> // Library for json 
#include "wifi/wifi.h"
#include <stdint.h>  // Include the necessary header for uint8_t
#include <Arduino.h>
#include <iostream>
// #include "main.h"
// MQTT broker details
extern const char *BROKER;
extern const int BROKER_PORT;

// MQTT topics
// not nown by default need to hit http request in order to authenticate
// cart token
extern char cartToken[63];
extern char *TOPIC_PUB;
extern char *TOPIC_SUB;

extern PubSubClient mqttClient; // mqttt client

extern StaticJsonDocument<256> docBuf;
extern int errorStatus; // Error flag
extern int mode; // initial mode 
extern const char* process;
extern const char* item_barcode;
void mqttCallback(char *topic, byte *payload, unsigned int length);
void mqttSetup();
void publishMqtt(int status);