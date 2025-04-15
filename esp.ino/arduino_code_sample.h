#ifndef ARDUINO_CODE_SAMPLE_H
#define ARDUINO_CODE_SAMPLE_H

/*
 * This file contains sample Arduino code that's compatible with your ESP8266 code.
 * This is provided for reference only - you should adapt your actual Arduino code.
 */

/*
// Sample Arduino code for communication with ESP8266

#include <ArduinoJson.h>

// Communication protocol defines
#define START_MARKER '<'
#define END_MARKER '>'

// Use Serial1 for ESP communication
#define ESP_SERIAL Serial1
#define ESP_BAUD_RATE 115200

// Buffer for incoming data
char incomingData[128];
int dataIndex = 0;
bool receivingData = false;

// Actuator states and modes
int pumpState = 0;  // 0 = off, 1 = on
int pumpMode = 2;   // 0 = off, 1 = on, 2 = auto
int lightState = 0; // 0 = off, 1 = on
int lightMode = 0;  // 0 = off, 1 = on, 2 = auto
int fanState = 0;   // 0 = off, 1 = on
int fanMode = 0;    // 0 = off, 1 = on, 2 = auto

void setup() {
  Serial.begin(115200);  // Debug serial
  ESP_SERIAL.begin(ESP_BAUD_RATE);  // ESP8266 communication
  
  Serial.println(F("Plant Care System"));
  Serial.println(F("Initializing..."));
}

void loop() {
  // Check for commands from ESP8266
  if (checkForCommand()) {
    processCommand(incomingData);
  }
  
  // Read sensors
  int lightValue = map(analogRead(A0), 0, 1023, 0, 100);
  int soilValue = map(analogRead(A1), 0, 1023, 0, 100);
  int rainValue = digitalRead(7) == LOW ? 1 : 0;
  float tempValue = 23.5; // Replace with actual DHT reading
  float humValue = 65.0;  // Replace with actual DHT reading
  
  // Control actuators based on modes
  updateActuators(lightValue, soilValue, rainValue, tempValue, humValue);
  
  // Send data to ESP every 5 seconds
  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate > 5000) {
    sendDataToESP(lightValue, soilValue, rainValue, tempValue, humValue);
    lastUpdate = millis();
  }
  
  // Other Arduino tasks
  // ...
}

bool checkForCommand() {
  bool newData = false;
  
  while (ESP_SERIAL.available() > 0) {
    char inChar = ESP_SERIAL.read();
    
    // Start marker received
    if (inChar == START_MARKER) {
      receivingData = true;
      dataIndex = 0;
      continue;
    }
    
    // End marker received
    if (inChar == END_MARKER && receivingData) {
      receivingData = false;
      incomingData[dataIndex] = '\0'; // Null-terminate
      newData = true;
      break;
    }
    
    // Store character if within message
    if (receivingData) {
      if (dataIndex < 127) {
        incomingData[dataIndex++] = inChar;
      } else {
        // Buffer overflow protection
        receivingData = false;
        dataIndex = 0;
      }
    }
  }
  
  return newData;
}

void processCommand(const char* command) {
  Serial.print(F("Command received: "));
  Serial.println(command);
  
  // Handle PING command
  if (strcmp(command, "PING") == 0) {
    ESP_SERIAL.print(START_MARKER);
    ESP_SERIAL.print("PONG");
    ESP_SERIAL.print(END_MARKER);
    return;
  }
  
  // Handle pump commands
  if (strncmp(command, "PUMP:MODE:", 10) == 0) {
    int mode = atoi(command + 10);
    pumpMode = mode;
    
    // Send acknowledgment
    ESP_SERIAL.print(START_MARKER);
    ESP_SERIAL.print("ACK:PUMP");
    ESP_SERIAL.print(END_MARKER);
  }
  
  // Handle light commands
  else if (strncmp(command, "LIGHT:MODE:", 11) == 0) {
    int mode = atoi(command + 11);
    lightMode = mode;
    
    // Send acknowledgment
    ESP_SERIAL.print(START_MARKER);
    ESP_SERIAL.print("ACK:LIGHT");
    ESP_SERIAL.print(END_MARKER);
  }
  
  // Handle fan commands
  else if (strncmp(command, "FAN:MODE:", 9) == 0) {
    int mode = atoi(command + 9);
    fanMode = mode;
    
    // Send acknowledgment
    ESP_SERIAL.print(START_MARKER);
    ESP_SERIAL.print("ACK:FAN");
    ESP_SERIAL.print(END_MARKER);
  }
}

void updateActuators(int light, int soil, int rain, float temp, float hum) {
  // Update pump based on mode
  if (pumpMode == 0) {
    pumpState = 0;  // Force off
  } else if (pumpMode == 1) {
    pumpState = 1;  // Force on
  } else {
    // Auto mode - turn on if soil moisture is low
    pumpState = (soil < 30) ? 1 : 0;
  }
  
  // Update light based on mode
  if (lightMode == 0) {
    lightState = 0;  // Force off
  } else if (lightMode == 1) {
    lightState = 1;  // Force on
  } else {
    // Auto mode - turn on if light level is low
    lightState = (light < 20) ? 1 : 0;
  }
  
  // Update fan based on mode
  if (fanMode == 0) {
    fanState = 0;  // Force off
  } else if (fanMode == 1) {
    fanState = 1;  // Force on
  } else {
    // Auto mode - turn on if temperature is high
    fanState = (temp > 28.0) ? 1 : 0;
  }
  
  // Now actually control the hardware
  digitalWrite(8, pumpState ? HIGH : LOW);  // Pump pin
  digitalWrite(9, lightState ? HIGH : LOW); // Light pin
  digitalWrite(10, fanState ? HIGH : LOW);  // Fan pin
}

void sendDataToESP(int lightPercent, int soilPercent, int rainValue, float temp, float hum) {
  // Create JSON document
  StaticJsonDocument<128> doc;
  
  // Add sensor data
  doc["light"] = lightPercent;
  doc["moisture"] = soilPercent;
  doc["rain"] = rainValue;
  
  // Format temperature and humidity with 1 decimal place
  char tempStr[6];
  dtostrf(temp, 2, 1, tempStr);
  doc["temp"] = tempStr;
  
  char humStr[6];
  dtostrf(hum, 2, 1, humStr);
  doc["hum"] = humStr;
  
  // Add actuator states
  doc["pump"] = pumpState;
  doc["pMode"] = pumpMode;
  
  // Optional: add other device states
  // doc["light"] = lightState;
  // doc["lMode"] = lightMode;
  // doc["fan"] = fanState;
  // doc["fMode"] = fanMode;
  
  // Serialize JSON to string
  char jsonBuffer[150];
  serializeJson(doc, jsonBuffer);
  
  // Send to ESP8266
  ESP_SERIAL.print(START_MARKER);
  ESP_SERIAL.print("DATA:");
  
  // Send in smaller chunks with delays to avoid buffer overflow
  const int CHUNK_SIZE = 16;
  size_t jsonSize = strlen(jsonBuffer);
  
  for (size_t i = 0; i < jsonSize; i += CHUNK_SIZE) {
    // Calculate how many bytes to send in this chunk
    size_t bytesToSend = min(CHUNK_SIZE, jsonSize - i);
    
    // Send this chunk
    ESP_SERIAL.write((const uint8_t*)(jsonBuffer + i), bytesToSend);
    ESP_SERIAL.flush();
    
    // Small delay between chunks
    delay(5);
  }
  
  ESP_SERIAL.print(END_MARKER);
  
  // Debug output
  Serial.print(F("Sent data to ESP: "));
  Serial.println(jsonBuffer);
}
*/

#endif
