#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ADS1115_lite.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_RESET     -1 
#define SCREEN_ADDRESS 0x3C 


Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
ADS1115_lite ads(ADS1115_DEFAULT_ADDRESS);  // 0x48 addr pin connected to GND

const int buttPin = 23;
const int screenPin = 5;
const int voltPin = 36;
int buttonState;
float voltValue;
long rawADS;
double resolution;

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

  ads_config();
}

void loop() {;

  buttonState = digitalRead(buttPin); //Check button state --> always 0, if pressed == 1;
  Serial.println(buttonState);

  voltValue = analogRead(voltPin) * 3.3 /4095.0; 
  if (buttonState == HIGH) {
    digitalWrite(screenPin, HIGH);
    rawADS = ads_read();
    resolution=4096.00/32752.00; // Found on the internet no idea why
    float voltADS = (rawADS * resolution)/1000.00; 
    readPotentialScreen(voltValue, voltADS);


  }
  else {
    delay(100);
    digitalWrite(screenPin, LOW);
  }

}

void readPotentialScreen (float voltage, float voltADS) {
  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
  display.setTextSize(1);
  display.clearDisplay();
  display.setCursor(0,5);
  display.println("VoltESP32 vs ADS1115");
  display.setCursor(30,30);
  display.println(voltage, 3);
  display.setCursor(70,30);
  display.println(voltADS, 3);
  // display.setCursor(90,30);
  // display.println("V");
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
    delay(3000);
}


void ads_config(){
  ads.setGain(ADS1115_REG_CONFIG_PGA_4_096V); // GAIN_ONE and resolution to ± 4.096V
  ads.setSampleRate(ADS1115_REG_CONFIG_DR_8SPS); // Set to the fastest MODE 860Samples per sec
}

int16_t ads_read(){ 
  ads.setMux(ADS1115_REG_CONFIG_MUX_SINGLE_0);  // Single mode input on AIN0 (AIN0 - GND)
  ads.triggerConversion();  // Triggered mannually 
  return ads.getConversion();  // returns int16_t value
}
