#include <Arduino.h>
#include "display.h"
#include "actuators.h"
#include "sensors.h"

// Define global objects
MCUFRIEND_kbv tft;
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

// UI refresh timing
unsigned long lastRefreshTime = 0;
#define REFRESH_INTERVAL 500  // Update display max twice per second

// Add this at the top with other variables
bool displayNeedsFullRedraw = false;

// Define pages for navigation
#define PAGE_MAIN 0
#define PAGE_CONTROLS 1
#define PAGE_INFO 2
uint8_t currentPage = PAGE_MAIN;

// Modern UI layout constants
#define HEADER_HEIGHT 0  // Removed header completely
#define CARD_MARGIN 6
#define CARD_PADDING 10
#define CARD_CORNER_RADIUS 8
#define BUTTON_CORNER_RADIUS 6

// Recalculate card sizes with more vertical space
#define CARD_WIDTH ((tft.width() - (CARD_MARGIN * 3)) / 2)
#define CARD_HEIGHT 80  // Slightly taller cards

// Make buttons larger and more touchable
#define BUTTON_HEIGHT 50
#define BUTTON_WIDTH 100
#define BUTTON_SPACING 5

// Modern color palette with better contrast
#define BACKGROUND_COLOR 0x0000  // Black
#define CARD_BACKGROUND   0x000 // Slightly lighter blue-gray
#define CARD_BORDER       0x6B6D // Medium gray
#define TEXT_PRIMARY      0xFFFF // White
#define TEXT_SECONDARY    0xCE59 // Light gray
#define ACCENT_PRIMARY    0x07FF // Cyan
#define ACCENT_SECONDARY  0xF81F // Magenta

// Sensor-specific colors
#define LIGHT_COLOR      0xFD20  // Amber/gold
#define MOISTURE_COLOR   0x07E0  // Green
#define TEMP_COLOR       0xF800  // Red
#define HUMIDITY_COLOR   0x5D9B  // Blue
#define RAIN_COLOR       0x001F  // Deep blue

// Status colors
#define STATUS_ON_COLOR  0x07E0  // Green
#define STATUS_OFF_COLOR 0xF800  // Red
#define STATUS_AUTO_COLOR 0xFFE0 // Yellow

// Touch interaction variables
unsigned long lastTouchTime = 0;
#define TOUCH_DEBOUNCE 300  // 300ms debounce for touch
bool touchProcessed = false;

// Helper function to draw a rounded rectangle
void drawRoundRect(int x, int y, int w, int h, int r, uint16_t color, bool fill) {
  if (fill) {
    tft.fillRoundRect(x, y, w, h, r, color);
  } else {
    tft.drawRoundRect(x, y, w, h, r, color);
  }
}

// Initialize the display
void initializeDisplay() {
  Serial.println("Initializing display...");
  
  uint16_t identifier = tft.readID();
  if (identifier == 0x0 || identifier == 0xFFFF) {
    identifier = 0x9341;
  }
  
  tft.begin(identifier);
  tft.setRotation(1); // Landscape mode
  tft.fillScreen(BACKGROUND_COLOR);
  
  // Text parameters for better readability
  tft.setTextWrap(false);
  tft.cp437(true);
  
  // Draw the main screen
  drawMainScreen();
  
  Serial.println("Display initialized");
}

// Draw a sensor card with value
void drawSensorCard(const char* label, int x, int y, int w, int h, float value, 
                    uint16_t color, bool hasDecimal = false, const char* unit = "%") {
  // Card background with shadow effect
  drawRoundRect(x + 2, y + 2, w, h, CARD_CORNER_RADIUS, 0x2104, true); // Shadow
  drawRoundRect(x, y, w, h, CARD_CORNER_RADIUS, CARD_BACKGROUND, true); // Background
  drawRoundRect(x, y, w, h, CARD_CORNER_RADIUS, color, false); // Border
  
  // Label at top
  tft.setTextSize(1);
  tft.setTextColor(color);
  tft.setCursor(x + 10, y + 10);
  tft.print(label);
  
  // Large value in center
  tft.setTextSize(3);
  tft.setTextColor(TEXT_PRIMARY);
  
  // Format value based on whether it needs decimal or not
  char valueStr[10];
  if (hasDecimal) {
    dtostrf(value, 4, 1, valueStr);
  } else {
    dtostrf(value, 3, 0, valueStr);
  }
  
  // Center the text
  int textWidth = strlen(valueStr) * 16;
  int textX = x + (w - textWidth) / 2;
  tft.setCursor(textX, y + h/2 - 10);
  tft.print(valueStr);
  
  // Unit at bottom right
  tft.setTextSize(1);
  tft.setCursor(x + w - 20, y + h - 15);
  tft.print(unit);
  
  // Add a visual indicator bar
  float percentage = hasDecimal ? value / 50.0 : value / 100.0;
  percentage = constrain(percentage, 0, 1);
  int barWidth = (w - 20) * percentage;
  
  // Bar background
  tft.fillRect(x + 10, y + h - 10, w - 20, 4, CARD_BORDER);
  // Bar value
  tft.fillRect(x + 10, y + h - 10, barWidth, 4, color);
}

