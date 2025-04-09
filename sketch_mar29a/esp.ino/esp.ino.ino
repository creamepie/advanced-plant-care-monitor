#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include <SoftwareSerial.h>

// Create web server object on port 80
ESP8266WebServer server(80);

// Define communication pins - IMPORTANT: These must match your physical connections
#define RX_PIN D6  // ESP RX (D6) connects to Arduino TX (pin 18)
#define TX_PIN D5  // ESP TX (D5) connects to Arduino RX (pin 19)
SoftwareSerial arduinoSerial(RX_PIN, TX_PIN); // RX, TX

// Message protocol - MUST MATCH your Arduino communication.h
#define START_MARKER '<'
#define END_MARKER '>'
#define SEPARATOR '|'

// Buffer for receiving data
const int MAX_MESSAGE_LENGTH = 64;
char receivedChars[MAX_MESSAGE_LENGTH];
boolean messageInProgress = false;
int charIndex = 0;

// Sensor values that will be updated from Arduino
int light = 0;
int soil = 0;
int rain = 0;
float temperature = 0.0;
float humidity = 0.0;
bool pumpActive = false;
int lightMode = 0; // 0=off, 1=on, 2=auto

// Add these new globals
bool fanActive = false;
int fanMode = 0;
int pumpMode = 2; // default to AUTO

// Connection status
bool connected = false;
unsigned long lastDataReceived = 0;

void setup() {
  // Initialize hardware serial for debug
  Serial.begin(115200);
  delay(1000);
  
  // Initialize software serial for Arduino communication
  arduinoSerial.begin(115200); // MUST MATCH ESP_BAUD_RATE on Arduino
  delay(500);
  
  Serial.println("\n\n=== ESP8266 Plant Care Dashboard ===");
  Serial.println("Using SoftwareSerial on pins D6(RX) and D5(TX)");
  
  // LED for status indication
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH); // LED off (active LOW)
  
  // Set WiFi to AP mode
  WiFi.mode(WIFI_AP);
  WiFi.softAP("PlantCare-System", "12345678");
  
  Serial.print("Access Point Started - IP: ");
  Serial.println(WiFi.softAPIP());
  
  // Setup web routes
  setupWebServer();
  
  // Send initial ping
  sendCommand("PING");
}

void loop() {
  // Handle client requests
  server.handleClient();
  
  // Process any incoming serial data
  readFromArduino();
  
  // Blink LED based on connection status
  blinkStatusLED();
  
  // Periodically send ping if disconnected
  static unsigned long lastPingTime = 0;
  if (!connected && millis() - lastPingTime > 5000) {
    sendCommand("PING");
    lastPingTime = millis();
  }
  
  // Allow background processing
  yield();
}

void readFromArduino() {
  while (arduinoSerial.available() > 0) {
    // Flash the LED to show activity
    digitalWrite(LED_BUILTIN, LOW); // LED on (active LOW)
    
    char inChar = arduinoSerial.read();
    
    // Start marker received
    if (inChar == START_MARKER) {
      messageInProgress = true;
      charIndex = 0;
      Serial.print("Start marker received: ");
    }
    // End marker received
    else if (inChar == END_MARKER) {
      messageInProgress = false;
      receivedChars[charIndex] = '\0'; // Null-terminate the string
      Serial.println(" [END]");
      parseMessage(receivedChars);
    }
    // Add character to buffer
    else if (messageInProgress) {
      if (charIndex < MAX_MESSAGE_LENGTH - 1) {
        receivedChars[charIndex++] = inChar;
        Serial.write(inChar); // Echo to debug serial
      }
    }
    
    // Return LED to normal state
    digitalWrite(LED_BUILTIN, HIGH); // LED off (active LOW)
  }
}

