#ifndef CONFIG_H
#define CONFIG_H

// Analog pin definitions for Arduino Mega if not already defined
#ifndef A0
#define A0 54  // First analog pin on Mega
#define A1 55
#define A2 56
#define A3 57
#define A4 58
#define A5 59
#define A14 68  // Analog pin 14 on Mega
#define A15 69  // Analog pin 15 on Mega
#endif

// Sensor pin definitions
#define LIGHT_SENSOR_PIN 24    // Digital pin for light sensor
#define RAIN_SENSOR_PIN 25     // Changed to digital pin
#define SOIL_MOISTURE_PIN 50   // Digital soil moisture sensor
#define DHTPIN 22              // DHT sensor

// Actuator pin definitions
#define PUMP_PIN 30            // Relay for water pump (ground-triggered)
#define RELAY_ACTIVE_LOW true  // Set to true for ground-triggered relay
#define LIGHT_PIN 6            // NeoPixel LEDs
#define NUM_LEDS 16            // Number of LEDs in your strip
#define BRIGHTNESS 150         // Default LED brightness

// Fan pin definition
#define FAN_PIN 31 // Digital pin for fan control

// UART Communication with ESP
#define ESP_SERIAL Serial1     // Use hardware Serial1 on Mega (pins 18/TX1, 19/RX1)
#define ESP_BAUD_RATE 115200   // Baud rate for ESP communication

// DHT sensor type
#define DHTTYPE DHT22         // Change to DHT11 if you're using that model

// Light mode constants
#define LIGHT_MODE_OFF 0
#define LIGHT_MODE_ON 1
#define LIGHT_MODE_AUTO 2

// Pump mode constants
#define PUMP_MODE_OFF 0
#define PUMP_MODE_ON 1
#define PUMP_MODE_AUTO 2

// Soil moisture thresholds
// Digital sensor only gives us 20% (dry) or 80% (wet), so use 50% as threshold
#define SOIL_MOISTURE_DRY_THRESHOLD 50     // Turn on pump when moisture below 50%
#define SOIL_MOISTURE_WET_THRESHOLD 50     // Turn off pump when moisture above 50%
#define PUMP_SAFETY_TIMEOUT 10000          // Maximum pump on time (10 seconds) for safety

// Timing parameters (milliseconds)
#define LIGHT_ON_DURATION 2000
#define PUMP_ON_DURATION 1000
#define READING_INTERVAL 5000
#define ESP_COMM_INTERVAL 10000 // Send data to ESP every 10 seconds

// Display settings
#define DISPLAY_UPDATE_INTERVAL 1000  // Update display every second

#endif // CONFIG_H