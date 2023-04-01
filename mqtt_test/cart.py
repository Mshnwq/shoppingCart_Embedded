import paho.mqtt.client as mqtt
from time import sleep
import random, json, requests


print("connected")
req = requests.get("http://localhost:1111/api/v1/cart/start_cart/lTJqvZc1KeLIR5yHYCkabMdU4evreusC0xyPHagMqnW3I97bM2AhL6fc")
print("request status: ", req.status_code)

def on_message_recived(client, userdata, message):
    topic = message.topic
    pay = message.payload.decode("utf-8")
    try:
        pay = json.loads(pay)
        # pay = json.load(pay)
        print(pay, end ='\n')
        if pay['mqtt_type'] == "check_weight":
            # read weight
            random_number = random.randint(0, 100)
            if abs(random_number - pay['weight']) > 0:
            # if True:
                resp = {
                            "mqtt_type": "scale_confirmation",
                            "process": pay['process'],
                            "sender": "cart-id",
                            "item_barcode": pay["item_barcode"],
                            "status":"pass"
                        }
                client.publish(topic, json.dumps(resp))
                return
            else:
                resp = {
                            "mqtt_type": "scale_confirmation",
                            "process": pay['process'],
                            "sender": "cart-id",
                            "item_barcode": pay["item_barcode"],
                            "status":"fail"
                        }
                client.publish(topic, json.dumps(resp))
                return
        elif pay['mqtt_type'] == "update_mode":
            resp = {
            "mqtt_type": "penetration_data",
            "data": "cart-id"
            }
            # client.publish(topic, json.dumps(resp))
            ...
        else:
            ...
    except json.decoder.JSONDecodeError:
        print("message received " ,str(message.payload.decode("utf-8")))

# client = Client(client_id=””, clean_session=True, userdata=None, protocol=MQTTv311, transport=”tcp”)
client = mqtt.Client(client_id="cart-1", clean_session=True, userdata=None)
client.connect(host="localhost", port=1883)

client.subscribe("/cart/lTJqvZc1KeLIR5yHYCkabMdU4evreusC0xyPHagMqnW3I97bM2AhL6fc")
client.on_message = on_message_recived
client.loop_start()

sleep(9999) 
print("test")