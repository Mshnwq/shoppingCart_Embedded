#include "wifi.h"
WiFiClient wifiClient;
boolean wifiSetup(){
    WiFi.disconnect(true);
    delay(1000);
    WiFi.onEvent(WiFiStationDisconnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_DISCONNECTED);
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    WiFi.hostname("ESP32scale"); // Set the device name
    unsigned long startTime = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startTime < 2000)
    {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    if( WiFi.status() != WL_CONNECTED){
        Serial.println("failed to connect");
        // ESP.restart();
    }
    Serial.println("Wifi Connected!");
    return true;
}
void reconnect(void *pvParameters){
    while(1){

    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.println("WiFi disconnected, reconnecting...");
        WiFi.begin(WIFI_SSID, WIFI_PASS);
        while (WiFi.status() != WL_CONNECTED)
        {
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            Serial.println("Connecting to WiFi...");
        }
        Serial.println("Connected to WiFi");
    }
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void WiFiStationDisconnected(WiFiEvent_t event, WiFiEventInfo_t info){
  Serial.println("Disconnected from WiFi access point");
  Serial.print("WiFi lost connection. Reason: ");
  Serial.println(info.wifi_sta_disconnected.reason);
  Serial.println("Trying to Reconnect");
  WiFi.begin(WIFI_SSID, WIFI_PASS);
}
    // boolean wifiSetup()
    // {
    //     WiFi.disconnect();
    //     delay(1000);
    //     WiFi.mode(WIFI_STA);
    //     wifi_config_t config;
    //     memset(&config, 0, sizeof(config));
    //     strcpy((char *)config.sta.ssid, WIFI_SSID);
    //     strcpy((char *)config.sta.password, WIFI_PASS);
    //     config.sta.scan_method = WIFI_ALL_CHANNEL_SCAN;
    //     config.sta.sort_method = WIFI_CONNECT_AP_BY_SIGNAL;
    //     config.sta.threshold.rssi = -80;
    //     config.sta.pmf_cfg.capable = true;
    //     config.sta.pmf_cfg.required = false;
    //     config.sta.authmode = WIFI_AUTH_WPA2_ENTERPRISE;
    //     strcpy((char *)config.sta.wpa2_enterprise.username, "Your username");
    //     strcpy((char *)config.sta.wpa2_enterprise.password, "Your password");
    //     esp_wifi_set_config(WIFI_IF_STA, &config);
    //     esp_wifi_connect();
    // }

    // void WiFiEvent(WiFiEvent_t event)
    // {
    //     switch (event)
    //     {
    //     case SYSTEM_EVENT_STA_DISCONNECTED:
    //         Serial.println(F("STA Disconnected. Reconnecting..."));
    //         WiFi.begin();
    //         break;
    //     default:
    //         break;
    //     }
    // }
    // void wifiSetup()
    // {
    //     WiFi.mode(WIFI_MODE_STA);
    //     WiFi.begin(WIFI_SSID, WIFI_PASS);
    //     // You could do a lot of init here
    //     if (WiFi.waitForConnectResult() == WL_CONNECTED)
    //         Serial.println(F("WiFi connected"));

    //     else
    //         Serial.println(F("WiFi not connected"));
    //     WiFi.onEvent(WiFiEvent);
    // }
