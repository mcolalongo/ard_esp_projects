#include <ESP8266WiFi.h>
#include "time.h"
#include<Firebase_ESP_Client.h>
#define API_KEY "APIKEYinFireBaseDB"
#define DATABASE_URL "[...]firebasedatabase.app/"
//Provide the token generation process info.
// #include "addons/TokenHelper.h"
// //Provide the RTDB payload printing info and other helper functions.
// #include "addons/RTDBHelper.h"

// Replace with SSID
#ifndef STASSID
#define STASSID "wifiid"
#define STAPSK "wifipssw"
#endif


const char* ssid = STASSID;
const char* password = STAPSK;
int sleepTime = 10e6; //10 seconds in us

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
bool signupOK = false;
int pinVolt = 12;
float volt;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  // pinMode(pinVolt, INPUT);
  // pinMode(D0, WAKEUP_PULLUP);
  Serial.begin(115200);
  // adcVolt = analogRead(pinVolt);
  // volt = adcVolt /(R2/(R1+R2));
  // volt = analogRead(pinVolt) * 5 / 1024; //4.2 max voltage of lipo
 
 
  // We start by connecting to a WiFi 
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(250);
    digitalWrite(LED_BUILTIN, LOW);
    delay(250);
    Serial.print(".");
  }
Serial.println();

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());


  // connect to Firebase;
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  if(Firebase.signUp(&config, &auth, "", "")){
    Serial.println("Firebase Sign Up Ok! ");
    signupOK = true; 
  }
  else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

   /* Assign the callback function for the long running token generation task */
  // config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h
  
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  
  
  Serial.print("pin voltage: ");
  Serial.println(volt);
  delay(1000);
  ESP.deepSleep(sleepTime); //60e6 --> 60 sec in us times 60 is 1 hour times 1 still 1 hour  
  // delay(10);

}



void loop() {
  // if (Firebase.ready() && signupOK){
  //   delay(200);

  //   Firebase.RTDB.getInt(&fbdo, "/counter/"); // this points at FirebaseData. It builds a request that I suppose gets
  //   // stored temporarily in the DATABASE SESSION
  //   int count = fbdo.intData(); //if the previous line is True then we can ask to the fbdo to print the integer.
  //   ++count;
  //   Serial.println(count);
  //   Firebase.RTDB.setFloat(&fbdo, "ESP8266/" + String(count) + "/batt_volt", volt);

  //   digitalWrite(LED_BUILTIN, HIGH);
  //   delay(1000);
  //   digitalWrite(LED_BUILTIN, LOW);

  //   Firebase.RTDB.setInt(&fbdo, "/counter/", count);
  //   delay(2000);
  // }

  // // ESP.deepsleep(60e6 * 60 * 1) //60e6 --> 60 sec in us times 60 is 1 hour times 1 still 1 hour
  
  // ESP.deepSleep(sleepTime); //60e6 --> 60 sec in us times 60 is 1 hour times 1 still 1 hour  

}


