#ifndef TRANSLATOR_H
#define TRANSLATOR_H

#include <Arduino.h>
#include <ArduinoJson.h>

// Helper function to extract string or numeric values from JSON with consistent output
float extractNumericValue(JsonVariant value) {
  if (value.is<float>() || value.is<int>()) {
    return value.as<float>();
  } 
  else if (value.is<const char*>() || value.is<String>()) {
    return String(value.as<const char*>()).toFloat();
  }
  return 0.0;
}

// Helper function to extract boolean values from various formats
bool extractBooleanValue(JsonVariant value) {
  if (value.is<bool>()) {
    return value.as<bool>();
  }
  else if (value.is<int>()) {
    return value.as<int>() > 0;
  }
  else if (value.is<const char*>() || value.is<String>()) {
    String strValue = value.as<String>();
    strValue.toLowerCase();
    return (strValue == "true" || strValue == "1" || strValue == "on");
  }
  return false;
}

#endif
