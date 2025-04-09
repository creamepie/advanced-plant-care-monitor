#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <Arduino.h>
#include "config.h"

// Communication protocol constants
#define START_MARKER '<'
#define END_MARKER '>'
#define SEPARATOR '|'

// Function declarations
void initializeESPCommunication();
void sendDataToESP(int light, int soil, int rain, float temp, float humidity);
bool receiveCommandFromESP(char* buffer, int bufferSize);
void processESPCommand(const char* command);

#endif // COMMUNICATION_H