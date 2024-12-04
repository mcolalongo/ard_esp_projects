#include<WiFi.h>
#include<Firebase_ESP_Client.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include "time.h"
//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

#define WIFI_SSID "wifi_ID"
#define WIFI_PASSWORD "psswd"
#define API_KEY "your_APIKEY"
#define DATABASE_URL "yourwebsitetofirebase.firebasedatabase.app/"
#define SEALEVELPRESSURE_HPA (1013.25)

// RTC_DATA_ATTR int count = 0; // save vars we don't want to loose after deep sleep (no need. Now counter is already in the DB)
// RTC_DATA_ATTR int bootCount = 0;

Adafruit_BME280 bme; // I2C

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

long DEEP_SLEEP_TIME_SEC = 3600 ; //each 1 hour
// long DEEP_SLEEP_TIME_SEC = 10;
bool signUpOK = false;
int LEDGPIO = 4;
int pinVolt = 2;
float R1 = 220; //in ohm
float R2 = 220; //in ohm
// int adcVolt;
float Volt;

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 3600; // in Paris/Rome GMT --> +1h so 3600
const int   daylightOffset_sec = 3600; //3600 se ora legale, 0 se ora quell altra del cazzo non ricordo mai dio cane

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(2000); // first time just in case of charging up the capacitor. Then its useless, but 2 seconds not a big differnce overall
  
  // This gives the cpacitor the time to fill up and guarantee the right amount of current.
  // if (bootCount == 0 ){
    // delay(10000);
  // }
  // ++bootCount;

  // //LEDPIN Set;
  pinMode(LEDGPIO,OUTPUT);
  pinMode(pinVolt, INPUT);
  // adcVolt = analogRead(pinVolt);
  // Volt = adcVolt /(R2/(R1+R2));
  // Volt = analogRead(pinVolt) * 4.2 / 4095; //4.2 max voltage of lipo
  Volt = ReadVoltage(pinVolt)/(R2/(R1+R2)); // ADC2 pin cannot be used when wifi is on, that's why we read voltage now and not later on
  // Serial.println(Volt);


  // WiFi Connection. If WiFi does not connect, iteration on dots until it does.
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.println("Connecting to Wifi");
  while(WiFi.status() !=  WL_CONNECTED){
    Serial.print(".");
    delay(100);
  }
  Serial.println();
  Serial.print("Successfully Connected to WiFi. IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  // connect to Firebase;
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  if(Firebase.signUp(&config, &auth, "", "")){
    Serial.println("Firebase Sign Up Ok! ");
    signUpOK = true; 
  }
  else{
    // Serial.println("%s\n", config.signer.signupError.message.c_str());
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }


  // Init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  // Serial.printf("%s\n", printLocalTime());
  // Serial.println(printLocalTime());

  config.token_status_callback = tokenStatusCallback; // not sure what it does
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);


  //Turn on the BMP80
  bool status;
  // default settings
  // (you can also pass in a Wire library object like &Wire2)
  status = bme.begin(0x76);  
  if (!status) {
  Serial.println("Could not find a valid BME280 sensor, check wiring!");
  while (1); // Don't proceed, loop forever
  }

}

void loop() {


  if (Firebase.ready() && signUpOK){
    delay(200);
    // if(Firebase.RTDB.getBool(&fbdo, "/counter/")){
    //   if(fbdo.dataType() == "int"){
    //     delay(200); // gives the time to get the value for some reason
    //     Serial.println(fbdo.intData());
    //   }
    // }

    Firebase.RTDB.getInt(&fbdo, "/counter/"); // this points at FirebaseData. It builds a request that I suppose gets
    // stored temporarily in the DATABASE SESSION
    int count = fbdo.intData(); //if the previous line is True then we can ask to the fbdo to print the integer.
    ++count;
    Firebase.RTDB.setString(&fbdo, "BMP280/" + String(count) + "/Time", printLocalTime());
    Firebase.RTDB.setFloat(&fbdo, "BMP280/" + String(count) + "/Temp", bme.readTemperature());
    Firebase.RTDB.setFloat(&fbdo, "BMP280/" + String(count) + "/Humd", bme.readHumidity());
    Firebase.RTDB.setFloat(&fbdo, "BMP280/" + String(count) + "/Alti", bme.readAltitude(SEALEVELPRESSURE_HPA));
    Firebase.RTDB.setFloat(&fbdo, "BMP280/" + String(count) + "/Pres", bme.readPressure()/100.0F);
    Firebase.RTDB.setFloat(&fbdo, "BMP280/" + String(count) + "/BVolt", Volt);

    digitalWrite(LEDGPIO, HIGH);
    delay(1000);
    digitalWrite(LEDGPIO, LOW);

    // Here we should update counter in DB + 1.
    Firebase.RTDB.setInt(&fbdo, "/counter/", count);

    
    // Serial.print("ESP32 woke up: ");
    // Serial.print(count);
    // Serial.println(" times"); // No need anymore
    delay(200);
    // esp deep sleep for TIME TO SLEEP time
    esp_sleep_enable_timer_wakeup(1000000ULL * DEEP_SLEEP_TIME_SEC);
    esp_deep_sleep_start();
  }

  // if(Firebase.RTDB.getBool(&fbdo, "/LED/sensorValue/")){
  //   if(fbdo.dataType() == "boolean"){
  //     delay(200); // gives the time to get the value for some reason
  //     // Serial.println(fbdo.boolData());
  //     if(fbdo.boolData() == 0){
  //       digitalWrite(LEDGPIO, LOW); 
  //     }
  //     else{
  //       digitalWrite(LEDGPIO, HIGH);
  //     }
  //   }
  // }
}


double ReadVoltage(byte pin){
  double reading = analogRead(pin); // Reference voltage is 3v3 so maximum reading is 3v3 = 4095 in range 0 to 4095
  if(reading < 1 || reading > 4095) return 0;
  // return -0.000000000009824 * pow(reading,3) + 0.000000016557283 * pow(reading,2) + 0.000854596860691 * reading + 0.065440348345433;
  return -0.000000000000016 * pow(reading,4) + 0.000000000118171 * pow(reading,3)- 0.000000301211691 * pow(reading,2)+ 0.001109019271794 * reading + 0.034143524634089;
} // Added an improved polynomial, use either, comment out as required




String printLocalTime()
{
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    exit(0);
    // return; 
    }
  //iSerial.println(&timeinfo, "%d-%m-%Y %H:%M:%S");
  // Serial.println(&timeinfo, "%d-%m-%Y %H:%M:%S");
  char time[20]; // define time array
  strftime(time, 20, "%d-%m-%Y %H:%M:%S", &timeinfo);
  return (time);
  // return Serial.println(&timeinfo, "%d-%m-%Y %H:%M:%S");
}
