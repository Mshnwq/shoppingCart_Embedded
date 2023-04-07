#include "wifi.h"
WiFiClient wifiClient;
boolean wifiSetup(){
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    WiFi.hostname("ESP32"); // Set the device name
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Wifi Connected!");
    return true;
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