// Draw a control button with state indicator
void drawControlButton(const char* label, int x, int y, int w, int h, 
                      uint16_t color, uint8_t state) {
  // Selected state effect
  uint16_t bgColor = CARD_BACKGROUND;
  uint16_t textColor = TEXT_PRIMARY;
  
  if (state == 1) { // ON
    bgColor = color;
    textColor = 0x0000; // Black text on colored background
  } else if (state == 2) { // AUTO
    bgColor = STATUS_AUTO_COLOR;
    textColor = 0x0000; // Black text
  }
  
  // Button with appropriate colors
  drawRoundRect(x, y, w, h, BUTTON_CORNER_RADIUS, bgColor, true);
  drawRoundRect(x, y, w, h, BUTTON_CORNER_RADIUS, color, false);
  
  // Text label
  tft.setTextSize(2);
  tft.setTextColor(textColor);
  
  // Center the text
  int textWidth = strlen(label) * 12;
  int textX = x + (w - textWidth) / 2;
  tft.setCursor(textX, y + h/2 - 8);
  tft.print(label);
  
  // Status indicator
  tft.setTextSize(1);
  const char* stateText = (state == 0) ? "OFF" : ((state == 1) ? "ON" : "AUTO");
  tft.setCursor(x + w/2 - 10, y + h - 15);
  tft.print(stateText);
}

// Draw the main screen with sensor cards
void drawMainScreen() {
  // Clear the entire screen
  tft.fillScreen(BACKGROUND_COLOR);
  
  // Calculate positions for a 2x2 grid of sensor cards
  int col1X = CARD_MARGIN;
  int col2X = CARD_MARGIN * 2 + CARD_WIDTH;
  int row1Y = CARD_MARGIN;  // Start from the top now
  int row2Y = row1Y + CARD_HEIGHT + CARD_MARGIN;
  
  // Draw improved rain indicator at the top right
  drawRainIndicator(readRainSensor());
  
  // Draw sensor cards in a grid
  drawSensorCard("LIGHT", col1X, row1Y, CARD_WIDTH, CARD_HEIGHT, 0, LIGHT_COLOR);
  drawSensorCard("MOISTURE", col2X, row1Y, CARD_WIDTH, CARD_HEIGHT, 0, MOISTURE_COLOR);
  drawSensorCard("TEMPERATURE", col1X, row2Y, CARD_WIDTH, CARD_HEIGHT, 0, TEMP_COLOR, true, "°C");
  drawSensorCard("HUMIDITY", col2X, row2Y, CARD_WIDTH, CARD_HEIGHT, 0, HUMIDITY_COLOR);
  
  // Calculate position for the control buttons - place at bottom with equal spacing
  int totalButtonWidth = BUTTON_WIDTH * 3 + BUTTON_SPACING * 2;
  int startX = (tft.width() - totalButtonWidth) / 2;
  int buttonY = row2Y + CARD_HEIGHT + CARD_MARGIN * 2;
  
  // Draw control buttons in a more prominent row with better spacing
  drawControlButton("LIGHT", startX, buttonY, BUTTON_WIDTH, BUTTON_HEIGHT, LIGHT_COLOR, getLightMode());
  drawControlButton("FAN", startX + BUTTON_WIDTH + BUTTON_SPACING, buttonY, BUTTON_WIDTH, BUTTON_HEIGHT, HUMIDITY_COLOR, getFanMode());
  drawControlButton("PUMP", startX + (BUTTON_WIDTH + BUTTON_SPACING) * 2, buttonY, BUTTON_WIDTH, BUTTON_HEIGHT, MOISTURE_COLOR, getPumpMode());
}

