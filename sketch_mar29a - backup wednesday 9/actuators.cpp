#include "actuators.h"
#include <avr/wdt.h>  // Add this line to fix wdt_reset errors
#include <Adafruit_NeoPixel.h>
#include <Arduino.h>

// Add or replace these globals at the top with other globals
bool pumpAutoMode = true;  // Define with initial value
unsigned long pumpStartTime = 0;
bool pumpActive = false;
uint8_t currentPumpMode = PUMP_MODE_AUTO;  // Default to auto mode
unsigned long lastPumpStateChange = 0;
const unsigned long PUMP_DEBOUNCE_TIME = 1000; // 1 second debounce

// Add this global variable
bool fanState = false;

// LED effect settings
#define FADE_STEPS 50       // Number of steps for fade effect
#define FADE_DELAY 20       // Milliseconds between fade steps
#define COLOR_CYCLE_DELAY 30 // Milliseconds between color changes

// Plant-friendly light colors
const uint32_t GROW_COLOR = Adafruit_NeoPixel::Color(255, 255, 255); // Full spectrum white
const uint32_t BLOOM_COLOR = Adafruit_NeoPixel::Color(255, 180, 210); // Pink/purple for flowering
const uint32_t SEEDLING_COLOR = Adafruit_NeoPixel::Color(140, 255, 140); // Gentle green for seedlings
const uint32_t NIGHT_COLOR = Adafruit_NeoPixel::Color(10, 0, 30);   // Very dim blue for night viewing

// Current light settings
uint8_t currentBrightness = 0;
uint32_t currentColor = GROW_COLOR;
bool fadeInProgress = false;

// Create NeoPixel object
Adafruit_NeoPixel pixels(NUM_LEDS, LIGHT_PIN, NEO_GRB + NEO_KHZ800);

// Global variables to track actuator states
bool pumpState = false;
uint8_t currentLightMode = LIGHT_MODE_AUTO; // Default to auto mode

void initializePump() {
  // Set pump pin as output
  pinMode(PUMP_PIN, OUTPUT);
  
  // Ensure pump starts in OFF state
  digitalWrite(PUMP_PIN, RELAY_ACTIVE_LOW ? HIGH : LOW);
  
  // Initialize state tracking variables
  pumpActive = false;
  currentPumpMode = PUMP_MODE_AUTO;
  
  Serial.println("Pump system initialized in AUTO mode");
}

void initializeFan() {
  pinMode(FAN_PIN, OUTPUT);
  digitalWrite(FAN_PIN, LOW); // Ensure fan starts off
  fanState = false;
  Serial.println("Fan initialized");
}

