
/**
 * Date: Feb 3, 2023
 * Author: Hayan Al-Machnouk
 */

#include <Arduino.h>
// #include <FreeRTOS.h>
// #include <WebSocketsClient.h>
#include <PubSubClient.h>       // MQTT client
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <iostream>
#include <ArduinoJson.h>        // For dealing with JSON 
#include <ArduinoHttpClient.h>  // for sending http request
#include "scale/main.h"
#include "wifi/wifi.h"
#include "accelerometer/acc.h"
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

static const int DOUT_PIN = 16; // mcu > HX711 dout pin
static const int SCK_PIN = 17;
// LED Pin
static const int led_red = 12;
static const int led_blu = 14;
static const int ctrl_pin = 34;
static const int led_pin = LED_BUILTIN;
// LED blink rates
static const int blink_rate = 2000;  // ms


// MQTT broker details
const char *BROKER = "192.168.172.66";
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

// Scale parameters
float currentWeight = 0;

// Task modes
lastMode;
currentMode;

// Our task: blink an LED at one rate
void toggleLED_1(void *parameter) {
  while(1) {
    digitalWrite(led_pin, HIGH);
    vTaskDelay(blink_rate/portTICK_PERIOD_MS);
    digitalWrite(led_pin, LOW);
    vTaskDelay(blink_rate/portTICK_PERIOD_MS);
    vTaskDelay(blink_rate/portTICK_PERIOD_MS);
    vTaskDelay(blink_rate/portTICK_PERIOD_MS);

    if (digitalRead(ctrl_pin) == HIGH) {
      printf("SUSPENDING\n");
      vTaskSuspend(scaleHandle);
      vTaskSuspend(slaveHandle);
    } else {
      printf("RESUMING\n");
      vTaskResume(scaleHandle);
      vTaskResume(slaveHandle);
    }
  }
}