void parseMessage(char* message) {
  // Update connection status
  connected = true;
  lastDataReceived = millis();
  
  Serial.print("Parsing message: ");
  Serial.println(message);
  
  // Count the separators to validate message format
  int separatorCount = 0;
  for (int i = 0; message[i] != '\0'; i++) {
    if (message[i] == SEPARATOR) separatorCount++;
  }
  
  // Expect 4 separators for 5 values
  if (separatorCount != 4) {
    Serial.print("Invalid message format. Expected 4 separators, found ");
    Serial.println(separatorCount);
    return;
  }
  
  // Parse the message - EXACTLY matching Arduino format
  int lightVal, soilVal, rainVal;
  float tempVal, humidVal;
  
  // Use strtok to safely parse each value
  char* token = strtok(message, "|");
  if (token) lightVal = atoi(token);
  
  token = strtok(NULL, "|");
  if (token) soilVal = atoi(token);
  
  token = strtok(NULL, "|");
  if (token) rainVal = atoi(token);
  
  token = strtok(NULL, "|");
  if (token) tempVal = atof(token);
  
  token = strtok(NULL, "|");
  if (token) humidVal = atof(token);
  
  // Update global variables
  light = lightVal;
  soil = soilVal;
  rain = rainVal;
  temperature = tempVal;
  humidity = humidVal;
  
  // Print parsed values
  Serial.println("Parsed values:");
  Serial.print("Light: "); Serial.println(light);
  Serial.print("Soil: "); Serial.println(soil);
  Serial.print("Rain: "); Serial.println(rain);
  Serial.print("Temperature: "); Serial.println(temperature);
  Serial.print("Humidity: "); Serial.println(humidity);
}

void sendCommand(const String& command) {
  arduinoSerial.print(START_MARKER);
  arduinoSerial.print(command);
  arduinoSerial.print(END_MARKER);
  
  Serial.print("Sent command: ");
  Serial.println(command);
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

// ===== Web Interface Code =====

void setupWebServer() {
  // Main page
  server.on("/", handleRoot);
  
  // API endpoints
  server.on("/api/data", HTTP_GET, handleApiData);
  server.on("/api/control", HTTP_POST, handleApiControl);
  
  // Static resources
  server.on("/styles.css", handleStyles);
  server.on("/script.js", handleScript);
  
  // Start server
  server.begin();
  Serial.println("Web server started - http://192.168.4.1");
}

void handleApiData() {
  // Create JSON object with sensor data
  DynamicJsonDocument doc(512);
  
  doc["light"] = light;
  doc["soil"] = soil;
  doc["rain"] = rain;
  doc["temperature"] = temperature;
  doc["humidity"] = humidity;
  doc["pumpActive"] = pumpActive;
  doc["lightMode"] = lightMode;
  doc["connected"] = connected;
  doc["lastUpdate"] = (millis() - lastDataReceived) / 1000;
  doc["pumpMode"] = pumpMode;       // Include pump mode
  doc["fanActive"] = fanActive;     // Include fan status
  doc["fanMode"] = fanMode;         // Include fan mode
  
  String jsonResponse;
  serializeJson(doc, jsonResponse);
  
  server.send(200, "application/json", jsonResponse);
}

void handleApiControl() {
  String device = server.arg("device");
  String action = server.arg("action");
  
  Serial.print("Control request: ");
  Serial.print(device);
  Serial.print(" -> ");
  Serial.println(action);
  
  // Process pump controls
  if (device == "pump") {
    if (action == "off") {
      pumpActive = false;
      pumpMode = 0;
      sendCommand("PUMP:MODE:0");  // OFF mode
    } 
    else if (action == "on") {
      pumpActive = true;
      pumpMode = 1;
      sendCommand("PUMP:MODE:1");  // ON mode
    } 
    else if (action == "auto") {
      pumpActive = false;
      pumpMode = 2;
      sendCommand("PUMP:MODE:2");  // AUTO mode
    }
  }
  
  // Process fan controls
  else if (device == "fan") {
    if (action == "off") {
      fanActive = false;
      fanMode = 0;
      sendCommand("FAN:MODE:0");
    } else if (action == "on") {
      fanActive = true;
      fanMode = 1;
      sendCommand("FAN:MODE:1");
    } else if (action == "auto") {
      fanActive = false;
      fanMode = 2;
      sendCommand("FAN:MODE:2");
    }
  }
  
  // Process light controls
  else if (device == "light") {
    if (action == "off") {
      lightMode = 0;
      sendCommand("LIGHT:MODE:0");
    } else if (action == "on") {
      lightMode = 1;
      sendCommand("LIGHT:MODE:1");
    } else if (action == "auto") {
      lightMode = 2;
      sendCommand("LIGHT:MODE:2");
    }
  }
  
  server.send(200, "text/plain", "OK");
}

void handleRoot() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Plant Care System</title>
  <link rel="stylesheet" href="styles.css">
</head>
<body>
  <div class="app">
    <header>
      <h1>🌿 Plant Care Dashboard</h1>
      <div class="status-badge connected">
        <span id="status-icon">●</span> 
        <span id="status-text">Connected</span>
      </div>
    </header>
    
    <main>
      <section class="sensor-panel">
        <div class="card temp-card">
          <div class="card-icon">🌡️</div>
          <div class="card-title">Temperature</div>
          <div class="card-value"><span id="temp-value">--</span>°C</div>
          <div class="progress-bar">
            <div class="progress-fill" id="temp-bar" style="width: 0%"></div>
          </div>
        </div>
        
        <div class="card humidity-card">
          <div class="card-icon">💧</div>
          <div class="card-title">Humidity</div>
          <div class="card-value"><span id="humidity-value">--</span>%</div>
          <div class="progress-bar">
            <div class="progress-fill" id="humidity-bar" style="width: 0%"></div>
          </div>
        </div>
        
        <div class="card light-card">
          <div class="card-icon">☀️</div>
          <div class="card-title">Light</div>
          <div class="card-value"><span id="light-value">--</span>%</div>
          <div class="progress-bar">
            <div class="progress-fill" id="light-bar" style="width: 0%"></div>
          </div>
        </div>
        
        <div class="card moisture-card">
          <div class="card-icon">🌊</div>
          <div class="card-title">Soil Moisture</div>
          <div class="card-value"><span id="moisture-value">--</span>%</div>
          <div class="progress-bar">
            <div class="progress-fill" id="moisture-bar" style="width: 0%"></div>
          </div>
        </div>
        
        <div class="card rain-card">
          <div class="card-icon">🌧️</div>
          <div class="card-title">Rain</div>
          <div class="card-value"><span id="rain-value">--</span>%</div>
          <div class="progress-bar">
            <div class="progress-fill" id="rain-bar" style="width: 0%"></div>
          </div>
        </div>
        
        <div class="card soil-card">
          <div class="card-icon">🌱</div>
          <div class="card-title">Soil Quality</div>
          <div class="card-value"><span id="soil-value">--</span>%</div>
          <div class="progress-bar">
            <div class="progress-fill" id="soil-bar" style="width: 0%"></div>
          </div>
        </div>
      </section>
      
      <section class="control-panel">
        <h2>System Controls</h2>
        
        <div class="control-cards">
          <div class="control-card">
            <div class="control-title">Water Pump</div>
            <div class="button-group">
              <button class="btn" id="pump-on">ON</button>
              <button class="btn" id="pump-off">OFF</button>
            </div>
            <div class="status-indicator" id="pump-status">OFF</div>
          </div>
          
          <div class="control-card">
            <div class="control-title">Grow Light</div>
            <div class="button-group">
              <button class="btn" id="light-off">OFF</button>
              <button class="btn" id="light-on">ON</button>
              <button class="btn" id="light-auto">AUTO</button>
            </div>
            <div class="status-indicator" id="light-status">OFF</div>
          </div>
        </div>
      </section>
    </main>
    
    <footer>
      <div id="update-time">Last update: --</div>
      <div>Plant Care System v1.0</div>
    </footer>
  </div>
  
  <script src="script.js"></script>
</body>
</html>
)rawliteral";

  server.send(200, "text/html", html);
}

