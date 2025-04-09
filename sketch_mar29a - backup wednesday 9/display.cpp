#include <Arduino.h>
#include "display.h"
#include "actuators.h"

// Define global objects
MCUFRIEND_kbv tft;
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

// Add these at the top with other variables
unsigned long lastRefreshTime = 0;
#define REFRESH_INTERVAL 2000 // 2 seconds in milliseconds

// Add these constants for the tri-state button
#define LIGHT_BUTTON_X 30
#define LIGHT_BUTTON_Y 170
#define LIGHT_BUTTON_W 120
#define LIGHT_BUTTON_H 45
#define BUTTON_RADIUS 8

// Page tracking
#define PAGE_MAIN 0
uint8_t currentPage = PAGE_MAIN;

// UI layout positions and sizes
#define HEADER_HEIGHT 50
#define BOX_MARGIN 12
#define BOX_HEIGHT 70
#define BOX_WIDTH 145
#define BOX_RADIUS 10

#define PUMP_BUTTON_X 170
#define PUMP_BUTTON_Y 170
#define PUMP_BUTTON_W 120
#define PUMP_BUTTON_H 45

// Add these constants for the new controls
#define FAN_BUTTON_X 30
#define FAN_BUTTON_Y 250
#define FAN_BUTTON_W 120
#define FAN_BUTTON_H 45

#define RAIN_STATUS_X 170
#define RAIN_STATUS_Y 250
#define RAIN_STATUS_W 120
#define RAIN_STATUS_H 45

// Touch data specific baud rate
#define TOUCH_SERIAL_BAUD 57600

// Status tracking
bool lightStatus = false;
bool pumpStatus = false;

// Modern minimalist color palette with accent colors
#define BACKGROUND_COLOR 0x0000  // Black
#define CARD_COLOR       0x0841  // Very dark blue
#define CARD_BORDER      0xFFFF  // White
#define HEADER_COLOR     0x001F  // Navy blue

// Text colors
#define TEXT_COLOR       0xFFFF  // White
#define SUBTITLE_COLOR   0xBDF7  // Light gray
#define VALUE_COLOR      0xFFFF  // White for maximum readability

// Distinct accent colors for each sensor
#define LIGHT_COLOR      0xFD20  // Warm amber/gold
#define MOISTURE_COLOR   0x07E0  // Bright green
#define TEMP_COLOR       0xF800  // Bright red
#define HUMIDITY_COLOR   0x07FF  // Cyan blue

// Status indicators
#define STATUS_ON_COLOR  0x07E0  // Green for "ON" status
#define STATUS_OFF_COLOR 0xF800  // Red for "OFF" status

// Modern UI elements
#define SHADOW_OFFSET 1
#define ICON_SIZE 12

void initializeDisplay() {
  Serial.println("Initializing display...");
  
  uint16_t identifier = tft.readID();
  Serial.print("Display ID: 0x");
  Serial.println(identifier, HEX);
  
  if (identifier == 0x0 || identifier == 0xFFFF) {
    Serial.println("Display ID invalid, forcing to 0x9341 (ILI9341)");
    identifier = 0x9341;
  }
  
  tft.begin(identifier);
  tft.setRotation(1); // Landscape mode
  
  // Fill background with black
  tft.fillScreen(BACKGROUND_COLOR);
  
  // Draw modern header with gradient
  for (int i = 0; i < HEADER_HEIGHT; i++) {
    uint16_t gradColor = map(i, 0, HEADER_HEIGHT, 0x001F, 0x071F); // Navy blue gradient
    tft.drawFastHLine(0, i, tft.width(), gradColor);
  }
  
  // Draw title with clean look
  tft.setTextSize(3);
  tft.setTextColor(TEXT_COLOR);
  tft.setCursor(30, 17);
  tft.print("PLANT MONITOR");
  
  // Draw small plant icon
  
  
  // Set text parameters for better readability
  tft.setTextWrap(false); // Prevents text from wrapping
  tft.cp437(true); // Use extended ASCII characters if needed
  
  // Draw main screen
  drawMainScreen();
}

