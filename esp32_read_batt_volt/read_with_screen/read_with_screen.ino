#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_RESET     -1 
#define SCREEN_ADDRESS 0x3C 
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
const int buttPin = 23;
const int screenPin = 5;
const int voltPin = 36;
int buttonState;
float voltValue;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(buttPin, INPUT);
  pinMode(screenPin, OUTPUT);
  pinMode(voltPin, INPUT);
  delay(1000);

  digitalWrite(screenPin, HIGH);
  loadProgramScreen();
  digitalWrite(screenPin, LOW);  
  delay(1000);

}

void loop() {;
  buttonState = digitalRead(buttPin); //Check button state --> always 0, if pressed == 1;
  Serial.println(buttonState);
  voltValue = analogRead(voltPin) * 3.3/4095.0;
  if (buttonState == HIGH) {
    digitalWrite(screenPin, HIGH);
    readPotentialScreen(voltValue);
  }
  else {
    delay(500);
    digitalWrite(screenPin, LOW);
  }
  // // put your main code here, to run repeatedly:
  // display.clearDisplay();
  // display.setTextSize(2);
  // display.setTextColor(WHITE);
  // display.setCursor(0,0);
  // display.println("Hello World");
  // display.display();
  // delay(3000);

  // display.clearDisplay();


}

void readPotentialScreen (float voltage) {
  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
  display.clearDisplay();
  display.setCursor(25,5);
  display.println("Voltage");
  display.setCursor(35,30);
  display.println(voltage, 2);
  display.setCursor(90,30);
  display.println("V");
  display.display();
}

void loadProgramScreen () {
      // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
    if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
      Serial.println(F("SSD1306 allocation failed"));
      for(;;); // Don't proceed, loop forever
    }

    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(20,0);
    display.println("Loading");
    display.display();
    int count = 0;
    for (int i = 0; i <= 3; i++){
      display.setCursor(40 + i + count,20);
      display.println(".");
      display.display();
      count += 10;
      delay(500);
    }
    delay(4000);
}
