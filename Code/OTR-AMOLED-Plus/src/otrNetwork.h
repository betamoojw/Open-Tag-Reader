// OPEN TAG READER
// Lilygo T-Display-S3 AMOELD PLUS
// C.Chad 2025

#ifndef OTR_NETWORK_H
#define OTR_NETWORK_H

#include <Arduino.h>
#include <lvgl.h>
#include "ui/ui.h"
#include "WiFi.h"
#include "otrUI.h"
#include "otrFileHandling.h"
#include "otrWebInterface.h"


struct StationInfo {
    uint8_t mac[6];
    uint8_t ip[4];
    uint16_t rxBytes;
    uint16_t txBytes;
    int rssi;  
};

void createAccessPoint(void);
bool isValidIPAvailable();



#endif