void drawPlantIcon(int x, int y, int size) {
  // Plant stem
  tft.drawLine(x, y, x, y - size, MOISTURE_COLOR);
  
  // Leaves
  tft.fillCircle(x - size/2, y - size/2, size/3, MOISTURE_COLOR);
  tft.fillCircle(x + size/2, y - size/2, size/3, MOISTURE_COLOR);
  tft.fillCircle(x, y - size, size/3, MOISTURE_COLOR);
}

void drawMainScreen() {
  // Clear the screen except for the header
  tft.fillRect(0, HEADER_HEIGHT, tft.width(), tft.height() - HEADER_HEIGHT, BACKGROUND_COLOR);
  
  // Draw sensor cards with distinct colors
  drawSensorCard("LIGHT", BOX_MARGIN, HEADER_HEIGHT + BOX_MARGIN, 
                BOX_WIDTH, BOX_HEIGHT, LIGHT_COLOR);
  
  drawSensorCard("MOISTURE", tft.width() - BOX_MARGIN - BOX_WIDTH, 
                HEADER_HEIGHT + BOX_MARGIN, BOX_WIDTH, BOX_HEIGHT, MOISTURE_COLOR);
  
  drawSensorCard("TEMPERATURE", BOX_MARGIN, 
                HEADER_HEIGHT + BOX_MARGIN*2 + BOX_HEIGHT, 
                BOX_WIDTH, BOX_HEIGHT, TEMP_COLOR);
  
  drawSensorCard("HUMIDITY", tft.width() - BOX_MARGIN - BOX_WIDTH, 
                HEADER_HEIGHT + BOX_MARGIN*2 + BOX_HEIGHT, 
                BOX_WIDTH, BOX_HEIGHT, HUMIDITY_COLOR);

  // Add rain sensor status
  drawControlButton("RAIN", RAIN_STATUS_X, RAIN_STATUS_Y, RAIN_STATUS_W, RAIN_STATUS_H, BLUE, false);

  // Add fan control button
  drawControlButton("FAN", FAN_BUTTON_X, FAN_BUTTON_Y, FAN_BUTTON_W, FAN_BUTTON_H, GREEN, false);
                
  // Update status indicators
  updateStatusIndicators();
}

void drawSensorCard(String label, int x, int y, int w, int h, uint16_t accentColor) {
  // Draw card with subtle shadow for depth
  tft.fillRect(x + SHADOW_OFFSET, y + SHADOW_OFFSET, w, h, 0x2104); // Dark shadow
  tft.fillRect(x, y, w, h, CARD_COLOR); // Card background
  
  // Draw colored accent border
  tft.drawRect(x, y, w, h, accentColor);
  tft.drawRect(x+1, y+1, w-2, h-2, accentColor); // Double line for emphasis
  
  // Draw sensor label with colored text
  tft.setTextSize(2);
  tft.setTextColor(accentColor);
  tft.setCursor(x + 10, y + 10);
  tft.print(label);
  
  // Draw units in bottom right
  String unit = "";
  if (label == "TEMPERATURE") {
    unit = "°C";
  } else {
    unit = "%";
  }
  
  tft.setTextSize(1);
  tft.setTextColor(accentColor);
  tft.setCursor(x + w - 20, y + h - 14);
  tft.print(unit);
  
  // Draw sensor icon
  drawSensorIcon(x + 20, y + 35, label, accentColor);
}

void drawSensorIcon(int x, int y, String type, uint16_t color) {
  if (type == "LIGHT") {
    // Sun icon
    tft.fillCircle(x, y, 4, color);
    // Rays
    for (int i = 0; i < 8; i++) {
      float angle = i * PI / 4;
      int x1 = x + sin(angle) * 5;
      int y1 = y + cos(angle) * 5;
      int x2 = x + sin(angle) * 8;
      int y2 = y + cos(angle) * 8;
      tft.drawLine(x1, y1, x2, y2, color);
    }
  } 
  else if (type == "MOISTURE") {
    // Water drop
    tft.fillCircle(x, y + 2, 3, color);
    for (int i = 0; i < 3; i++) {
      tft.drawLine(x - 3 + i*3, y + 2, x, y - 4, color);
    }
  }
  else if (type == "TEMPERATURE") {
    // Thermometer
    tft.drawRect(x - 1, y - 5, 3, 10, color);
    tft.fillRect(x - 1, y + 2, 3, 3, color);
    tft.fillCircle(x, y + 5, 3, color);
  }
  else if (type == "HUMIDITY") {
    // Humidity waves
    for (int i = 0; i < 3; i++) {
      tft.drawLine(x - 4, y - i*3, x + 4, y - i*3, color);
    }
  }
}

