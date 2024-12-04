#include <WiFi.h>
#include <PubSubClient.h>
#include "ArduinoJson.h"
#include "DHT.h"

#define DHTPIN 4
#define DHTTYPE DHT11
#define LEDPIN 2


DHT dht(DHTPIN, DHTTYPE); // initialize dht object from DHT class

const char* ssid = "WiFi_ID";
const char* password = "WiFi_PSSWD";

// Add your MQTT Broker IP address, example:
//const char* mqtt_server = "192.168.1.144";
const char* mqtt_server = "192.168.1.XXX";

WiFiClient espClient;
PubSubClient client(espClient);
//Create ArduinoJson object
JsonDocument doc;

long lastMsg = 0;
char msg[50];
int value = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(LEDPIN, OUTPUT);
  Serial.begin(115200);
  dht.begin(); // initialize the dht sensor
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float f = dht.readTemperature(true); // for Farenheit unit measure
  nanValues(h,t,f);
  float hif = dht.computeHeatIndex(f, h);
  float hic = dht.computeHeatIndex(t, h, false);
  // create the JSON file
  doc["sensorName"] = "DHT11";
  doc["temp"] = t;
  doc["hum"] = h;
  char buffer[256];
  serializeJson(doc, buffer);


  if (!client.connected()) {
    reconnect();
    // reconnect_DHT11();
  }
  client.loop();

  client.publish("livingroom/dht11sensor",  buffer); 
  delay(2000);
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
    Serial.println("Lamp is ON");
    digitalWrite(LEDPIN, HIGH);
  }
  else{
    Serial.println("Lamp is OFF");
    digitalWrite(LEDPIN, LOW);
  }
}

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
    if (client.connect("ESP32_clientID", "mqtt-user", "mqtt-password")) {
      Serial.println("connected");
      // Subscribe
      client.subscribe("office/light/switch");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}




void nanValues(float h, float t, float f){
  if (isnan(h) || isnan(t) || isnan(f)){
    Serial.println(F("Failed to tead from DHT sensor!"));
    return;
  }
}