void handleStyles() {
  String css = R"rawliteral(
/* Modern CSS with nice visual styles */
* {
  box-sizing: border-box;
  margin: 0;
  padding: 0;
  font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Oxygen, Ubuntu, sans-serif;
}

:root {
  --primary-color: #23936e;
  --secondary-color: #4CAF50;
  --dark-color: #1b3a4b;
  --light-color: #f3f8f2;
  --danger-color: #e53935;
  --warning-color: #ffb74d;
  --info-color: #29b6f6;
  --border-radius: 12px;
  --card-shadow: 0 4px 6px rgba(0, 0, 0, 0.1), 0 1px 3px rgba(0, 0, 0, 0.08);
  --transition: all 0.3s ease;
}

body {
  background-color: #eceff1;
  color: var(--dark-color);
  line-height: 1.6;
}

.app {
  max-width: 1200px;
  margin: 0 auto;
  padding: 1rem;
}

header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  margin-bottom: 2rem;
  padding-bottom: 1rem;
  border-bottom: 2px solid var(--primary-color);
}

h1 {
  color: var(--primary-color);
  font-size: 2rem;
}

h2 {
  color: var(--primary-color);
  margin-bottom: 1rem;
}

.status-badge {
  display: flex;
  align-items: center;
  gap: 0.5rem;
  padding: 0.5rem 1rem;
  border-radius: 999px;
  font-weight: 500;
  font-size: 0.875rem;
}