void drawControlButton(String label, int x, int y, int width, int height, uint16_t color, bool active) {
  // Draw button background
  tft.fillRect(x, y, width, height, active ? color : CARD_COLOR);
  tft.drawRect(x, y, width, height, color);

  // Draw label
  tft.setTextSize(2);
  tft.setTextColor(TEXT_COLOR);
  tft.setCursor(x + 10, y + 10);
  tft.print(label);
}

void updateRainStatus(bool rainDetected) {
  // Update rain status button
  drawControlButton("RAIN", RAIN_STATUS_X, RAIN_STATUS_Y, RAIN_STATUS_W, RAIN_STATUS_H, BLUE, rainDetected);
}

void updateFanStatus(bool fanState) {
  // Update fan control button
  drawControlButton("FAN", FAN_BUTTON_X, FAN_BUTTON_Y, FAN_BUTTON_W, FAN_BUTTON_H, GREEN, fanState);
}

void updateDisplay(int lightPercent, int moisturePercent, float temperature, float humidity, bool rainDetected, bool fanState) {
  static int lastLightPercent = -1;
  static int lastMoisturePercent = -1;
  static float lastTemperature = -1.0;
  char buffer[10];
  
  // Check for invalid values
  if (isnan(value) || value < -100 || value > 1000) {
    // Display NaN for invalid readings
    strcpy(buffer, "NaN");
    
    // Debug output when invalid value detected
    Serial.print("INVALID "); Serial.print(type); 
    Serial.println(" value detected");
  } else if (type == "TEMPERATURE") {
    // Format with one decimal place for temperature
    dtostrf(value, 4, 1, buffer);
  } else {
    // Format as integer for percentages
    dtostrf(value, 3, 0, buffer);
  }
  
  // Debug - print formatted value that will be displayed

  // Center value in card for better visibility
  int textWidth = strlen(buffer) * 18; // Adjust for text size 3
  int textX = x + (w - textWidth) / 2;
  
  // Draw value with a shadow for better readability
  tft.setTextColor(0x4208); // Shadow color
  tft.setCursor(textX + 1, y + 40 + 1);
  tft.print(buffer);
  
  // Actual value text
  tft.setTextColor(VALUE_COLOR);
  tft.setCursor(textX, y + 40);
  tft.print(buffer);
  
  // Add mini graph at bottom of card for visual representation
  int barWidth;
  if (isnan(value) || value < 0) {
    barWidth = 0; // No bar for invalid values
  } else {
    barWidth = map(value, 0, type == "TEMPERATURE" ? 40 : 100, 0, w - 20);
    // Ensure barWidth is within valid range
    barWidth = constrain(barWidth, 0, w - 20);
  }
  
  // Clear previous bar
  tft.fillRect(x + 10, y + h - 12, w - 20, 6, CARD_COLOR);
  // Draw new bar
  tft.fillRect(x + 10, y + h - 12, barWidth, 6, color);
  // Graph outline
  tft.drawRect(x + 10, y + h - 12, w - 20, 6, TEXT_COLOR);
}

void refreshDisplay(int lightPercent, int moisturePercent, float temperature, float humidity) {
  unsigned long currentTime = millis();
  
  // Only update every REFRESH_INTERVAL milliseconds
  if (currentTime - lastRefreshTime >= REFRESH_INTERVAL) {
    lastRefreshTime = currentTime;
    
    // Update the display with new sensor values
    updateDisplay(lightPercent, moisturePercent, temperature, humidity);
    
    // Debug output to confirm display is refreshing
    Serial.print("REFRESH,");
    Serial.print(lightPercent); Serial.print(",");
    Serial.print(moisturePercent); Serial.print(",");
    Serial.print(temperature); Serial.print(",");
    Serial.println(humidity);
  }
}

