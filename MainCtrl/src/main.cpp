
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
// TaskHandle_t websocketHandle; commented due to unused 
TaskHandle_t slaveHandle;
TaskHandle_t scrtyHandle;
TaskHandle_t scaleHandle;
// Create a handle for the queue
QueueHandle_t xQueue;

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
const char *BROKER = "192.168.172.66";
const int BROKER_PORT = 1883;
const int HTTP_PORT = 1111;
// const char* USERNAME = "your_mqtt_username"; CURRENTLY NO NEED FOR USERNAME AND PASSWORD
// const char* PASSWORD = "your_mqtt_password";

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

enum taskModes {
  LOCKED,
  INITIAL,
  ACTIVE,
  WEIGHING,
  ALARM,
  ERROR,
  ABANDONED,
  PAID,
};

// RTOS
taskModes currentMode = LOCKED;
taskModes lastMode = INITIAL;

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

// void onWebSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
//   switch (type) {
//     case WStype_DISCONNECTED:
//       break;
//     case WStype_CONNECTED:
//       break;
//     case WStype_TEXT:
//       // Update the state based on the received message
//       switch (payload[0]) { // TODO PROTOCOL
//         case '0':
//           break;
//         case '1':
//           currentMode = LOCKED;
//           break;
//         case '2':
//           currentMode = ACTIVE;
//           break;
//         case '3':
//           currentMode = ALARM;
//           break;
//         case '4':
//           currentMode = WEIGHING;
//           break;
//         case '5':
//           currentMode = PAID;
//           break;
//         case '6':
//           currentMode = LOCKED;
//           break;
//     }
//   }
// }

// void webSocketProcess(void *pvParameters) {
//   // Initialize WiFi
//   WiFi.mode(WIFI_STA);
//   WiFi.begin(WIFI_SSID, WIFI_PASS);

//   while (WiFi.status() != WL_CONNECTED) {
//     delay(1000);
//     Serial.print(".");
//     // Serial.print("Heap remaining WIFI (words): ");
//     // Serial.println(uxTaskGetStackHighWaterMark(NULL));

//     // Serial.print("Heap before WIFI (words): ");
//     // Serial.println(xPortGetFreeHeapSize());
//   }


//   // Address, Port, and URL path 
//   webSocket.begin(WEBSOCKET_ADDRESS, WEBSOCKET_PORT, "/ws/555");
//   // AebSocket event handler
//   webSocket.onEvent(onWebSocketEvent);
//   // if connection failed retry every 5s
//   webSocket.setReconnectInterval(5000);

//   Serial.println("");
//   Serial.println("Socket connected");
//   Serial.print("at Port: ");
//   Serial.println(WEBSOCKET_PORT);

//   while (true) {
//     webSocket.loop();
//     // Blink Blue LED if Socket is Active
//     digitalWrite(led_pin, HIGH);
//     vTaskDelay(blink_rate / portTICK_PERIOD_MS);
//     digitalWrite(led_pin, LOW);
//     vTaskDelay(blink_rate / portTICK_PERIOD_MS);

//     // Serial.print("Heap remaining WIFI (words): ");
//     // Serial.println(uxTaskGetStackHighWaterMark(NULL));

//     // Serial.print("Heap after WIFI (words): ");
//     // Serial.println(xPortGetFreeHeapSize());
    
//     Serial.print("Current mode: ");
//     Serial.println(currentMode);

