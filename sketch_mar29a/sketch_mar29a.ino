#include <Arduino.h>
#include <avr/wdt.h>
#include "config.h"
#include "sensors.h"
#include "actuators.h"
#include "display.h"
#include "communication.h"

// Timing variables
unsigned long lastDisplayUpdate = 0;
unsigned long lastPumpCheck = 0;
unsigned long lastLightCheck = 0;
unsigned long lastFanCheck = 0;
unsigned long lastESPComm = 0;

// Buffer for ESP commands
char espCommandBuffer[128];

void setup() {
  // Initialize serial communication
  Serial.begin(115200);
  delay(100); // Give serial a moment to start up
  Serial.println("\n\n=== Plant Care System Initializing ===");
  
  // Disable watchdog during initialization
  wdt_disable();
  
  // Initialize all modules
  initializeSensors();
  initializeActuators();
  initializeDisplay();
  initializeESPCommunication();
  
  // Show loading animation on display
  drawLoadingScreen();
  
  // Draw main screen
  drawMainScreen();
  
  // Enable watchdog timer (8-second timeout)
  wdt_enable(WDTO_8S);
  
  Serial.println("Initialization complete. Running...");
}

void loop() {
  // Reset watchdog timer
  wdt_reset();
  
  // Update sensor readings
  updateSensorReadings();
  
  // Current time for timing checks
  unsigned long currentMillis = millis();
  
  // Update actuators based on sensor readings and current modes
  if (currentMillis - lastPumpCheck >= PUMP_CONTROL_INTERVAL) {
    updatePumpBasedOnMode(moistureReading);
    lastPumpCheck = currentMillis;
  }
  
  if (currentMillis - lastLightCheck >= LIGHT_CONTROL_INTERVAL) {
    updateLightBasedOnMode(lightReading);
    lastLightCheck = currentMillis;
  }
  
  if (currentMillis - lastFanCheck >= FAN_CONTROL_INTERVAL) {
    updateFanBasedOnMode(temperatureReading);
    lastFanCheck = currentMillis;
  }
  
  // Update display
  if (currentMillis - lastDisplayUpdate >= DISPLAY_UPDATE_INTERVAL) {
    refreshDisplay(lightReading, moistureReading, temperatureReading, humidityReading, getFanState());
    lastDisplayUpdate = currentMillis;
  }
  
  // Check for commands from ESP
  if (receiveCommandFromESP(espCommandBuffer, sizeof(espCommandBuffer))) {
    processESPCommand(espCommandBuffer);
  }
  
  // Send data to ESP
  if (currentMillis - lastESPComm >= ESP_COMM_INTERVAL) {
    sendDataToESP(lightReading, moistureReading, rainSensorState, temperatureReading, humidityReading);
    lastESPComm = currentMillis;
  }
}