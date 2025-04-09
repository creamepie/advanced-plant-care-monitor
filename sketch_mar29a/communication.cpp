#include "communication.h"
#include "actuators.h" // Make sure this is included so pumpAutoMode is declared
#include <avr/wdt.h>
#include <ArduinoJson.h>

// Buffer for incoming data
char incomingData[64];
int dataIndex = 0;
bool receivingData = false;

void initializeESPCommunication() {
  // Initialize hardware serial for ESP communication
  ESP_SERIAL.begin(ESP_BAUD_RATE);
  
  // Wait for serial to be ready
  delay(100);
  
  Serial.println(F("ESP communication initialized"));
}

void sendDataToESP(int lightPercent, int moisturePercent, int rainValue, float temperature, float humidity) {
  // Create a JSON document
  DynamicJsonDocument jsonData(256);
  
  // Add sensor values
  jsonData["light"] = lightPercent;
  jsonData["moisture"] = moisturePercent;
  jsonData["rain"] = rainValue;
  jsonData["temperature"] = temperature;
  jsonData["humidity"] = humidity;
  
  // Add system state info
  jsonData["pumpActive"] = getPumpState();
  jsonData["lightActive"] = getLightState();
  jsonData["pumpMode"] = getPumpMode();
  jsonData["lightMode"] = getLightMode();
  
  // Convert to string
  String jsonString;
  serializeJson(jsonData, jsonString);
  
  // Send to ESP with DATA: prefix
  ESP_SERIAL.print("DATA:");
  ESP_SERIAL.println(jsonString);
  
  Serial.print("Sent to ESP: ");
  Serial.println(jsonString);
}

bool receiveCommandFromESP(char* buffer, int bufferSize) {
  bool newDataReceived = false;
  
  while (ESP_SERIAL.available() > 0) {
    char inChar = ESP_SERIAL.read();
    
    // Check for start marker
    if (inChar == START_MARKER) {
      receivingData = true;
      dataIndex = 0;
      continue;
    }
    
    // Check for end marker
    if (inChar == END_MARKER) {
      receivingData = false;
      buffer[dataIndex] = '\0'; // Null-terminate the string
      newDataReceived = true;
      break;
    }
    
    // Add to buffer if receiving and not exceeding buffer size
    if (receivingData && dataIndex < bufferSize - 1) {
      buffer[dataIndex] = inChar;
      dataIndex++;
    }
    
    // Reset if buffer overflows
    if (dataIndex >= bufferSize - 1) {
      dataIndex = 0;
      receivingData = false;
    }
  }
  
  return newDataReceived;
}

// Improve ESP command processing with better error handling
void processESPCommand(const char* command) {
  // Check for null or empty command
  if (!command || command[0] == '\0') {
    Serial.println("ERROR: Empty ESP command received");
    return;
  }
  
  Serial.print("Processing ESP command: ");
  Serial.println(command);
  
  // Example command format: "PUMP:ON", "LIGHT:MODE:2"
  
  // Check for pump commands
  if (strncmp(command, "PUMP:", 5) == 0) {
    if (strcmp(command + 5, "ON") == 0) {
      setPumpState(true);
      pumpAutoMode = false; // Set to manual mode
      Serial.println(F("Command received: Pump ON"));
    } 
    else if (strcmp(command + 5, "OFF") == 0) {
      setPumpState(false);
      Serial.println(F("Command received: Pump OFF"));
    }
    else if (strcmp(command + 5, "AUTO") == 0) {
      pumpAutoMode = true;
      Serial.println(F("Command received: Pump AUTO"));
    }
    else {
      Serial.print("ERROR: Unknown pump command: ");
      Serial.println(command + 5);
    }
  }
  
  // Check for light commands
  else if (strncmp(command, "LIGHT:MODE:", 11) == 0) {
    int mode = atoi(command + 11);
    if (mode >= 0 && mode <= 2) {
      setLightMode(mode);
      Serial.print(F("Command received: Light mode "));
      Serial.println(mode);
    }
    else {
      Serial.print("ERROR: Invalid light mode: ");
      Serial.println(mode);
    }
  }
  
  // Unknown command type
  else {
    Serial.print("ERROR: Unknown command type: ");
    Serial.println(command);
  }
}