#include <Arduino.h>
// #include <FreeRTOS.h>
// #include <task.h>
#include <WiFi.h>
// #include <WebSocketsServer.h>

/**
 * Date: Feb 3, 2023
 * Author: Hayan Al-Machnouk
 */

// Use only core 1 for demo purposes
#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif

// Pins
static const int led_pin = LED_BUILTIN;
// LED blink rates
static const int blink_rate = 1000;  // ms

#define WEBSOCKET_PORT 80
#define WIFI_SSID "stc_wifi_Attar"
#define WIFI_PASS "0543033373"

// WiFiServer server(WEBSOCKET_PORT);
// WebSocketsServer webSocket = WebSocketsServer(WEBSOCKET_PORT);

// Our task: blink an LED at one rate
void toggleLED_1(void *parameter) {
  while(1) {
    digitalWrite(led_pin, HIGH);
    vTaskDelay(blink_rate / portTICK_PERIOD_MS);
    digitalWrite(led_pin, LOW);
    vTaskDelay(blink_rate / portTICK_PERIOD_MS);
  }
}

void webSocketProcess(void *pvParameters) {
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // webSocket.begin();
  // webSocket.onEvent(onWebSocketEvent);

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  while (true) {
    // webSocket.loop();
    // Blink Blue LED if Socket is Active
    digitalWrite(led_pin, HIGH);
    vTaskDelay(blink_rate / portTICK_PERIOD_MS);
    digitalWrite(led_pin, LOW);
    vTaskDelay(blink_rate / portTICK_PERIOD_MS);
  }
}

void slaveCtrlTask(void *pvParameters) {
  // Code for slave control task
}

void scaleTask(void *pvParameters) {
  // Code for scale task
}

void securityTask(void *pvParameters) {
  // Code for security task
}

// void onWebSocketEvent(uint8_t client_num, WStype_t type, uint8_t * payload, size_t length) {
//   switch (type) {
//     case WStype_DISCONNECTED:
//       break;
//     case WStype_CONNECTED:
//       break;
//     case WStype_TEXT:
//       break;
//   }
// }

void setup() {

  Serial.begin(115200);

  // Configure pins
  pinMode(led_pin, OUTPUT);

  xTaskCreatePinnedToCore(
              toggleLED_1,   // Function to be called
              // webSocketProcess,   // Function to be called
              "WebSocketProcess", // Name of task
              1024,               // Stack size in bytes
              NULL,               // Parameter to pass to function
              1,                  // Task priority (0 to configMAX_PRIORITIES - 1)
              NULL,               // Task handle
              1);                 // core to run on
  // xTaskCreatePinnedToCore(securityTask, "SecurityTask", 1024, NULL, 1, NULL, 1);
  // xTaskCreatePinnedToCore(slaveCtrlTask, "SlaveCtrlTask", 1024, NULL, 1, NULL, 1);
  // xTaskCreatePinnedToCore(scaleTask, "ScaleTask", 1024, NULL, 1, NULL, 1);
}

void loop() {
  // Do nothing
  // setup() and loop() run in their own task with priority 1 in core 1
  // on ESP32
}
