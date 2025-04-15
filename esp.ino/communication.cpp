#include "communication.h"
#include "translator.h"

// Initialize global variables
SoftwareSerial arduinoSerial(RX_PIN, TX_PIN);

const int MAX_MESSAGE_LENGTH = 512; // Define size constant - increased from 384
char* receivedChars = NULL; // Changed: Will allocate dynamically

boolean messageInProgress = false;
int charIndex = 0;

// Sensor values
int light = 0;
int soil = 0;
int rain = 0;
float temperature = 0.0;
float humidity = 0.0;
bool pumpActive = false;
int lightMode = 0;
bool fanActive = false;
int fanMode = 0;
int pumpMode = 2; // default to AUTO

// LED timing
unsigned long rxLedOffTime = 0;
unsigned long txLedOffTime = 0;
const int LED_BLINK_DURATION = 50;

// Connection status
bool connected = false;
unsigned long lastDataReceived = 0;

// Add LED pattern variables
uint8_t txLedPattern = LED_PATTERN_NONE;
uint8_t rxLedPattern = LED_PATTERN_NONE;
unsigned long txPatternEndTime = 0;
unsigned long rxPatternEndTime = 0;
unsigned long ledPatternStartTime = 0;

void initCommunication() {
  // Allocate buffer memory
  if (receivedChars == NULL) {
    receivedChars = new char[MAX_MESSAGE_LENGTH]; // Allocate the buffer
  }

  // Initialize LED pins for communication indicators
  pinMode(TX_LED_PIN, OUTPUT);
  pinMode(RX_LED_PIN, OUTPUT);
  digitalWrite(TX_LED_PIN, LOW);
  digitalWrite(RX_LED_PIN, LOW);
  
  // LED for status indication
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH); // LED off (active LOW)
  
  // Initialize software serial for Arduino communication
  // Change to match Arduino's baudrate
  arduinoSerial.begin(115200);  // Set to 115200 to match Arduino Serial1
  arduinoSerial.setTimeout(2000); // Increase timeout to 2 seconds
  delay(500);
  
  Serial.println("\n\n=== ESP8266 Plant Care Dashboard ===");
  Serial.println("Using SoftwareSerial on pins D6(RX) and D5(TX)");
  Serial.println("Buffer size set to " + String(MAX_MESSAGE_LENGTH) + " bytes");
  Serial.println("Communication initialized at 115200 baud");

  // Add diagnostics
  Serial.println("\n=== Communication Diagnostics ===");
  Serial.println("Checking SoftwareSerial availability...");
  
  // Send a test character to verify TX is working
  arduinoSerial.write('!');
  Serial.println("TX test character sent");
  
  // Try to read any existing data to clear buffers
  while (arduinoSerial.available() > 0) {
    char c = arduinoSerial.read();
    Serial.print("Found character in buffer: ");
    Serial.println(c);
  }
  
  Serial.println("Buffer cleared");
  Serial.println("Communication setup complete");

  // Make LEDs blink to show system is starting up
  setTxLedPattern(LED_PATTERN_SLOW_BLINK, 3000); // Blink for 3 seconds
  setRxLedPattern(LED_PATTERN_SLOW_BLINK, 3000); // Blink for 3 seconds
}

