#include "sensors.h"
#include "config.h"  // Add this to get pin definitions
#include <DHT.h>
#include <avr/wdt.h>  // Add watchdog support

// Define DHT sensor
#define DHTPIN 22       // Digital pin connected to DHT sensor
#define DHTTYPE DHT22  // DHT 22 (AM2302) - change to DHT11 if using that sensor

// Create DHT instance
DHT dht(DHTPIN, DHTTYPE);

// Add at the top of the file with other globals
int lastLightPercent = 50; // Initial value for smoothing

// Add a moving average for soil moisture readings
#define MOISTURE_SAMPLES 5
int moistureReadings[MOISTURE_SAMPLES] = {0};
int moistureIndex = 0;

// Global sensor variables
int lightReading = 0;
int moistureReading = 0;
bool rainSensorState = false;
float temperatureReading = 0.0;
float humidityReading = 0.0;

// Timing variables for sensor updates
unsigned long lastSensorUpdate = 0;

void initializeSensors() {
  // Set pin modes for sensors
  pinMode(LIGHT_SENSOR_PIN, INPUT);  // Digital light sensor
  pinMode(SOIL_MOISTURE_PIN, INPUT);  // Digital soil moisture sensor
  
  // Optional: Add pull-up resistor for more stable readings
  // digitalWrite(SOIL_MOISTURE_PIN, HIGH);  // Enable internal pull-up

  pinMode(RAIN_SENSOR_PIN, INPUT_PULLUP);   // Digital rain sensor
  
  // Initialize DHT sensor
  pinMode(DHTPIN, INPUT_PULLUP);
  dht.begin();
  
  delay(1000); // Allow sensors to stabilize
  
  Serial.println(F("All sensors initialized"));
}

// Fix the light sensor inversion issue
float readLightSensor() {
  // Read the sensor 
  int rawValue;
  
  // Handle digital or analog sensor
  if (LIGHT_SENSOR_PIN < 54) {  // Digital pin
    // Invert the reading: 0→100 (bright), 1→0 (dark)
    rawValue = digitalRead(LIGHT_SENSOR_PIN) ? 0 : 1023;
  } else {  // Analog pin
    rawValue = analogRead(LIGHT_SENSOR_PIN);
    // Important: INVERT the mapping for correct behavior
    // We want 0 for darkness and 100 for brightness
  }
  
  // Debug raw reading
  Serial.print("Light sensor raw value: ");
  Serial.println(rawValue);
  
  // FIXED: This mapping was backwards - we want 0→dark, 100→bright
  float percent = map(rawValue, 0, 1023, 0, 100);  // Correct mapping (0=dark, 1023=bright)
  
  // Ensure value stays within 0-100 range
  percent = constrain(percent, 0, 100);
  
  // Debug final percentage
  Serial.print("Light percentage: ");
  Serial.println(percent);
  
  return percent;
}

// Replace these functions:

