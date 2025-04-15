#ifndef ARDUINO_COMPAT_H
#define ARDUINO_COMPAT_H

/*
 * Arduino Compatibility Reference
 * ------------------------------
 * This file documents how the ESP8266 communication matches the Arduino implementation.
 * 
 * PROTOCOL NOTES:
 * 1. Arduino uses Serial1 at 115200 baud for ESP communication
 * 2. Messages are framed with START_MARKER '<' and END_MARKER '>'
 * 3. Arduino sends data in the format: <DATA:{"key":value,...}>
 * 4. Arduino sends JSON with shortened keys to save space (e.g., "temp" not "temperature")
 * 5. Arduino expects ESP commands in the format: <COMMAND:VALUE>
 * 6. Arduino has a 128-byte buffer for receiving messages
 * 7. Arduino sends smaller chunks with delays to prevent buffer overflow
 */

/*
 * ARDUINO JSON FORMAT:
 * 
 * {
 *   "light": 75,            // Light level percentage (0-100)
 *   "moisture": 45,         // Soil moisture percentage (0-100)
 *   "rain": 0,              // Rain detection (0 = no rain, 1 = rain detected)
 *   "temp": "23.5",         // Temperature in Celsius as string with 1 decimal place
 *   "hum": "65.0",          // Humidity percentage as string with 1 decimal place
 *   "pump": 0,              // Pump state (0 = off, 1 = on)
 *   "pMode": 2              // Pump mode (0 = off, 1 = on, 2 = auto)
 * }
 * 
 * The Arduino may also include optional fields for:
 * - Fan state and mode
 * - Light state and mode
 */

/*
 * COMMANDS SUPPORTED BY ARDUINO:
 * 
 * 1. <PUMP:MODE:0>  - Turn pump off
 * 2. <PUMP:MODE:1>  - Turn pump on
 * 3. <PUMP:MODE:2>  - Set pump to auto mode
 * 4. <LIGHT:MODE:0> - Turn light off
 * 5. <LIGHT:MODE:1> - Turn light on
 * 6. <LIGHT:MODE:2> - Set light to auto mode
 * 7. <FAN:MODE:0>   - Turn fan off
 * 8. <FAN:MODE:1>   - Turn fan on
 * 9. <FAN:MODE:2>   - Set fan to auto mode
 * 10. <PING>        - Request a PONG response
 * 11. <RESET>       - Reset the communication
 * 
 * The Arduino will respond with:
 * - <PONG> for PING requests
 * - <ACK:COMMAND> for successful commands
 */

/*
 * ESP8266 COMPATIBILITY REQUIREMENTS:
 * 
 * 1. Use 115200 baud rate for communication with Arduino
 * 2. Use SoftwareSerial to communicate with Arduino
 * 3. Send smaller commands to avoid overflowing Arduino's 128-byte buffer
 * 4. Include START_MARKER '<' and END_MARKER '>' on all messages
 * 5. Expect JSON data to use shortened keys (e.g., "temp" not "temperature")
 * 6. Respond to PING requests with PONG
 * 7. Handle communication timeouts with reconnection logic
 */

#endif