void initializeActuators() {
  // Initialize NeoPixel
  pixels.begin();
  pixels.setBrightness(BRIGHTNESS);
  pixels.clear(); // Set all pixels to 'off'
  pixels.show();  // Initialize all pixels to 'off'
  
  // Properly initialize pump pin with pull-up
  pinMode(PUMP_PIN, OUTPUT);
  digitalWrite(PUMP_PIN, RELAY_ACTIVE_LOW ? HIGH : LOW);  // Ensure pump is OFF
  pumpActive = false;
  pumpAutoMode = true;
  
  // Initialize pump
  initializePump();
  
  // Initialize fan
  initializeFan();
  
  Serial.println("All actuators initialized");
  Serial.println("WS2812 RGB LEDs configured for plant lighting");
}
  
  // Start with very low brightness for elegance
  pixels.setBrightness(10);
  
  // PHASE 1: Gentle wake-up - subtle fade in of soft blue
  uint32_t softBlue = Adafruit_NeoPixel::Color(40, 60, 90);
  
  // Fill all pixels with the soft blue color
  for(int i = 0; i < NUM_LEDS; i++) {
    pixels.setPixelColor(i, softBlue);
  }
  
  // Gentle fade in
  for(int b = 5; b <= 40; b += 1) {
    pixels.setBrightness(b);
    pixels.show();
    delay(20);
  }
  
  delay(200);
  
  // PHASE 2: Professional gradient - smooth transition to a warmer tone
  uint32_t warmWhite = Adafruit_NeoPixel::Color(90, 80, 60);
  
  // Gentle cross-fade to warm white
  for(int step = 0; step <= 30; step++) {
    float ratio = (float)step / 30.0;
    
    // Calculate intermediate color
    uint8_t r = (1-ratio) * ((softBlue >> 16) & 0xFF) + ratio * ((warmWhite >> 16) & 0xFF);
    uint8_t g = (1-ratio) * ((softBlue >> 8) & 0xFF) + ratio * ((warmWhite >> 8) & 0xFF);
    uint8_t b = (1-ratio) * (softBlue & 0xFF) + ratio * (warmWhite & 0xFF);
    
    uint32_t blendedColor = Adafruit_NeoPixel::Color(r, g, b);
    
    for(int i = 0; i < NUM_LEDS; i++) {
      pixels.setPixelColor(i, blendedColor);
    }
    
    pixels.show();
    delay(20);
  }
  
  delay(200);
  
  // PHASE 3: Sophisticated reveal - elegant transition to final plant light
  
  delay(200);
  wdt_reset();
  
  // PHASE 3: Sophisticated reveal - elegant transition to final plant light
  uint32_t plantLight = SEEDLING_COLOR;
  
  // Graceful transition to actual plant light
  for(int step = 0; step <= 40; step++) {
    wdt_reset();
    float ratio = (float)step / 40.0;
    
    // Calculate intermediate color
    uint8_t r = (1-ratio) * ((warmWhite >> 16) & 0xFF) + ratio * ((plantLight >> 16) & 0xFF);
    uint8_t g = (1-ratio) * ((warmWhite >> 8) & 0xFF) + ratio * ((plantLight >> 8) & 0xFF);
    uint8_t b = (1-ratio) * (warmWhite & 0xFF) + ratio * (plantLight & 0xFF);
    
    uint32_t blendedColor = Adafruit_NeoPixel::Color(r, g, b);
    
    for(int i = 0; i < NUM_LEDS; i++) {
      pixels.setPixelColor(i, blendedColor);
    }
    
    // Also gradually increase brightness to full
    int newBrightness = 40 + (ratio * (BRIGHTNESS - 40));
    pixels.setBrightness(newBrightness);
    
    pixels.show();
    delay(20);
  }
  
  // Brief pause to appreciate the final state
  delay(300);
  wdt_reset();
  
  // Restore appropriate state based on mode
  if (currentLightMode == LIGHT_MODE_OFF) {
    // Elegant fade out if we should be off
    for(int b = BRIGHTNESS; b >= 0; b -= 5) {
      wdt_reset();
      pixels.setBrightness(b);
      pixels.show();
      delay(20);
    }
    pixels.clear();
    pixels.show();
  } else {
    // Otherwise, restore proper color and brightness
    setLightColor(savedColor);
    pixels.setBrightness(savedBrightness);
    pixels.show();
  }
  
  Serial.println("Startup animation complete");
}

// Update softTransition to use watchdog-safe delays
void softTransition(bool turnOn) {
  fadeInProgress = true;
  
  uint8_t startBrightness = turnOn ? 0 : BRIGHTNESS;
  uint8_t endBrightness = turnOn ? BRIGHTNESS : 0;
  
  // SHORTER fade with fewer steps for safety
  #define SAFE_FADE_STEPS 25
  
  // Calculate step size
  int step = (endBrightness - startBrightness) / SAFE_FADE_STEPS;
  
  // Perform the fade with safety measures
  for (int i = 0; i <= SAFE_FADE_STEPS; i++) {
    // Reset watchdog in each iteration
    wdt_reset();
    
    // Calculate current brightness
    currentBrightness = startBrightness + (step * i);
    
    // Set brightness
    pixels.setBrightness(currentBrightness);
    
    // Set all LEDs to the current color
    for (int p = 0; p < NUM_LEDS; p++) {
      pixels.setPixelColor(p, currentColor);
    }
    
    pixels.show();
    delay(10); // Use a MUCH shorter delay
  }
  
  // Reset watchdog after operation
  wdt_reset();
  fadeInProgress = false;
}

void setLightState(bool state) {
  if (state) {
    // Check if lights are currently off
    if (currentBrightness == 0) {
      // Run swirl animation first
      swirlAnimation(currentColor);
      
      // Then do the soft transition to target brightness
      currentBrightness = 80; // Animation ends at this brightness
      softTransition(true);   // Finish transitioning to full brightness
    } else {
      // Lights are already on, just do soft transition
      softTransition(true);
    }
  } else {
    // Soft-off effect
    softTransition(false);
  }
}

void setLightColor(uint32_t newColor) {
  if (currentColor == newColor) return;
  
  // Store new target color
  uint32_t oldColor = currentColor;
  currentColor = newColor;
  
  // Only do transition if lights are on
  if (currentBrightness > 0) {
    // Interpolate between colors
    for (int i = 0; i <= FADE_STEPS; i++) {
      wdt_reset();
      float ratio = (float)i / FADE_STEPS;
      
      // Extract RGB components
      uint8_t oldR = (oldColor >> 16) & 0xFF;
      uint8_t oldG = (oldColor >> 8) & 0xFF;
      uint8_t oldB = oldColor & 0xFF;
      
      uint8_t newR = (newColor >> 16) & 0xFF;
      uint8_t newG = (newColor >> 8) & 0xFF;
      uint8_t newB = newColor & 0xFF;
      
      // Calculate intermediate color
      uint8_t r = oldR + ratio * (newR - oldR);
      uint8_t g = oldG + ratio * (newG - oldG);
      uint8_t b = oldB + ratio * (newB - oldB);
      
      uint32_t intermediateColor = Adafruit_NeoPixel::Color(r, g, b);
      
      // Set all LEDs to the intermediate color
      for (int p = 0; p < NUM_LEDS; p++) {
        pixels.setPixelColor(p, intermediateColor);
      }
      
      pixels.show();
      delay(10);  // Fast but smooth transition
    }
  }
  
  // Apply the color immediately if light is off (will be visible when turned on)
  for (int p = 0; p < NUM_LEDS; p++) {
    pixels.setPixelColor(p, newColor);
  }
  
  pixels.show();
  Serial.print("Light color changed to: 0x");
  Serial.println(newColor, HEX);
}

