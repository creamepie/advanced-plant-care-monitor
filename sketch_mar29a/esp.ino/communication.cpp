// ...existing code...

void parseMessage(char* message) {
  // Update connection status
  connected = true;
  lastDataReceived = millis();
  
  // Check if message is potentially valid (not empty)
  if (!message || strlen(message) < 5) {
    Serial.println("ERROR: Message too short to be valid");
    return;
  }
  
  // Check for DATA: prefix (JSON format)
  if (strncmp(message, "DATA:", 5) == 0) {
    String jsonStr = message + 5; // Skip "DATA:" prefix
    
    // Verify the string has actual JSON content
    if (jsonStr.length() < 2 || jsonStr[0] != '{') {
      Serial.println("ERROR: Invalid JSON format");
      Serial.println(jsonStr);
      return;
    }
    
    // Debug: Show received data length to help troubleshoot
    Serial.print("Received JSON (length=");
    Serial.print(jsonStr.length());
    Serial.print("): ");
    Serial.println(jsonStr);
    
    // Use a smaller JSON document size to reduce memory usage
    DynamicJsonDocument doc(384);
    DeserializationError error = deserializeJson(doc, jsonStr);
    
    if (error) {
      Serial.print("ERROR: JSON parsing failed - ");
      Serial.println(error.c_str());
      return;
    }
    
    // Successfully parsed - update values
    // Use explicit checks before updating to avoid crashes
    if (doc.containsKey("light")) light = doc["light"].as<int>();
    if (doc.containsKey("moisture")) soil = doc["moisture"].as<int>();
    if (doc.containsKey("rain")) rain = doc["rain"].as<int>();
    
    // Handle strings or numbers for temperature/humidity
    if (doc.containsKey("temp")) {
      if (doc["temp"].is<float>())
        temperature = doc["temp"].as<float>();
      else if (doc["temp"].is<const char*>())
        temperature = atof(doc["temp"].as<const char*>());
    } else if (doc.containsKey("temperature")) {
      if (doc["temperature"].is<float>())
        temperature = doc["temperature"].as<float>();
      else if (doc["temperature"].is<const char*>())
        temperature = atof(doc["temperature"].as<const char*>());
    }
    
    if (doc.containsKey("hum")) {
      if (doc["hum"].is<float>())
        humidity = doc["hum"].as<float>();
      else if (doc["hum"].is<const char*>())
        humidity = atof(doc["hum"].as<const char*>());
    } else if (doc.containsKey("humidity")) {
      if (doc["humidity"].is<float>())
        humidity = doc["humidity"].as<float>();
      else if (doc["humidity"].is<const char*>())
        humidity = atof(doc["humidity"].as<const char*>());
    }
    
    if (doc.containsKey("pump")) pumpActive = doc["pump"].as<bool>();
    if (doc.containsKey("pMode")) pumpMode = doc["pMode"].as<int>();
    else if (doc.containsKey("pumpMode")) pumpMode = doc["pumpMode"].as<int>();
    
    if (doc.containsKey("lightMode")) lightMode = doc["lightMode"].as<int>();
    if (doc.containsKey("fanActive")) fanActive = doc["fanActive"].as<bool>();
    if (doc.containsKey("fanMode")) fanMode = doc["fanMode"].as<int>();
    
    // Update latestData for API access
    DynamicJsonDocument apiDoc(256);
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
    Serial.print(", humidity="); Serial.println(humidity);
    
    return;
  }
  
  // ...existing code...
}

// Improved function to handle larger message sizes with better error checking
void readFromArduino() {
  static unsigned long lastCompleteMessage = 0;
  static unsigned long messageStartTime = 0;
  const unsigned long MESSAGE_TIMEOUT = 1000; // Increased timeout for larger messages
  
  // Process data while available
  while (arduinoSerial.available() > 0) {
    // Turn on RX LED when data is received
    digitalWrite(RX_LED_PIN, HIGH);
    rxLedOffTime = millis() + LED_BLINK_DURATION;
    
    char inChar = arduinoSerial.read();
    
    // Handle start marker
    if (inChar == START_MARKER) {
      messageInProgress = true;
      charIndex = 0;
      messageStartTime = millis();
      
      // Debug start marker found
      Serial.println("Start marker received");
      continue; // Skip storing the marker
    }
    
    // Handle end marker
    if (inChar == END_MARKER && messageInProgress) {
      messageInProgress = false;
      receivedChars[charIndex] = '\0'; // Null-terminate
      lastCompleteMessage = millis();
      
      // Debug complete message
      Serial.print("Complete message received (len=");
      Serial.print(charIndex);
      Serial.println(")");
      
      // Process message
      parseMessage(receivedChars);
      continue; // Skip storing the marker
    }
    
    // Store character if within a message
    if (messageInProgress) {
      if (charIndex < MAX_MESSAGE_LENGTH - 1) {
        receivedChars[charIndex++] = inChar;
        
        // Debug character receipt on regular intervals
        if (charIndex % 20 == 0) {
          Serial.print("Received ");
          Serial.print(charIndex);
          Serial.println(" chars so far...");
        }
      } else {
        // Buffer overflow - abort message collection
        messageInProgress = false;
        Serial.println("ERROR: Message too long, buffer overflow");
      }
    }
  }
  
  // Handle timeout for incomplete messages
  if (messageInProgress && (millis() - messageStartTime > MESSAGE_TIMEOUT)) {
    messageInProgress = false;
    Serial.print("ERROR: Message timeout after ");
    Serial.print(charIndex);
    Serial.println(" chars");
    Serial.print("Partial message: ");
    
    // Print the received portion of the message for debugging
    if (charIndex > 64) {
      // Only show first 64 chars to avoid flooding serial
      for (int i = 0; i < 64; i++) {
        Serial.print(receivedChars[i]);
      }
      Serial.println("..."); // Indicate there's more
    } else {
      // Print the entire partial message if it's short
      receivedChars[charIndex] = '\0'; // Null-terminate
      Serial.println(receivedChars);
    }
  }
}

// ...existing code...
