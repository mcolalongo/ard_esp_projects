#include <WiFi.h>
#include <PubSubClient.h>
#include "ArduinoJson.h"
#include "DHT.h"

#define PINGATE 21
#define LEDPIN 2


const char* ssid = "wifiid";
const char* password = "wifipswwrd";
const char* mqtt_server = "192.168.1.235";

WiFiClient espClient;
PubSubClient client(espClient);



void setup() {
  pinMode(LEDPIN, OUTPUT);
  pinMode(PINGATE, OUTPUT);
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}



void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop(); 
}



//////////////////////////// Functions ////////////////////

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}



void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP32DevBoard_clientID", "mqtt-user", "mqtt-password")) {
      Serial.println("connected");
      // Subscribe
      client.subscribe("livingroom/waterpump/switch");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();
  Serial.println(messageTemp);
  if (messageTemp == "ON"){
    Serial.println("Pump is ON");
    digitalWrite(PINGATE, HIGH);
    digitalWrite(LEDPIN, HIGH);
  }
  else{
    Serial.println("Pump is OFF");
    digitalWrite(PINGATE, LOW);
    digitalWrite(LEDPIN, LOW);
  }
}