.status-badge.connected {
  background-color: rgba(76, 175, 80, 0.15);
  color: #2e7d32;
}

.status-badge.disconnected {
  background-color: rgba(229, 57, 53, 0.15);
  color: #c62828;
}

#status-icon {
  font-size: 1rem;
}

main {
  display: grid;
  gap: 2rem;
}

.sensor-panel {
  display: grid;
  grid-template-columns: repeat(auto-fill, minmax(300px, 1fr));
  gap: 1rem;
}

.card {
  background-color: white;
  border-radius: var(--border-radius);
  padding: 1.5rem;
  box-shadow: var(--card-shadow);
  transition: var(--transition);
  position: relative;
}

.card:hover {
  transform: translateY(-5px);
}

.card-icon {
  font-size: 2rem;
  margin-bottom: 0.5rem;
}

.card-title {
  font-weight: 600;
  color: var(--dark-color);
  margin-bottom: 0.5rem;
}

.card-value {
  font-size: 2rem;
  font-weight: 700;
  margin: 0.5rem 0;
}

.progress-bar {
  height: 10px;
  background-color: #e0e0e0;
  border-radius: 5px;
  overflow: hidden;
  margin-top: 1rem;
}

.progress-fill {
  height: 100%;
  border-radius: 5px;
  transition: width 0.5s ease-out;
}

