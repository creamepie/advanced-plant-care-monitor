#ifndef LED_PATTERNS_H
#define LED_PATTERNS_H

/*
 * LED Indicator Patterns
 * 
 * This file documents the LED patterns used for visual status indication
 * 
 * D0 LED (RX_LED_PIN) - Indicates data received from Arduino
 * D2 LED (TX_LED_PIN) - Indicates data sent to Arduino
 * 
 * Pattern Meanings:
 * 
 * 1. Slow Blink (1Hz)
 *    - System is starting up
 *    - Not connected to Arduino
 * 
 * 2. Fast Blink (5Hz)
 *    - Data is being transmitted (TX) or received (RX)
 *    - Temporary indicator of active communication
 * 
 * 3. Double Pulse
 *    - Successfully connected to Arduino
 *    - Successful message parsing
 *    - Periodic heartbeat when system is running normally
 * 
 * 4. Solid ON
 *    - Data is being actively sent or received
 *    - Brief indicator for actual data transmission
 * 
 * Both LEDs off:
 *    - No active communication
 *    - System is in standby mode
 * 
 * Both LEDs slow blinking:
 *    - System lost connection to Arduino
 *    - Attempting to reconnect
 */

#endif