// Draw a rain status indicator
void drawRainIndicator(bool isRaining) {
  int boxWidth = 80;
  int boxHeight = 40;
  int boxX = tft.width() - boxWidth - 5;
  int boxY = 5;  // Position at top right
  
  // Background with highlight effect
  uint16_t bgColor = isRaining ? RAIN_COLOR : CARD_BACKGROUND;
  drawRoundRect(boxX, boxY, boxWidth, boxHeight, 8, bgColor, true);
  
  // Add a subtle pulsing border for rain
  if (isRaining) {
    // Double border effect for active state
    drawRoundRect(boxX, boxY, boxWidth, boxHeight, 8, RAIN_COLOR + 0x0821, false);
    drawRoundRect(boxX-1, boxY-1, boxWidth+2, boxHeight+2, 9, RAIN_COLOR + 0x1842, false);
  } else {
    drawRoundRect(boxX, boxY, boxWidth, boxHeight, 8, RAIN_COLOR, false);
  }
  
  // Improved text with icon
  tft.setTextSize(1);
  tft.setTextColor(TEXT_PRIMARY);
  
  // Draw raindrop icon
  if (isRaining) {
    // Animated raindrop if raining
    for (int i = 0; i < 3; i++) {
      int dropX = boxX + 12;
      int dropY = boxY + 12 + i*5;
      tft.fillCircle(dropX, dropY, 2, TEXT_PRIMARY);
    }
  }
  
  // Text label with better positioning
  tft.setCursor(boxX + 25, boxY + 15);
  tft.print(isRaining ? "RAINING" : "NO RAIN");
}

// Define the updateDisplaySimple function BEFORE it's used
void updateDisplaySimple(int lightPercent, int moisturePercent, float temperature, float humidity, bool fanState) {
  static int lastLightPercent = -1;
  static int lastMoisturePercent = -1;
  static float lastTemperature = -1.0;
  static float lastHumidity = -1.0;
  static bool lastRainState = false;
  static uint8_t lastLightMode = 255;
  static uint8_t lastFanMode = 255;
  static uint8_t lastPumpMode = 255;
  
  // Calculate positions once
  int col1X = CARD_MARGIN;
  int col2X = CARD_MARGIN * 2 + CARD_WIDTH;
  int row1Y = CARD_MARGIN;
  int row2Y = row1Y + CARD_HEIGHT + CARD_MARGIN;
  int totalButtonWidth = BUTTON_WIDTH * 3 + BUTTON_SPACING * 2;
  int startX = (tft.width() - totalButtonWidth) / 2;
  int buttonY = row2Y + CARD_HEIGHT + CARD_MARGIN * 2;
  
  // Update all values without threshold checking
  if (lightPercent != lastLightPercent) {
    drawSensorCard("LIGHT", col1X, row1Y, CARD_WIDTH, CARD_HEIGHT, lightPercent, LIGHT_COLOR);
    lastLightPercent = lightPercent;
  }
  
  if (moisturePercent != lastMoisturePercent) {
    drawSensorCard("MOISTURE", col2X, row1Y, CARD_WIDTH, CARD_HEIGHT, moisturePercent, MOISTURE_COLOR);
    lastMoisturePercent = moisturePercent;
  }
  
  if (temperature != lastTemperature) {
    drawSensorCard("TEMPERATURE", col1X, row2Y, CARD_WIDTH, CARD_HEIGHT, temperature, TEMP_COLOR, true, "°C");
    lastTemperature = temperature;
  }
  
  if (humidity != lastHumidity) {
    drawSensorCard("HUMIDITY", col2X, row2Y, CARD_WIDTH, CARD_HEIGHT, humidity, HUMIDITY_COLOR);
    lastHumidity = humidity;
  }
  
  // Check rain status
  bool currentRain = readRainSensor();
  if (currentRain != lastRainState) {
    drawRainIndicator(currentRain);
    lastRainState = currentRain;
  }
  
  // Update control buttons status
  uint8_t currentLightMode = getLightMode();
  if (currentLightMode != lastLightMode) {
    drawControlButton("LIGHT", startX, buttonY, BUTTON_WIDTH, BUTTON_HEIGHT, LIGHT_COLOR, currentLightMode);
    lastLightMode = currentLightMode;
  }
  
  uint8_t currentFanMode = getFanMode();
  if (currentFanMode != lastFanMode) {
    drawControlButton("FAN", startX + BUTTON_WIDTH + BUTTON_SPACING, buttonY, BUTTON_WIDTH, BUTTON_HEIGHT, HUMIDITY_COLOR, currentFanMode);
    lastFanMode = currentFanMode;
  }
  
  uint8_t currentPumpMode = getPumpMode();
  if (currentPumpMode != lastPumpMode) {
    drawControlButton("PUMP", startX + (BUTTON_WIDTH + BUTTON_SPACING) * 2, buttonY, BUTTON_WIDTH, BUTTON_HEIGHT, MOISTURE_COLOR, currentPumpMode);
    lastPumpMode = currentPumpMode;
  }
}

