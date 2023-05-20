#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_sleep.h>
#include <Arduino.h>

// const gpio_num_t SWITCH_PIN = GPIO_NUM_4;
RTC_DATA_ATTR int bootCount = 0;

void buttonTask(void *pvParameters) {
  // Wait for switch press to initiate deep sleep
  while (digitalRead(GPIO_NUM_14) == HIGH) {
    Serial.println("esp is running");
    
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
  Serial.println("start sleeping...");

  // Switch pressed, initiate deep sleep
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_14, LOW);
  esp_deep_sleep_start();
}

void setup() {
  // pinMode(GPIO_NUM_33, INPUT_PULLUP);
   //Increment boot number and print it every re
  pinMode(16, OUTPUT);
  Serial.begin(9600);
  digitalWrite(16, HIGH);
  delay(3000);
  ++bootCount;
  Serial.println("Boot number: " + String(bootCount));
  xTaskCreatePinnedToCore(buttonTask, "Button Task", 4096, NULL, 1, NULL, 1);
}

void loop() {
  // Do nothing in loop
  // Serial.println("Task is running");
  // delay(1000);
}
// 2n3904 h331