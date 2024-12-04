/*********
  Complete project details at https://randomnerdtutorials.com  
*********/


#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_RESET     -1 
#define SCREEN_ADDRESS 0x3C 
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
const int buttPin = 19;
const int screenPin = 23;
// int buttonState;

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME280 bme; // I2C

unsigned long delayTime;

void setup() {

  delay(2000);
  Serial.begin(9600);
  pinMode(screenPin, OUTPUT);
  pinMode(buttPin, INPUT_PULLUP);
  digitalWrite(screenPin, HIGH);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x64)
   // init done
  display.display();
  delay(100);
  display.clearDisplay();
  display.display();
  display.setTextSize(1.2);
  display.setTextColor(WHITE);

  bool status;
  // default settings
  // (you can also pass in a Wire library object like &Wire2)
  status = bme.begin(0x76);  
  if (!status) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1); // Don't proceed, loop forever
  }

  // Address assigned, we can turn off the screen to widthdraw less power
  digitalWrite(screenPin, LOW);
}



void loop() { 
  int buttState = digitalRead(buttPin);
  Serial.println(buttState);
  if (buttState == 1){
    digitalWrite(screenPin, HIGH);
    readValues(bme.readTemperature(), bme.readHumidity(), bme.readPressure()/100.0, bme.readAltitude(SEALEVELPRESSURE_HPA));
    delay(5000);
    digitalWrite(screenPin, LOW);

  }
  
}



void readValues (float temp, float hum, float pres, float alt) {
    printValues();
    display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
    display.clearDisplay();
    display.setTextColor(WHITE);
    display.setTextSize(1);
    display.setCursor(10, 5);
    display.setTextSize(1);
    display.println("Temp.:");
    display.setCursor(10, 15);
    display.print(temp, 1);
    display.println(" C");
    display.setCursor(80, 5);
    display.println("Humid.:");
    display.setCursor(85, 15);
    display.print(hum,1);
    display.println(" %");
    display.setCursor(10, 40);
    display.println("Pres.:");
    display.setCursor(1, 50);
    display.print(pres,1); 
    display.println(" hPa");
    display.setCursor(80, 40);
    display.println("Altit.:");
    display.setCursor(80, 50);
    display.print(alt);
    display.println(" m");
    display.display();
}



void printValues() {
  Serial.print("Temperature = ");
  Serial.print(bme.readTemperature());
  Serial.println(" *C");
  
  // Convert temperature to Fahrenheit
  /*Serial.print("Temperature = ");
  Serial.print(1.8 * bme.readTemperature() + 32);
  Serial.println(" *F");*/
  
  Serial.print("Pressure = ");
  Serial.print(bme.readPressure() / 100.0F);
  Serial.println(" hPa");

  Serial.print("Approx. Altitude = ");
  Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
  Serial.println(" m");

  Serial.print("Humidity = ");
  Serial.print(bme.readHumidity());
  Serial.println(" %");

  Serial.println();
}