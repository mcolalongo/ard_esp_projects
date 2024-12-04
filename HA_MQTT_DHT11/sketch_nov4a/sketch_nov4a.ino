/*
  SimpleMQTTClient.ino
  The purpose of this exemple is to illustrate a simple handling of MQTT and Wifi connection.
  Once it connects successfully to a Wifi network and a MQTT broker, it subscribe to a topic and send a message to it.
  It will also send a message delayed 5 seconds later.
*/

#include "EspMQTTClient.h"
#include "ArduinoJson.h"
#include "DHT.h"

#define DHTPIN 4
#define DHTTYPE DHT11


DHT dht(DHTPIN, DHTTYPE); // initialize dht object from DHT class


EspMQTTClient client(
  "WiFi ID",
  "WiFi PSSWD",
  "192.168.1.XXX",  // MQTT Broker server ip --> Same as HA
  "mqtt-user",   // Can be omitted if not needed
  "mqtt-password",   // Can be omitted if not needed
  "ESP32DevKitV1",     // Client name that uniquely identify your device
  1883              // The MQTT port, default to 1883. this line can be omitted
);


//Create ArduinoJson object;
JsonDocument doc;


void setup()
{
  delay(1000);
  Serial.begin(115200);
  dht.begin(); // initialize the dht sensor

  // Optional functionalities of EspMQTTClient
  client.enableDebuggingMessages(); // Enable debugging messages sent to serial output
  client.enableHTTPWebUpdater(); // Enable the web updater. User and password default to values of MQTTUsername and MQTTPassword. These can be overridded with enableHTTPWebUpdater("user", "password").
  client.enableOTA(); // Enable OTA (Over The Air) updates. Password defaults to MQTTPassword. Port is the default OTA port. Can be overridden with enableOTA("password", port).
  // client.enableLastWillMessage("TestClient/lastwill", "I am going offline");  // You can activate the retain flag by setting the third parameter to true
}


void loop()
{
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

  client.loop();
  onConnectionEstablished();

  delay(500);
  // serializeJson(doc, Serial); // prints json in the fashion way we like.

}


// This function is called once everything is connected (Wifi and MQTT)
// WARNING : YOU MUST IMPLEMENT IT IF YOU USE EspMQTTClient
void onConnectionEstablished()
{
  char buffer[256];
  serializeJson(doc, buffer);
  // Serial.println(buffer);
  // Subscribe to "mytopic/test" and display received message to Serial
  client.subscribe("livingroom/dht11sensor", [](const String & payload) {
    Serial.println(payload);
  });
  client.publish("livingroom/dht11sensor",  buffer); // You can activate the retain flag by setting the third parameter to true

  
  // client.subscribe("office/light/switch", onTestMessageReceived);
  // Test Subscribe listening MQTT publishing
  client.subscribe("office/light/switch", [](const String & payload) { //need to try pound sign to speed up
    Serial.println(payload);
    });

  // Publish a message to "mytopic/test"

  // Execute delayed instructions
  // client.executeDelayed(5 * 1000, []() {
  //   client.publish("mytopic/wildcardtest/test123", "This is a message sent 5 seconds later from ESP32", true);
  // });
}

void onTestMessageReceived(const String& message) {
  Serial.println("message received from office/light/switch: " + message);
}


void nanValues(float h, float t, float f){
  if (isnan(h) || isnan(t) || isnan(f)){
    Serial.println(F("Failed to tead from DHT sensor!"));
    return;
  }
}
