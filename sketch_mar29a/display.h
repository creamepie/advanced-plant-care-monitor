#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>
#include <TouchScreen.h>

// Add explicit Arduino Mega analog pin definitions
#ifndef A0
#define A0 54  // First analog pin on Mega
#define A1 55
#define A2 56
#define A3 57
#define A4 58
#define A5 59
#endif

// Touchscreen pins
#define YP A3  
#define XM A2  
#define YM 9   
#define XP 8   

// Touchscreen calibration
#define TS_MINX 120
#define TS_MAXX 940
#define TS_MINY 120
#define TS_MAXY 900
#define MINPRESSURE 10
#define MAXPRESSURE 1000

// Color definitions
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

// Declare global objects as extern
extern MCUFRIEND_kbv tft;
extern TouchScreen ts;

// Function prototypes
void initializeDisplay();
void drawMainScreen();
void drawLoadingScreen();
void drawHeader(const char* title);
void drawSensorCard(const char* label, int x, int y, int w, int h, float value, 
                   uint16_t color, bool hasDecimal, const char* unit);
void drawControlButton(const char* label, int x, int y, int w, int h, 
                      uint16_t color, uint8_t state);
void drawRainIndicator(bool isRaining);
void updateDisplay(int lightPercent, int moisturePercent, float temperature, float humidity, bool fanState);
void updateDisplaySimple(int lightPercent, int moisturePercent, float temperature, float humidity, bool fanState);
void handleTouchInput();
void processTouchOnCurrentPage(int x, int y);
void refreshDisplay(int lightPercent, int moisturePercent, float temperature, float humidity, bool fanState);
uint16_t rainbow(byte value);

#endif // DISPLAY_H