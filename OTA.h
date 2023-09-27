#ifndef _OTA_H
#define _OTA_H

#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <Update.h>

extern const char* host;

extern WebServer server;

extern const char* loginIndex;
extern const char* serverIndex;

void OTASetup();
void OTALoop();

#endif