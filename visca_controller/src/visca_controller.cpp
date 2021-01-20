#include "visca_controller.h"

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
  handleSerialControl();
  receiveViscaData();
  handleHardwareControl();
}

unsigned long time_now = 0;
bool panIdle = true;
bool tiltIdle = true;
int ptLow = 441;
int ptHight = 581;
int ptMaxSpeed = 5;
void handleHardwareControl() {
  processPan(analogRead(PAN));

  processTilt(analogRead(TILT));
  
  processButtons();
}


void receiveViscaData() {
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


void handleSerialControl() {
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
    }
  }
}

void processButtons() {
  int globalSpeed = analogRead(ZOOM);
  int zoomSpeed = map(globalSpeed, 0, 1023, 0, 15);
  if(millis() > time_now + 100) {
    time_now = millis();

    collectCurrentButtonStatus();

    int btn1 = buttons[0];
    if(buttonPressed(btn1) == true) {
      setButtonStatus(btn1, true);
      sendZoomPacket(0x20, zoomSpeed);
    } else if(buttonReleased(btn1) == true) {
      setButtonStatus(btn1, false);
      sendViscaPacket(zoomStop, sizeof(zoomStop));
    }

    int btn2 = buttons[1];
    if(buttonPressed(btn2) == true) {
      setButtonStatus(btn2, true);
      sendZoomPacket(0x30, zoomSpeed);
    } else if(buttonReleased(btn2) == true) {
      setButtonStatus(btn2, false);
      sendViscaPacket(zoomStop, sizeof(zoomStop));
    }

    int btn5 = buttons[4];
    if(buttonPressed(btn5) == true) {
      setButtonStatus(btn5, true);
      initCameras();
    } else if(buttonReleased(btn5) == true) {
      setButtonStatus(btn5, false);
    }
  }
}

void collectCurrentButtonStatus() {
  for (int i = 0; i < sizeof(buttons) / sizeof (buttons [0]); i++) {
    bitWrite(buttonCurrentStatus, buttons[i], (int) digitalRead(buttons[i]));
  }
}

bool buttonPressed(uint8_t button) {
  return getCurrentButtonStatus(button) == true && getPreviousButtonStatus(button) == false;
}

bool buttonReleased(uint8_t button) {
  return getCurrentButtonStatus(button) == false && getPreviousButtonStatus(button) == true;
}

bool getCurrentButtonStatus(uint8_t button) {
  return (bool) bitRead(buttonCurrentStatus, button);
}

void sendZoomPacket(byte zoomDir, int zoomSpeed) {
  uint8_t zoomDirSpeed = (uint8_t) zoomDir + zoomSpeed;
  zoom[4] = zoomDirSpeed;
  sendViscaPacket(zoom, sizeof(zoom));
}

bool getPreviousButtonStatus(uint8_t input) {
  return (bool) bitRead(buttonPreviousStatus, input);
}

void setButtonStatus(uint8_t input, bool status) {
  bitWrite(buttonPreviousStatus, input, (int) status);
}

void processPan(int pan) {
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
}

void processTilt(int tilt) {
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
}

void toggleFocusControl() {
  sendViscaPacket(focusModeInq, sizeof(focusModeInq));
  delay(100);
  receiveViscaData();
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
  if(echoCommand == true) {
    Serial.println();
  }
}

void initCameras() {
  //Send Address command
  Serial.println("Setting addresses...");
  sendViscaPacket(address_command, sizeof(address_command));
  delay(delayTime);  //delay to allow camera time for next command
  receiveViscaData();
  
  // Turn off IR control
  Serial.println("Disabling IR control...");
  sendViscaPacket(ir_off, sizeof(ir_off));
  delay(delayTime);  //delay to allow camera time for next command
  receiveViscaData();

  //Send IF_clear command
  Serial.println("Sending IF_Clear...");
  sendViscaPacket(if_clear, sizeof(if_clear));
  delay(delayTime);  //delay to allow camera time for next command
  receiveViscaData();
}
