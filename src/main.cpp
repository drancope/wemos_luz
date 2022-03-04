#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <PubSubClient.h>

#include "priv_datareal.h"
#define USE_SERIAL Serial
#define PINLUZ D4

void callback(char* topic, byte* payload, unsigned int length);

//variables
bool luz = true;
char broker[] = BROKER;
WiFiClient Client;
PubSubClient client(broker, 1883, callback, Client);

//código
void switch_light() {
  luz = !luz;
  digitalWrite(PINLUZ, luz);
}

void callback(char* topic, byte* payload, unsigned int length) {
  switch_light();
}

void connectWiFi(String SSIDD, String PASSD) {
  char *ssid = new char[SSIDD.length() +1 ];
  strcpy(ssid, SSIDD.c_str());
  char *pass = new char[PASSD.length() +1 ];
  strcpy(pass, PASSD.c_str());
  WiFi.mode(WIFI_STA);
//  WiFi.config(ip, gateway, subnet, dns1);
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected! IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnectMQTTClient(char *broker)
{
    while (!client.connected())
    {
        Serial.print("Attempting MQTT connection to ");
        Serial.print(broker);
        if (client.connect(CLIENT_NAME.c_str()))
        {
            Serial.println("  ...connected");
        }
        else
        {
            Serial.print(" Retying in 5 seconds - failed, rc=");
            Serial.println(client.state());

            delay(5000);
        }
    }
}

void createMQTTClient(char *broker)
{
    client.setServer(broker, 1883);
    reconnectMQTTClient(broker);
}

void setup() {
  pinMode(PINLUZ, OUTPUT);

  USE_SERIAL.begin(115200);
  USE_SERIAL.println();

  WiFi.mode(WIFI_STA);
//  WiFi.config(ip, gateway, subnet, dns1);
  connectWiFi(ssidd, passs);
  createMQTTClient(broker);
  char *topic = new char[CLIENT_TELEMETRY_TOPIC.length() +1 ];
  strcpy(topic, CLIENT_TELEMETRY_TOPIC.c_str());
  client.subscribe(topic);
}

void loop() {

//  reconnectMQTTClient(broker);
  if (!client.connected())
  {
    reconnectMQTTClient(broker);
  }
  client.loop();
  if ((WiFi.status() == WL_CONNECTED)) {
     Serial.println("Conectado.");
  }
  delay(500);
}
