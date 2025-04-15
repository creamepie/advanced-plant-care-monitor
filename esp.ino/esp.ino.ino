#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include <SoftwareSerial.h>
#include "communication.h"
#include "webserver.h"
#include "html_content.h"

// Create web server object on port 80
ESP8266WebServer server(80);
String latestData = "{}";

void setup() {
  // Initialize hardware serial for debug
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n\n=== ESP8266 Plant Care System Starting ===");
  Serial.println("Firmware compiled: " __DATE__ " " __TIME__);
  Serial.print("ESP8266 Chip ID: ");
  Serial.println(ESP.getChipId(), HEX);
  Serial.print("Flash Chip Size: ");
  Serial.print(ESP.getFlashChipSize() / 1024);
  Serial.println(" KB");
  
  // Initialize communications
  initCommunication();
  
  // Set WiFi to AP mode
  WiFi.mode(WIFI_AP);
  WiFi.softAP("PlantCare-System", "12345678");
  
  Serial.print("Access Point Started - IP: ");
  Serial.println(WiFi.softAPIP());
  
  // Initialize latestData with default values
  DynamicJsonDocument initDoc(512);
  initDoc["light"] = 0;
  initDoc["soil"] = 0;
  initDoc["rain"] = 0;
  initDoc["temperature"] = 0;
  initDoc["humidity"] = 0;
  initDoc["pumpActive"] = false;
  initDoc["lightMode"] = 0;
  initDoc["fanActive"] = false;
  initDoc["fanMode"] = 0;
  initDoc["pumpMode"] = 2;
  
  serializeJson(initDoc, latestData);
  Serial.println("Initial latestData: " + latestData);
  
  // Setup web server
  setupWebServer();
  
  // Send initial ping
  sendCommand("PING");
  
  // Add a diagnostic message about communication settings
  Serial.println("\n=== Communication Settings ===");
  Serial.println("Using SoftwareSerial at 9600 baud");
  Serial.println("RX Pin: D6 (Connect to Arduino TX)");
  Serial.println("TX Pin: D5 (Connect to Arduino RX)");
  Serial.println("Protocol: <COMMAND>");
  Serial.println("Buffer size: " + String(MAX_MESSAGE_LENGTH) + " bytes");
  
  // Run a connection test
  testArduinoConnection();
  
  // Add LED test sequence
  Serial.println("Testing LED indicators...");
  // Test TX LED (D2)
  digitalWrite(TX_LED_PIN, HIGH);
  delay(500);
  digitalWrite(TX_LED_PIN, LOW);
  delay(500);
  
  // Test RX LED (D0)
  digitalWrite(RX_LED_PIN, HIGH);
  delay(500);
  digitalWrite(RX_LED_PIN, LOW);
}

// Add this function to test connection with Arduino
void testArduinoConnection() {
  Serial.println("\n=== Testing Arduino Connection ===");
  Serial.println("Sending test commands to Arduino...");
  
  // Send various test commands
  sendCommand("PING");
  delay(500);
  sendCommand("GET_DATA");
  delay(500);
  sendCommand("TEST");
  
  Serial.println("Test commands sent. Check for responses above.");
  Serial.println("If no response, check wiring and Arduino code.");
}

void loop() {
  // Track timing
  static unsigned long lastYield = 0;
  static unsigned long lastPingTime = 0;
  static unsigned long lastConnectionCheck = 0;
  unsigned long currentMillis = millis();
  
  // Handle LED indicators
  updateLedPatterns();
  
  // Process any incoming serial data - highest priority
  readFromArduino();
  
  // Handle client requests
  server.handleClient();
  
  // Blink status LED based on connection status
  blinkStatusLED();
  
  // Check connection status every 5 seconds
  if (currentMillis - lastConnectionCheck > 5000) {
    Serial.println("\n--- System Status ---");
    Serial.println("Connection to Arduino: " + String(connected ? "Connected" : "Disconnected"));
    Serial.println("Time since last data: " + String((currentMillis - lastDataReceived) / 1000) + " seconds");
    Serial.println("Current sensor values:");
    Serial.println("  Light: " + String(light) + "%");
    Serial.println("  Soil Moisture: " + String(soil) + "%"); 
    Serial.println("  Temperature: " + String(temperature) + "°C");
    Serial.println("  Humidity: " + String(humidity) + "%");
    Serial.println("  Rain detected: " + String(rain ? "Yes" : "No"));
    Serial.println("Device status:");
    Serial.println("  Pump: " + String(pumpActive ? "ON" : "OFF") + " (Mode: " + String(pumpMode) + ")");
    Serial.println("  Light: " + String(lightMode == 1 ? "ON" : "OFF") + " (Mode: " + String(lightMode) + ")");
    Serial.println("  Fan: " + String(fanActive ? "ON" : "OFF") + " (Mode: " + String(fanMode) + ")");
    
    // Display WiFi status
    Serial.print("WiFi AP: ");
    Serial.print(WiFi.softAPgetStationNum());
    Serial.println(" client(s) connected");
    
    lastConnectionCheck = currentMillis;
  }
  
  // Periodically send ping if disconnected
  if (!connected && currentMillis - lastPingTime > 5000) {
    Serial.println("Sending PING to Arduino...");
    sendCommand("PING");
    lastPingTime = currentMillis;
  }
  
  // Periodically send ping if disconnected
  static unsigned long lastJsonDebugTime = 0;
  if (currentMillis - lastJsonDebugTime > 15000) {
    Serial.println("\n--- JSON Debug ---");
    Serial.println("Last received data: " + latestData);
    
    // Send a data request to Arduino
    if (connected) {
      sendCommand("GET_DATA");
      Serial.println("Sent GET_DATA command to Arduino");
    }
    
    lastJsonDebugTime = currentMillis;
  }
  
  // Allow background processing every 50ms to prevent WDT resets
  if (currentMillis - lastYield > 50) {
    yield();
    lastYield = currentMillis;
  }
  
  // Monitor memory usage - helps diagnose crashes
  static unsigned long lastMemReport = 0;
  if (currentMillis - lastMemReport > 10000) {
    Serial.print("Free heap: ");
    Serial.println(ESP.getFreeHeap());
    lastMemReport = currentMillis;
  }
  
  // Add more detailed diagnostic for disconnections
  static bool wasPreviouslyConnected = false;
  
  if (connected && !wasPreviouslyConnected) {
    Serial.println("\n*** CONNECTED to Arduino! ***");
    wasPreviouslyConnected = true;
    
    // Show connected status with LEDs
    setTxLedPattern(LED_PATTERN_DOUBLE_PULSE, 1000);
    setRxLedPattern(LED_PATTERN_DOUBLE_PULSE, 1000);
  }
  else if (!connected && wasPreviouslyConnected) {
    Serial.println("\n*** DISCONNECTED from Arduino! ***");
    Serial.println("  - Check physical connections");
    Serial.println("  - Verify Arduino is powered and running");
    Serial.println("  - Verify baud rates match (115200)");
    Serial.println("  - Reset both devices if problem persists");
    wasPreviouslyConnected = false;
    
    // Show disconnected status with LEDs
    setTxLedPattern(LED_PATTERN_SLOW_BLINK, 5000); // Long slow blink on disconnect
    setRxLedPattern(LED_PATTERN_SLOW_BLINK, 5000);
  }
  
  // Periodically flash LEDs when connected to show system is working
  static unsigned long lastLedIndicator = 0;
  if (connected && (currentMillis - lastLedIndicator > 10000)) { // Every 10 seconds
    setTxLedPattern(LED_PATTERN_DOUBLE_PULSE, 500);
    lastLedIndicator = currentMillis;
  }
}