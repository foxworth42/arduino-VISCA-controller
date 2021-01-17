#include "visca-controller.h"

//LiquidCrystal_I2C lcd(0x27,20,4);
SoftwareSerial visca(VISCARX, VISCATX);

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  for (uint8_t i = 4; i <= 12; i++) {
    pinMode(i,INPUT);
  }
  visca.begin(9600);

  initCameras();
  
  Serial.println("Started");
}

void loop() {
  readSerial();
  receiveData();
  readButtons();
  // int talentDimmer = map(analogRead(TALENT_DIMMER_PIN), 0, 1023, 0, 255);
}

void receiveData() {
  static byte ndx = 0;
  byte rc;
  while (visca.available() > 0) {
    rc = visca.read();
    
    if (rc != 0xFF) {
      viscaMessage[ndx] = rc;
      ndx++;
      if (ndx >= numChars) {
        ndx = numChars - 1;
      }
    }
    else {
      for (uint8_t i = 0; i < ndx; i++) {
        Serial.print("0x");
        Serial.print(viscaMessage[i], HEX);
        Serial.print(" ");
      }
      Serial.println("0xFF");
      ndx = 0;
      byte packet[3] = { 0x10, 0x41, 0xFF };
      visca.write(packet, 3);
    }
  }
}


void readSerial() {
  if (Serial.available() > 0)
  {
    char inChar = Serial.read(); // read incoming serial data:

    switch(inChar) {

// General/Toggles
      case '1':
        initCameras();
        break;
      case '2':
        toggleFocusControl();
        break;
      case '8':
        sendViscaPacket(callLedOn, sizeof(callLedOn));
        break;
      case '9':
        sendViscaPacket(callLedBlink, sizeof(callLedBlink));
        break;
      case '0':
        sendViscaPacket(callLedOff, sizeof(callLedOff));
        break;

// Pan/Tilt
      case 'q':
        sendViscaPacket(panUpLeft, sizeof(panUpLeft));
        break;
      case 'w':
        sendViscaPacket(panUp, sizeof(panUp));
        break;
      case 'e':
        sendViscaPacket(panUpRight, sizeof(panUpRight));
        break;
      case 'a':
        sendViscaPacket(panLeft, sizeof(panLeft));
        break;
      case 's':
        sendViscaPacket(panStop, sizeof(panStop));
        break;
      case 'd':
        sendViscaPacket(panRight, sizeof(panRight));
        break;
      case 'z':
        sendViscaPacket(panDownLeft, sizeof(panDownLeft));
        break;
      case 'x':
        sendViscaPacket(panDown, sizeof(panDown));
        break;
      case 'c':
        sendViscaPacket(panDownRight, sizeof(panDownRight));
        break;

// Zoom
      case 'r':
        sendViscaPacket(zoomTele, sizeof(zoomTele));
        break;
      case 'f':
        sendViscaPacket(zoomStop, sizeof(zoomStop));
        break;
      case 'v':
        sendViscaPacket(zoomWide, sizeof(zoomWide));
        break;

// Focus
      case 't':
        sendViscaPacket(focusFar, sizeof(focusFar));
        break;
      case 'g':
        sendViscaPacket(focusStop, sizeof(focusStop));
        break;
      case 'b':
        sendViscaPacket(focusNear, sizeof(focusNear));
        break;
    }
  }
}

