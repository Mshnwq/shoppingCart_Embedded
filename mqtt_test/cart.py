import paho.mqtt.client as mqtt
from time import sleep
import random, json, requests
import config.env as env

print("connected")
req = requests.get(f"http://{env.URL}:1111/api/v1/cart/start_cart/AN1kVAUYNynaPvk6nmyS3D6a36R42B2R0kQ338rcM7ERqF2O5GrERSco")
print("request status: ", req.status_code)
count = 0
def on_message_recived(client, userdata, message):
    global count
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
            if count < 3:
                client.publish(topic, json.dumps(resp))
                count +=1
            ...
        else:
            ...
    except json.decoder.JSONDecodeError:
        print("message received " ,str(message.payload.decode("utf-8")))

# client = Client(client_id=””, clean_session=True, userdata=None, protocol=MQTTv311, transport=”tcp”)
client = mqtt.Client(client_id="cart-1", clean_session=True, userdata=None)
client.connect(host=env.URL, port=1883)

client.subscribe("/cart/AN1kVAUYNynaPvk6nmyS3D6a36R42B2R0kQ338rcM7ERqF2O5GrERSco")
client.on_message = on_message_recived
client.loop_start()

sleep(9999) 
print("test")