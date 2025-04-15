#include "webserver.h"
#include "html_content.h" // Contains the main HTML page
#include "communication.h"

// Include FS for serving files (if icons/manifest are stored in SPIFFS)
// #include <FS.h>

// Placeholder for manifest content if not using SPIFFS
const char MANIFEST_JSON[] PROGMEM = R"rawliteral(
{
  "name": "Plant Care",
  "short_name": "PlantCare",
  "description": "ESP8266 Plant Care Dashboard",
  "start_url": "/",
  "display": "standalone",
  "background_color": "#1e1e1e",
  "theme_color": "#66bb6a",
  "icons": [
    {
      "src": "/icon-192.png",
      "sizes": "192x192",
      "type": "image/png"
    },
    {
      "src": "/icon-512.png",
      "sizes": "512x512",
      "type": "image/png"
    }
  ]
}
)rawliteral";

// Placeholder for icon data (replace with actual byte arrays or use SPIFFS)
// Example: const unsigned char icon_192_png[] PROGMEM = { ... byte data ... };
// Example: const size_t icon_192_png_len = sizeof(icon_192_png);

void handleRoot() {
  server.send(200, "text/html", HTML_CONTENT);
}

void handleApiData() {
  // Create JSON object with sensor data - removed try-catch
  StaticJsonDocument<512> doc;
  
  // Copy values from globals to prevent race conditions
  int currentLight = light;
  int currentSoil = soil;
  int currentRain = rain;
  float currentTemp = temperature;
  float currentHumidity = humidity;
  
  // Validate values before adding to response
  doc["light"] = constrain(currentLight, 0, 100);
  doc["soil"] = constrain(currentSoil, 0, 100);
  doc["rain"] = currentRain ? 1 : 0;
  doc["temperature"] = isnan(currentTemp) ? 0 : currentTemp;
  doc["humidity"] = isnan(currentHumidity) ? 0 : currentHumidity;
  
  // Add status information
  doc["pumpActive"] = pumpActive;
  doc["lightMode"] = lightMode;
  doc["connected"] = connected;
  doc["lastUpdate"] = (millis() - lastDataReceived) / 1000;
  doc["pumpMode"] = pumpMode;
  doc["fanActive"] = fanActive;
  doc["fanMode"] = fanMode;
  
  // Add system info
  doc["uptime"] = millis() / 1000;
  doc["freeHeap"] = ESP.getFreeHeap();
  
  String jsonResponse;
  serializeJson(doc, jsonResponse);
  
  // Set proper headers to prevent caching issues
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "application/json", jsonResponse);
}

void handleApiControl() {
  String device = server.arg("device");
  String action = server.arg("action");
  
  // Validate input
  if (device.length() == 0 || action.length() == 0) {
    server.send(400, "text/plain", "Missing device or action parameter");
    return;
  }
  
  Serial.print("Control request: ");
  Serial.print(device);
  Serial.print(" -> ");
  Serial.println(action);
  
  bool validCommand = false;
  
  // Process pump controls
  if (device == "pump") {
    if (action == "off") {
      pumpActive = false;
      pumpMode = 0;
      sendCommand("PUMP:MODE:0");
      validCommand = true;
    } 
    else if (action == "on") {
      pumpActive = true;
      pumpMode = 1;
      sendCommand("PUMP:MODE:1");
      validCommand = true;
    } 
    else if (action == "auto") {
      pumpMode = 2;
      sendCommand("PUMP:MODE:2");
      validCommand = true;
    }
  }
  
  // Process fan controls  
  else if (device == "fan") {
    if (action == "off") {
      fanActive = false;
      fanMode = 0;
      sendCommand("FAN:MODE:0");
      validCommand = true;
    } else if (action == "on") {
      fanActive = true;
      fanMode = 1;
      sendCommand("FAN:MODE:1");
      validCommand = true;
    } else if (action == "auto") {
      fanMode = 2;
      sendCommand("FAN:MODE:2");
      validCommand = true;
    }
  }
  
  // Process light controls
  else if (device == "light") {
    if (action == "off") {
      lightMode = 0;
      sendCommand("LIGHT:MODE:0");
      validCommand = true;
    } else if (action == "on") {
      lightMode = 1;
      sendCommand("LIGHT:MODE:1");
      validCommand = true;
    } else if (action == "auto") {
      lightMode = 2;
      sendCommand("LIGHT:MODE:2");
      validCommand = true;
    }
  }
  
  // Return appropriate status
  if (validCommand) {
    server.send(200, "text/plain", "OK");
  } else {
    server.send(400, "text/plain", "Invalid command");
  }
}

void handleManifest() {
  server.send(200, "application/manifest+json", MANIFEST_JSON);
}

void handleIcon192() {
  // REMINDER: Implement icon serving!
  // Choose ONE of the options below and delete the others.
  // You MUST create the actual icon-192.png file.

  // Option 1: Serve from PROGMEM (Convert PNG to byte array first)
  // const unsigned char icon_192_png[] PROGMEM = { /* byte data */ };
  // const size_t icon_192_png_len = sizeof(icon_192_png);
  // server.send_P(200, "image/png", (const char*)icon_192_png, icon_192_png_len);

  // Option 2: Serve from SPIFFS (Upload icon-192.png to SPIFFS first)
  // #include <FS.h> // Make sure FS is included at the top
  // File iconFile = SPIFFS.open("/icon-192.png", "r");
  // if (iconFile) {
  //   server.streamFile(iconFile, "image/png");
  //   iconFile.close();
  // } else {
  //   server.send(404, "text/plain", "Icon 192 not found in SPIFFS");
  // }

  // Option 3: Placeholder 404 (Default - shows error until implemented)
   server.send(404, "text/plain", "Icon 192 not found - Implement serving in webserver.cpp");
}

void handleIcon512() {
  // REMINDER: Implement icon serving!
  // Choose ONE of the options below and delete the others.
  // You MUST create the actual icon-512.png file.

  // Option 1: Serve from PROGMEM (Convert PNG to byte array first)
  // const unsigned char icon_512_png[] PROGMEM = { /* byte data */ };
  // const size_t icon_512_png_len = sizeof(icon_512_png);
  // server.send_P(200, "image/png", (const char*)icon_512_png, icon_512_png_len);

  // Option 2: Serve from SPIFFS (Upload icon-512.png to SPIFFS first)
  // #include <FS.h> // Make sure FS is included at the top
  // File iconFile = SPIFFS.open("/icon-512.png", "r");
  // if (iconFile) {
  //   server.streamFile(iconFile, "image/png");
  //   iconFile.close();
  // } else {
  //   server.send(404, "text/plain", "Icon 512 not found in SPIFFS");
  // }

  // Option 3: Placeholder 404 (Default - shows error until implemented)
   server.send(404, "text/plain", "Icon 512 not found - Implement serving in webserver.cpp");
}

void handleNotFound() {
  server.send(404, "text/plain", "Not Found");
}

void setupWebServer() {
  // Option: Initialize SPIFFS if serving files from it
  // if(!SPIFFS.begin()){
  //   Serial.println("An Error has occurred while mounting SPIFFS");
  //   return;
  // }

  server.on("/", HTTP_GET, handleRoot);
  server.on("/api/data", HTTP_GET, handleApiData);
  server.on("/api/control", HTTP_POST, handleApiControl);

  // Add routes for manifest and icons
  server.on("/manifest.json", HTTP_GET, handleManifest);
  server.on("/icon-192.png", HTTP_GET, handleIcon192);
  server.on("/icon-512.png", HTTP_GET, handleIcon512);

  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("Web server started - http://192.168.4.1");
}