#ifndef VISCACONTROLLER
#define VISCACONTROLLER

#include <Arduino.h>
#include <EEPROM.h>
//#include <LiquidCrystal_I2C.h>
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

const byte numChars = 16;
byte viscaMessage[numChars];

// Pan/Tilt
byte panTilt[9] =       { 0x81, 0x01, 0x06, 0x01, 0x00, 0x00, 0x03, 0x03, 0xFF };
byte panUp[9] =         { 0x81, 0x01, 0x06, 0x01, 0x00, 0x00, 0x03, 0x01, 0xFF }; // 8x 01 06 01 0p 0t 03 01 ff
byte panDown[9] =       { 0x81, 0x01, 0x06, 0x01, 0x00, 0x00, 0x03, 0x02, 0xFF }; // 8x 01 06 01 0p 0t 03 02 ff
byte panLeft[9] =       { 0x81, 0x01, 0x06, 0x01, 0x00, 0x00, 0x01, 0x03, 0xFF }; // 8x 01 06 01 0p 0t 01 03 ff
byte panRight[9] =      { 0x81, 0x01, 0x06, 0x01, 0x00, 0x00, 0x02, 0x03, 0xFF }; // 8x 01 06 01 0p 0t 02 03 ff
byte panUpLeft[9] =     { 0x81, 0x01, 0x06, 0x01, 0x00, 0x00, 0x01, 0x01, 0xFF }; // 8x 01 06 01 0p 0t 01 01 ff
byte panUpRight[9] =    { 0x81, 0x01, 0x06, 0x01, 0x00, 0x00, 0x02, 0x01, 0xFF }; // 8x 01 06 01 0p 0t 02 01 ff
byte panDownLeft[9] =   { 0x81, 0x01, 0x06, 0x01, 0x00, 0x00, 0x01, 0x02, 0xFF }; // 8x 01 06 01 0p 0t 01 02 ff
byte panDownRight[9] =  { 0x81, 0x01, 0x06, 0x01, 0x00, 0x00, 0x02, 0x02, 0xFF }; // 8x 01 06 01 0p 0t 02 02 ff

byte panStop[9] =       { 0x81, 0x01, 0x06, 0x01, 0x09, 0x09, 0x03, 0x03, 0xFF }; // Camera Stop
byte panTiltPosReq[5] = { 0x81, 0x09, 0x06, 0x12, 0xff }; // Resp: y0 50 0p 0q 0r 0s 0t 0u 0v 0w ff ; pqrs: pan position ; tuvw: tilt position

// Zoom
byte zoomTele[6] =    { 0x81, 0x01, 0x04, 0x07, 0x2E, 0xff }; // 8x 01 04 07 2p ff 
byte zoomWide[6] =    { 0x81, 0x01, 0x04, 0x07, 0x3E, 0xff }; // 8x 01 04 07 3p ff
byte zoomStop[6] =    { 0x81, 0x01, 0x04, 0x07, 0x00, 0xff };
byte zoomDirect[9] =  { 0x81, 0x01, 0x04, 0x47, 0x00, 0x00, 0x00, 0x00, 0xff }; // 8x 01 04 47 0p 0q 0r 0s ff
byte zoomPosReq[5] =  { 0x81, 0x09, 0x04, 0x47, 0xff }; // Resp: y0 50 0p 0q 0r 0s ff 

// Focus
byte focusAuto[6] =     { 0x81, 0x01, 0x04, 0x38, 0x02, 0xff };
byte focusManual[6] =   { 0x81, 0x01, 0x04, 0x38, 0x03, 0xff };
byte focusDirect[9] =   { 0x81, 0x01, 0x04, 0x48, 0x00, 0x00, 0x00, 0x00, 0xff }; // 8x 01 04 48 0p 0q 0r 0s ff pqrs: focus position
byte focusModeInq[5] =  { 0x81, 0x09, 0x04, 0x38, 0xff }; // Resp: y0 50 0p ff ; p=2: Auto, p=3: Manual

// Iris / Gain
byte aeAuto[6] =      { 0x81, 0x01, 0x04, 0x39, 0x00, 0xff };
byte aeManual[6] =    { 0x81, 0x01, 0x04, 0x39, 0x03, 0xff };
byte irisDirect[9] =  { 0x81, 0x01, 0x04, 0x4B, 0x00, 0x00, 0x00, 0x00, 0xff }; // 8x 01 04 4B 0p 0q 0r 0s ff pqrs: Iris position, range 0..50
byte gainDirect[9] =  { 0x81, 0x01, 0x04, 0x4C, 0x00, 0x00, 0x00, 0x00, 0xff }; // 8x 01 04 4c 0p 0q 0r 0s ff pqrs: Gain position, values: 12-21dB.
byte aeModeInq[5] =   { 0x81, 0x09, 0x04, 0x39, 0xff }; // Resp: y0 50 0p ff ; p=0: Auto, p=3: Manual

// White Balance
byte wbAuto[6] =        { 0x81, 0x01, 0x04, 0x35, 0x00, 0xff };
byte wbTableManual[6] = { 0x81, 0x01, 0x04, 0x35, 0x06, 0xff };
byte wbTableDirect[9] = { 0x81, 0x01, 0x04, 0x75, 0x00, 0x00, 0x0, 0x00, 0xff }; // 8x 01 04 75 0p 0q 0r 0s ff pqrs = wb table.

// Config
byte address_command[4] = { 0x88, 0x30, 0x01, 0xFF }; // Sets camera address (Needed for Daisy Chaining)
byte if_clear[5] =        { 0x88, 0x01, 0x00, 0x01, 0xFF }; // Checks to see if communication line is clear
byte ir_off[6] =          { 0x81, 0x01, 0x06, 0x09, 0x03, 0xff }; // Turn off IR control (required for speed control of Pan/Tilt)
byte callLedOn[6] =     { 0x81, 0x01, 0x33, 0x01, 0x01, 0xff};
byte callLedOff[6] =    { 0x81, 0x01, 0x33, 0x01, 0x00, 0xff};
byte callLedBlink[6] =  { 0x81, 0x01, 0x33, 0x01, 0x02, 0xff};

int delayTime = 500;  //Time between commands

#endif
