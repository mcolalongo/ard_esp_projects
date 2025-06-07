// NB the version of ESP8266 board module has to be 2.5.2. The 3.1.2 causes problems with wifi connection. Didn't try other than 2.5.2


#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "ArduinoJson.h"



const char* ssid = "WiFI-ID";
const char* password = "WIFI-PSSWD";
const char* mqtt_server = "192.168.1.235";
uint64_t time_ms = 60e6;
//D1 mini in A0 has already voltage divider of 220k/ 100k but the ESP12F module does not!
int _moisture,sensor_analog;
const int sensor_pin = A0;  /* Soil moisture sensor O/P pin */
int sensorON = 12;
int moisture;

WiFiClient espClient;
PubSubClient client(espClient);
JsonDocument doc;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(74880);
  Serial.println("micro booted up");
  // turn on sensor is the GPIO pin that turns on the VCC for the capacitor sensor
  pinMode(sensorON, OUTPUT);
  digitalWrite(sensorON, HIGH);
  delay(5000); //delay helps the adc to get stable and it waits the sensor to stabilize once sensorON that feeds VCC goes HIGH

  // read sensor analog value --> which is the Aout of the moisture sensor 
  // average reading (ADC reading has to go before WIFI goes on, otherwise the pin cant be used)
  float tmp = 0;
  for (int i=0; i<10; i++){
    sensor_analog = analogRead(sensor_pin); // it goes from 1024 when dry ( about 1 volt down to) to 540 when 100% wet
    moisture = map(sensor_analog,540,1024,100,0);
    tmp = tmp + moisture;
  }
  Serial.print(moisture/10);
  Serial.println("%");
  Serial.println(sensor_analog);   
  
  

  setup_wifi();
  client.setServer(mqtt_server, 1883);
  // client.setCallback(callback); on this esp no need for callback


}

void loop() {
  // put your main code here, to run repeatedly:

  doc["moisture"] = moisture;
  char buffer[256];
  serializeJson(doc, buffer);


  if (!client.connected()) {
    reconnect();
    // reconnect_DHT11();
  }
  client.loop();

  client.publish("ESP12F/moisture",  buffer);
  delay(2000);

  Serial.println("Going to sleep...");
  ESP.deepSleep(time_ms * 60); //each 1h wake up
  // ESP.deepSleep(5e6); //each 5s wake up

}


void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());



}void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP12F_moisture", "mqtt-user", "mqtt-password")) {
      Serial.println("connected");
      // Subscribe
      client.subscribe("ESP12F/moisture");
    } else {
      Serial.println("Failed to connect. ESP12F is in deepsleep!");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}







