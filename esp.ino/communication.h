#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>
#include <ArduinoJson.h>
#include <ESP8266WebServer.h>

// Define communication pins
#define RX_PIN D6  // ESP RX (D6) connects to Arduino TX (pin 18)
#define TX_PIN D5  // ESP TX (D5) connects to Arduino RX (pin 19)
#define TX_LED_PIN D2  // Data being sent to Arduino - visible LED
#define RX_LED_PIN D0  // Data being received from Arduino - visible LED

// Add pattern definitions for LED indicators
#define LED_PATTERN_NONE 0
#define LED_PATTERN_SLOW_BLINK 1
#define LED_PATTERN_FAST_BLINK 2
#define LED_PATTERN_DOUBLE_PULSE 3
#define LED_PATTERN_SOLID 4

// Add variables for LED patterns
extern uint8_t txLedPattern;
extern uint8_t rxLedPattern;
extern unsigned long ledPatternStartTime;

// Message protocol
#define START_MARKER '<'
#define END_MARKER '>'
#define SEPARATOR '|'

// Buffer for receiving data
extern const int MAX_MESSAGE_LENGTH; // Only declare as extern here, don't define with a value
extern char* receivedChars; // Changed: Remove size from extern declaration
extern boolean messageInProgress;
extern int charIndex;

// LED timing variables
extern unsigned long rxLedOffTime;
extern unsigned long txLedOffTime;
extern const int LED_BLINK_DURATION;

// Connection status
extern bool connected;
extern unsigned long lastDataReceived;

// Sensor values
extern int light;
extern int soil;
extern int rain;
extern float temperature;
extern float humidity;
extern bool pumpActive;
extern int lightMode;
extern bool fanActive;
extern int fanMode;
extern int pumpMode;

// External objects
extern ESP8266WebServer server;
extern String latestData;
extern SoftwareSerial arduinoSerial;

#define MAX_MESSAGE_SIZE 512  // Increase buffer size for larger messages

// Function declarations
void initCommunication();
void readFromArduino();
void parseMessage(char* message);
void sendCommand(const String& command);
void blinkStatusLED();
void updateLEDs();
void parseIncomingData(const String &input);

// Add function declarations for LED patterns
void setTxLedPattern(uint8_t pattern, unsigned long duration = 500);
void setRxLedPattern(uint8_t pattern, unsigned long duration = 500);
void updateLedPatterns();

#endif