#include <Arduino.h>
#include <WiFiNINA.h>
#include <secrets.h>
#include <SPI.h>
#include <ArduinoMqttClient.h>
#include <Servo.h>


char ap_ssid[] = AP_SSID;
char ap_password[] = AP_PASSWORD;

char shiftr_username[] = SHIFTR_USERNAME;
char shiftr_password[] = SHIFTER_PASSWORD;

int wifi_status = WL_IDLE_STATUS;
int mqtt_status = MQTT_CONNECTION_TIMEOUT;
int led = LED_BUILTIN;

#define SERVO_PIN 7

Servo servo;

char broker[] = "flintstinger385.cloud.shiftr.io";
int brokerPort = 8883;

char topic[] = "action/+";

unsigned long previousMillis = 0;
unsigned int interval = 1000;
int count = 0;

WiFiSSLClient wifiClient;
MqttClient mqttClient(wifiClient);

void printStatus()
{
  // print the SSID of the network you're attached to:

  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:

  IPAddress ip = WiFi.localIP();

  Serial.print("IP Address: ");
  Serial.println(ip);
}

void setup()
{
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial);

  pinMode(led, OUTPUT);
  
  servo.attach(SERVO_PIN);

  while (wifi_status != WL_CONNECTED){
    wifi_status = WiFi.begin(ap_ssid, ap_password);
    if (wifi_status != WL_CONNECTED)
    {
      Serial.println("Wifi connection failed!");
      delay(3000);
      continue;
    }

    Serial.println("You're connected to the Wifi!");
  }

  printStatus();
  
  mqttClient.setUsernamePassword(shiftr_username, shiftr_password);
  while (mqtt_status != MQTT_SUCCESS) {
    if (!mqttClient.connect(broker, brokerPort))
    {
      mqtt_status = mqttClient.connectError();
      Serial.print("MQTT connection failed! Error code = ");
      Serial.println(mqtt_status);
      delay(3000);
      continue;
    }
    
    mqtt_status = MQTT_SUCCESS;
    Serial.println("You're connected to the MQTT broker!");
  };

  mqttClient.subscribe(topic);
}

void loop()
{
  if (mqttClient.parseMessage()){
    String responseTopic = mqttClient.messageTopic();
    Serial.println(responseTopic);
    
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
}