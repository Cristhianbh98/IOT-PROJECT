import paho.mqtt.client as mqtt 
import random
import time

MQTT_BROKER = "broker.hivemq.com" 

client = mqtt.Client("TEMPERATURE")
client.connect(MQTT_BROKER, port = 1883) 

while True:
  t = random.randint(15, 30)
  h = random.randint(40, 60)
  msg = "{\"t\": " + str(t) + ", \"h\": " + str(h) + "}"
  client.publish("IOT/TEMHUM", msg)
  print("Just published " + str(msg) + " to topic IOT/TEMHUM")
  time.sleep(5)