#ifndef VISCACONTROLLER
#define VISCACONTROLLER

#include <Arduino.h>
//#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>

// Toggle for echoing VISCA commands sent/received over serial monitor.
#define DEBUG_VISCA 0

// Pin assignments from arduno shield.
// Analog inputs
#define PAN 0
#define TILT 1
#define ZOOM 2
#define AUX1 3

// Serial I/O for VISCA communication
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
const int buttons[] = {
    BTN1,
    BTN2,
    BTN3,
    BTN4,
    BTN5,
    BTN6,
    BTN7,
    BTN8,
    BTN9,
    BTN10
};

const int delayTime = 500;  //Time between commands
const byte maxViscaMessageSize = 16;
byte viscaMessage[maxViscaMessageSize];

byte buttonPreviousStatus = 0x00;
byte buttonCurrentStatus = 0x00;

byte analogCurrentStatus = 0x00;
byte analogPreviousStatus = 0x00;
const int ptMaxSpeed = 5;
int panThresholds[2] = {0, 0};
int tiltThresholds[2] = {0, 0};
int zoomThresholds[2] = {0, 0};
int auxThresholds[2] = {0, 0};

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
// Tele: 8x 01 04 07 2p ff 
// Wide: 8x 01 04 07 2p ff 
byte zoomCommand[6] = { 0x81, 0x01, 0x04, 0x07, 0x2F, 0xff }; // 8x 01 04 07 2p ff
byte zoomTele[6] =    { 0x81, 0x01, 0x04, 0x07, 0x2F, 0xff }; // 8x 01 04 07 2p ff
byte zoomWide[6] =    { 0x81, 0x01, 0x04, 0x07, 0x3F, 0xff }; // 8x 01 04 07 3p ff
byte zoomStop[6] =    { 0x81, 0x01, 0x04, 0x07, 0x00, 0xff };
byte zoomDirect[9] =  { 0x81, 0x01, 0x04, 0x47, 0x00, 0x00, 0x00, 0x00, 0xff }; // 8x 01 04 47 0p 0q 0r 0s ff pqrs: zoomCommand position
byte zoomPosReq[5] =  { 0x81, 0x09, 0x04, 0x47, 0xff }; // Resp: y0 50 0p 0q 0r 0s ff


// Focus
byte focusAuto[6] =     { 0x81, 0x01, 0x04, 0x38, 0x02, 0xff };
byte focusManual[6] =   { 0x81, 0x01, 0x04, 0x38, 0x03, 0xff };
byte focusDirect[9] =   { 0x81, 0x01, 0x04, 0x48, 0x00, 0x00, 0x00, 0x00, 0xff }; // 8x 01 04 48 0p 0q 0r 0s ff pqrs: focus position
byte focusFar[6] =      { 0x81, 0x01, 0x04, 0x08, 0x20, 0xff };
byte focusNear[6] =     { 0x81, 0x01, 0x04, 0x08, 0x30, 0xff };
byte focusStop[6] =     { 0x81, 0x01, 0x04, 0x08, 0x00, 0xff };
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
byte addressCommand[4] = {0x88, 0x30, 0x01, 0xFF }; // Sets camera address (Needed for Daisy Chaining)
byte ifClear[5] =        {0x88, 0x01, 0x00, 0x01, 0xFF }; // Checks to see if communication line is clear
byte irOff[6] =          {0x81, 0x01, 0x06, 0x09, 0x03, 0xff }; // Turn off IR control (required for speed control of Pan/Tilt on TelePresence cameras)
byte callLedOn[6] =     { 0x81, 0x01, 0x33, 0x01, 0x01, 0xff };
byte callLedOff[6] =    { 0x81, 0x01, 0x33, 0x01, 0x00, 0xff };
byte callLedBlink[6] =  { 0x81, 0x01, 0x33, 0x01, 0x02, 0xff };
/*
 * Video formats values:
 * Value    HDMI    SDI
 * 0x00     1080p25 1080p25
 * 0x01     1080p30 1080p30
 * 0x02     1080p50 720p50
 * 0x03     1080p60 720p60
 * 0x04     720p25  720p25
 * 0x05     720p30  720p30
 * 0x06     720p50  720p50
 * 0x07     720p60  720p60
 */
byte format = 0x01;
byte videoFormat[7] = { 0x81, 0x01, 0x35, 0x00, format, 0x00, 0xff }; // 8x 01 35 0p 0q 0r ff p = reserved, q = video mode, r = Used in PrecisionHD 720p camera.

void sendViscaPacket(byte *packet, int byteSize);
void handleHardwareControl();
void receiveViscaData();
void handleSerialControl();
void processButtons();
void collectCurrentButtonStatus();
bool buttonPressed(uint8_t button);
bool buttonReleased(uint8_t button);
bool getCurrentButtonStatus(uint8_t button);
void sendZoomPacket(byte zoomDir, int zoomSpeed);
bool getAnalogCurrentStatus(uint8_t input);
void setAnalogCurrentStatus(uint8_t input, bool status);
bool getAnalogPreviousStatus(uint8_t input);
void setAnalogPreviousStatus(uint8_t input, bool status);
bool getPreviousButtonStatus(uint8_t input);
void setButtonStatus(uint8_t input, bool status);
void processPan(int pan);
void processTilt(int tilt);
void processZoom(int zoom);
void toggleFocusControl();
void initCameras();
void calibrateAnalogControls();

#endif
