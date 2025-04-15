#include "communication.h"
#include "actuators.h"
#include <avr/wdt.h>
#include <ArduinoJson.h>

// Buffer for incoming data
char incomingData[128]; // Increased from 64 to 128 bytes for larger commands
int dataIndex = 0;
bool receivingData = false;

// Add message sequence counter for reliability tracking
uint16_t messageCounter = 0;

// Add last successful communication timestamp
unsigned long lastSuccessfulComm = 0;

void initializeESPCommunication() {
  // Initialize hardware serial for ESP communication
  ESP_SERIAL.begin(ESP_BAUD_RATE);
  
  // Wait for serial to be ready
  delay(100);
  
  Serial.println(F("ESP communication initialized"));
}

// Add ESP communication health check
bool isESPResponsive() {
  // If we've communicated successfully in the last 30 seconds, assume ESP is still connected
  if (millis() - lastSuccessfulComm < 30000) {
    return true;
  }
  
  // Try to ping ESP
  ESP_SERIAL.print(START_MARKER);
  ESP_SERIAL.print("PING");
  ESP_SERIAL.print(END_MARKER);
  
  // Wait up to 250ms for response
  unsigned long startTime = millis();
  while (millis() - startTime < 250) {
    if (ESP_SERIAL.available() > 0) {
      char c = ESP_SERIAL.read();
      if (c == START_MARKER) {
        // Look for "PONG" response
        String response = ESP_SERIAL.readStringUntil(END_MARKER);
        if (response == "PONG") {
          lastSuccessfulComm = millis();
          return true;
        }
      }
    }
    delay(5);
    wdt_reset();
  }
  
  return false;
}

// Add ESP connection reset function
bool resetESPCommunication() {
  Serial.println(F("Resetting ESP communication..."));
  ESP_SERIAL.end();
  delay(100);
  ESP_SERIAL.begin(ESP_BAUD_RATE);
  delay(100);
  wdt_reset();
  
  // Send a reset signal to ESP
  ESP_SERIAL.print(START_MARKER);
  ESP_SERIAL.print("RESET");
  ESP_SERIAL.print(END_MARKER);
  
  delay(100);
  return true;
}

