#include <Adafruit_SSD1306.h>
#include <splash.h>

/*
  AnalogReadSerial

  Reads an analog input on pin 0, prints the result to the Serial Monitor.
  Graphical representation is available using Serial Plotter (Tools > Serial Plotter menu).
  Attach the center pin of a potentiometer to pin A0, and the outside pins to +5V and ground.

  This example code is in the public domain.

  https://www.arduino.cc/en/Tutorial/BuiltInExamples/AnalogReadSerial
*/

const int potPin = 36;
float voltValue;
const float r1 = 10000.0;
const float r2 = 10000.0; // 10kOhm

// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(115200);
}

// the loop routine runs over and over again forever:
void loop() {
  // read the input on analog pin 0:
  float sensorValue = analogRead(potPin);
  voltValue = ((3.3/4095.) * sensorValue); // 1.036 is a correction factor for the right value detection
  // print out the value you read:
  Serial.print(sensorValue, 2);
  Serial.print(" --> ");
  Serial.println(voltValue, 3);
  delay(1000);  // delay in between reads for stability
}
