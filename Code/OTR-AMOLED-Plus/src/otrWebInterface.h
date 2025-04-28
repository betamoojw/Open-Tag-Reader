// OPEN TAG READER
// Lilygo T-Display-S3 AMOELD PLUS
// C.Chad 2025

#ifndef OTR_WEBINTERFACE_H
#define OTR_WEBINTERFACE_H

#include <Arduino.h>
#include <lvgl.h>
#include "ui/ui.h"
#include "otrPowerManagement.h"
#include "otrTime.h"
#include "otrNetwork.h"
#include "otrUI.h"
#include "ESPAsyncWebServer.h"

String testToHtml(void);
void sendWebpage(AsyncWebServerRequest *request, const String& htmlContent);
void setupWebRoutes();
void startWebServer(void);
#endif