#include <WiFi.h>
#include <PubSubClient.h>
#include "ArduinoJson.h"
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define SEALEVELPRESSURE_HPA (1020.0)

Adafruit_BME280 bme; // I2C
// Variable Declaration
const char* ssid = "wifiID";
const char* password = "wifiPSSW";
// Add your MQTT Broker IP address, example:
const char* mqtt_server = "192.168.1.235";
const int LEDPIN = 14; // LED pin
const int GASPIN = 35; // ADC pin

WiFiClient espClient;
PubSubClient client(espClient);
//Create ArduinoJson object
JsonDocument doc; //We definitely need this for collecting the BMP280 data

int value = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(LEDPIN, OUTPUT);
  pinMode(GASPIN, INPUT);
  Serial.begin(115200);

  setup_wifi();
  // Connect to mqtt_server
  client.setServer(mqtt_server, 1883);
  // Set Call Back function which takes the callback function as an argument. Function so be called for the constructor only if we wait for a response from the Home Assistant. Like a button pressed or action
  client.setCallback(callback);
  

  // check the bmp280 sensor
  bool status;
  // default settings
  // (you can also pass in a Wire library object like &Wire2)
  status = bme.begin(0x76);  
  if (!status) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    Serial.println("Infinite Loop Started...");
    while (1); // Don't proceed, loop forever
  }
}

void loop() { // Loop function of microcrontroller
  float gas = analogRead(GASPIN);
  
  float h =  bme.readHumidity();
  float t = bme.readTemperature();
  float alt = bme.readAltitude(SEALEVELPRESSURE_HPA);
  float press = bme.readPressure()/100.0F;
  // Serial.println(t);
  // Serial.print("Gas Sensing Analog Value:");
  // Serial.println(gas);
  // create the JSON file
  doc["sensorName"] = "BMP280";
  doc["temp_bmp"] = t;
  doc["hum_bmp"] = h;
  doc["alt_bmp"] = alt;
  doc["press_bmp"] = press;
  doc["gas_mq2"] = gas;
  char buffer[256]; // https://arduinojson.org/v6/how-to/use-arduinojson-with-pubsubclient/ (Serializing a JSON document into an MQTT message)
  serializeJson(doc, buffer); //needs the json doc + buffer which is a temporary string which prints like { "car" : toyota, "seats" : 5} etc

  // connect to MQTT broker
  if (!client.connected()) {
    reconnect();
 
  }
  client.loop(); // function needed to constantly run the MQTT broker

  client.publish("livingroom/bmp280sensor",  buffer);
  delay(1000); 
}


void callback(char *topic, byte *message, unsigned int length) {
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

// reconnect in case of failure 
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP32_clientIDBMP280", "mqtt-user", "mqtt-password")) {
      Serial.println("connected");
      // Subscribe
      client.subscribe("livingroom/light/switch");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }   
  }
}

