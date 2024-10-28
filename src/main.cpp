#include <Arduino.h>
#include <WiFiNINA.h>
#include <secrets.h>
#include <SPI.h>

char ssid[] = AP_SSID;
char password[] = AP_PASSWORD;
int status = WL_IDLE_STATUS;
int led = LED_BUILTIN;

WiFiServer server(80);

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

  status = WiFi.beginAP(ssid, password);

  if (status != WL_AP_LISTENING)
  {
    Serial.println("Creating access point failed");
    while (true);
  }

  delay(10000);
  
  server.begin();
  printStatus();
}

void loop()
{
  if (status != WiFi.status())
  {
    status = WiFi.status();
    if (status == WL_AP_CONNECTED) {
      Serial.println("Device connected to AP");
    } else {
      Serial.println("Device disconnected from AP");
    }
  }
  
  WiFiClient client = server.available();

  if (client)
  {
    String currentLine = "";
    while (client.connected())
    {
      if (client.available())
      {
        char c = client.read();
        Serial.println(c);

        if (c == '\n')
        {
          if (currentLine.length() == 0)
          {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();
            client.print("Click <a href=\"/H\">here</a> turn the LED on<br>");
            client.print("Click <a href=\"/L\">here</a> turn the LED off<br>");
            client.println();
            break;
          }
          else {
            currentLine = "";
          }
        }
        else if (c != '\r') {
          currentLine += c;
        }

        if (currentLine.endsWith("GET /H"))
        {
          digitalWrite(led, HIGH);
        }
        else if (currentLine.endsWith("GET /L")){
          digitalWrite(led, LOW);
        }
      }
    }

    client.stop();
    Serial.println("client disconnected");
  }
  
}