
/**
 * Date: Feb 3, 2023
 * Author: Hayan Al-Machnouk
 */

#include <Arduino.h>
// #include <FreeRTOS.h>
#include <WiFi.h>
#include <WebSocketsClient.h>

// Use only core 1 for demo purposes
#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif

// Task Handles
TaskHandle_t websocketHandle;
TaskHandle_t slaveCtrlHandle;
TaskHandle_t securityHandle;
TaskHandle_t scaleHandle;

// LED Pin
static const int led_pin = LED_BUILTIN;
// LED blink rates
static const int blink_rate = 2000;  // ms

#define WIFI_SSID "-"
#define WIFI_PASS "-"
#define WEBSOCKET_PORT 5555
#define WEBSOCKET_ADDRESS "api-airport-dev.scfs.tech"
WebSocketsClient webSocket; // websocket client instance

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
  }
}

void onWebSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:
      break;
    case WStype_CONNECTED:
      break;
    case WStype_TEXT:
      // Update the state based on the received message
      switch (payload[0]) { // TODO PROTOCOL
        case '0':
          vTaskSuspend(scaleHandle);
          vTaskSuspend(slaveCtrlHandle);
          break;
        case '1':
          currentMode = LOCKED;
          break;
        case '2':
          currentMode = ACTIVE;
          break;
        case '3':
          currentMode = ALARM;
          break;
        case '4':
          currentMode = WEIGHING;
          break;
        case '5':
          currentMode = PAID;
          break;
        case '6':
          currentMode = LOCKED;
          break;
    }
  }
}

void webSocketProcess(void *pvParameters) {
  // Initialize WiFi
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
    // Serial.print("Heap remaining WIFI (words): ");
    // Serial.println(uxTaskGetStackHighWaterMark(NULL));

    // Serial.print("Heap before WIFI (words): ");
    // Serial.println(xPortGetFreeHeapSize());
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Address, Port, and URL path 
  webSocket.begin(WEBSOCKET_ADDRESS, WEBSOCKET_PORT, "/ws/555");
  // AebSocket event handler
  webSocket.onEvent(onWebSocketEvent);
  // if connection failed retry every 5s
  webSocket.setReconnectInterval(5000);

  Serial.println("");
  Serial.println("Socket connected");
  Serial.print("at Port: ");
  Serial.println(WEBSOCKET_PORT);

  while (true) {
    webSocket.loop();
    // Blink Blue LED if Socket is Active
    digitalWrite(led_pin, HIGH);
    vTaskDelay(blink_rate / portTICK_PERIOD_MS);
    digitalWrite(led_pin, LOW);
    vTaskDelay(blink_rate / portTICK_PERIOD_MS);

    // Serial.print("Heap remaining WIFI (words): ");
    // Serial.println(uxTaskGetStackHighWaterMark(NULL));

    // Serial.print("Heap after WIFI (words): ");
    // Serial.println(xPortGetFreeHeapSize());

    if (lastMode != currentMode) {
      switch (currentMode) {
        case LOCKED:
          vTaskSuspend(scaleHandle);
          vTaskSuspend(slaveCtrlHandle);
          break;
        case INITIAL:
          break;
        case ALARM:
          break;
        case ACTIVE:
          break;
        case ERROR:
          break;
        case WEIGHING:
          // activate tasks
          vTaskResume(scaleHandle);
          vTaskResume(slaveCtrlHandle);
          break;
        case ABANDONED:
          break;
        case PAID:
          break;
      }
      lastMode = currentMode;
    }
  }
}

void slaveCtrlTask(void *pvParameters) {
  // Code for slave control task
  while (true)
  {
    /* code */ // TODO Penetration
  vTaskDelay(blink_rate / portTICK_PERIOD_MS);
  }
}

void scaleTask(void *pvParameters) {
  // Code for scale task
  while (true)
  {
    /* code */ // TODO scale
  vTaskDelay(blink_rate / portTICK_PERIOD_MS);
  }
}

void securityTask(void *pvParameters) {
  // Code for security task
  while (true)
  {
    /* code */ // TODO security protocol
  vTaskDelay(blink_rate / portTICK_PERIOD_MS);
  }
  
}

void setup() {

  Serial.begin(9600);

  // Configure pins
  pinMode(led_pin, OUTPUT);

  xTaskCreatePinnedToCore(
              toggleLED_1,        // Function to be called
              // webSocketProcess,   // Function to be called
              "WebSocketProcess", // Name of task
              4096,               // Stack size in bytes
              NULL,               // Parameter to pass to function
              1,                  // Task priority (0 to configMAX_PRIORITIES - 1)
              &websocketHandle,   // Task handle
              0);                 // core to run on
  xTaskCreatePinnedToCore(slaveCtrlTask, "SlaveCtrlTask", 2048, NULL, 1, &slaveCtrlHandle, 1);
  xTaskCreatePinnedToCore(securityTask, "SecurityTask", 2048, NULL, 1, &securityHandle, 1);
  xTaskCreatePinnedToCore(scaleTask, "ScaleTask", 2048, NULL, 1, &scaleHandle, 1);
}

void loop() {
  // Do nothing
  // setup() and loop() run in their own task with priority 1 in core 1
  // on ESP32
}
