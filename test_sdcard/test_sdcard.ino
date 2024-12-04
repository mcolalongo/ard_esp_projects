/*
 * This ESP32 code is created by esp32io.com
 *
 * This ESP32 code is released in the public domain
 *
 * For more detail (instruction and wiring diagram), visit https://esp32io.com/tutorials/esp32-sd-card
 */

#include <SD.h>

#define PIN_SPI_CS 5 // The ESP32 pin GPIO5

File myFile;

void setup() {
  Serial.begin(9600);

  if (!SD.begin(PIN_SPI_CS)) {
    Serial.println(F("SD CARD FAILED, OR NOT PRESENT!"));
    while (1); // stop the program
  }

  Serial.println(F("SD CARD INITIALIZED."));

  if (!SD.exists("esp32.txt")) {
    Serial.println(F("esp32.txt doesn't exist. Creating esp32.txt file..."));
    // create a new file by opening a new file and immediately close it
    myFile = SD.open("esp32.txt", FILE_WRITE);
    myFile.close();
  }

  // recheck if file is created or not
  if (SD.exists("esp32.txt"))
    Serial.println(F("esp32.txt exists on SD Card."));
  else
    Serial.println(F("esp32.txt doesn't exist on SD Card."));
}

void loop() {
}
