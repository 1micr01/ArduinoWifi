#include <Arduino.h>
#include <WiFiNINA.h>
#include <secrets.h>
#include <SPI.h>
#include <ArduinoMqttClient.h>

char ssid[] = AP_SSID;
char password[] = AP_PASSWORD;
int status = WL_IDLE_STATUS;
int led = LED_BUILTIN;

char broker[] = "test.mosquitto.org";
int brokerPort = 8886;

char topic[] = "arduino/simple";

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

  // print where to go in a browser:

  Serial.print("To see this page in action, open a browser to http://");

  Serial.println(ip);
}

void setup()
{
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial);

  pinMode(led, OUTPUT);

  while (status != WL_CONNECTED){
    status = WiFi.begin(ssid, password);
    delay(10000);
  }

  printStatus();
  
  if (!mqttClient.connect(broker, brokerPort)){
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());
    while (true);
  }

  Serial.println("You're connected to the MQTT broker!");
  Serial.println();
}

void loop()
{
  mqttClient.poll();

  unsigned long currentMillis = millis();

  if ((currentMillis - previousMillis) >= interval)
  {
    previousMillis = currentMillis;

    Serial.println("Sending to topic:");
    Serial.println(topic);
    Serial.println("Message:");
    Serial.println("Mojn");
    Serial.println(count);
    Serial.println();

    mqttClient.beginMessage(topic);
    mqttClient.print("Mojn");
    mqttClient.endMessage();

    count++;
  }
    
}