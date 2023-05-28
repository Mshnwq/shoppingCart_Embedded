import paho.mqtt.client as mqtt
from time import sleep
import json, requests
import config.env as env

def on_connect(client, userdata, flags, rc):
    print("Connected with result code " + str(rc))

def on_message(client, userdata, message):
    topic = message.topic
    pay = message.payload.decode("utf-8")
    try:
        pay = json.loads(pay)
        print("message received " ,str(message.payload.decode("utf-8")))
        if pay['mqtt_type'] == "response_add_item":
            if pay['status'] == "scale_fail":
                sleep(5)
                request = {
                                "mqtt_type": "request_add_item",
                                "sender": "user-1",
                                "item_barcode": "123123",
                                "timestamp": 123445564
                            }
                client.publish(topic, json.dumps(request))
    except json.decoder.JSONDecodeError:
        print("message received " ,str(message.payload.decode("utf-8")))

client = mqtt.Client(client_id="mobile1")
# client.on_connect = on_connect
client.on_message = on_message
client.connect(host=env.URL, port=1883, keepalive=60)
client.loop_start()
print("connected")
req = requests.get(f"http://{env.URL}:1111/api/v1/cart/start_cart/lTJqvZc1KeLIR5yHYCkabMdU4evreusC0xyPHagMqnW3I97bM2AhL6fc")
print("request status: ", req.status_code)
# start the code by subscribing to channel
topic = "/cart/lTJqvZc1KeLIR5yHYCkabMdU4evreusC0xyPHagMqnW3I97bM2AhL6fc"
client.subscribe(topic)
# request add item
request = {
    "mqtt_type": "request_add_item",
    "sender": "user-1",
    "item_barcode": "123123",
    "timestamp": 123445564
}
client.publish(topic, json.dumps(request))
client.loop_start()
sleep(9999) 

