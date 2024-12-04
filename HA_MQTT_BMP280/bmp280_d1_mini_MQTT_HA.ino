#include <WiFi.h>
#include <PubSubClient.h>
#include "ArduinoJson.h"
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define SEALEVELPRESSURE_HPA (1020.0)

Adafruit_BME280 bme; // I2C
//Test sensitive data MySuperSecretPswd
// Variable Declaration
const char* ssid = "wifi_id";
const char* password = "mypsww";
// Add your MQTT Broker IP address, example:
const char* mqtt_server = "192.168.1.235";
const int pinVolt = 36;
const int pinVCC = 33;
const int LEDPIN = 14; // We will probably not use the ledpin this time.
float R1 = 4600; //in ohm
float R2 = 4600; //in ohm
// int adcVolt;
float Volt;


WiFiClient espClient;
PubSubClient client(espClient);
//Create ArduinoJson object
JsonDocument doc; //We definitely need this for collecting the BMP280 data

long lastMsg = 0;
char msg[50];
int value = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(LEDPIN, OUTPUT);
  pinMode(pinVolt, INPUT);
  pinMode(pinVCC, OUTPUT);
  Serial.begin(115200);
  setup_wifi();
  // Read Input Voltage --> When on USB it should get around 4.99V / 5V
  Volt = ReadVoltage(pinVolt)/(R2/(R1+R2)) + 0.1; // ADC2 pin cannot be used when wifi is on, that's why we read voltage now and not later on. +0.1 is a correction needed cause adc in ESP32 sucks
  Serial.println(Volt);
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
  
  float h =  bme.readHumidity();
  float t = bme.readTemperature();
  float alt = bme.readAltitude(SEALEVELPRESSURE_HPA);
  float press = bme.readPressure()/100.0F;
  Serial.println(t);
  // create the JSON file
  doc["sensorName"] = "BMP280";
  doc["temp_bmp"] = t;
  doc["hum_bmp"] = h;
  doc["alt_bmp"] = alt;
  doc["press_bmp"] = press;
  char buffer[256]; // https://arduinojson.org/v6/how-to/use-arduinojson-with-pubsubclient/ (Serializing a JSON document into an MQTT message)
  serializeJson(doc, buffer); //needs the json doc + buffer which is a temporary string which prints like { "car" : toyota, "seats" : 5} etc

  // connect to MQTT broker
  if (!client.connected()) {
    reconnect();
 
  }
  client.loop(); // function needed to constantly run the MQTT broker

  client.publish("livingroom/bmp280sensor",  buffer);
  delay(2000); 
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

double ReadVoltage(byte pin){
  double reading = analogRead(pin); // Reference voltage is 3v3 so maximum reading is 3v3 = 4095 in range 0 to 4095
  
  if(reading < 1 || reading > 4095){
    return 0;
  } 
  
  return -0.000000000009824 * pow(reading,3) + 0.000000016557283 * pow(reading,2) + 0.000854596860691 * reading + 0.065440348345433;
  // return -0.000000000000016 * pow(reading,4) + 0.000000000118171 * pow(reading,3)- 0.000000301211691 * pow(reading,2)+ 0.001109019271794 * reading + 0.034143524634089;
}

