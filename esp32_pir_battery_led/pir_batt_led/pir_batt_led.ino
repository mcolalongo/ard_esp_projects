// No WiFi, it should work with the HT7833. Idea is to leave it in deep sleep, wake up when needed.

const int gpioLED = 23;
const int gpioRedLED = 22;
const int gpioPIR = 4; // PIR VCC goes to 5V to properly function

RTC_DATA_ATTR int wakeupCounter = 0; //variable stored in ESP memory. Doesn't get cancelled after reboot. Unless boot button or unplug

void setup() {

  // delay(1000); //to startup
  Serial.begin(115200);
  pinMode(gpioLED, OUTPUT); //set led to output for the light
  pinMode(gpioRedLED, OUTPUT); //set led to output for the light
  pinMode(gpioPIR, INPUT); //set PIR Data pin to receiver

  Serial.print("Wakeup Counter = ");
  Serial.println(wakeupCounter);

  if (wakeupCounter != 0) { //here the first time we plugg the esp32 counter will be 0, so trigger will not run. It goes in loop, wake up counts +1 and goes to sleep
    Serial.println("ESP32 woken up");
    triggerAlert();
  }
  // else{
  //   sleepDevice();
  // }

}



void loop() {
  ++wakeupCounter; //Increment wakeupcounter --> from now on, each wake up after sleepDevice will triggerAlert() in void setup()
  sleepDevice();
}


void triggerAlert() {
  Serial.println("Motion Detected - Turn on the light!");
  digitalWrite(gpioLED, HIGH);
  delay(5000);
}


void sleepDevice() {
  digitalWrite(gpioLED, LOW);
  for (int i = 0; i <= 3; ++i){
    digitalWrite(gpioRedLED, HIGH);
    delay(500);
    digitalWrite(gpioRedLED, LOW);
    delay(500);
  }
  Serial.println("Deep Sleep Mode!");
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_4, HIGH); //wakes ESP32 up if certain GPIO triggered (for us is the DATA PIR), HIGH attend 1 (ones)
  esp_deep_sleep_start(); //send esp to sleep
}