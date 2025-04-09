#ifndef SENSORS_H
#define SENSORS_H

#include <Arduino.h>
#include <DHT.h>
#include "config.h"

// Function declarations
bool readLightSensor(int* lightPercent);
bool readMoistureSensor(int* moisturePercent);
bool readRainSensor(); // Updated to return a boolean
bool readTemperatureSensor(float* temperature);
bool readHumiditySensor(float* humidity);
void initializeSensors();
int getSoilMoisturePercent();

#endif // SENSORS_H