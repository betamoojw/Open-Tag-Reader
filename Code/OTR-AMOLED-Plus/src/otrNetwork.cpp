// OPEN TAG READER
// Lilygo T-Display-S3 AMOELD PLUS
// C.Chad 2025

#include "otrNetwork.h"
#include "LilyGo_AMOLED.h"
#include "WiFi.h"


const char *ssid = "OpenTagReader";
const char *password = "opentagreader";


void createAccessPoint(void) {
    // Create the access point
    WiFi.softAP(ssid, password);
    IPAddress ip = WiFi.softAPIP();
    String ipAddressStr = ip.toString();
    updateIPAddress(ipAddressStr);
    #ifdef OTR_DEBUG
        Serial.println("Access point created");
        Serial.print("IP address: "); Serial.println(WiFi.softAPIP());
    #endif
}


bool isValidIPAvailable() {
    return WiFi.softAPIP() != INADDR_NONE;
}

int checkForConnectionsToAccessPoint(void) {
    if (!isValidIPAvailable()) {
        return 0;
    } else {
        int numConnections = WiFi.softAPgetStationNum();
        if (numConnections > 0) {
            #ifdef OTR_DEBUG
                Serial.print("Number of connections: "); 
                Serial.println(numConnections);
            #endif
        
            return numConnections;
        }
        return 0;
    }
}