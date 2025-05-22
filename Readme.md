# Advanced Plant Care Monitoring System

## Table of Contents
1. [Overview](#overview)
2. [System Architecture](#system-architecture)
3. [Technical Stack](#technical-stack)
4. [Web Interface](#web-interface)
5. [Features](#features)
6. [Installation and Setup](#installation-and-setup)
7. [Usage Guidelines](#usage-guidelines)
8. [Integration Guide](#integration-guide)
9. [Maintenance](#maintenance)
10. [Troubleshooting](#troubleshooting)
11. [Safety Considerations](#safety-considerations)
12. [Future Enhancements](#future-enhancements)
13. [Contributing](#contributing)
14. [License](#license)
15. [Acknowledgments](#acknowledgments)
16. [Contact](#contact)

## Overview
The Advanced Plant Care Monitoring System is an intelligent IoT-based solution designed to automate and optimize plant care through real-time monitoring and automated control of environmental conditions. This system combines various sensors, actuators, and a user interface to create a comprehensive plant care ecosystem that ensures optimal growing conditions for plants.

## System Architecture

### Hardware Components
1. **Microcontroller**
   - Arduino Mega 2560 as the main controller
     - Clock Speed: 16 MHz
     - Flash Memory: 256 KB
     - SRAM: 8 KB
     - EEPROM: 4 KB
   - ESP8266/ESP32 for WiFi connectivity and web interface
     - WiFi: 802.11 b/g/n
     - Dual-core processor (ESP32)
     - Built-in TCP/IP stack

2. **Sensors**
   - DHT22 Temperature and Humidity Sensor
     - Temperature Range: -40°C to 80°C
     - Humidity Range: 0-100%
     - Accuracy: ±0.5°C, ±2% RH
   - Digital Light Sensor
   - Digital Rain Sensor
   - Digital Soil Moisture Sensor

3. **Actuators**
   - Water Pump with Relay Control
   - LED Grow Lights (NeoPixel Strip)
   - Cooling Fan
   - Relay Modules

4. **Display**
   - Integrated display for local monitoring

### Software Components
1. **Arduino Mega Firmware**
   - Core Libraries:
     - `Arduino.h` (Core Arduino library)
     - `avr/wdt.h` (Watchdog Timer)
   - Custom Libraries:
     - `config.h` (System configuration)
     - `sensors.h/cpp` (Sensor management)
     - `actuators.h/cpp` (Actuator control)
     - `display.h/cpp` (Display interface)
     - `communication.h/cpp` (ESP communication)

2. **ESP Module Firmware**
   - Libraries:
     - `ESP8266WiFi` or `WiFi.h` (ESP32)
     - `ArduinoJson` (JSON handling)
     - `WebServer` (HTTP server)

## Technical Stack

### 1. Hardware Stack
- **Main Controller**: Arduino Mega 2560
- **Communication Module**: ESP8266/ESP32
- **Sensors**: DHT22, Light, Rain, Soil Moisture
- **Actuators**: Water Pump, LED Strip, Fan, Relays

### 2. Software Stack
- **Arduino Mega Firmware**
  - Core Arduino libraries
  - Custom system libraries
  - Watchdog timer implementation
- **ESP Module Firmware**
  - WiFi connectivity
  - Web server
  - JSON processing
- **Communication Protocol**
  - Serial Communication (UART)
  - Baud Rate: 115200
  - JSON-based data exchange
  - HTTP/HTTPS for web interface

## Web Interface

### 1. Web Server Features
- Built-in HTTP server on ESP module
- Real-time data monitoring
- Remote control interface
- Configuration management

### 2. Available Endpoints
1. **Monitoring Endpoints**
   ```
   GET /api/sensors
   - Returns current sensor readings
   - Includes temperature, humidity, light, moisture, rain status
   ```

2. **Control Endpoints**
   ```
   POST /api/actuators/pump
   - Control water pump
   - Parameters: mode (auto/manual), state (on/off)

   POST /api/actuators/lights
   - Control LED lights
   - Parameters: mode, brightness, state

   POST /api/actuators/fan
   - Control cooling fan
   - Parameters: mode, state
   ```

3. **Configuration Endpoints**
   ```
   GET /api/config
   - Retrieve current system configuration

   POST /api/config
   - Update system parameters
   - Includes thresholds, timing parameters
   ```

### 3. Integration Possibilities
1. **Home Automation Systems**
   - Home Assistant
   - OpenHAB
   - Domoticz
   - Through MQTT protocol

2. **Cloud Platforms**
   - AWS IoT
   - Google Cloud IoT
   - Azure IoT Hub
   - Through REST API

3. **Mobile Applications**
   - Custom mobile apps
   - Through REST API
   - Real-time updates via WebSocket

### 4. Data Flow
```
[Arduino Mega] <--Serial--> [ESP Module] <--WiFi--> [Web Interface]
     |                           |                        |
  Sensors/                    Web Server              User Interface
Actuators                    Data Processing         Remote Control
```

### 5. Security Features
- WiFi authentication
- Basic HTTP authentication
- Secure communication (HTTPS)
- API key authentication

## Features

### Environmental Monitoring
- Real-time temperature monitoring
- Humidity level tracking
- Light intensity measurement
- Soil moisture detection
- Rain detection

### Automated Control Systems
1. **Watering System**
   - Automatic watering based on soil moisture
   - Configurable moisture thresholds
   - Safety timeouts and cooldown periods
   - Manual override capability

2. **Lighting System**
   - Automatic LED control based on ambient light
   - Configurable brightness levels
   - Multiple operation modes (Auto/Manual)
   - Energy-efficient operation

3. **Temperature Control**
   - Automatic fan control
   - Temperature threshold-based activation
   - Hysteresis control to prevent rapid cycling

### Safety Features
- Pump safety timeout (20 seconds maximum)
- Cooldown periods between operations
- Debounce protection for relays
- System status monitoring
- Error detection and reporting

## Technical Specifications

### Pin Configuration
- Light Sensor: Digital Pin 24
- Rain Sensor: Digital Pin 25
- Soil Moisture: Digital Pin 50
- DHT Sensor: Digital Pin 22
- Water Pump: Digital Pin 30
- LED Lights: Digital Pin 26
- Fan Control: Digital Pin 31
- ESP Communication: Serial1 (Pins 18/19)

### Timing Parameters
- Sensor Reading Interval: 2 seconds
- Display Update Interval: 0.5 seconds
- Pump Control Interval: 5 seconds
- Fan Control Interval: 10 seconds
- ESP Communication Interval: 10 seconds

### Thresholds
- Soil Moisture:
  - Very Dry: 15%
  - Very Wet: 90%
- Temperature:
  - High: 30°C
  - Low: 25°C
- Light:
  - Dark: 25%
  - Bright: 75%

## Installation and Setup

### Hardware Setup
1. Connect all sensors to their respective pins as defined in config.h
2. Connect actuators (pump, lights, fan) to their designated pins
3. Connect the ESP module to Serial1 (pins 18/19)
4. Power the system using a stable 5V power supply

### Software Setup
1. Install required libraries:
   - DHT sensor library
   - Adafruit NeoPixel
   - ESP8266WiFi (for ESP module)
   - ArduinoJson

2. Configure the system:
   - Set WiFi credentials in ESP firmware
   - Adjust thresholds in config.h if needed
   - Upload firmware to both Arduino Mega and ESP module

## Usage Guidelines

### Local Operation
1. Power on the system
2. Monitor the local display for current readings
3. Use physical buttons (if available) for manual control
4. Observe system behavior and adjust thresholds if needed

### Remote Operation
1. Connect to the system's WiFi network
2. Access the web interface using the ESP's IP address
3. Monitor real-time sensor data
4. Control actuators through the web interface
5. Configure system parameters remotely

## Integration Guide

### 1. Home Assistant Integration
1. Install MQTT broker
2. Configure ESP module for MQTT
3. Add MQTT sensors and switches in Home Assistant
4. Configure automations

### 2. Cloud Platform Integration
1. Set up cloud IoT platform
2. Configure device authentication
3. Set up data pipelines
4. Configure alerts and notifications

### 3. Mobile App Integration
1. Set up development environment
2. Implement REST API client
3. Add real-time updates
4. Implement user authentication

## Maintenance

### Regular Maintenance
1. Check sensor readings weekly
2. Clean sensors monthly
3. Inspect water pump and tubing
4. Clean fan and air filters
5. Check LED operation

### Troubleshooting
1. If sensors show incorrect readings:
   - Check connections
   - Clean sensor surfaces
   - Verify power supply
2. If actuators don't respond:
   - Check relay operation
   - Verify power supply
   - Check control signals
3. If ESP connection fails:
   - Verify WiFi credentials
   - Check power supply
   - Reset ESP module

## Safety Considerations
1. Always use a ground-fault circuit interrupter (GFCI) for water pump
2. Keep electrical components away from water
3. Use appropriate wire gauges for current requirements
4. Implement proper grounding
5. Follow local electrical codes

## Future Enhancements
1. Mobile app integration
2. Cloud data storage
3. Machine learning for optimization
4. Additional sensor types
5. Advanced scheduling features
6. Energy consumption monitoring
7. Plant growth tracking
8. Weather integration

## Contributing
Contributions are welcome! Please feel free to submit pull requests, report issues, or suggest improvements.

## License
This project is licensed under the MIT License - see the LICENSE file for details.

## Acknowledgments
- Arduino community
- ESP8266 community
- Open-source sensor libraries
- Contributors and testers

## Contact
For support or inquiries, please open an issue in the repository or contact the maintainers. 