// Update the display with new sensor values
void updateDisplay(int lightPercent, int moisturePercent, float temperature, float humidity, bool fanState) {
  updateDisplaySimple(lightPercent, moisturePercent, temperature, humidity, fanState);
}

// Handle touch input with improved mapping and feedback
void handleTouchInput() {
  TSPoint p = ts.getPoint();
  
  // Reset pins immediately - critical for proper operation
  pinMode(XP, OUTPUT);
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
  pinMode(YM, OUTPUT);
  
  // Quick return if no valid touch
  if (p.z <= MINPRESSURE || p.z >= MAXPRESSURE) {
    return;
  }
  
  // Debounce check
  unsigned long currentTime = millis();
  if (currentTime - lastTouchTime < TOUCH_DEBOUNCE) {
    return;
  }
  
  // Only do the mapping calculation when needed
  int y = map(p.x, 120, 900, tft.height(), 0);
  int x = map(p.y, 120, 940, 0, tft.width());
  
  // Debug touch coordinates less frequently
  static unsigned long lastTouchDebug = 0;
  if (currentTime - lastTouchDebug > 1000) {
    Serial.print("Touch at x=");
    Serial.print(x);
    Serial.print(", y=");
    Serial.println(y);
    lastTouchDebug = currentTime;
  }
  
  // Process touch
  processTouchOnCurrentPage(x, y);
  
  // Update debounce timer
  lastTouchTime = currentTime;
  touchProcessed = true;
  
  // Signal that display needs full redraw after touch action
  displayNeedsFullRedraw = true;
}

// Improve processTouchOnCurrentPage with non-blocking visual feedback
void processTouchOnCurrentPage(int x, int y) {
  // Calculate positions once
  int col1X = CARD_MARGIN;
  int col2X = CARD_MARGIN * 2 + CARD_WIDTH;
  int row1Y = CARD_MARGIN; 
  int row2Y = row1Y + CARD_HEIGHT + CARD_MARGIN;
  int totalButtonWidth = BUTTON_WIDTH * 3 + BUTTON_SPACING * 2;
  int startX = (tft.width() - totalButtonWidth) / 2;
  int buttonY = row2Y + CARD_HEIGHT + CARD_MARGIN * 2;
  
  // State for visual feedback (we'll use this instead of delay())
  static unsigned long feedbackStartTime = 0;
  static int feedbackButton = -1;
  static int feedbackX = 0, feedbackY = 0, feedbackW = 0, feedbackH = 0;
  static uint16_t feedbackColor = 0;
  static uint8_t feedbackMode = 0;
  
  // Check if we need to end visual feedback
  if (feedbackButton >= 0 && millis() - feedbackStartTime >= 100) {
    // Redraw the appropriate button with updated mode
    if (feedbackButton == 0) {
      drawControlButton("LIGHT", feedbackX, feedbackY, feedbackW, feedbackH, LIGHT_COLOR, feedbackMode);
    } else if (feedbackButton == 1) {
      drawControlButton("FAN", feedbackX, feedbackY, feedbackW, feedbackH, HUMIDITY_COLOR, feedbackMode);
    } else if (feedbackButton == 2) {
      drawControlButton("PUMP", feedbackX, feedbackY, feedbackW, feedbackH, MOISTURE_COLOR, feedbackMode);
    }
    feedbackButton = -1; // Reset feedback state
    return; // We've processed feedback, exit now
  }
  
  // Main page button handling
  if (currentPage == PAGE_MAIN) {
    // Light button
    if (y > buttonY && y < buttonY + BUTTON_HEIGHT && 
        x > startX && x < startX + BUTTON_WIDTH) {
      // Visual feedback - store the state instead of using delay
      drawRoundRect(startX, buttonY, BUTTON_WIDTH, BUTTON_HEIGHT, BUTTON_CORNER_RADIUS, 0xFFFF, true);
      feedbackStartTime = millis();
      feedbackButton = 0;
      feedbackX = startX;
      feedbackY = buttonY;
      feedbackW = BUTTON_WIDTH;
      feedbackH = BUTTON_HEIGHT;
      
      // Toggle light mode
      uint8_t currentMode = getLightMode();
      uint8_t newMode = (currentMode + 1) % 3;
      setLightMode(newMode);
      feedbackMode = newMode;
    }
    
    // Fan button
    else if (y > buttonY && y < buttonY + BUTTON_HEIGHT && 
             x > startX + BUTTON_WIDTH + BUTTON_SPACING && 
             x < startX + BUTTON_WIDTH*2 + BUTTON_SPACING) {
      // Visual feedback
      drawRoundRect(startX + BUTTON_WIDTH + BUTTON_SPACING, buttonY, BUTTON_WIDTH, BUTTON_HEIGHT, BUTTON_CORNER_RADIUS, 0xFFFF, true);
      feedbackStartTime = millis();
      feedbackButton = 1;
      feedbackX = startX + BUTTON_WIDTH + BUTTON_SPACING;
      feedbackY = buttonY;
      feedbackW = BUTTON_WIDTH;
      feedbackH = BUTTON_HEIGHT;
      
      // Toggle fan mode
      uint8_t currentMode = getFanMode();
      uint8_t newMode = (currentMode + 1) % 3;
      setFanMode(newMode);
      feedbackMode = newMode;
    }
    
    // Pump button
    else if (y > buttonY && y < buttonY + BUTTON_HEIGHT && 
             x > startX + (BUTTON_WIDTH + BUTTON_SPACING)*2 && 
             x < startX + (BUTTON_WIDTH + BUTTON_SPACING)*2 + BUTTON_WIDTH) {
      // Visual feedback
      drawRoundRect(startX + (BUTTON_WIDTH + BUTTON_SPACING)*2, buttonY, BUTTON_WIDTH, BUTTON_HEIGHT, BUTTON_CORNER_RADIUS, 0xFFFF, true);
      feedbackStartTime = millis();
      feedbackButton = 2;
      feedbackX = startX + (BUTTON_WIDTH + BUTTON_SPACING)*2;
      feedbackY = buttonY;
      feedbackW = BUTTON_WIDTH;
      feedbackH = BUTTON_HEIGHT;
      
      // Toggle pump mode
      uint8_t currentMode = getPumpMode();
      uint8_t newMode = (currentMode + 1) % 3;
      setPumpMode(newMode);
      feedbackMode = newMode;
    }
  }
}