// Simplify the rainbowCycle function
void rainbowCycle(int wait) {
  // Only cycle through a small number of steps
  for (uint16_t j = 0; j < 50; j += 5) {
    // Reset watchdog
    wdt_reset();
    
    for (uint16_t i = 0; i < pixels.numPixels(); i++) {
      // Calculate color based on position
      int pos = (i * 256 / pixels.numPixels() + j) & 255;
      
      if (pos < 85) {
        pixels.setPixelColor(i, Adafruit_NeoPixel::Color(255 - pos * 3, pos * 3, 0));
      } else if (pos < 170) {
        pos -= 85;
        pixels.setPixelColor(i, Adafruit_NeoPixel::Color(0, 255 - pos * 3, pos * 3));
      } else {
        pos -= 170;
        pixels.setPixelColor(i, Adafruit_NeoPixel::Color(pos * 3, 0, 255 - pos * 3));
      }
    }
    pixels.show();
    delay(10); // Much shorter delay
  }
  
  // Reset watchdog after animation
  wdt_reset();
}

void swirlAnimation(uint32_t targetColor) {
  // Start with all LEDs off
  pixels.clear();
  pixels.show();
  wdt_reset();
  
  // Set to medium brightness during animation
  pixels.setBrightness(60);
  
  // Single elegant revolution with trailing effect
  const int tailLength = NUM_LEDS/2;  // Trail length - half the ring
  
  // Just one complete revolution
  for (int i = 0; i <= NUM_LEDS; i++) {
    wdt_reset();
    pixels.clear();
    
    // Draw the tail with gradient
    for (int t = 0; t < tailLength; t++) {
      // Calculate position with wrap-around
      int pos = (i - t + NUM_LEDS) % NUM_LEDS;
      
      // Calculate fade factor - tail gets dimmer toward the end
      float fade = 1.0 - ((float)t / tailLength);
      
      // Extract RGB components
      uint8_t r = ((targetColor >> 16) & 0xFF) * fade;
      uint8_t g = ((targetColor >> 8) & 0xFF) * fade;
      uint8_t b = (targetColor & 0xFF) * fade;
      
      // Set pixel with faded color
      pixels.setPixelColor(pos, Adafruit_NeoPixel::Color(r, g, b));
    }
    
    pixels.show();
    delay(30);  // Slower, more elegant movement
  }
  
  // Fill in the entire ring with a smooth expand
  for (int i = 0; i < NUM_LEDS; i++) {
    wdt_reset();
    // Fill in one more LED each time
    pixels.setPixelColor(i, targetColor);
    pixels.show();
    delay(10);
  }
  
  // Final gentle pulse
  for (int b = 60; b < 100; b += 2) {
    wdt_reset();
    pixels.setBrightness(b);
    pixels.show();
    delay(5);
  }
  for (int b = 100; b > 80; b -= 2) {
    wdt_reset();
    pixels.setBrightness(b);
    pixels.show();
    delay(5);
  }
  
  wdt_reset();
}

void setPumpMode(uint8_t mode) {
  // Store previous mode
  uint8_t previousMode = currentPumpMode;
  
  // Update mode
  currentPumpMode = mode;
  
  // Take action based on new mode
  switch(mode) {
    case PUMP_MODE_OFF:
      // Turn off pump immediately
      if (pumpActive) {
        digitalWrite(PUMP_PIN, RELAY_ACTIVE_LOW ? HIGH : LOW);
        pumpActive = false;
      }
      break;
      
    case PUMP_MODE_ON:
      // Turn on pump immediately
      if (!pumpActive) {
        digitalWrite(PUMP_PIN, RELAY_ACTIVE_LOW ? LOW : HIGH);
        pumpActive = true;
        pumpStartTime = millis();
      }
      break;
      
    case PUMP_MODE_AUTO:
      // Don't change pump state immediately
      // Will be controlled by sensor readings
      break;
  }
  
  // Debug output
  Serial.print("Pump mode changed to: ");
  Serial.println(mode == PUMP_MODE_OFF ? "OFF" : (mode == PUMP_MODE_ON ? "ON" : "AUTO"));
  
  // Special case - when transitioning to AUTO, remember the time
  if (mode == PUMP_MODE_AUTO && previousMode != PUMP_MODE_AUTO && pumpActive) {
    pumpStartTime = millis();  // Reset timer for safety timeout
  }
}

