#include "Visca.h"

Visca::Visca(bool debug)
{
    Visca::debug = debug;
}

void Visca::begin(Stream &viscaControl, Stream &debugOutput)
{
    Visca::viscaControl = viscaControl;
    Visca::debugOutput = debugOutput;
}

void Visca::sendPacket(unsigned char *packet, int byteSize) {
    if (Visca::debug == true) {
        Visca::debugOutput.print("Sending:");
    }
    for (int i = 0; i < byteSize; i++) {
        if (Visca::debug == true) {
            Visca::debugOutput.print(" 0x");
            Visca::debugOutput.print(packet[i], HEX);
        }

        Visca::viscaControl.write(packet[i]);
    }
    if (Visca::debug == true) {
        Visca::debugOutput.println();
    }
}

void Visca::receiveViscaData() {
    static unsigned char ndx = 0;
    while (Visca::viscaControl.available() > 0) {
        unsigned char rc = Visca::viscaControl.read();

        if (rc != 0xFF) {
            Visca::viscaMessage[ndx] = rc;
            ndx++;
            if (ndx >= Visca::maxViscaMessageSize) {
                ndx = Visca::maxViscaMessageSize - 1;
            }
        } else {
            if (Visca::viscaMessage[0] == 0x90) {
                if (Visca::debug == true) {
                    if (Visca::viscaMessage[1] == 0x50) {
                        Visca::debugOutput.println("Command: OK");
                    }
                }

                if (Visca::viscaMessage[1] == 0x60) {
                    switch (Visca::viscaMessage[2]) {
                        case 0x01:
                            Visca::debugOutput.println("Error: Message length error");
                        case 0x02:
                            Visca::debugOutput.println("Error: Syntax error");
                        case 0x03:
                            Visca::debugOutput.println("Error: Command buffer full");
                        case 0x04:
                            Visca::debugOutput.println("Error: Command cancelled");
                        case 0x05:
                            Visca::debugOutput.println("Error: No socket (to be cancelled)");
                        case 0x41:
                            Visca::debugOutput.println("Error: Command not executable");
                        default:
                            Visca::debugOutput.print("Unknown Error: ");
                            for (uint8_t i = 0; i < ndx; i++) {
                                Visca::debugOutput.print("0x");
                                Visca::debugOutput.print(viscaMessage[i], HEX);
                                Visca::debugOutput.print(" ");
                            }
                            Visca::debugOutput.println("0xFF");
                    }
                }
            }
            ndx = 0;
        }
    }
}
