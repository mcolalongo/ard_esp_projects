/*
  ESP32 Soil Moisture Sensor
  http:://www.electronicwings.com
*/

int _moisture,sensor_analog;
const int sensor_pin = A0;  /* Soil moisture sensor O/P pin */
int sensorON = 12;

void setup(void){
  Serial.begin(115200);     /* Set the baudrate to 115200*/
  pinMode(sensorON, OUTPUT);
  digitalWrite(sensorON, HIGH);
}

void loop(void){
  sensor_analog = analogRead(sensor_pin);
  Serial.print("Sensor Analog Val: ");
  Serial.print(sensor_analog);
  Serial.println("");
  int moisture = map(sensor_analog,540,1024,100,0);
  // _moisture = ( 100 - ( (sensor_analog/1024.00) * 100 ) );
  Serial.print("Moisture = ");
  // Serial.print(_moisture);  /* Print Temperature on the serial window */
  Serial.print(moisture);
  Serial.println("%");
  delay(50);              /* Wait for 1000mS */
}