// Improved function to handle larger message sizes with better error checking
void readFromArduino() {
  static unsigned long lastCompleteMessage = 0;
  static unsigned long messageStartTime = 0;
  const unsigned long MESSAGE_TIMEOUT = 2000; // 2 seconds timeout for incomplete messages
  
  // Check if data is available
  if (arduinoSerial.available() > 0) {
    // Show data reception with LED
    setRxLedPattern(LED_PATTERN_FAST_BLINK, 500); // Fast blink while receiving
    Serial.println("Data available on arduinoSerial: " + String(arduinoSerial.available()) + " bytes");
  }
  
  // Process data while available
  while (arduinoSerial.available() > 0) {
    // Turn on RX LED when data is received
    digitalWrite(RX_LED_PIN, HIGH);
    rxLedOffTime = millis() + LED_BLINK_DURATION;
    
    char inChar = arduinoSerial.read();
    
    // Debug received character for troubleshooting (just enable temporarily)
    if (messageInProgress || inChar == START_MARKER) {
      Serial.print("RX char: '"); 
      if (inChar >= 32 && inChar <= 126) { // Printable ASCII
        Serial.print(inChar);
      } else {
        Serial.print("0x");
        Serial.print((byte)inChar, HEX);
      }
      Serial.println("'");
    }
    
    // Reset message if new start marker is received
    if (inChar == START_MARKER) {
      if (messageInProgress) {
        // We received a new start marker before the end of the previous message
        Serial.println("WARNING: New message started before previous message completed");
      }
      messageInProgress = true;
      charIndex = 0;
      messageStartTime = millis();
      Serial.println("Message start detected");
      continue; // Skip storing the marker
    }
    
    // Handle end marker
    if (inChar == END_MARKER && messageInProgress) {
      messageInProgress = false;
      receivedChars[charIndex] = '\0'; // Null-terminate
      lastCompleteMessage = millis();
      
      // Log complete message for debugging
      Serial.print("Complete message received (len=");
      Serial.print(charIndex);
      Serial.print("): ");
      Serial.println(receivedChars);
      
      // Process message
      parseMessage(receivedChars);
      continue; // Skip storing the marker
    }
    
    // Store character if within a message
    if (messageInProgress) {
      if (charIndex < MAX_MESSAGE_LENGTH - 1) {
        receivedChars[charIndex++] = inChar;
      } else {
        // Buffer overflow - abort message collection
        messageInProgress = false;
        Serial.println("ERROR: Message too long, buffer overflow");
        // Print partial message for debugging
        receivedChars[charIndex] = '\0'; // Null-terminate
        Serial.print("Overflow partial message: ");
        Serial.println(receivedChars);
      }
    }
  }
  
  // Handle timeout for incomplete messages
  if (messageInProgress && (millis() - messageStartTime > MESSAGE_TIMEOUT)) {
    messageInProgress = false;
    Serial.print("ERROR: Message timeout after ");
    Serial.print(charIndex);
    Serial.print(" chars (");
    Serial.print(millis() - messageStartTime);
    Serial.println("ms)");
    
    // Print partial message for debugging
    receivedChars[charIndex] = '\0'; // Null-terminate
    Serial.print("Partial message: ");
    Serial.println(receivedChars);
    
    // Handle special case - if the partial message has valid JSON, try to parse it
    if (strncmp(receivedChars, "DATA:", 5) == 0 && 
        charIndex > 10 && 
        receivedChars[5] == '{') {
      Serial.println("Attempting to parse partial message as it might be valid JSON");
      // Add a closing brace if it might be missing
      if (receivedChars[charIndex-1] != '}') {
        if (charIndex < MAX_MESSAGE_LENGTH - 2) {
          receivedChars[charIndex++] = '}';
          receivedChars[charIndex] = '\0';
        }
      }
      parseMessage(receivedChars);
    }
  }
}

