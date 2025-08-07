// Simple Sketch to check whethet the pump works or not!
// Next will be with home-assistant and MQTT

#define PINGATE 21

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(PINGATE, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(2000);
  digitalWrite(PINGATE, HIGH);
  delay(2000);
  digitalWrite(PINGATE, LOW);
}