uint8_t getPumpMode() {
  return currentPumpMode;
}

bool getPumpState() {
  return pumpActive;
}

// Replace setPumpState function completely
void setPumpState(bool state) {
  unsigned long currentTime = millis();
  static uint8_t lastReportedState = 255; // Initialize to invalid value
  
  // Only log when state or active status changes (reduces serial spam)
  if (state != pumpActive || lastReportedState != currentPumpMode) {
    Serial.print("PUMP REQUEST: Turn ");
    Serial.print(state ? "ON" : "OFF");
    Serial.print(" in mode ");
    Serial.print(currentPumpMode);
    Serial.print(" (current state: ");
    Serial.print(pumpActive ? "ON" : "OFF");
    Serial.println(")");
    lastReportedState = currentPumpMode;
  }
  
  // Strong debounce protection - prevent rapid cycling
  if (currentTime - lastPumpStateChange < PUMP_DEBOUNCE_TIME) {
    return; // Silently ignore too-frequent changes
  }
  
  // Only change state if it's actually different
  if (state != pumpActive) {
    // Actually change the physical pin
    digitalWrite(PUMP_PIN, state ? (RELAY_ACTIVE_LOW ? LOW : HIGH) : (RELAY_ACTIVE_LOW ? HIGH : LOW));
    
    // Update state tracking
    pumpActive = state;
    lastPumpStateChange = currentTime;
    
    // Store start time if turning on
    if (state) {
      pumpStartTime = currentTime;
    }
    
    // Log the change
    Serial.print("PUMP STATE CHANGED TO: ");
    Serial.println(state ? "ON" : "OFF");
  }
}

// Replace/modify your pump update function
void updatePumpBasedOnMode(int soilMoisturePercent) {
  static unsigned long lastUpdate = 0;
  
  // Only update once per second maximum
  if (millis() - lastUpdate < 1000) {
    return;
  }
  lastUpdate = millis();
  
  // For digital sensor, reading is already stable (either 20% or 80%)
  // No need for the averaging array with digital readings
  
  // Only take action if in AUTO mode
  if (currentPumpMode == PUMP_MODE_AUTO) {
    // Safety check first
    if (pumpActive && (millis() - pumpStartTime > PUMP_SAFETY_TIMEOUT)) {
      Serial.println("SAFETY: Pump timeout reached");
      setPumpState(false);
      return;
    }
    
    // Digital sensor gives us 20% (dry) or 80% (wet)
    // So we can use 50% as the decision boundary
    if (!pumpActive && soilMoisturePercent < 50) {
      Serial.println("AUTO: Soil too dry - turning pump on");
      setPumpState(true);
    } 
    else if (pumpActive && soilMoisturePercent > 50) {
      Serial.println("AUTO: Soil moisture sufficient - turning pump off");
      setPumpState(false);
    }
  }
}

bool getLightState() {
  return currentBrightness > 0;
}

void setLightMode(uint8_t mode) {
  currentLightMode = mode;
  
  switch(mode) {
    case LIGHT_MODE_OFF:
      // Turn off lights
      setLightState(false);
      break;
      
    case LIGHT_MODE_ON:
      // Set to full spectrum white and turn on
      setLightColor(GROW_COLOR);
      setLightState(true);
      break;
      
    case LIGHT_MODE_AUTO:
      // In auto mode, use a more natural color
      setLightColor(SEEDLING_COLOR);
      // Don't change light state, it will be controlled by the sensor
      break;
  }
  
  Serial.print("Light mode changed to: ");
  Serial.println(mode == LIGHT_MODE_OFF ? "OFF" : (mode == LIGHT_MODE_ON ? "ON" : "AUTO"));
}

uint8_t getLightMode() {
  return currentLightMode;
}

void updateLightBasedOnMode(int lightPercent) {
  // Only take action if in AUTO mode
  if (currentLightMode == LIGHT_MODE_AUTO) {
    if (lightPercent < 30) {
      // It's dark, turn on lights with a natural daylight color
      if (!getLightState()) {
        setLightColor(SEEDLING_COLOR);
        setLightState(true);
      }
      Serial.println("AUTO: Turning ON lights due to low light levels");
    } 
    else if (lightPercent > 70) {
      // It's bright, turn off lights
      if (getLightState()) {
        setLightState(false);
      }
      Serial.println("AUTO: Turning OFF lights due to sufficient light");
    }
  }
}