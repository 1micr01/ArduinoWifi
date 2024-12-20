#include <Arduino.h>
#include <WiFiNINA.h>
#include <secrets.h>
#include <SPI.h>
#include <ArduinoMqttClient.h>
#include <Servo.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <ArduinoJson.h>

#define SERVO_PIN 7
#define DHT_PIN 1

char ap_ssid[] = AP_SSID;
char ap_password[] = AP_PASSWORD;
int wifiStatus = WL_IDLE_STATUS;
WiFiSSLClient wifiClient;

char brokerHost[] = "6d50cc68ea9d4e079719910a30d98aee.s1.eu.hivemq.cloud";
int brokerPort = 8883;
char mqtt_broker_username[] = HIVEMQ_USERNAME;
char mqtt_broker_password[] = HIVEMQ_PASSWORD;
int mqttStatus = MQTT_CONNECTION_TIMEOUT;
char topic[] = "action/+";
MqttClient mqttClient(wifiClient);
JsonDocument doc;

int led = LED_BUILTIN;
Servo servo;
DHT dht(DHT_PIN, DHT11);

unsigned long previousMillis = 0;
unsigned int interval = 1000;

void onActionMessage(int messageSize)
{
  String responseTopic = mqttClient.messageTopic();
  Serial.println(responseTopic);
  Serial.println(mqttClient.messageQoS());
  
  while (mqttClient.available())
  {
    String response = mqttClient.readString();
    Serial.println(response);
    String action = responseTopic.substring(responseTopic.indexOf("/") + 1);
    if (action == "led")
    {
      if (response == "on")
      {
        digitalWrite(led, HIGH);
      }
      else if (response == "off")
      {
        digitalWrite(led, LOW);
      }
    }
    else if (action == "servo")
    {
      servo.write(response.toInt());
    }
  }
}

void sendMessage(String topic, String message, int qos, bool retain) 
{
  mqttClient.beginMessage(topic, retain, qos);
  mqttClient.print(message);
  mqttClient.endMessage();
}

void setup()
{
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial);

  pinMode(led, OUTPUT);
  
  servo.attach(SERVO_PIN);
  dht.begin();

  while (wifiStatus != WL_CONNECTED){
    wifiStatus = WiFi.begin(ap_ssid, ap_password);
    if (wifiStatus != WL_CONNECTED)
    {
      Serial.println("Wifi connection failed!");
      delay(3000);
      continue;
    }

    Serial.println("You're connected to the Wifi! SSID: " + String(ap_ssid));
  }

  mqttClient.beginWill("health", true, 1);
  mqttClient.print("dead");
  mqttClient.endWill();
  
  mqttClient.setUsernamePassword(mqtt_broker_username, mqtt_broker_password);
  mqttClient.setCleanSession(false);
  while (mqttStatus != MQTT_SUCCESS) {
    if (!mqttClient.connect(brokerHost, brokerPort))
    {
      mqttStatus = mqttClient.connectError();
      Serial.print("MQTT connection failed! Error code = ");
      Serial.println(mqttStatus);
      delay(3000);
      continue;
    }
    
    mqttStatus = MQTT_SUCCESS;
    Serial.println("You're connected to the MQTT broker: " + String(brokerHost));
  };

  sendMessage("health", "alive", 1, true);

  mqttClient.onMessage(onActionMessage);
  mqttClient.subscribe(topic, 1);
}

void publish()
{
  if (!isnan(dht.readHumidity()) && !isnan(dht.readTemperature()))
  {
    doc["Humidity"] = dht.readHumidity();
    doc["Temperature"] = dht.readTemperature();
    String jsonPayload;
    serializeJson(doc, jsonPayload);
    sendMessage("dht", jsonPayload, 1, false);
  }
}

void loop()
{
  mqttClient.poll();
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;
    publish();
  }
}