#include "visca-controller.h"

LiquidCrystal_I2C lcd(0x27,16,2);
SoftwareSerial visca(VISCARX, VISCATX);

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  for (uint8_t i = 2; i <= 12; i++) {
    pinMode(i,INPUT);
  }
  
}

void loop() {
  
}
