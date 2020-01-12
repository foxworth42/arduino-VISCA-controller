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
      case '1':
        initCameras();
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

      case 'w':
        sendViscaPacket(panUp, sizeof(panUp));
        break;
      case 'a':
        sendViscaPacket(panLeft, sizeof(panLeft));
        break;
      case 's':
        sendViscaPacket(panDown, sizeof(panDown));
        break;
      case 'd':
        sendViscaPacket(panRight, sizeof(panRight));
        break;
      case 'q':
        sendViscaPacket(panStop, sizeof(panStop));
        break;
        
      case 'r':
        sendViscaPacket(zoomTele, sizeof(zoomTele));
        break;
      case 'f':
        sendViscaPacket(zoomStop, sizeof(zoomStop));
        break;
      case 'v':
        sendViscaPacket(zoomWide, sizeof(zoomWide));
        break;
    }
  }
}

unsigned long time_now = 0;
void readButtons() {
  if(millis() > time_now + 100) {
    time_now = millis();

    bool button1 = digitalRead(8);
    if(button1 == true) {
      sendViscaPacket(panTiltPosReq, sizeof(panTiltPosReq));
    }
  }
}

void sendViscaPacket(byte* packet, int byteSize) {
  Serial.print("Sending:");
  for (int i = 0; i < byteSize; i++) 
  {
    Serial.print(" 0x");
    Serial.print(packet[i], HEX);
    visca.write(packet[i]); 
  }
  Serial.println();
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
