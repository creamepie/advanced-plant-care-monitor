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

// Find your getLightPercent() function and replace it with this:
int getLightPercent() {
  // Read the sensor 
  int rawValue = digitalRead(LIGHT_SENSOR_PIN) ? 0 : 1023;  // If using digital sensor
  // OR
  // int rawValue = analogRead(LIGHT_SENSOR_PIN);  // If using analog sensor
  
  // Debug raw reading
  Serial.print("Light sensor raw value: ");
  Serial.println(rawValue);
  
  // Invert the map function if light logic seems reversed
  // If sensor outputs HIGH in darkness and LOW in light, use this:
  int percent = map(rawValue, 1023, 0, 0, 100);
  
  // If sensor outputs LOW in darkness and HIGH in light, use this instead:
  // int percent = map(rawValue, 0, 1023, 0, 100);
  
  // Ensure value stays within 0-100 range
  percent = constrain(percent, 0, 100);
  
  return percent;
}

// Add this function after getLightPercent()
bool readLightSensor(int* lightPercent) {
  // Use the existing getLightPercent function
  *lightPercent = getLightPercent();
  return true;
}

// Replace these functions:

// Replace your existing getSoilMoisturePercent function with this:
int getSoilMoisturePercent() {
  // Read the raw sensor value
  int rawValue = digitalRead(SOIL_MOISTURE_PIN) ? 0 : 1023;

  // Add bounds check to prevent array overflow
  if (moistureIndex < 0 || moistureIndex >= MOISTURE_SAMPLES) {
    moistureIndex = 0;  // Reset index if out of bounds
  }

  // Add the new reading to the moving average
  moistureReadings[moistureIndex] = rawValue;
  moistureIndex = (moistureIndex + 1) % MOISTURE_SAMPLES;

  // Calculate the average
  int sum = 0;
  for (int i = 0; i < MOISTURE_SAMPLES; i++) {
    sum += moistureReadings[i];
  }
  int averageRaw = sum / MOISTURE_SAMPLES;

  // Map the average raw value to a percentage
  int percent = map(averageRaw, 1023, 0, 0, 100);
  percent = constrain(percent, 0, 100);

  return percent;
}

// Update the wrapper function to match the light sensor exactly
bool readMoistureSensor(int* moisturePercent) {
  // Use the existing getSoilMoisturePercent function
  *moisturePercent = getSoilMoisturePercent();
  return true;
}

// Add reading function for the new rain sensor
bool readRainSensor() {
  return digitalRead(RAIN_SENSOR_PIN) == LOW; // Rain detected if LOW
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

void printSensorReadings() {
  int lightPercent;
  int moisturePercent;
  int rainValue;
  float temperature, humidity;
  
  Serial.println("--------- SENSOR READINGS ---------");
  
  if (readLightSensor(&lightPercent)) {
    Serial.print("Light: "); Serial.print(lightPercent); Serial.println("%");
  }
  
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