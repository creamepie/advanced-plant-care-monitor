#include <Arduino.h>
#include <avr/wdt.h>  // Add watchdog timer library

// Define analog pins for Arduino Mega explicitly
#ifndef A0
#define A0 54  // First analog pin on Mega
#define A1 55
#define A2 56
#define A3 57
#define A4 58
#define A5 59
#endif

#include "config.h"
#include "sensors.h"
#include "actuators.h"
#include "display.h"
#include "communication.h" // Add this to your includes

// Add these globals at the top of your file
unsigned long lastLogTime = 0;
#define LOG_INTERVAL 300000 // Log every 5 minutes (in milliseconds)

unsigned long lastESPCommTime = 0; // Add this global
char espCommand[64];              // Add this global
int rainValue = 0;                // Add this global
int soilValue = 0;                // Add this global

int lightPercent;
int moisturePercent;
float temperature, humidity;

// Add these globals for debouncing and timing
unsigned long lastPumpUpdateTime = 0;
#define PUMP_UPDATE_INTERVAL 1000 // Minimum 1 second between pump updates

unsigned long lastDisplayRefreshTime = 0;
#define DISPLAY_REFRESH_INTERVAL 1000 // Refresh display every 1 second

// Add this function to reset watchdog during long operations
void safeDelay(unsigned long ms) {
  unsigned long start = millis();
  while (millis() - start < ms) {
    wdt_reset();  // Reset watchdog during long operations
    delay(10);    // Short delay that won't cause issues
  }
}

// Function to handle automatic plant care based on sensor readings
void handleAutomation(int lightPercent, int moisturePercent) {
  // Use the new updateLightBasedOnMode function for light control
  updateLightBasedOnMode(lightPercent);
  
  // Moisture control - water the plant when soil is too dry
  if (moisturePercent < 20) {
    setPumpState(true);
    delay(2000); // Run pump for 2 seconds
    setPumpState(false);
    Serial.println("AUTO: Watering plant due to low moisture levels");
  }
}

// Function to log sensor data for analysis
void logSensorData(int light, int moisture, float temp, float humidity) {
  // Only log at the specified interval
  unsigned long currentTime = millis();
  if (currentTime - lastLogTime < LOG_INTERVAL) {
    return;
  }
  
  lastLogTime = currentTime;
  
  // Format: Timestamp, Light%, Moisture%, Temp, Humidity
  Serial.print("DATA_LOG,");
  Serial.print(currentTime / 1000); // seconds since start
  Serial.print(",");
  Serial.print(light);
  Serial.print(",");
  Serial.print(moisture);
  Serial.print(",");
  Serial.print(temp);
  Serial.print(",");
  Serial.println(humidity);
}

// Add this function to calibrate the touch screen
void calibrateTouch() {
  // For debugging the touch screen
  Serial.println("Touch Screen Calibration Mode");
  Serial.println("Touch the screen in different places and note the values");
  Serial.println("Press Reset button when done");
  
  tft.fillScreen(0x001F); // Blue background
  tft.setTextColor(0xFFFF); // White text
  tft.setTextSize(3);
  tft.setCursor(40, 40);
  tft.print("TOUCH TEST");
  tft.setTextSize(2);
  tft.setCursor(20, 100);
  tft.print("Touch anywhere on");
  tft.setCursor(20, 120);
  tft.print("screen to test.");
  
  // Red target at center
  tft.fillCircle(tft.width()/2, tft.height()/2, 5, 0xF800);
  
  unsigned long startTime = millis();
  // Add safety timeout of 60 seconds
  while(millis() - startTime < 60000) { // Exit after 60 seconds
    wdt_reset(); // Reset watchdog during calibration
    
    TSPoint p = ts.getPoint();
    
    // CRITICAL: Reset pins
    pinMode(XP, OUTPUT);
    pinMode(XM, OUTPUT);
    pinMode(YP, OUTPUT);
    pinMode(YM, OUTPUT);
    
    if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
      // Draw a dot where touched
      int x = map(p.y, TS_MINX, TS_MAXX, 0, tft.width());
      int y = map(p.x, TS_MINY, TS_MAXY, 0, tft.height());
      
      // Draw a small dot at the touch location
      tft.fillCircle(x, y, 3, 0xFFE0); // Yellow dot
      
      // Update text with coordinates
      tft.fillRect(20, 160, 200, 60, 0x001F); // Clear previous text
      tft.setCursor(20, 160);
      tft.print("RAW: ");
      tft.print(p.x);
      tft.print(",");
      tft.print(p.y);
      
      tft.setCursor(20, 180);
      tft.print("MAP: ");
      tft.print(x);
      tft.print(",");
      tft.print(y);
      
      Serial.print("Touch at RAW(");
      Serial.print(p.x);
      Serial.print(",");
      Serial.print(p.y);
      Serial.print(") MAP(");
      Serial.print(x);
      Serial.print(",");
      Serial.println(y);
      
      delay(100); // Brief delay
    }
  }
  
  Serial.println("Calibration mode timed out - returning to normal operation");
}

