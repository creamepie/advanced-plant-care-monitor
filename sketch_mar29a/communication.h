#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <Arduino.h>
#include "actuators.h"
#include <avr/wdt.h>
#include <Adafruit_NeoPixel.h>

#define START_MARKER '<'
#define END_MARKER '>'
#define SEPARATOR '|'

// Define the serial port to use for ESP communication
// This assumes you're using Serial1 for ESP - adjust if you're using a different port
#define ESP_SERIAL Serial1
#define ESP_BAUD_RATE 115200

// Function declarations
void initializeESPCommunication();
bool receiveCommandFromESP(char* buffer, int bufferSize);
void sendDataToESP(int lightPercent, int moisturePercent, int rainValue, float temperature, float humidity);
void processESPCommand(const char* command);
bool isESPResponsive();
bool resetESPCommunication();
void playStartupAnimation();  // Add this line

// Declare as extern, not define
extern bool pumpAutoMode;

#endif // COMMUNICATION_H