//     if (lastMode != currentMode) {
//       Serial.print("Update mode");
//       switch (currentMode) {
//         case LOCKED:
//           // locked tasks
//           vTaskSuspend(scaleHandle);
//           vTaskSuspend(slaveHandle);
//           vTaskResume(scrtyHandle);
//           break;
//         case INITIAL:
//           // initial tasks
//           vTaskSuspend(scaleHandle);
//           vTaskSuspend(slaveHandle);
//           vTaskSuspend(scrtyHandle);
//           break;
//         case ALARM:
//         // alarm tasks
//           vTaskResume(scrtyHandle);
//           break;
//         case ACTIVE:
//           // active tasks
//           vTaskSuspend(scaleHandle);
//           vTaskResume (slaveHandle);
//           break;
//         case ERROR:
//           break;
//         case WEIGHING:
//           // weighing tasks
//           vTaskResume(scaleHandle);
//           vTaskResume(slaveHandle);
//           vTaskResume(scrtyHandle);
//           break;
//         case ABANDONED:
//           break;
//         case PAID:
//           // paid tasks
//           vTaskSuspend(scaleHandle);
//           vTaskResume(slaveHandle);
//           break;
//       }
//       lastMode = currentMode;
//       Serial.print("Last mode: ");
//       Serial.println(lastMode);
//     }
//   }
// }

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

    if (xQueueReceive(xQueue, &receivedDoc, portMAX_DELAY) == pdPASS)
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
    if(comp <= 5){
      // Create a JSON object
       pub["status"] = "pass";
       // Serialize the JSON object to a string
       String jsonString;
       serializeJson(pub, jsonString);
       const char *myChar = jsonString.c_str();
       mqttClient.publish(TOPIC_PUB, myChar);
       Serial.println("send pass to mqtt");
    }else{
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

void securityTask(void *pvParameters) {
  // Code for security task
  while (true)
  {
    /* code */ // TODO security protocol
    printf("SECURITY\n");
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

// MQTT TASK: keep connection alive with mqtt broker
void mpuTask(void *parameter)
{
  while (1)
  {
    checkMovement();
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

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

  if(strcmp(mqtt_type, "check_weight") == 0){
    const float actual_weight = docBuf["weight"];
    xQueueSend(xQueue, &docBuf, 0);
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
  // xTaskCreatePinnedToCore(
  //     toggleLED_1, // Function to be called
  //     // webSocketProcess,   // Function to be called
  //     "WebSocketProcess", // Name of task
  //     4096,               // Stack size in bytes
  //     NULL,               // Parameter to pass to function
  //     1,                  // Task priority (0 to configMAX_PRIORITIES - 1)
  //     NULL,   // Task handle
  //     0);                 // core to run on
  // xTaskCreatePinnedToCore(slaveCtrlTask, "SlaveCtrlTask", 2048, NULL, 1, &slaveHandle, 1);
  // xTaskCreatePinnedToCore(securityTask, "SecurityTask", 2048, NULL, 1, &scrtyHandle, 1);
  // xTaskCreatePinnedToCore(scaleTask, "ScaleTask", 2048, NULL, 1, &scaleHandle, 1);
  // Task to run forever

  // Create the queue with a capacity of 10 integers
  xQueue = xQueueCreate(1, sizeof(StaticJsonDocument<256>));

  xTaskCreatePinnedToCore( // Use xTaskCreate() in vanilla FreeRTOS
      mqtt,                // Function to be called
      "Mqtt client",       // Name of task
      4096,                // Stack size (bytes in ESP32, words in FreeRTOS)
      NULL,                // Parameter to pass to function
      1,                   // Task priority (0 to configMAX_PRIORITIES - 1)
      NULL,                // Task handle
      1);            // Run on one core for demo purposes (ESP32 only)
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
      NULL,        // Task handle
      1);                  // Run on one core for demo purposes (ESP32 only)
  xTaskCreatePinnedToCore( // Use xTaskCreate() in vanilla FreeRTOS
      mpuTask,             // Function to be called
      "accelometer",       // Name of task
      4096,                // Stack size (bytes in ESP32, words in FreeRTOS)
      NULL,                // Parameter to pass to function
      1,                   // Task priority (0 to configMAX_PRIORITIES - 1)
      NULL,                // Task handle
      1);                  // Run on one core for demo purposes (ESP32 only)
  }

void loop() {
  // Do nothing
  // setup() and loop() run in their own task with priority 1 in core 1
  // on ESP32
}
