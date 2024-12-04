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
float average = 0;

// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
}

// the loop routine runs over and over again forever:
void loop() {
  // read the input on analog pin 0:
  float sensorValue = analogRead(potPin);
  voltValue = ((3.28*1.0327/4095.0) * sensorValue); // error Value of 1.033 percent $\sim$
  
  for (int i = 0; i < 10; i++){
    average = average + voltValue;
    delay(50);
  }

  average = average / 11;

  // print out the value you read:
  Serial.print(sensorValue, 2);
  Serial.print(" --> ");
  Serial.print(voltValue, 2);
  Serial.print(" --> Avg Value: ");
  Serial.println(average, 2);
  delay(1000);  // delay in between reads for stability
}
