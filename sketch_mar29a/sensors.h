#ifndef SENSORS_H
#define SENSORS_H

#include <Arduino.h>
#include "config.h"
#include <DHT.h>

// Function prototypes
void initializeSensors();
float readLightSensor(); // Changed from int to float to match implementation
int readMoistureSensor();
bool readRainSensor();
float readTemperature();
float readHumidity();
void updateSensorReadings();

// Global sensor reading variables
extern int lightReading;
extern int moistureReading;
extern bool rainSensorState;
extern float temperatureReading;
extern float humidityReading;

#endif // SENSORS_H