// Receive command with unchanged interface
bool receiveCommandFromESP(char* buffer, int bufferSize) {
  bool newDataReceived = false;
  unsigned long startTime = millis();
  const unsigned long RECEIVE_TIMEOUT = 100; // 100ms max time to spend here
  
  // Process data with a timeout to prevent blocking
  while (ESP_SERIAL.available() > 0 && (millis() - startTime < RECEIVE_TIMEOUT)) {
    char inChar = ESP_SERIAL.read();
    
    // Reset watchdog while reading
    wdt_reset();
    
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
      lastSuccessfulComm = millis(); // Update last communication timestamp
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

void sendDataToESP(int lightPercent, int moisturePercent, int rainValue, float temperature, float humidity) {
  // Reset watchdog before operation
  wdt_reset();
  
  // Create smaller, more efficient JSON with reduced precision
  StaticJsonDocument<128> jsonData; // Reduced size to improve stability
  
  // Only include essential data to reduce message size and round values to reduce string length
  jsonData["light"] = lightPercent;
  jsonData["moisture"] = moisturePercent;
  jsonData["rain"] = rainValue ? 1 : 0;
  
  // Reduce decimal places to save bytes
  char tempStr[6];
  dtostrf(temperature, 2, 1, tempStr); // Format as #.# to save space
  jsonData["temp"] = tempStr; // Use shorter key name
  
  char humStr[6];
  dtostrf(humidity, 2, 1, humStr);
  jsonData["hum"] = humStr; // Use shorter key name
  
  // Only include essential status info
  jsonData["pump"] = (int)getPumpState();
  jsonData["pMode"] = (int)getPumpMode();
  
  // Serialize to buffer first to know exact size
  char jsonBuffer[150]; // Smaller buffer size
  size_t jsonSize = serializeJson(jsonData, jsonBuffer);
  
  // Ensure we're not too large for ESP to receive
  if (jsonSize > 120) {
    Serial.println("ERROR: JSON payload too large");
    return;
  }
  
  // Log the size for debugging
  Serial.print("JSON size: ");
  Serial.print(jsonSize);
  Serial.println(" bytes");
  
  // Send start marker with a more robust approach
  ESP_SERIAL.print(START_MARKER);
  ESP_SERIAL.flush(); // Wait for data to be sent
  delay(5); // Short delay to ensure ESP has time to process
  
  // Send data prefix
  ESP_SERIAL.print("DATA:");
  ESP_SERIAL.flush();
  delay(5);
  
  // Send in smaller chunks with pauses to prevent buffer overflow
  const int CHUNK_SIZE = 16;
  for (size_t i = 0; i < jsonSize; i += CHUNK_SIZE) {
    // Calculate how many bytes to send in this chunk
    size_t bytesToSend = min(CHUNK_SIZE, jsonSize - i);
    
    // Send this chunk
    ESP_SERIAL.write((const uint8_t*)(jsonBuffer + i), bytesToSend);
    ESP_SERIAL.flush(); // Wait for transmission to complete
    
    // Small delay between chunks
    delay(5);
    
    // Reset watchdog in case this takes a while
    wdt_reset();
  }
  
  // Send end marker with a delay for ESP to process
  delay(5);
  ESP_SERIAL.print(END_MARKER);
  ESP_SERIAL.flush();
  
  // Log sent data
  Serial.print("Sent to ESP: ");
  Serial.println(jsonBuffer);
  
  // Reset watchdog after operation
  wdt_reset();
  
  // Update last successful communication time
  lastSuccessfulComm = millis();
}

void processESPCommand(const char* command) {
  Serial.println(command);
  
  // Handle ACK responses
  if (strcmp(command, "ACK") == 0) {
    lastSuccessfulComm = millis();
    return;
  }
  
  // Handle PONG responses
  if (strcmp(command, "PONG") == 0) {
    lastSuccessfulComm = millis();
    return;
  }
  
  // Command format examples: "PUMP:MODE:1", "LIGHT:MODE:2", "FAN:MODE:1"
  
  // Check for pump commands
  if (strncmp(command, "PUMP:MODE:", 10) == 0) {
    int mode = atoi(command + 10);
    if (mode >= 0 && mode <= 2) {
      setPumpMode(mode);
      Serial.print(F("Command received: Pump mode set to "));
      Serial.println(mode);
      
      // Send acknowledgment
      ESP_SERIAL.print(START_MARKER);
      ESP_SERIAL.print("ACK:PUMP");
      ESP_SERIAL.print(END_MARKER);
    } else {
      Serial.print(F("ERROR: Invalid pump mode: "));
      Serial.println(mode);
    }
  }
  
  // Check for light commands
  else if (strncmp(command, "LIGHT:MODE:", 11) == 0) {
    int mode = atoi(command + 11);
    if (mode >= 0 && mode <= 2) {
      setLightMode(mode);
      Serial.print(F("Command received: Light mode set to "));
      Serial.println(mode);
      
      // Send acknowledgment
      ESP_SERIAL.print(START_MARKER);
      ESP_SERIAL.print("ACK:LIGHT");
      ESP_SERIAL.print(END_MARKER);
    } else {
      Serial.print(F("ERROR: Invalid light mode: "));
      Serial.println(mode);
    }
  }
  
  // Check for fan commands
  else if (strncmp(command, "FAN:MODE:", 9) == 0) {
    int mode = atoi(command + 9);
    if (mode >= 0 && mode <= 2) {
      setFanMode(mode);
      Serial.print(F("Command received: Fan mode set to "));
      Serial.println(mode);
      
      // Send acknowledgment
      ESP_SERIAL.print(START_MARKER);
      ESP_SERIAL.print("ACK:FAN");
      ESP_SERIAL.print(END_MARKER);
    } else {
      Serial.print(F("ERROR: Invalid fan mode: "));
      Serial.println(mode);
    }
  }
  
  // Unknown command type
  else {
    Serial.print(F("ERROR: Unknown command type: "));
    Serial.println(command);
  }
}