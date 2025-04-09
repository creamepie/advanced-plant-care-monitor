#ifndef ACTUATORS_H
#define ACTUATORS_H

#include <Arduino.h>
#include "config.h"  // Make sure to include config.h for pin definitions

// Global variables for pump control
extern unsigned long pumpStartTime;
extern bool pumpActive;
extern bool pumpAutoMode; // Expose pumpAutoMode if needed elsewhere
extern bool pumpAutoMode;
extern uint8_t currentPumpMode;

#ifndef FAN_MODE_OFF
  #define FAN_MODE_OFF 0
  #define FAN_MODE_ON 1
  #define FAN_MODE_AUTO 2
#endif

// Function declarations
void initializeActuators();
void setLightMode(uint8_t mode);
uint8_t getLightMode();
void updateLightBasedOnMode(int lightPercent);
bool getPumpState();
bool getLightState();
void startupAnimation();
void setLightState(bool state);
void setLightColor(uint32_t newColor);
void swirlAnimation(uint32_t targetColor);
void softTransition(bool turnOn);
void rainbowCycle(int wait);

// Pump-related function declarations
void initializePump();
void setPumpMode(uint8_t mode);
uint8_t getPumpMode();
void updatePumpBasedOnMode(int soilMoisturePercent);
void setPumpState(bool state);

// Fan-related function declarations
void initializeFan();
void setFanState(bool state);
bool getFanState();
void setFanMode(uint8_t mode);
uint8_t getFanMode();
void updateFanBasedOnMode(float temperature);

#endif // ACTUATORS_H