// Enhanced parsing function with more robust error handling
void parseMessage(char* message) {
  // Show successful message parsing with LED
  setRxLedPattern(LED_PATTERN_DOUBLE_PULSE, 300); // Double pulse on successful parsing

  // Update connection status
  connected = true;
  lastDataReceived = millis();
  
  // Check if message is potentially valid (not empty)
  if (!message || strlen(message) < 5) {
    Serial.println("ERROR: Message too short to be valid");
    return;
  }
  
  // Debug the raw message
  Serial.print("Raw message received: ");
  Serial.println(message);
  
  // Check for DATA: prefix (JSON format)
  if (strncmp(message, "DATA:", 5) == 0) {
    String jsonStr = message + 5; // Skip "DATA:" prefix
    
    // Verify the string has actual JSON content
    if (jsonStr.length() < 2 || jsonStr[0] != '{') {
      Serial.println("ERROR: Invalid JSON format");
      Serial.println(jsonStr);
      return;
    }
    
    // Attempt to fix truncated JSON by adding closing brace if needed
    if (jsonStr.indexOf('}') == -1) {
      jsonStr += "}";
      Serial.println("Added missing closing brace to JSON data");
    }
    
    // Parse the JSON
    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, jsonStr);
    
    if (error) {
      Serial.print("ERROR: JSON parsing failed - ");
      Serial.println(error.c_str());
      Serial.println("JSON string that failed: " + jsonStr);
      return;
    }
    
    // Successfully parsed - update values with proper key mapping
    // Map from actual JSON keys to our internal variables
    
    // Light level (same key name)
    if (doc.containsKey("light")) {
      light = doc["light"].as<int>();
    }
    
    // Soil moisture (same key name)
    if (doc.containsKey("moisture")) {
      soil = doc["moisture"].as<int>();
    }
    
    // Rain detection (same key name)
    if (doc.containsKey("rain")) {
      rain = doc["rain"].as<int>();
    }
    
    // Temperature - handle both numeric and string formats
    // The Arduino sends "temp" instead of "temperature"
    if (doc.containsKey("temp")) {
      if (doc["temp"].is<float>()) {
        temperature = doc["temp"].as<float>();
      } else if (doc["temp"].is<String>() || doc["temp"].is<const char*>()) {
        // Convert from string to float
        temperature = doc["temp"].as<String>().toFloat();
      }
    } else if (doc.containsKey("temperature")) {
      temperature = doc["temperature"].as<float>();
    }
    
    // Humidity - handle both numeric and string formats
    // The Arduino sends "hum" instead of "humidity"
    if (doc.containsKey("hum")) {
      if (doc["hum"].is<float>()) {
        humidity = doc["hum"].as<float>();
      } else if (doc["hum"].is<String>() || doc["hum"].is<const char*>()) {
        // Convert from string to float
        humidity = doc["hum"].as<String>().toFloat();
      }
    } else if (doc.containsKey("humidity")) {
      humidity = doc["humidity"].as<float>();
    }
    
    // Pump status - can be "pump" or "pumpActive"
    if (doc.containsKey("pump")) {
      pumpActive = doc["pump"].as<int>() == 1;
    } else if (doc.containsKey("pumpActive")) {
      pumpActive = doc["pumpActive"].as<bool>();
    }
    
    // Light active/mode - can be "light" (1/0) or "lightActive" or "lightMode"
    if (doc.containsKey("lightMode")) {
      lightMode = doc["lightMode"].as<int>();
    } else if (doc.containsKey("lightActive")) {
      lightMode = doc["lightActive"].as<bool>() ? 1 : 0;
    }
    
    // Fan status
    if (doc.containsKey("fan")) {
      fanActive = doc["fan"].as<int>() == 1;
    } else if (doc.containsKey("fanActive")) {
      fanActive = doc["fanActive"].as<bool>();
    }
    
    // Mode settings
    if (doc.containsKey("pMode")) {
      pumpMode = doc["pMode"].as<int>();
    } else if (doc.containsKey("pumpMode")) {
      pumpMode = doc["pumpMode"].as<int>();
    }
    
    if (doc.containsKey("fMode")) {
      fanMode = doc["fMode"].as<int>();
    } else if (doc.containsKey("fanMode")) {
      fanMode = doc["fanMode"].as<int>();
    }
    
    if (doc.containsKey("lMode")) {
      lightMode = doc["lMode"].as<int>();
    }
    
    // Update latestData for API access - create a consistent format for the web interface
    DynamicJsonDocument apiDoc(512);
    apiDoc["light"] = light;
    apiDoc["soil"] = soil;
    apiDoc["rain"] = rain;
    apiDoc["temperature"] = temperature;
    apiDoc["humidity"] = humidity;
    apiDoc["pumpActive"] = pumpActive;
    apiDoc["lightMode"] = lightMode;
    apiDoc["fanActive"] = fanActive;
    apiDoc["fanMode"] = fanMode;
    apiDoc["pumpMode"] = pumpMode;
    
    // Serialize to the latestData string for the API
    String newJsonData;
    serializeJson(apiDoc, newJsonData);
    latestData = newJsonData;
    
    // Print debug info about the parsed values
    Serial.println("Successfully parsed JSON data:");
    Serial.print("light="); Serial.print(light);
    Serial.print(", soil="); Serial.print(soil);
    Serial.print(", rain="); Serial.print(rain);
    Serial.print(", temp="); Serial.print(temperature);
    Serial.print(", humidity="); Serial.print(humidity);
    Serial.print(", pumpActive="); Serial.print(pumpActive ? "true" : "false");
    Serial.print(", pumpMode="); Serial.print(pumpMode);
    Serial.print(", lightMode="); Serial.println(lightMode);
    
    // Show data update with LED
    setRxLedPattern(LED_PATTERN_SOLID, 200); // Solid ON briefly

    return;
  }
  
  // Check for special command responses
  if (strcmp(message, "PONG") == 0) {
    Serial.println("Received PONG response");
    return;
  }
  
  if (strcmp(message, "ACK") == 0) {
    Serial.println("Received ACK response");
    return;
  }
  
  // If we get here, it's an unknown message format
  Serial.print("WARNING: Unknown message format: ");
  Serial.println(message);
}

// Enhanced function to handle commands with fallback
void sendCommand(const String& command) {
  // Show data transmission with LED
  setTxLedPattern(LED_PATTERN_SOLID, 100); // Solid ON briefly
  
  // Flash TX LED to indicate data transmission
  digitalWrite(TX_LED_PIN, HIGH);
  txLedOffTime = millis() + LED_BLINK_DURATION;
  
  // Add a short delay to ensure Arduino is ready to receive
  delay(10);
  
  // Flush any input before sending
  while (arduinoSerial.available()) {
    arduinoSerial.read();
  }
  
  // Ensure command isn't too large for Arduino's 128-byte buffer
  if (command.length() > 100) {
    Serial.println("ERROR: Command too large for Arduino buffer");
    return;
  }
  
  // Send command with markers
  arduinoSerial.print(START_MARKER);
  arduinoSerial.print(command);
  arduinoSerial.print(END_MARKER);
  arduinoSerial.flush(); // Wait for transmission to complete
  
  Serial.print("Sent command to Arduino: ");
  Serial.println(command);
  
  // Wait briefly for a response
  unsigned long startTime = millis();
  while (millis() - startTime < 500) {
    if (arduinoSerial.available() > 0) {
      Serial.println("Response detected!");
      break;
    }
    yield(); // Allow background tasks
  }

  // After sending, show a fast blink pattern
  setTxLedPattern(LED_PATTERN_FAST_BLINK, 500); // Fast blink for 500ms
}