// Add this function to your sketch
void calibrateSensors() {
  Serial.println("=== SENSOR CALIBRATION MODE ===");
  Serial.println("This will print raw sensor values for 30 seconds");
  Serial.println("Use these values to adjust your sensor conversion formulas");
  
  unsigned long startTime = millis();
  
  while (millis() - startTime < 30000) {
    // Read soil moisture
    int soilRaw = analogRead(SOIL_MOISTURE_PIN);
    
    // Read light sensor
    int lightRaw;
    if (LIGHT_SENSOR_PIN < 54) {  // Digital pin
      lightRaw = digitalRead(LIGHT_SENSOR_PIN) ? 1023 : 0;
    } else {  // Analog pin
      lightRaw = analogRead(LIGHT_SENSOR_PIN);
    }
    
    // Print values
    Serial.print("Soil moisture raw: ");
    Serial.print(soilRaw);
    Serial.print("  |  Light raw: ");
    Serial.println(lightRaw);
    
    delay(500);  // Print every half second
  }
  
  Serial.println("=== CALIBRATION COMPLETE ===");
  Serial.println("Update your sensor conversion code based on these values");
}

// Fix function conflict - startupLightShow is defined but startupAnimation is called
void setup() {
  // Initialize watchdog timer to 8 seconds
  wdt_enable(WDTO_8S);

  Serial.begin(115200);
  Serial.println("Plant Care System with Display");
  
  initializeActuators();
  initializeSensors();
  initializeDisplay();
  initializeESPCommunication(); // Add after other initializations
  
  // Run our cool LED startup animation - use the correct function
  startupAnimation(); // Change this line instead of calling startupAnimation()
  
  delay(1000); // Wait for sensors to stabilize
  Serial.println("Setup complete. Starting sensor readings...");
  
  // Uncomment this line to run calibration at startup
  // calibrateSensors();
}

void loop() {
  // Reset watchdog at the start of each loop
  wdt_reset();

  // Get current time
  unsigned long currentTime = millis();

  // Read all sensors
  bool lightSuccess = readLightSensor(&lightPercent);
  bool moistureSuccess = readMoistureSensor(&moisturePercent);
  bool tempSuccess = readTemperatureSensor(&temperature);
  bool humidSuccess = readHumiditySensor(&humidity);
  bool rainDetected = readRainSensor();

  // Apply fallback values for failed readings
  if (!tempSuccess) temperature = 25.0;
  if (!humidSuccess) humidity = 50.0;

  // Handle automated plant care (debounced)
  if (currentTime - lastPumpUpdateTime >= PUMP_UPDATE_INTERVAL) {
    handleAutomation(lightPercent, moisturePercent);
    lastPumpUpdateTime = currentTime;
  }

  // Refresh display (debounced)
  if (currentTime - lastDisplayRefreshTime >= DISPLAY_REFRESH_INTERVAL) {
    updateDisplay(lightPercent, moisturePercent, temperature, humidity, getFanState()); // Pass fanState
    lastDisplayRefreshTime = currentTime;
  }

  // Log data for analysis
  logSensorData(lightPercent, moisturePercent, temperature, humidity);

  // Communicate with ESP
  if (currentTime - lastESPCommTime >= ESP_COMM_INTERVAL) {
    sendDataToESP(lightPercent, moisturePercent, rainDetected ? 1 : 0, temperature, humidity);
    lastESPCommTime = currentTime;
  }

  // Check for incoming commands from ESP
  if (receiveCommandFromESP(espCommand, sizeof(espCommand))) {
    processESPCommand(espCommand);
  }

  // Handle automatic fan control
  updateFanBasedOnMode(temperature);

  // Handle automatic pump control
  updatePumpBasedOnMode(moisturePercent);

  // Add this line to handle touch input
  handleTouchInput();

  // Reset watchdog at the end of the loop
  wdt_reset();

  // Small delay to prevent excessive loop frequency
  delay(10);
}

// Add timeout to touch handling
void handleTouchInputWithTimeout(unsigned long maxTime) {
  unsigned long startTime = millis();
  
  // Get touch point
  TSPoint p = ts.getPoint();
  
  // Reset pins IMMEDIATELY
  pinMode(XP, OUTPUT);
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
  pinMode(YM, OUTPUT);
  
  // Only process if the touch is valid AND we haven't exceeded timeout
  if (p.z > MINPRESSURE && p.z < MAXPRESSURE && (millis() - startTime) < maxTime) {
    // Map touch coordinates to screen coordinates
    int y = map(p.x, TS_MINY, TS_MAXY, 0, tft.height());
    int x = map(p.y, TS_MINX, TS_MAXX, 0, tft.width());
    
    // Check if touch is in the bottom status bar area
    if (y > (tft.height() - 30)) {
      // Left half - Light control
      if (x < tft.width()/2) {
        uint8_t currentMode = getLightMode();
        uint8_t newMode = (currentMode + 1) % 3;
        setLightMode(newMode);
        updateStatusIndicators();
      }
      // Right half - Pump control
      else {
        bool currentPumpState = getPumpState();
        setPumpState(!currentPumpState);
        updateStatusIndicators();
      }
    }
  }
}

// Add this function at the end of the file
int freeMemory() {
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}