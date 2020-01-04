#ifndef VISCACONTROLLER
#define VISCACONTROLLER

#include <Arduino.h>
#include <EEPROM.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>

// Pin assignments from arduno shield.

// Analog inputs
#define PAN 0
#define TILT 1
#define ZOOM 2
#define AUX1 3
#define AUX2 4
#define AUX3 5

// Serial I/O for VISCA
#define VISCARX 2
#define VISCATX 3

// Button inputs
#define BTN1 4
#define BTN2 5
#define BTN3 6
#define BTN4 7
#define BTN5 8
#define BTN6 9
#define BTN7 10
#define BTN8 11
#define BTN9 12
#define BTN10 13

#endif