unsigned long time_now = 0;
unsigned long pt_now = 0;
int lastZoomPos;
int lastZoom;
bool panIdle = true;
bool tiltIdle = true;
int ptLow = 441;
int ptHight = 581;
void readButtons() {
  int globalSpeed = analogRead(ZOOM);
  int zoomSpeed = map(globalSpeed, 0, 1023, 0, 15);
//    int ptMaxSpeed = map(globalSpeed, 0, 1023, 0, 7);
  int ptMaxSpeed = 5;
  int pan = analogRead(PAN);
  int tilt = analogRead(TILT);

    if(pan < ptLow || ptHight < pan) {
      uint8_t panSpeed;
      if(pan < ptLow) {
        // Left
        panSpeed = map(pan, ptLow, 0, 0, ptMaxSpeed);
        panTilt[6] = 0x01;
      } else {
        panSpeed = map(pan, ptHight, 1018, 0, ptMaxSpeed);
        // Right
        panTilt[6] = 0x02;
      }

      if(panTilt[4] != panSpeed) {
        panTilt[4] = panSpeed;
        sendViscaPacket(panTilt, sizeof(panTilt), true);
      }
      panIdle = false;
    } else {
      // Stop Pan
      panTilt[4] = 0x00;
      panTilt[6] = 0x03;
      if(panIdle == false) {
        sendViscaPacket(panTilt, sizeof(panTilt), true);
        panIdle = true;
      }
    }

    if(tilt < ptLow || ptHight < tilt) {
      uint8_t tiltSpeed;
      if(tilt < ptLow) {
        // Down
        tiltSpeed = map(tilt, ptLow, 0, 0, ptMaxSpeed);
        panTilt[7] = 0x02;
      } else {
        // Up
        tiltSpeed = map(tilt, ptHight, 1018, 0, ptMaxSpeed);
        panTilt[7] = 0x01;
      }
      
      if(panTilt[5] != tiltSpeed) {
        panTilt[5] = tiltSpeed;
        sendViscaPacket(panTilt, sizeof(panTilt), true);
      }
      tiltIdle = false;
    } else {
      // Stop Tilt
      panTilt[5] = 0x00;
      panTilt[7] = 0x03;
      if(tiltIdle == false) {
        sendViscaPacket(panTilt, sizeof(panTilt), true);
        tiltIdle = true;
      }
    }

    
  
  if(millis() > time_now + 100) {
    time_now = millis();

//    if(
//    if(zoomPos < lastZoomPos - 2 || lastZoomPos + 2 < zoomPos) {
//      lastZoomPos = zoomPos;
//      zoomPos = map(zoomPos, 0, 1023, 0, 2883);
//      byte mask = 0xF;
//      Serial.println(zoomPos);
//      zoomDirect[7] = zoomPos & mask;
//      zoomDirect[6] = zoomPos >> 4 & mask;
//      zoomDirect[5] = zoomPos >> 8 & mask;
//      zoomDirect[4] = zoomPos >> 12 & mask;
//      sendViscaPacket(zoomDirect, sizeof(zoomDirect), true);
//    }

    if((bool) digitalRead(BTN1) == true) {
      sendViscaPacket(zoomPosReq, sizeof(zoomPosReq), false);
    }

    if((bool) digitalRead(BTN3) == true) {
      uint8_t zoomDirSpeed = (uint8_t) 0x20 + zoomSpeed;
      zoom[4] = zoomDirSpeed;
      sendViscaPacket(zoom, sizeof(zoom));
    }
    
    if((bool) digitalRead(BTN5) == true) {
      uint8_t zoomDirSpeed = (uint8_t) 0x30 + zoomSpeed;
      zoom[4] = zoomDirSpeed;
      sendViscaPacket(zoom, sizeof(zoom));
    }

//    Serial.println((bool) digitalRead(BTN7));
//    if((bool) digitalRead(BTN7) == false) {
//      sendViscaPacket(zoomStop, sizeof(zoomStop));
//    }
  }
}

void handleButton(uint8_t input, uint8_t bitPosition) {
  bool previousStatus = (bool) bitRead(buttonStatus, bitPosition);
  bool currentStatus = (bool) digitalRead(input);
  if(previousStatus != currentStatus) {
    if(currentStatus == true) {
      bitWrite(buttonStatus, bitPosition, (int) currentStatus);
    } else {
      
    }
  }
}

void toggleFocusControl() {
  sendViscaPacket(focusModeInq, sizeof(focusModeInq));
  delay(100);
  receiveData();
  Serial.print("Current Focus Status: ");
  if(viscaMessage[2] == 2) {
    Serial.println("Auto, Toggling to manual");
    sendViscaPacket(focusManual, sizeof(focusManual));
  } else {
    Serial.println("Manual, Toggling to auto");
    sendViscaPacket(focusAuto, sizeof(focusAuto));
  }
}


void sendViscaPacket(byte* packet, int byteSize, bool echoCommand, bool sendPacket) {
  if(echoCommand == true) {
    Serial.print("Sending:");
  }
//  visca.write(byte 0x81);
  for (int i = 0; i < byteSize; i++) 
  {
    if(echoCommand == true) {
      Serial.print(" 0x");
      Serial.print(packet[i], HEX);
    }
    if(sendPacket == true) {
      visca.write(packet[i]);
    } 
  }
//  visca.write(byte 0xFF);
  if(echoCommand == true) {
    Serial.println();
  }
}

void initCameras() {
  //Send Address command
  Serial.println("Setting addresses...");
  sendViscaPacket(address_command, sizeof(address_command));
  delay(delayTime);  //delay to allow camera time for next command
  receiveData();
  
  // Turn off IR control
  Serial.println("Disabling IR control...");
  sendViscaPacket(ir_off, sizeof(ir_off));
  delay(delayTime);  //delay to allow camera time for next command
  receiveData();

  //Send IF_clear command
  Serial.println("Sending IF_Clear...");
  sendViscaPacket(if_clear, sizeof(if_clear));
  delay(delayTime);  //delay to allow camera time for next command
  receiveData();
}
