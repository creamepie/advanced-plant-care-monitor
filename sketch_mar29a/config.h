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
#define LIGHT_PIN 26            // NeoPixel LEDs
#define NUM_LEDS 24            // Number of LEDs in your strip
#define BRIGHTNESS 100         // Default LED brightness

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

// Improved timing parameters
#define SENSOR_READ_INTERVAL 2000      // Read sensors every 2 seconds (was scattered throughout code)
#define DISPLAY_UPDATE_INTERVAL 500    // Update display every 0.5 seconds 
#define PUMP_CONTROL_INTERVAL 5000     // Check pump logic every 5 seconds
#define FAN_CONTROL_INTERVAL 10000     // Check fan logic every 10 seconds
#define LIGHT_CONTROL_INTERVAL 5000    // Check light logic every 5 seconds
#define ESP_RECEIVE_TIMEOUT 100        // Maximum time to spend in ESP receive function

// System safety parameters
#define PUMP_MIN_RUN_TIME 3000         // Minimum pump run time (3 seconds)
#define PUMP_COOLDOWN_TIME 120000      // Cooldown between auto activations (2 minutes)

// Moisture thresholds with wider hysteresis
#define SOIL_VERY_DRY_THRESHOLD 15     // Turn on pump when moisture below 15% (was 20%)
#define SOIL_VERY_WET_THRESHOLD 85     // Turn off pump when moisture above 85% (was 80%)

// CORRECTED light level thresholds for fixed sensor
// Now that the sensor is correctly giving 0 for dark and 100 for bright:
#define LIGHT_DARK_THRESHOLD 25        // Turn on lights when below 25% brightness (room is dark)
#define LIGHT_BRIGHT_THRESHOLD 75      // Turn off lights when above 75% brightness (room is bright)

// Temperature thresholds for fan
#define TEMP_HIGH_THRESHOLD 30.0       // Turn on fan when above 30°C
#define TEMP_LOW_THRESHOLD 25.0        // Turn off fan when below 25°C

// Relay-specific parameters for stable operation
#define RELAY_SETTLE_TIME 20      // ms to wait after toggling relay state
#define RELAY_RETRIGGER_DELAY 50  // ms to wait before checking relay state again

// Updated timing parameters for better pump stability
#define PUMP_DEBOUNCE_TIME 10000        // Minimum 10 seconds between pump relay state changes
#define PUMP_SAFETY_TIMEOUT 20000        // Max 20 seconds pump on time for safety
#define PUMP_MIN_RUN_TIME 10000         // Minimum 10 seconds pump run time once started
#define PUMP_COOLDOWN_TIME 180000       // 3 minutes cooldown between auto activations
#define PUMP_MAINTENANCE_INTERVAL 1800000 // Run pump at least every 30 minutes if very dry

// Extremely wide moisture thresholds to prevent oscillation
#define SOIL_VERY_DRY_THRESHOLD 15     // Turn on pump when moisture below 15%
#define SOIL_VERY_WET_THRESHOLD 90     // Turn off pump when moisture above 90%

#endif // CONFIG_H