#include "visca_controller.h"

//LiquidCrystal_I2C lcd(0x27,20,4);
SoftwareSerial viscaOutput(VISCARX, VISCATX);

void setup() {
    Serial.begin(115200);
    while (!Serial) { ; // wait for serial port to connect. Needed for native USB port only
    }

    for (uint8_t i = 4; i <= 12; i++) {
        pinMode(i, INPUT);
    }
    viscaOutput.begin(9600);

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
bool zoomIdle = true;
int analogLowThreshold = 441;
int analogHighThreshold = 581;
int ptMaxSpeed = 5;

void handleHardwareControl() {
    processPan(analogRead(PAN));

    processTilt(analogRead(TILT));

    processZoom(analogRead(ZOOM));

    processButtons();
}


void receiveViscaData() {
    static byte ndx = 0;
    byte rc;
    while (viscaOutput.available() > 0) {
        rc = viscaOutput.read();

        if (rc != 0xFF) {
            viscaMessage[ndx] = rc;
            ndx++;
            if (ndx >= maxViscaMessageSize) {
                ndx = maxViscaMessageSize - 1;
            }
        } else {
            if (DEBUG_VISCA == 1) {
                for (uint8_t i = 0; i < ndx; i++) {
                    Serial.print("0x");
                    Serial.print(viscaMessage[i], HEX);
                    Serial.print(" ");
                }
                Serial.println("0xFF");
            }
            ndx = 0;
            byte packet[3] = {0x10, 0x41, 0xFF};
            viscaOutput.write(packet, 3);
        }
    }
}


void handleSerialControl() {
    if (Serial.available() > 0) {
        char inChar = Serial.read(); // read incoming serial data:

        switch (inChar) {
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
    int globalSpeed = analogRead(AUX1);
    int buttonZoomSpeed = map(globalSpeed, 0, 1023, 0, 15);
    if (millis() > time_now + 100) {
        time_now = millis();

        collectCurrentButtonStatus();

        int btn1 = buttons[0];
        if (buttonPressed(btn1) == true) {
            setButtonStatus(btn1, true);
            sendZoomPacket(0x20, buttonZoomSpeed);
        } else if (buttonReleased(btn1) == true) {
            setButtonStatus(btn1, false);
            sendViscaPacket(zoomStop, sizeof(zoomStop));
        }

        int btn2 = buttons[1];
        if (buttonPressed(btn2) == true) {
            setButtonStatus(btn2, true);
            sendZoomPacket(0x30, buttonZoomSpeed);
        } else if (buttonReleased(btn2) == true) {
            setButtonStatus(btn2, false);
            sendViscaPacket(zoomStop, sizeof(zoomStop));
        }

        int btn5 = buttons[4];
        if (buttonPressed(btn5) == true) {
            setButtonStatus(btn5, true);
            initCameras();
        } else if (buttonReleased(btn5) == true) {
            setButtonStatus(btn5, false);
        }
    }
}

void collectCurrentButtonStatus() {
    for (int i = 0; i < (int) (sizeof(buttons) / sizeof(buttons[0])); i++) {
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
    zoomCommand[4] = zoomDirSpeed;
    sendViscaPacket(zoomCommand, sizeof(zoomCommand));
}

bool getPreviousButtonStatus(uint8_t input) {
    return (bool) bitRead(buttonPreviousStatus, input);
}

void setButtonStatus(uint8_t input, bool status) {
    bitWrite(buttonPreviousStatus, input, (int) status);
}

void processZoom(int zoom) {
    int zoomMaxSpeed = 15;
    if (zoom < analogLowThreshold || analogHighThreshold < zoom) {
        uint8_t zoomSpeed;
        byte zoomDir;
        if (zoom < analogLowThreshold) {
            // Zoom Out
            zoomSpeed = map(zoom, analogLowThreshold, 0, 0, zoomMaxSpeed);
            zoomDir = 0x30;
        } else {
            // Zoom In
            zoomSpeed = map(zoom, analogHighThreshold, 1018, 0, zoomMaxSpeed);
            zoomDir = 0x20;
        }

        uint8_t zoomDirSpeed = (uint8_t) zoomDir + zoomSpeed;
        if (zoomCommand[4] != zoomDirSpeed) {
            zoomCommand[4] = zoomDirSpeed;
            sendViscaPacket(zoomCommand, sizeof(zoomCommand));
        }
        zoomIdle = false;
    } else {
        // Stop Zoom
        if (zoomIdle == false) {
            sendViscaPacket(zoomStop, sizeof(zoomStop));
            zoomIdle = true;
        }
    }
}

void processPan(int pan) {
    if (pan < analogLowThreshold || analogHighThreshold < pan) {
        uint8_t panSpeed;
        if (pan < analogLowThreshold) {
            // Left
            panSpeed = map(pan, analogLowThreshold, 0, 0, ptMaxSpeed);
            panTilt[6] = 0x01;
        } else {
            panSpeed = map(pan, analogHighThreshold, 1018, 0, ptMaxSpeed);
            // Right
            panTilt[6] = 0x02;
        }

        if (panTilt[4] != panSpeed) {
            panTilt[4] = panSpeed;
            sendViscaPacket(panTilt, sizeof(panTilt));
        }
        panIdle = false;
    } else {
        // Stop Pan
        panTilt[4] = 0x00;
        panTilt[6] = 0x03;
        if (panIdle == false) {
            sendViscaPacket(panTilt, sizeof(panTilt));
            panIdle = true;
        }
    }
}

void processTilt(int tilt) {
    if (tilt < analogLowThreshold || analogHighThreshold < tilt) {
        uint8_t tiltSpeed;
        if (tilt < analogLowThreshold) {
            // Down
            tiltSpeed = map(tilt, analogLowThreshold, 0, 0, ptMaxSpeed);
            panTilt[7] = 0x02;
        } else {
            // Up
            tiltSpeed = map(tilt, analogHighThreshold, 1018, 0, ptMaxSpeed);
            panTilt[7] = 0x01;
        }

        if (panTilt[5] != tiltSpeed) {
            panTilt[5] = tiltSpeed;
            sendViscaPacket(panTilt, sizeof(panTilt));
        }
        tiltIdle = false;
    } else {
        // Stop Tilt
        panTilt[5] = 0x00;
        panTilt[7] = 0x03;
        if (tiltIdle == false) {
            sendViscaPacket(panTilt, sizeof(panTilt));
            tiltIdle = true;
        }
    }
}

void toggleFocusControl() {
    sendViscaPacket(focusModeInq, sizeof(focusModeInq));
    delay(100);
    receiveViscaData();
    Serial.print("Current Focus Status: ");
    if (viscaMessage[2] == 2) {
        Serial.println("Auto, Toggling to manual");
        sendViscaPacket(focusManual, sizeof(focusManual));
    } else {
        Serial.println("Manual, Toggling to auto");
        sendViscaPacket(focusAuto, sizeof(focusAuto));
    }
}


void sendViscaPacket(byte *packet, int byteSize) {
    if (DEBUG_VISCA == 1) {
        Serial.print("Sending:");
    }
    for (int i = 0; i < byteSize; i++) {
        if (DEBUG_VISCA == 1) {
            Serial.print(" 0x");
            Serial.print(packet[i], HEX);
        }

        viscaOutput.write(packet[i]);
    }
    if (DEBUG_VISCA == 1) {
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
