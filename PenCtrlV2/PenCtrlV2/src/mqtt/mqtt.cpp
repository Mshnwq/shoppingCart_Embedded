#include "mqtt.h"

// Setup function for mqtt

void mqttSetup(){
    // subscribe to mqtt broker
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
}
// callback function for receiving MQTT messages
void mqttCallback(char *topic, byte *payload, unsigned int length)
{
//   Serial.print("Message received [");
//   Serial.print(topic);
//   Serial.print("]: ");

//   for (int i = 0; i < length; i++)
//   {
//     Serial.print((char)payload[i]);
//   }

//   Serial.println();
  DeserializationError error = deserializeJson(docBuf, payload, length);
  if (error)
  {
    Serial.print("JSON deserialization failed: ");
    Serial.println(error.c_str());
    return;
  }
  const char *mqtt_type = docBuf["mqtt_type"]; // Assuming the payload contains a field named "message"

    if (strcmp(mqtt_type, "check_weight") == 0) {
      process = docBuf["process"];
      item_barcode = docBuf["item_barcode"];
    }
    if (strcmp(mqtt_type, "request_add_item") == 0) {
        mode = 1; // when reciving add item request update pentration mode to weighing
    }
    if ((strcmp(mqtt_type, "response_add_item") == 0) && (strcmp(docBuf["status"], "item_not_found") == 0)) {
        mode = 0; // when item was not found set mode back to active
    }
    if((strcmp(mqtt_type, "scale_confirmation") == 0) && (strcmp(docBuf["status"], "fail") == 0)){
      mode = 0; // when scale readings doesn't match role back to active mode
    }
    if((strcmp(mqtt_type, "scale_confirmation") == 0) && (strcmp(docBuf["status"], "pass") == 0)){
      mode = 2; // when scale matches change to transient mode
    }
    if((strcmp(mqtt_type, "alarm_detection") == 0) && (docBuf["trigger"] == false)){
      errorStatus = 0; // reset the error flag
    }
    if((strcmp(mqtt_type, "request_remove_item") == 0)){
      mode = 3; // first stage of remove item penetration
    }
  }

// status 0 = success, 1 = error, final succ
void publishMqtt(int status){
    StaticJsonDocument<256> pub;
    pub["mqtt_type"] = "penetration_data";
    pub["sender"] = "cart-slave-1";
    pub["status"] = status;
    pub["process"] = process;
    pub["item_barcode"] = item_barcode;
    String jsonString;
    serializeJson(pub, jsonString);
    const char *myChar = jsonString.c_str();
    mqttClient.publish(TOPIC_PUB, myChar);
    Serial.println("send pass to mqtt");
}