.temp-card .progress-fill { background-color: #ff7043; }
.humidity-card .progress-fill { background-color: #29b6f6; }
.light-card .progress-fill { background-color: #ffca28; }
.moisture-card .progress-fill { background-color: #26c6da; }
.rain-card .progress-fill { background-color: #5c6bc0; }
.soil-card .progress-fill { background-color: #8d6e63; }

.control-panel {
  background-color: white;
  border-radius: var(--border-radius);
  padding: 1.5rem;
  box-shadow: var(--card-shadow);
}

.control-cards {
  display: grid;
  grid-template-columns: repeat(auto-fill, minmax(250px, 1fr));
  gap: 1rem;
}

.control-card {
  border: 1px solid #e0e0e0;
  border-radius: var(--border-radius);
  padding: 1rem;
}

.control-title {
  font-weight: 600;
  margin-bottom: 1rem;
}

.button-group {
  display: flex;
  gap: 0.5rem;
  margin-bottom: 1rem;
}

.btn {
  background-color: #e0e0e0;
  color: var(--dark-color);
  border: none;
  padding: 0.5rem 1rem;
  border-radius: 4px;
  cursor: pointer;
  font-weight: 500;
  transition: var(--transition);
  flex: 1;
}

.btn:hover {
  background-color: #bdbdbd;
}

.btn.active {
  background-color: var(--primary-color);
  color: white;
}

.status-indicator {
  text-align: center;
  padding: 0.5rem;
  border-radius: 4px;
  font-weight: 500;
  background-color: #f5f5f5;
}

.status-indicator.on {
  background-color: rgba(76, 175, 80, 0.15);
  color: #2e7d32;
}

.status-indicator.off {
  background-color: rgba(229, 57, 53, 0.15);
  color: #c62828;
}

.status-indicator.auto {
  background-color: rgba(255, 183, 77, 0.15);
  color: #ef6c00;
}

footer {
  margin-top: 2rem;
  padding-top: 1rem;
  border-top: 1px solid #e0e0e0;
  display: flex;
  justify-content: space-between;
  color: #757575;
  font-size: 0.875rem;
}

@media (max-width: 768px) {
  header {
    flex-direction: column;
    align-items: flex-start;
    gap: 1rem;
  }
  
  .status-badge {
    align-self: flex-start;
  }
  
  .card-value {
    font-size: 1.5rem;
  }
  
  footer {
    flex-direction: column;
    gap: 0.5rem;
  }
}
)rawliteral";

  server.send(200, "text/css", css);
}

void handleScript() {
  String js = R"rawliteral(
// Plant Care System Dashboard JavaScript
document.addEventListener('DOMContentLoaded', function() {
  // Initialize data update
  updateData();
  setInterval(updateData, 2000);
  
  // Set up button event listeners
  document.getElementById('pump-on').addEventListener('click', function() {
    sendControl('pump', 'on');
  });
  
  document.getElementById('pump-off').addEventListener('click', function() {
    sendControl('pump', 'off');
  });
  
  document.getElementById('light-off').addEventListener('click', function() {
    sendControl('light', 'off');
  });
  
  document.getElementById('light-on').addEventListener('click', function() {
    sendControl('light', 'on');
  });
  
  document.getElementById('light-auto').addEventListener('click', function() {
    sendControl('light', 'auto');
  });
});

// Fetch sensor data from ESP8266 API
function updateData() {
  fetch('/api/data')
    .then(response => response.json())
    .then(data => {
      // Update connection status
      if (data.connected) {
        document.querySelector('.status-badge').classList.add('connected');
        document.querySelector('.status-badge').classList.remove('disconnected');
        document.getElementById('status-text').textContent = 'Connected';
        document.getElementById('status-icon').textContent = '●';
      } else {
        document.querySelector('.status-badge').classList.remove('connected');
        document.querySelector('.status-badge').classList.add('disconnected');
        document.getElementById('status-text').textContent = 'Disconnected';
        document.getElementById('status-icon').textContent = '●';
      }
      
      // Update sensor values and gauges
      updateSensor('temp', data.temperature, '°C', 15, 40);
      updateSensor('humidity', data.humidity, '%', 0, 100);
      updateSensor('light', data.light, '%', 0, 100);
      updateSensor('moisture', data.soil, '%', 0, 100);
      updateSensor('rain', data.rain, '%', 0, 100);
      updateSensor('soil', data.soil, '%', 0, 100);
      
      // Update control status indicators
      updatePumpStatus(data.pumpActive);
      updateLightStatus(data.lightMode);
      
      // Update last update time
      document.getElementById('update-time').textContent = 
        'Last update: ' + new Date().toLocaleTimeString();
    })
    .catch(error => {
      console.error('Error fetching data:', error);
      document.querySelector('.status-badge').classList.remove('connected');
      document.querySelector('.status-badge').classList.add('disconnected');
      document.getElementById('status-text').textContent = 'Disconnected';
    });
}

// Update a sensor display with new value
function updateSensor(id, value, unit, min, max) {
  // Update the value text
  document.getElementById(id + '-value').textContent = value;
  
  // Calculate percentage for the progress bar
  const percentage = Math.min(100, Math.max(0, ((value - min) / (max - min)) * 100));
  
  // Update the progress bar
  document.getElementById(id + '-bar').style.width = percentage + '%';
}

// Update pump status indicator
function updatePumpStatus(isActive) {
  const statusElement = document.getElementById('pump-status');
  const onButton = document.getElementById('pump-on');
  const offButton = document.getElementById('pump-off');
  
  if (isActive) {
    statusElement.textContent = 'ON';
    statusElement.classList.add('on');
    statusElement.classList.remove('off');
    onButton.classList.add('active');
    offButton.classList.remove('active');
  } else {
    statusElement.textContent = 'OFF';
    statusElement.classList.add('off');
    statusElement.classList.remove('on');
    onButton.classList.remove('active');
    offButton.classList.add('active');
  }
}

// Update light mode status indicator
function updateLightStatus(mode) {
  const statusElement = document.getElementById('light-status');
  const offButton = document.getElementById('light-off');
  const onButton = document.getElementById('light-on');
  const autoButton = document.getElementById('light-auto');
  
  // Reset all buttons
  offButton.classList.remove('active');
  onButton.classList.remove('active');
  autoButton.classList.remove('active');
  
  // Remove all status classes
  statusElement.classList.remove('off', 'on', 'auto');
  
  // Set appropriate status based on mode
  if (mode === 0) {
    statusElement.textContent = 'OFF';
    statusElement.classList.add('off');
    offButton.classList.add('active');
  } else if (mode === 1) {
    statusElement.textContent = 'ON';
    statusElement.classList.add('on');
    onButton.classList.add('active');
  } else if (mode === 2) {
    statusElement.textContent = 'AUTO';
    statusElement.classList.add('auto');
    autoButton.classList.add('active');
  }
}

// Send control commands to ESP8266
function sendControl(device, action) {
  const formData = new FormData();
  formData.append('device', device);
  formData.append('action', action);
  
  fetch('/api/control', {
    method: 'POST',
    body: formData
  })
  .then(response => {
    if (response.ok) {
      // Force an immediate data update
      updateData();
    }
  })
  .catch(error => {
    console.error('Error sending command:', error);
  });
}
)rawliteral";

  server.send(200, "application/javascript", js);
}