void blinkStatusLED() {
  static unsigned long lastBlink = 0;
  static bool ledState = false;
  
  // Check if connection timed out
  if (millis() - lastDataReceived > 10000) {
    connected = false;
  }
  
  // Blink faster if disconnected
  unsigned long blinkInterval = connected ? 1000 : 250;
  
  if (millis() - lastBlink > blinkInterval) {
    ledState = !ledState;
    digitalWrite(LED_BUILTIN, ledState ? LOW : HIGH); // Remember LED is active LOW
    lastBlink = millis();
  }
}

void updateLEDs() {
  unsigned long currentMillis = millis();
  
  // Update LED patterns
  updateLedPatterns();
  
  // Handle simple LED timers (for compatibility)
  if (txLedOffTime > 0 && currentMillis > txLedOffTime) {
    if (txLedPattern == LED_PATTERN_NONE) {
      digitalWrite(TX_LED_PIN, LOW);
    }
    txLedOffTime = 0;
  }
  
  if (rxLedOffTime > 0 && currentMillis > rxLedOffTime) {
    if (rxLedPattern == LED_PATTERN_NONE) {
      digitalWrite(RX_LED_PIN, LOW);
    }
    rxLedOffTime = 0;
  }
}

void parseIncomingData(const String &input) {
  // Only process if it starts with "DATA:"
  if (input.startsWith("DATA:")) {
    String jsonString = input.substring(5);
    latestData = jsonString; 
    Serial.println("Parsed JSON from Arduino: ");
    Serial.println(jsonString);
  }
}

// Handle LED patterns for data visualization
void setTxLedPattern(uint8_t pattern, unsigned long duration) {
  txLedPattern = pattern;
  txPatternEndTime = millis() + duration;
  ledPatternStartTime = millis();
}

void setRxLedPattern(uint8_t pattern, unsigned long duration) {
  rxLedPattern = pattern;
  rxPatternEndTime = millis() + duration;
  ledPatternStartTime = millis();
}

void updateLedPatterns() {
  unsigned long currentMillis = millis();
  unsigned long elapsedTime = currentMillis - ledPatternStartTime;
  
  // Update TX LED pattern
  if (currentMillis > txPatternEndTime) {
    // Pattern ended, turn off LED and reset pattern
    digitalWrite(TX_LED_PIN, LOW);
    txLedPattern = LED_PATTERN_NONE;
  } else {
    // Process active pattern
    switch (txLedPattern) {
      case LED_PATTERN_SLOW_BLINK:
        digitalWrite(TX_LED_PIN, (elapsedTime % 1000) < 500);
        break;
      case LED_PATTERN_FAST_BLINK:
        digitalWrite(TX_LED_PIN, (elapsedTime % 200) < 100);
        break;
      case LED_PATTERN_DOUBLE_PULSE:
        {
          unsigned long cycle = elapsedTime % 1000;
          digitalWrite(TX_LED_PIN, (cycle < 100) || (cycle > 200 && cycle < 300));
        }
        break;
      case LED_PATTERN_SOLID:
        digitalWrite(TX_LED_PIN, HIGH);
        break;
      default:
        digitalWrite(TX_LED_PIN, LOW);
    }
  }
  
  // Update RX LED pattern
  if (currentMillis > rxPatternEndTime) {
    // Pattern ended, turn off LED and reset pattern
    digitalWrite(RX_LED_PIN, LOW);
    rxLedPattern = LED_PATTERN_NONE;
  } else {
    // Process active pattern
    switch (rxLedPattern) {
      case LED_PATTERN_SLOW_BLINK:
        digitalWrite(RX_LED_PIN, (elapsedTime % 1000) < 500);
        break;
      case LED_PATTERN_FAST_BLINK:
        digitalWrite(RX_LED_PIN, (elapsedTime % 200) < 100);
        break;
      case LED_PATTERN_DOUBLE_PULSE:
        {
          unsigned long cycle = elapsedTime % 1000;
          digitalWrite(RX_LED_PIN, (cycle < 100) || (cycle > 200 && cycle < 300));
        }
        break;
      case LED_PATTERN_SOLID:
        digitalWrite(RX_LED_PIN, HIGH);
        break;
      default:
        digitalWrite(RX_LED_PIN, LOW);
    }
  }
}