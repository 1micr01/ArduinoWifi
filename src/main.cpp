#include <Arduino.h>
#include <WiFiNINA.h>
#include <secrets.h>
#include <SPI.h>

char ssid[] = AP_SSID;
char password[] = AP_PASSWORD;
int status = WL_IDLE_STATUS;
int led = LED_BUILTIN;

WiFiSSLClient client;

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
  
  if (client.connect("demosamples.azurewebsites.net", 443))
  {
    Serial.println("connected to server");

    client.println("GET /StaticHTMLTest/Default.html HTTP/1.1");

    client.println("Host: demosamples.azurewebsites.net");

    client.println("Connection: close");

    client.println();
  }
}

void loop()
{
  while (client.available())
  {
    char c = client.read();
    Serial.print(c);
  }
  
  if (!client.connected())
  {
    Serial.println();

    Serial.println("disconnecting from server.");

    client.stop();

    while (true);
  }
  
}