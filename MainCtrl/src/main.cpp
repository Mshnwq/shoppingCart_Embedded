
/**
 * Date: Feb 3, 2023
 * Author: Hayan Al-Machnouk
 */

#include <Arduino.h>
// #include <FreeRTOS.h>
// #include <WebSocketsClient.h>
#include <PubSubClient.h>       // MQTT client
#include <cstdlib>
#include <string>
#include <ctime>
#include <cmath>
#include <iostream>
#include <ArduinoJson.h>        // For dealing with JSON 
#include <ArduinoHttpClient.h>  // for sending http request
#include "scale/main.h"
#include "wifi/wifi.h"
#include "accelerometer/acc.h"
#include "helper.h"
#include "lock/lock.h"

using namespace std;

// Use only core 1 for demo purposes
// #if CONFIG_FREERTOS_UNICORE
// static const BaseType_t app_cpu = 0;
// #else
// static const BaseType_t app_cpu = 1;
// #endif

// Task Handles
TaskHandle_t colecHandle;
TaskHandle_t penetHandle;
TaskHandle_t breakHandle;
TaskHandle_t alarmHandle;
TaskHandle_t scaleHandle;
TaskHandle_t resetHandle;
TaskHandle_t mpuHandle;
// Create a handle for the queue
QueueHandle_t xQueuePenet;
QueueHandle_t xQueueScale;

int DOUT_PIN = 16; // mcu > HX711 dout pin
int SCK_PIN = 17;
// LED Pin
static const int led_red = 12;
static const int led_blu = 14;
static const int ctrl_pin = 34;
static const int led_pin = LED_BUILTIN;
// LED blink rates
static const int blink_rate = 2000;  // ms


// MQTT broker details
// const char *BROKER = "192.168.30.66";
const char *BROKER = "192.168.90.66";
const int BROKER_PORT = 1883;
const int HTTP_PORT = 1111;

// MQTT topics
// not nown by default need to hit http request in order to authenticate
char *TOPIC_PUB = new char[63];
char *TOPIC_SUB = new char[63];
HttpClient httpClient = HttpClient(wifiClient, BROKER, HTTP_PORT); // http client
PubSubClient mqttClient(wifiClient); // mqttt client

// HX711 constructor:
HX711_ADC LoadCell(DOUT_PIN, SCK_PIN);

// Json Variable to Hold Sensor Readings
// JSONVar readings;

// sensors_event_t a, g, temp;
// float accX, accY, accZ, currX, currY, currZ;
float scaleReading = 0;

// global buffer for xqueue data exchance
StaticJsonDocument<256> docBuf;


void pentTask(void *pvParameters) {
  // Code for slave control task
  // while (true)
  // {
  //   int myArray[10] = {0,0,0,1,0,0,0,0,0};
  //   // Convert the array to a string representation
  //   String arrayStr;
  //   for (int i = 0; i < sizeof(myArray) / sizeof(int); i++)
  //   {
  //     arrayStr += to_string(myArray[i]) + " ";
  //   }
  //   // send data to mqtt server
  //   // mqtt_type: pent_data
  //   mqttClient.publish(TOPIC_PUB, data);
  //   vTaskDelay(1000/ portTICK_PERIOD_MS);
  // }
}

void scaleTask(void *pvParameters) {
  // Code for scale task
  StaticJsonDocument<256> receivedDoc;
  while (true)
  {
    // Wait for data to be received from the queue
    if (xQueueReceive(xQueueScale, &receivedDoc, portMAX_DELAY) == pdPASS)
    {
      // Data received successfully
      Serial.print("Received data: ");
      serializeJson(receivedDoc, Serial);
      Serial.println();
    }
    else
    {
      // Error receiving data
      Serial.println("Error receiving data");
    }
    float db_weight = receivedDoc["weight"];
    float scale_weight;
    float total =0;
    float comp;
    int count = 0;
    int motionDetected = 0;
    // Do something with the received data
    for(int i =0 ; i< 100; i++){
      static boolean newDataReady = 0;
      // check for new data/start next conversion:
      if (LoadCell.update())
        newDataReady = true;

      // get smoothed value from the dataset:
      if (newDataReady)
      {
        // check motion before and after reading
        float movement1 = checkMovement();
        scale_weight = LoadCell.getData();
        float movement2 = checkMovement();
        if (movement1 < 11 && movement2 < 11)
        {
          total += scale_weight;
          count++;
        }
        else
        {
          motionDetected++;
        }
      }else
      {
        Serial.println("No data available at ADC.");
      };
      vTaskDelay(1 / portTICK_PERIOD_MS);
    }
    
    StaticJsonDocument<256> pub;
    pub["mqtt_type"] = "scale_confirmation";
    pub["sender"] = "cart-1";
    pub["item_barcode"] = receivedDoc["item_barcode"];
    pub["process"] = receivedDoc["process"];
    float avg = total/count;
    comp = fabs(db_weight - avg);
    Serial.printf("Current scale readings: %.2f \n", avg);
    Serial.printf("Current db_weight: %.2f \n", db_weight);
    Serial.printf("Result: %.2f\n", comp);
    if(motionDetected >= 3){
      pub["status"] = "acce_fail";
      // Serialize the JSON object to a string
      String jsonString;
      serializeJson(pub, jsonString);
      const char *myChar = jsonString.c_str();
      mqttClient.publish(TOPIC_PUB, myChar);
      Serial.println("send pass to mqtt");
    }
    else if(comp <= 5){
      // Create a JSON object
       pub["status"] = "pass";
       // Serialize the JSON object to a string
       String jsonString;
       serializeJson(pub, jsonString);
       const char *myChar = jsonString.c_str();
       mqttClient.publish(TOPIC_PUB, myChar);
       Serial.println("send pass to mqtt");
    }
    else
    {
       // Create a JSON object
       pub["status"] = "fail";
       // Serialize the JSON object to a string
       String jsonString;
       serializeJson(pub, jsonString);
       const char *myChar = jsonString.c_str();
       mqttClient.publish(TOPIC_PUB, myChar);
       Serial.println("send fail to mqtt");
    }
  }
}

