/*
  The circuit:
    SD card attached to SPI bus as follows:
 ** MOSI - pin 11 on Arduino Uno/Duemilanove/Diecimila
 ** MISO - pin 12 on Arduino Uno/Duemilanove/Diecimila
 ** CLK - pin 13 on Arduino Uno/Duemilanove/Diecimila
 ** CS - depends on your SD card shield or module.
 		Pin 4 used here for consistency with other Arduino examples
*/
// include the SD library:
#include <SPI.h>
#include <SD.h>
#include <TMRpcm.h>

TMRpcm tmrpcm;

const int SD_CS = 10;
const int buzz = 2;


void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  pinMode(buzz, OUTPUT);
  tmrpcm.speakerPin = 9;

  if(!SD.begin(SD_CS)) {
    Serial.println("SD card not found...");
    return 0;
  }
  else{
    for (int i = 0; i < 2; i++){ //Buzz Twice
      tone(buzz, 1000);
      delay(100);
      noTone(buzz);
      delay(100);
    }
    Serial.println("SD Card is OK!");
  }

  delay(100);
  tmrpcm.volume(5);
  tmrpcm.play("/nique.wav");


}

void loop(void) {
}