// Improved moisture sensor reading with validation
int readSoilMoistureSensor() {
  static int lastReading = 50; // Start with a middle value
  static unsigned long lastReadTime = 0;
  static const unsigned long READ_INTERVAL = 1000; // Read every 1000ms
  static int readingsCount = 0;
  static int readingsSum = 0;
  static int readingsMissed = 0;
  
  // Only take new readings at the specified interval
  unsigned long currentTime = millis();
  if (currentTime - lastReadTime < READ_INTERVAL) {
    return lastReading; // Return last reading instead of constantly reading
  }
  
  lastReadTime = currentTime;
  
  // Read the raw sensor value
  int rawValue;
  
  // Handle digital or analog sensor
  if (SOIL_MOISTURE_PIN < 54) {  // Digital pin
    // For digital sensor - HIGH typically means dry, LOW means wet
    // But we add error detection - check twice to validate
    bool reading1 = digitalRead(SOIL_MOISTURE_PIN);
    delay(5);
    bool reading2 = digitalRead(SOIL_MOISTURE_PIN);
    
    // Only accept the reading if both checks match
    if (reading1 == reading2) {
      rawValue = reading1 ? 0 : 100; // 0% moisture when HIGH (dry), 100% moisture when LOW (wet)
    } else {
      // If inconsistent, log a warning and use last reading
      Serial.println("WARNING: Inconsistent soil moisture sensor readings, using last valid value");
      return lastReading;
    }
  } else {  // Analog pin
    // Take multiple readings for better accuracy
    int sum = 0;
    for (int i = 0; i < 3; i++) {
      sum += analogRead(SOIL_MOISTURE_PIN);
      delay(5);
    }
    rawValue = sum / 3;
    
    // Map analog value to percentage
    rawValue = map(rawValue, 0, 1023, 100, 0);  // Assuming 0=wet, 1023=dry
    
    // Check for extreme readings that may indicate disconnection
    if (rawValue <= 5 || rawValue >= 95) {
      readingsMissed++;
      if (readingsMissed > 3) {
        Serial.println("WARNING: Multiple extreme moisture readings - sensor may be disconnected");
      }
      
      // If we have multiple sequential extreme readings, return last stable value
      if (readingsMissed > 5) {
        return lastReading;
      }
    } else {
      readingsMissed = 0; // Reset missed counter if reading is in normal range
    }
  }
  
  // Apply moving average over 5 readings
  readingsCount++;
  readingsSum += rawValue;
  if (readingsCount > 5) {
    readingsCount = 5;
    readingsSum = readingsSum - lastReading + rawValue;
  }
  
  // Calculate rolling average
  int smoothedValue = readingsSum / readingsCount;
  
  // Apply additional smoothing filter to reduce noise (90% old value, 10% new value)
  smoothedValue = (lastReading * 9 + smoothedValue * 1) / 10;
  
  // Debug raw vs smoothed reading
  Serial.print("Soil moisture raw: ");
  Serial.print(rawValue);
  Serial.print(", smoothed: ");
  Serial.println(smoothedValue);
  
  // Add range constraints for extra safety
  smoothedValue = constrain(smoothedValue, 5, 95);
  
  // Update last reading and return
  lastReading = smoothedValue;
  return smoothedValue;
}

// Update the wrapper function
bool readMoistureSensor(int* moisturePercent) {
  *moisturePercent = readSoilMoistureSensor(); 
  return true;
}

// Improved rain sensor reading with shorter debounce
bool readRainSensor() {
  static bool lastRainState = false;
  static unsigned long lastStateChange = 0;
  static const unsigned long DEBOUNCE_DELAY = 500; // 500ms debounce (was 1000ms)
  
  // Ensure the function doesn't take too long with watchdog reset
  wdt_reset();
  
  // Read current state - adjust based on your sensor's logic
  bool currentReading = digitalRead(RAIN_SENSOR_PIN) == LOW; // Rain detected if LOW
  
  // If state changed, update the last state change time
  if (currentReading != lastRainState) {
    lastStateChange = millis();
  }
  
  // Only change the actual state if debounce time has passed
  if ((millis() - lastStateChange) > DEBOUNCE_DELAY) {
    lastRainState = currentReading;
  }
  
  // Debug output only when state changes
  static bool lastReportedState = false;
  if (lastRainState != lastReportedState) {
    Serial.print("Rain sensor: ");
    Serial.println(lastRainState ? "RAIN DETECTED" : "NO RAIN");
    lastReportedState = lastRainState;
  }
  
  return lastRainState;
}

bool readTemperatureSensor(float* temperature) {
  // Start timer for timeout detection
  unsigned long startTime = millis();
  
  // Read temperature from DHT sensor with retry mechanism
  float reading = NAN;
  int attempts = 0;
  
  while (isnan(reading) && attempts < 3) {
    reading = dht.readTemperature();
    attempts++;
    
    if (!isnan(reading)) break;
    
    // Short delay between attempts
    delay(50);
    
    // Safety timeout
    if (millis() - startTime > 1000) {
      Serial.println("DHT temperature read timeout!");
      return false;
    }
  }
  
  // Check if we got a valid reading
  if (isnan(reading)) {
    Serial.println("DHT temperature read failed after " + String(attempts) + " attempts");
    return false;
  }
  
  *temperature = reading;
  return true;
}