void alarmTask(void *pvParameters) {
  // Code for alarm task
  while (true)
  {
    /* code */ // TODO security protocol
    printf("ALARM\n");
    vTaskDelay(blink_rate*5 / portTICK_PERIOD_MS);
  }
  
}

void breakTask(void *pvParameters) {
  // Code for break task
  while (true)
  {
    /* code */ // TODO security protocol
    printf("BREAKS\n");
    vTaskDelay(blink_rate*5 / portTICK_PERIOD_MS);
  }
  
}
void penetration(void *pvParameters)
{
  // Code for break task
  while (true)
  {
    StaticJsonDocument<256> pube;
    pube["mqtt_type"] = "penetration_data";
    pube["sender"] = "cart-1";
       // Serialize the JSON object to a string
       String jsonString;
       serializeJson(pube, jsonString);
       const char *myChar = jsonString.c_str();
       Serial.println(mqttClient.publish(TOPIC_PUB, myChar));
       Serial.println("Published data!");
       vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

// MQTT TASK: keep connection alive with mqtt broker
void mqtt(void *parameter)
{
  while (1)
  {
    // check for incoming MQTT messages
    mqttClient.loop();
    // Serial.println("Mqtt connection is alive!");

    // wait for a few seconds
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}

// Scale update HX registers
void scaleUpdate(void *arameter){
  while(1){
    static boolean newDataReady = 0;
    // check for new data/start next conversion:
    if (LoadCell.update())
       newDataReady = true;

    // get smoothed value from the dataset:
    if (newDataReady)
    {
       float data = LoadCell.getData();
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

// callback function for receiving MQTT messages
void mqttCallback(char *topic, byte *payload, unsigned int length)
{
  // Serial.print("Message received [");
  // Serial.print(topic);
  // Serial.print("]: ");

  // for (int i = 0; i < length; i++)
  // {
  //   Serial.print((char)payload[i]);
  // }

  // Serial.println();
  DeserializationError error = deserializeJson(docBuf, payload, length);
  if (error)
  {
    Serial.print("JSON deserialization failed: ");
    Serial.println(error.c_str());
    return;
  }
  const char *mqtt_type = docBuf["mqtt_type"]; // Assuming the payload contains a field named "message"

  if (strcmp(mqtt_type, "check_weight") == 0) {
    const float actual_weight = docBuf["weight"];
    // resum tasks incase update failed
    // vTaskResume(resetHandle);
    // vTaskResume(mpuHandle); 
    xQueueSend(xQueueScale, &docBuf, 0);
  }
  if (strcmp(mqtt_type, "update_mode") == 0) {
      int mode = docBuf["mode"];
      updateMode(mode);
    }
    if (strcmp(mqtt_type, "alarm_detection") == 0)
    {
      boolean alarm = docBuf["trigger"];
      if(alarm)
        updateMode(4);
    }
  }

// Create an instance of the SerialDebug library
void setup() {

  Serial.begin(9600);
  Serial.printf("CurrentMode: %d\n", currentMode);
  // updateMode(1);
  pinMode(26, OUTPUT);
  pinMode(27, OUTPUT);
  Serial.printf("New mode: %d\n", currentMode);
  // updateMode(0); // set cart mode to Locked
  wifiSetup(); // connect to wifi
  // mpuSetup();
  // scaleSetup(); // scale setup

  // Connect Cart to the Backend using HTTP request
  httpClient.get("/api/v1/cart/start_cart/AN1kVAUYNynaPvk6nmyS3D6a36R42B2R0kQ338rcM7ERqF2O5GrERSco");

  // read the status code and body of the response
  int statusCode = httpClient.responseStatusCode();
  String response = httpClient.responseBody();

  const size_t capacity = JSON_OBJECT_SIZE(8) + 8*120;
  DynamicJsonDocument doc(capacity);
  DeserializationError error = deserializeJson(doc, response);

  if(statusCode == 200){
    if (error)
    {
      Serial.print("deserializeJson() failed: ");
      Serial.println(error.f_str());
    }
    else
    {
      const char *token = doc["token"];
      strcpy(TOPIC_PUB, "/cart/");
      strcpy(TOPIC_SUB, "/cart/");
      strcat(TOPIC_PUB, token);
      strcat(TOPIC_SUB, token);
    }
  }else
    Serial.println("Failed to connect Cart to Backend.");
  // connect to MQTT broker
  mqttClient.setServer(BROKER, BROKER_PORT);
  mqttClient.setCallback(mqttCallback);
  while (!mqttClient.connected())
  {
    if (mqttClient.connect("ESP32Client"))
    {
      Serial.println("Connected to MQTT broker.");
      mqttClient.subscribe(TOPIC_SUB);
      Serial.println("Subscribed to: ");
      Serial.println(TOPIC_SUB);
    }
    else
    {
      Serial.print("Failed to connect to MQTT broker, rc=");
      Serial.println(mqttClient.state());
      delay(1000);
    }
  }
  // xTaskCreatePinnedToCore(mqtt, "Mqtt client", 4096, NULL, 1, NULL, 1);
  // xTaskCreatePinnedToCore(mpuTask, "Accelerometer Task", 4096, NULL, 1, &mpuHandle, 1);
  // xTaskCreatePinnedToCore(breakTask, "Break Task", 1024, NULL, 1, &breakHandle, 1);
  // xTaskCreatePinnedToCore(scaleTask, "Scale Task", 4096, NULL, 1, &scaleHandle, 1);
  // xTaskCreatePinnedToCore(alarmTask, "Alarm Task", 1024, NULL, 1, &alarmHandle, 1);
  // xTaskCreatePinnedToCore(scaleUpdate, "Reset Scale Registires by update", 1024, NULL, 1, &resetHandle, 1);

  // Create the queue with a capacity of 10 integers
  xQueueScale = xQueueCreate(1, sizeof(StaticJsonDocument<256>));

  // xTaskCreatePinnedToCore(pentTask, "penteration sending", 1024, NULL, 1, &penetHandle, 1);
  xTaskCreatePinnedToCore( // Use xTaskCreate() in vanilla FreeRTOS
      mqtt,                // Function to be called
      "Mqtt client",       // Name of task
      4096,                // Stack size (bytes in ESP32, words in FreeRTOS)
      NULL,                // Parameter to pass to function
      1,                   // Task priority (0 to configMAX_PRIORITIES - 1)
      NULL,                // Task handle
      1);            // Run on one core for demo purposes (ESP32 only)
  xTaskCreatePinnedToCore( // Use xTaskCreate() in vanilla FreeRTOS
       penetration,                // Function to be called
       "Mqtt client",       // Name of task
       4096,                // Stack size (bytes in ESP32, words in FreeRTOS)
       NULL,                // Parameter to pass to function
       1,                   // Task priority (0 to configMAX_PRIORITIES - 1)
       NULL,                // Task handle
       1);                  // Run on one core for demo purposes (ESP32 only)
  xTaskCreatePinnedToCore( // Use xTaskCreate() in vanilla FreeRTOS
      scaleTask,                // Function to be called
      "Scale Task",       // Name of task
      4096,                // Stack size (bytes in ESP32, words in FreeRTOS)
      NULL,                // Parameter to pass to function
      1,                   // Task priority (0 to configMAX_PRIORITIES - 1)
      &scaleHandle,                // Task handle
      1);                  // Run on one core for demo purposes (ESP32 only)
  xTaskCreatePinnedToCore( // Use xTaskCreate() in vanilla FreeRTOS
      scaleUpdate,           // Function to be called
      "Keep scale synced with registers",        // Name of task
      1024,                // Stack size (bytes in ESP32, words in FreeRTOS)
      NULL,                // Parameter to pass to function
      1,                   // Task priority (0 to configMAX_PRIORITIES - 1)
      &resetHandle,        // Task handle
      1);                  // Run on one core for demo purposes (ESP32 only)
  // xTaskCreatePinnedToCore( // Use xTaskCreate() in vanilla FreeRTOS
  //     mpuTask,             // Function to be called
  //     "accelometer",       // Name of task
  //     4096,                // Stack size (bytes in ESP32, words in FreeRTOS)
  //     NULL,                // Parameter to pass to function
  //     1,                   // Task priority (0 to configMAX_PRIORITIES - 1)
  //     NULL,                // Task handle
  //     1);                  // Run on one core for demo purposes (ESP32 only)
  }

// Task to run forever
void loop() {
  // Do nothing
  // setup() and loop() run in their own task with priority 1 in core 1
  // on ESP32
}
