import paho.mqtt.client as mqtt
from time import sleep
import json, time

topic = "/cart/AN1kVAUYNynaPvk6nmyS3D6a36R42B2R0kQ338rcM7ERqF2O5GrERSco"

def on_connect(client, userdata, flags, rc):
    print("Connected with result code " + str(rc))
    # Subscribe to the desired MQTT topics here

def on_message(client, userdata, message):
    payload = message.payload.decode("utf-8")
    try:
        payload = json.loads(payload)
        print("Message received:", payload)
        
    except json.decoder.JSONDecodeError:
        print("Message received:", payload)

def add_pressed():
    # Perform action for button 1 press
    request = {
        "mqtt_type": "request_add_item",
        "sender": "user-1",
        "item_barcode": "123",
        "timestamp": int(time.time()) 
    }
    client.publish(topic, json.dumps(request))

def update_pressed(mode: str):
    # Perform action for button 2 press
    request = {
        'mqtt_type': 'update_mode',
        'sender': 'user-1',
        'mode': mode,
        'timestamp': int(time.time())
    }
    client.publish(topic, json.dumps(request))

# MQTT client setup
client = mqtt.Client(client_id="user-1")
client.on_connect = on_connect
client.on_message = on_message
client.connect("192.168.55.66", port=1883, keepalive=60)
client.loop_start()

print("Connected to MQTT broker")

# Start the code by subscribing to the channel
client.subscribe(topic)

button = ''
while button != 'q':
    button = input("Press button: ")
    
    if button == 'a':
        add_pressed()
    elif button in ['0', '1','2', '3', '4', '5'] :
        update_pressed(button)
    else:
        print("Invalid button")

# Stop the MQTT client loop and disconnect
client.loop_stop()
client.disconnect()
