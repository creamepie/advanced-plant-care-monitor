# Advanced Plant Care Monitor

A sophisticated plant monitoring and automation system that combines multiple sensors and actuators to create an optimal growing environment for plants. This system provides real-time monitoring, automated care, and a user-friendly interface for plant management.

## System Overview

The Advanced Plant Care Monitor is a comprehensive solution that integrates various sensors and actuators to create an intelligent plant care system. It monitors environmental conditions, provides automated care, and offers real-time feedback through both a local display and a web interface.

### Key Features

- Real-time monitoring of multiple environmental parameters
- Automated plant care based on sensor readings
- Beautiful LED lighting system with multiple modes
- Smart water management with safety features
- Temperature and humidity control
- Rain detection
- Local touch display interface
- Web-based dashboard for remote monitoring
- Robust error handling and safety features

## Hardware Components

### Sensors

1. **Light Sensor (Digital)**
   - Pin: 24
   - Type: Digital light sensor
   - Function: Measures ambient light levels
   - Output: Percentage (0-100%)
   - Used for: Automatic light control and monitoring

2. **Soil Moisture Sensor (Digital)**
   - Pin: 50
   - Type: Digital moisture sensor
   - Function: Measures soil moisture content
   - Output: Binary state (Dry/Wet)
   - Used for: Automatic watering control

3. **Rain Sensor**
   - Pin: 25
   - Type: Digital rain sensor
   - Function: Detects presence of rain
   - Output: Boolean (Rain detected/No rain)
   - Used for: Preventing overwatering during rain

4. **Temperature and Humidity Sensor (DHT22)**
   - Pin: 22
   - Type: DHT22 (AM2302)
   - Function: Measures ambient temperature and humidity
   - Output: 
     - Temperature: -40°C to 80°C
     - Humidity: 0-100%
   - Used for: Environmental monitoring and fan control

### Actuators

1. **LED Grow Light System**
   - Pin: 26
   - Type: WS2812 RGB LED strip
   - Configuration: 24 LEDs
   - Features:
     - Multiple color modes (Grow, Bloom, Seedling, Night)
     - Smooth transitions and animations
     - Automatic brightness control
     - Power-saving features

2. **Water Pump**
   - Pin: 30
   - Type: Relay-controlled water pump
   - Features:
     - Safety timeout (10 seconds maximum)
     - Debounce protection
     - Multiple operation modes (Auto/Manual)
     - Ground-triggered relay

3. **Fan System**
   - Pin: 31
   - Type: Relay-controlled fan
   - Features:
     - Temperature-based control
     - Multiple operation modes
     - Automatic humidity control

### Display System

- Type: MCUFRIEND TFT Display with Touch
- Features:
  - Real-time sensor readings
  - Control buttons for all systems
  - Status indicators
  - Modern UI with cards and progress bars

## Software Architecture

### Core Components

1. **Sensor Management**
   - Continuous monitoring of all sensors
   - Data smoothing and validation
   - Error detection and handling
   - Calibration support

2. **Actuator Control**
   - Smart automation based on sensor readings
   - Safety features and timeouts
   - Multiple operation modes
   - Smooth transitions and animations

3. **Display Interface**
   - Real-time updates
   - Touch controls
   - Status indicators
   - Modern UI design

4. **Communication System**
   - ESP8266 integration
   - Web dashboard
   - JSON data transmission
   - Command processing

### Operation Modes

1. **Light System Modes**
   - OFF: Complete shutdown
   - ON: Manual control
   - AUTO: Sensor-based control
     - Activates when light < 30%
     - Deactivates when light > 70%

2. **Pump System Modes**
   - OFF: Complete shutdown
   - ON: Manual control
   - AUTO: Sensor-based control
     - Activates when moisture < 50%
     - Deactivates when moisture > 50%
     - Safety timeout: 10 seconds

3. **Fan System Modes**
   - OFF: Complete shutdown
   - ON: Manual control
   - AUTO: Temperature-based control
     - Activates at high temperature
     - Deactivates at low temperature

## Safety Features

1. **Pump Safety**
   - Maximum runtime limit
   - Debounce protection
   - Rain detection integration
   - Dry-run protection

2. **System Protection**
   - Watchdog timer (8-second timeout)
   - Error detection and recovery
   - Safe mode operation
   - Automatic error logging

3. **Power Management**
   - Efficient LED control
   - Power-saving modes
   - Safe shutdown procedures

## Web Interface

The system includes a web-based dashboard accessible through the ESP8266 module, providing:

- Real-time sensor readings
- System status monitoring
- Remote control capabilities
- Historical data viewing
- System configuration options

## Installation and Setup

1. **Hardware Setup**
   - Connect sensors to specified pins
   - Install actuators as per pin configuration
   - Connect display system
   - Set up ESP8266 communication

2. **Software Installation**
   - Upload Arduino code to main controller
   - Configure ESP8266 with web interface
   - Set up initial parameters
   - Run calibration if needed

3. **Configuration**
   - Adjust sensor thresholds
   - Set up automation parameters
   - Configure display preferences
   - Set up web interface

## Maintenance

1. **Regular Checks**
   - Sensor calibration
   - System status verification
   - Error log review
   - Performance monitoring

2. **Troubleshooting**
   - Sensor validation
   - Actuator testing
   - Communication verification
   - Error recovery procedures

## Technical Specifications

- **Controller**: Arduino Mega
- **Display**: MCUFRIEND TFT with Touch
- **Communication**: ESP8266
- **Power Supply**: 5V DC
- **Operating Temperature**: -40°C to 80°C
- **Humidity Range**: 0-100%
- **Update Frequency**: 1 second (sensors)
- **Web Update Rate**: 5 seconds

## Future Enhancements

1. **Planned Features**
   - Mobile app integration
   - Advanced analytics
   - Machine learning optimization
   - Multi-zone support

2. **Potential Improvements**
   - Additional sensor types
   - Enhanced automation
   - Extended communication options
   - Advanced UI features

## Support and Documentation

For additional support and documentation:
- Check the project repository
- Review the code documentation


## License

This project is licensed under the MIT License - see the LICENSE file for details. 