bool readHumiditySensor(float* humidity) {
  // Start timer for timeout detection
  unsigned long startTime = millis();
  
  // Read humidity from DHT sensor with retry mechanism
  float reading = NAN;
  int attempts = 0;
  
  while (isnan(reading) && attempts < 3) {
    reading = dht.readHumidity();
    attempts++;
    
    if (!isnan(reading)) break;
    
    // Short delay between attempts
    delay(50);
    
    // Safety timeout
    if (millis() - startTime > 1000) {
      Serial.println("DHT humidity read timeout!");
      return false;
    }
  }
  
  // Check if we got a valid reading
  if (isnan(reading)) {
    Serial.println("DHT humidity read failed after " + String(attempts) + " attempts");
    return false;
  }
  
  *humidity = reading;
  return true;
}

// Fix the printSensorReadings function to properly use the readLightSensor function
void printSensorReadings() {
  int lightPercent;
  int moisturePercent;
  float temperature, humidity;
  
  Serial.println("--------- SENSOR READINGS ---------");
  
  // Fix: Call readLightSensor without arguments and store the result
  lightPercent = (int)readLightSensor(); // Cast float to int
  Serial.print("Light: "); 
  Serial.print(lightPercent); 
  Serial.println("%");
  
  // ...existing code for other sensor readings...
  if (readMoistureSensor(&moisturePercent)) {
    Serial.print("Soil Moisture: "); Serial.print(moisturePercent); Serial.println("%");
  }
  
  if (readRainSensor()) {
    Serial.println("Rain detected!");
  } else {
    Serial.println("No rain detected.");
  }
  
  if (readTemperatureSensor(&temperature)) {
    Serial.print("Temperature: "); Serial.print(temperature); Serial.println("°C");
  }
  
  if (readHumiditySensor(&humidity)) {
    Serial.print("Humidity: "); Serial.print(humidity); Serial.println("%");
  }
}

int readMoistureSensor() {
  return readSoilMoistureSensor();
}

float readTemperature() {
  float temperature = 0.0;
  if (readTemperatureSensor(&temperature)) {
    return temperature;
  }
  return temperatureReading; // Return last valid reading if new reading fails
}

float readHumidity() {
  float humidity = 0.0;
  if (readHumiditySensor(&humidity)) {
    return humidity;
  }
  return humidityReading; // Return last valid reading if new reading fails
}

// Update the updateSensorReadings function to fix the implementation
void updateSensorReadings() {
  // Reset watchdog before readings
  wdt_reset();
  
  // Get current time
  unsigned long currentTime = millis();
  
  // Only update readings at the specified interval
  if (currentTime - lastSensorUpdate >= SENSOR_READ_INTERVAL) {
    // Read all sensors
    lightReading = readLightSensor();
    moistureReading = readMoistureSensor();
    rainSensorState = readRainSensor();
    
    // DHT sensor is slow, so read it less frequently
    static unsigned long lastDHTUpdate = 0;
    if (currentTime - lastDHTUpdate >= SENSOR_READ_INTERVAL * 2) {
      temperatureReading = readTemperature();
      humidityReading = readHumidity();
      lastDHTUpdate = currentTime;
    }
    
    // Log readings
    Serial.println("Sensor Readings:");
    Serial.print("Light: "); Serial.print(lightReading); Serial.println("%");
    Serial.print("Moisture: "); Serial.print(moistureReading); Serial.println("%");
    Serial.print("Rain: "); Serial.println(rainSensorState ? "Yes" : "No");
    Serial.print("Temperature: "); Serial.print(temperatureReading); Serial.println(" °C");
    Serial.print("Humidity: "); Serial.print(humidityReading); Serial.println("%");
    
    // Update timestamp
    lastSensorUpdate = currentTime;
  }
  
  // Reset watchdog after readings
  wdt_reset();
}