// Refresh display with sensor data
void refreshDisplay(int lightPercent, int moisturePercent, float temperature, float humidity, bool fanState) {
  unsigned long currentTime = millis();
  
  // IMPORTANT: Always check for touch input on EVERY call - this keeps touch responsive
  handleTouchInput();
  
  // Only update the visual elements at the refresh interval
  if (currentTime - lastRefreshTime >= REFRESH_INTERVAL || displayNeedsFullRedraw) {
    lastRefreshTime = currentTime;
    displayNeedsFullRedraw = false;
    
    // Update all display elements without thresholds
    updateDisplaySimple(lightPercent, moisturePercent, temperature, humidity, fanState);
  }
}

// Draw a loading splash screen
void drawLoadingScreen() {
  tft.fillScreen(BACKGROUND_COLOR);
  
  // Draw a fancy loading animation
  drawRoundRect(tft.width()/2 - 100, tft.height()/2 - 20, 200, 40, 5, CARD_BACKGROUND, true);
  
  tft.setTextColor(TEXT_PRIMARY);
  tft.setTextSize(2);
  tft.setCursor(tft.width()/2 - 75, tft.height()/2 - 10);
  tft.print("LOADING...");
  
  // Progress bar animation
  for (int i = 0; i < 180; i += 10) {
    tft.fillRect(tft.width()/2 - 90 + i, tft.height()/2 + 30, 10, 10, 
                 rainbow(map(i, 0, 180, 0, 255)));
    delay(50);
  }
}

// Generate rainbow colors for the loading animation
uint16_t rainbow(byte value) {
  // Value is expected to be 0-255
  if (value < 85) {
    return tft.color565(255 - value * 3, 0, value * 3);
  } else if (value < 170) {
    value -= 85;
    return tft.color565(0, value * 3, 255 - value * 3);
  } else {
    value -= 170;
    return tft.color565(value * 3, 255 - value * 3, 0);
  }
}

// Add these function declarations if they're missing from the header
void drawHeader(const char* title) {
  // Draw a header with the given title
  tft.fillRect(0, 0, tft.width(), HEADER_HEIGHT, ACCENT_PRIMARY);
  tft.setTextColor(TEXT_PRIMARY);
  tft.setTextSize(2);
  
  // Center the text
  int textWidth = strlen(title) * 12;
  int textX = (tft.width() - textWidth) / 2;
  tft.setCursor(textX, 10);
  tft.print(title);
}