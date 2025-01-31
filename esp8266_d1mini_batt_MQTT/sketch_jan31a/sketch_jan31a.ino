// NB the version of ESP8266 board module has to be 2.5.2. The 3.1.2 causes problems with wifi connection. Didn't try other than 2.5.2


include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "ArduinoJson.h"


#define VPIN A0
#define LEDPIN 2

const char* ssid = "WIFI_ID";
const char* password = "WIFI_PASS";
const char* mqtt_server = "192.168.1.235";
float volt;
uint64_t time_ms = 60e6;
//D1 mini in A0 has already voltage divider of 220k/ 100k 
int R1 = 4600; //ohm
int R2 = 4600; //ohm


WiFiClient espClient;
PubSubClient client(espClient);
JsonDocument doc;


void setup() {
  // put your setup code here, to run once:
  pinMode(LEDPIN, OUTPUT);
  pinMode(A0, INPUT);
  Serial.begin(74880);
  delay(5000);

  Serial.println("Booted Up");

  // average reading
  float tmp = 0;
  for (int i=0; i<10; i++){
    volt = float(analogRead(VPIN))/1024*3.3*(R1+R2)/(R2); //3.3 max voltage A0 can receive == 1024 analog value
    tmp = tmp + volt;
  }
  Serial.print(tmp/10);  
  Serial.println("V");
  
  setup_wifi();

  client.setServer(mqtt_server, 1883);
  // client.setCallback(callback); on this esp no need for callback


}

void loop() {
  // put your main code here, to run repeatedly:

  doc["voltage"] = volt;
  char buffer[256];
  serializeJson(doc, buffer);


  if (!client.connected()) {
    reconnect();
    // reconnect_DHT11();
  }
  client.loop();

  client.publish("ESP8266/voltage",  buffer);
  delay(2000);

  Serial.println("Going to sleep...");
  ESP.deepSleep(time_ms * 30); //each 30min wake up

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
    if (client.connect("ESP8266_clientID", "mqtt-user", "mqtt-password")) {
      Serial.println("connected");
      // Subscribe
      client.subscribe("ESP8266/voltage");
    } else {
      Serial.println("Failed to connect. ESP8266 is in deepsleep!");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}







