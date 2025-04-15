#ifndef WEBSERVER_H
#define WEBSERVER_H

#include <Arduino.h>
#include <ESP8266WebServer.h>

// Function declarations
void setupWebServer();
void handleRoot();
void handleApiData();
void handleApiControl();
void handleStyles();
void handleScript();

#endif