void slaveCtrlTask(void *pvParameters) {
  // Code for slave control task
  while (true)
  {
    /* code */ // TODO Penetration
    printf("SLAVE\n");
    digitalWrite(led_red, HIGH);
    vTaskDelay(blink_rate/portTICK_PERIOD_MS);
    digitalWrite(led_red, LOW);
    vTaskDelay(blink_rate/portTICK_PERIOD_MS);
  }
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
    float comp;
    int count = 0;
    // Do something with the received data
    for(int i =0 ; i< 100; i++){
      static boolean newDataReady = 0;
      // check for new data/start next conversion:
      if (LoadCell.update())
        newDataReady = true;

      // get smoothed value from the dataset:
      if (newDataReady)
      {
        scale_weight = LoadCell.getData();
        comp = fabs(db_weight - scale_weight);
        Serial.printf("Current scale readings: %.2f \n", scale_weight);
        Serial.printf("Current db_weight: %.2f \n", db_weight);
        if(comp < 5){
          break;
        }
      }else
      {
        Serial.println("No data available at ADC.");
      };
      vTaskDelay(5 / portTICK_PERIOD_MS);
    }
    
    StaticJsonDocument<256> pub;
    pub["mqtt_type"] = "scale_confirmation";
    pub["sender"] = "cart-1";
    pub["item_barcode"] = receivedDoc["item_barcode"];
    Serial.printf("Result: %.2f\n", comp);
    if(comp <= 5) {
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

// MQTT TASK: keep connection alive with mqtt broker
void mqtt(void *parameter)
{
  while (1)
  {
    // check for incoming MQTT messages
    mqttClient.loop();
    // Serial.println("Mqtt connection is alive!");

    // publish a message to the MQTT broker
    // mqttClient.publish(TOPIC_PUB, "Hello, MQTT!");
    // Serial.println("sent new message!");

    // wait for a few seconds
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

// MPU TASK: Accelerometer task
void mpuTask(void *parameter)
{
  while (1)
  {
    checkMovement();
    vTaskDelay(1000 / portTICK_PERIOD_MS);
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
  Serial.print("Message received [");
  Serial.print(topic);
  Serial.print("]: ");

  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }

  Serial.println();
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
    .// resum tasks incase update failed
    vTaskResume(resetHandle);
    vTaskResume(mpuHandle); 
    xQueueSend(xQueueScale, &docBuf, 0);
  }
  if (strcmp(mqtt_type, "update_mode") == 0) {
    // update current
    currentMode = mqtt_type
    if (lastMode != currentMode) {
        Serial.print("Update mode");
        switch (currentMode) {
          case LOCKED:
            // locked tasks
            vTaskSuspend(scaleHandle);
            vTaskSuspend(alarmHandle);
            vTaskSuspend(penetHandle);
            vTaskSuspend(resetHandle);
            vTaskSuspend(mpuHandle);
            vTaskResume(breakHandle);
            break;
          case ALARM:
            // alarm tasks
            vTaskResume(scrtyHandle);
            break;
          case ACTIVE:
            // active tasks
            vTaskSuspend(resetHandle);
            vTaskSuspend(scaleHandle);
            vTaskSuspend(alarmHandle);
            vTaskSuspend(breakHandle);
            vTaskSuspend(mpuHandle);
            vTaskResume(penetHandle);
            break;
          case ERROR:
            break;
          case WEIGHING:
            // weighing tasks
            vTaskResume(breakHandle);
            vTaskResume(resetHandle);
            vTaskResume(mpuHandle);
            vTaskResume(penetHandle);
            break;
          case ABANDONED:
            // paid tasks 
            vTaskSuspend(resetHandle);
            vTaskSuspend(scaleHandle);
            vTaskSuspend(breakHandle);
            vTaskSuspend(alarmHandle);
            vTaskSuspend(resetHandle);
            vTaskSuspend(mpuHandle);
            vTaskResume(breakHandle);
            break;
          case PAID:
            // paid tasks
            vTaskSuspend(resetHandle);
            vTaskSuspend(scaleHandle);
            vTaskSuspend(breakHandle);
            vTaskSuspend(alarmHandle);
            vTaskSuspend(resetHandle);
            vTaskSuspend(mpuHandle);
            vTaskResume(penetHandle);
            break;
        }
      lastMode = currentMode;
      Serial.print("Last mode: ");
      Serial.println(lastMode);
    }
  }
}

// Create an instance of the SerialDebug library
void setup() {

  Serial.begin(9600);
  // Configure pins
  // pinMode(led_pin, OUTPUT);
  // pinMode(led_red, OUTPUT);
  // pinMode(led_blu, OUTPUT);
  // pinMode(ctrl_pin, INPUT);

  wifiSetup(); // connect to wifi
  mpuSetup();
  scaleSetup(); // scale setup

  // Connect Cart to the Backend using HTTP request
  httpClient.get("/api/v1/cart/start_cart/MuoHgSPyqIlr71w0ooqkhbLFhJalZry4KjYXgM0XxvUknuwRE2WrHw2o");

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
  xTaskCreatePinnedToCore(
      // PenetrationProcess,   // Function to be called
      toggleLED_1,   // Function to be called
      "PenetrationProcess", // Name of task
      4096,                 // Stack size in bytes
      NULL,                 // Parameter to pass to function
      1,                    // Task priority (0 to configMAX_PRIORITIES - 1)
      &penetHandle,   // Task handle
      0);                   // core to run on
  xTaskCreatePinnedToCore(mqtt, "Mqtt client", 4096, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(mpuTask, "Accelerometer Task", 4096, NULL, 1, &mpuHandle, 1);
  xTaskCreatePinnedToCore(breakTask, "Break Task", 1024, NULL, 1, &breakHandle, 1);
  xTaskCreatePinnedToCore(scaleTask, "Scale Task", 4096, NULL, 1, &scaleHandle, 1);
  xTaskCreatePinnedToCore(alarmTask, "Alarm Task", 1024, NULL, 1, &alarmHandle, 1);
  xTaskCreatePinnedToCore(scaleUpdate, "Reset Scale Registires by update", 1024, NULL, 1, &resetHandle, 1);

  // Create the queue with a capacity of 10 integers
  xQueuePenet = xQueueCreate(1, sizeof(StaticJsonDocument<256>));
  xQueueScale = xQueueCreate(1, sizeof(StaticJsonDocument<256>));
}

// Task to run forever
void loop() {
  // Do nothing
  // setup() and loop() run in their own task with priority 1 in core 1
  // on ESP32
}