void handleTouchInput() {
  // Get touch point
  TSPoint p = ts.getPoint();
  
  // CRITICAL: Reset pins IMMEDIATELY
  pinMode(XP, OUTPUT);
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
  pinMode(YM, OUTPUT);
  
  // Only process if the touch is valid
  if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    Serial.print("RAW TOUCH: ");
    Serial.print(p.x);
    Serial.print(",");
    Serial.print(p.y);
    Serial.print(" pressure=");
    Serial.println(p.z);
    
    // FIXED COORDINATE MAPPING - INVERT X MAPPING
    int y = map(p.x, 100, 900, tft.height(), 0);     // Inverted Y mapping
    int x = map(p.y, 100, 900, 0, tft.width());      // FIXED: Inverted X mapping
    
    Serial.print("MAPPED: ");
    Serial.print(x);
    Serial.println(y);
    
    // Check if touch is in the BOTTOM status bar area
    if (y > (tft.height() - 30)) {
      Serial.println("Touch in bottom status bar");
      
      // Left half - Light control
      if (x < tft.width()/2) {
        // Cycle light mode: OFF -> ON -> AUTO -> OFF
        uint8_t currentMode = getLightMode();
        uint8_t newMode = (currentMode + 1) % 3;
        setLightMode(newMode);
        
        Serial.print("Light set to mode: ");
        Serial.println(newMode);
        
        // Update status indicators
        updateStatusIndicators();
        
        // Visual feedback
        tft.fillCircle(tft.width()/4, tft.height() - 50, 5, 0xFFFF);
        delay(50);
        tft.fillCircle(tft.width()/4, tft.height() - 50, 5, BACKGROUND_COLOR);
      }
      // Right half - Pump control
      else {
        bool currentPumpState = getPumpState();
        setPumpState(!currentPumpState);
        
        Serial.print("Pump set to: ");
        Serial.println(!currentPumpState ? "ON" : "OFF");
        
        // Update status indicators
        updateStatusIndicators();
        
        // Visual feedback
        tft.fillCircle(3*tft.width()/4, tft.height() - 50, 5, 0xFFFF);
        delay(50);
        tft.fillCircle(3*tft.width()/4, tft.height() - 50, 5, BACKGROUND_COLOR);
      }
      
      // Delay to prevent double-touches
      delay(300);
    }
  }
}

void updateStatusIndicators() {
  // Clear the entire status area first
  tft.fillRect(0, tft.height() - 30, tft.width(), 30, BACKGROUND_COLOR);
  tft.drawFastHLine(0, tft.height() - 30, tft.width(), TEXT_COLOR);
  tft.drawFastHLine(0, tft.height() - 8, tft.width(), TEXT_COLOR);
  
  // Draw light status with correct mode
  uint8_t lightMode = getLightMode();
  String lightLabel;
  uint16_t lightColor;
  
  switch(lightMode) {
    case LIGHT_MODE_OFF:
      lightLabel = "OFF";
      lightColor = STATUS_OFF_COLOR;
      break;
    case LIGHT_MODE_ON:
      lightLabel = "ON";
      lightColor = STATUS_ON_COLOR;
      break;
    case LIGHT_MODE_AUTO:
      lightLabel = "AUTO";
      lightColor = 0xFFE0; // Yellow
      break;
  }
  
  // Draw Light Indicator
  tft.fillCircle(tft.width()/4 - 45, tft.height() - 19, 6, lightColor);
  tft.setTextSize(2);
  tft.setTextColor(TEXT_COLOR);
  tft.setCursor(tft.width()/4 - 35, tft.height() - 25);
  tft.print("LIGHT: ");
    tft.setTextColor(lightColor);
  tft.print(lightLabel);
  
  // Draw Pump Indicator
  bool pumpState = getPumpState();
  tft.fillCircle(3*tft.width()/4 - 45, tft.height() - 19, 6, pumpState ? STATUS_ON_COLOR : STATUS_OFF_COLOR);
  tft.setTextColor(TEXT_COLOR);
  tft.setCursor(3*tft.width()/4 - 35, tft.height() - 25);
  tft.print("PUMP: ");
  tft.setTextColor(pumpState ? STATUS_ON_COLOR : STATUS_OFF_COLOR);
  tft.print(pumpState ? "ON" : "OFF");
}