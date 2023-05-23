#include <PubSubClient.h> // Library for mqtt client
#include <ArduinoJson.h> // Library for json 
#include "wifi/wifi.h"
#include <stdint.h>  // Include the necessary header for uint8_t
#include <Arduino.h>
// MQTT broker details
const char *BROKER = "192.168.22.66";
const int BROKER_PORT = 1883;

// MQTT topics
// not nown by default need to hit http request in order to authenticate
// cart token
char cartToken[63] = "/cart/AN1kVAUYNynaPvk6nmyS3D6a36R42B2R0kQ338rcM7ERqF2O5GrERSco";
char *TOPIC_PUB = cartToken;
char *TOPIC_SUB = cartToken;

PubSubClient mqttClient(wifiClient); // mqttt client

StaticJsonDocument<256> docBuf;
int errorStatus = 0; // Error flag
int mode = 1; // initial mode 
char* process;
char* item_barcode;
void mqttCallback(char *topic, byte *payload, unsigned int length);
void mqttSetup();
void publishMqtt(int status);