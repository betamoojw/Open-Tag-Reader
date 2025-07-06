#include "otrScanning.h"

char RFID[17];

RFIDReader rfidreader;

WL_134A_RFID rfid;
    
void RFIDReader::begin(void) {
    if (!readerSerial) {
        //Serial already active
    } else {
        Serial1.begin(config.baud, SERIAL_8N1, config.RXD1, config.TXD1);
        Serial1.setTimeout(config.timeout);
    }
    pinMode(config.RFIDPowerPin, OUTPUT); 
    activate();
}

String RFIDReader::monitorSerial(void) {
    String readRFID;
    const char startByte = '9';
    const int timeout = 100;
    if (readerBoard) {
        //WL134A
        if (readerSerial) {
            if (Serial1.available()) {
                if (Serial1.available() >= rfidMessageLength) {
                    rfid.readPacket();
                    WL_134A_RFID::rfidRead readRFID = rfid.getReading();
                }
            }
        } else {
            if (Serial.available()) {
                if (Serial.available() >= rfidMessageLength) {
                    rfid.readPacket();
                    WL_134A_RFID::rfidRead readRFID = rfid.getReading();
                }
            }
        }
        return readRFID;

    } else {
        //Priority 1 board - Received as ASCII text
        if (readerSerial) {
            if (Serial1.available()) {
                readRFID = Serial1.readString(); //read until timeout
                readRFID.replace('_', ' ');
                readRFID.trim();
                readRFID.toCharArray(RFID, 16);
                Serial.println(RFID);
            }
        } else {
            if (Serial.available()) {
                if(Serial.peek() == startByte) {
                    readRFID = Serial.readString(); //read until timeout
                    readRFID.replace('_', ' ');
                    readRFID.trim();
                    readRFID.toCharArray(RFID, 16);
                    #ifdef OTR_DEBUG
                        Serial.print("RFIDREADER:monitorSerial - Serial Received ");
                        Serial.println(RFID);
                    #endif
                } else {
                    String temp = Serial.readString();
                }

            }
        }
        return readRFID;
    }
    return "";
}
   
bool RFIDReader::validateRFID(const char* RFID) {
    // RFID is 16 characters long
    // 1st 3 digits are tag companyeg 982 = Allflex, 940 = Shearwell
    // 4th character is a space
    scanError = "";

    if (strlen(RFID) != 16) { 
        if (strlen(RFID)>16) {
            scanError = "Too long";
        } else {
            scanError = "Too short";
        }
        #ifdef OTR_DEBUG
            Serial.println("RFIDREADER:validateRFID -RFID is not 16 characters long");
            Serial.print("Length: ");
            Serial.println(strlen(RFID));
            Serial.println(scanError);  
        #endif
        return false;
    }
    if (strncmp(RFID + 3, " ", 1) != 0) {
        
        scanError = "Invalid seperator";
        #ifdef OTR_DEBUG
            Serial.println("RFIDReader:validateRFID - RFID first three digits not separated by a space");
            Serial.println(scanError);
        #endif
        return false;
    }
    // Check for duplicates recent scans
    bool duplicate = false;
    int scanIndex = 10;
    for (int i = 0; i < scanIndex; i++) {
        if (previousScans[i] == RFID) {
            duplicate = true;
            break;
        }
    }
    if (duplicate) {
        scanError = "Duplicate";
        #ifdef OTR_DEBUG
            Serial.println("RFIDReader:validateRFID - RFID is a duplicate");
            Serial.println(scanError);
        #endif
        
        return false;
    }
    for (int i = scanIndex - 1; i > 0; i--) {
        previousScans[i] = previousScans[i-1];
    }
    previousScans[0] = RFID;
    return true;
}

void WL_134A_RFID:: readPacket()   {
    
    char packet[rfid_packet_size];
    if (readerSerial) {
        packet[rfid_packet_startcode] = Serial1.read();
    } else {
        packet[rfid_packet_startcode] = Serial.read();
    }
    // Packet should be transmitted as follows:
    // LSB                                                                                      MSB
    // ____________________________________________________________________________________________
    // | Start |  Tag | Country | Data | Animal | Reserved | Reserved | Checksum | ~Checksum | End |
    // | Code  |  ID  |   ID    | Flag |  Flag  |          |          |          |           | Code| 
    // | 1byte |10byte|  4byte  | 1byte|  1byte |   4byte  |   6byte  |   1byte  |   1byte   |1byte|
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // 0       1      11        15     16       17         21         27         28          29    30
    
    if (packet[rfid_packet_startcode] != 0x02) {
        //not the start
        return;
    }
    uint8_t read;
    if (readerSerial) {
        read = Serial1.readBytes(&(packet[rfid_packet_ID]), rfid_packet_size - 1);
    } else {
        read = Serial.readBytes(&(packet[rfid_packet_ID]), rfid_packet_size - 1);
    }

    if (read != rfid_packet_size - 1) {
        //wrong length
        #ifdef OTR_DEBUG
            Serial.println("Wrong length");
        #endif
        return;
    }
    //calculate checksum
    uint8_t checksum = 0;
    for (int i = rfid_packet_ID; i < rfid_packet_checksum; i++) {
        checksum = checksum ^ packet[i];
    }

    if (checksum != packet[rfid_packet_checksum]) {
        //wrong checksum
        #ifdef OTR_DEBUG
            Serial.println("Wrong checksum");
        #endif
        return;
    }
    if (static_cast<uint8_t>(~checksum) != packet[rfid_packet_checksum_inverted]) {
        //wrong checksum
        #ifdef OTR_DEBUG
            Serial.println("Wrong checksum inverted");
        #endif
        return;
    }
    if (packet[rfid_packet_endcode] != 0x07) {
        // end code missmatch
        if(packet[rfid_packet_endcode] != 0x03) {
        //FDX tag
            #ifdef OTR_DEBUG    
                Serial.println("FDX tag detected");
            #endif
            return;
        }  
        #ifdef OTR_DEBUG
            Serial.println("Wrong endcode");
        #endif
        return;
    }
    //tagID and countryID are low data first.
    //coverting to decimal requires shifting left by 4 bits
    //start with tagID
    reading.tagID = 0;
    reading.countryID = 0;
    for (int i =rfid_packet_countryID-1; i >= rfid_packet_ID; --i)
    {
        //each time we get to a new character the current
        //value is shifted left by 4 bits (multiplied by 16)
        reading.tagID = reading.tagID << 4;
        //then we convert the current character to a number and add it to the current value
        reading.tagID += hex_to_uint64(packet[i]);
    }
    //same for countryID
    for (int i = rfid_packet_data-1; i >= rfid_packet_countryID; --i)
    {
        reading.countryID = reading.countryID << 4;
        reading.countryID += hex_to_uint64(packet[i]);
    }
    reading.isAnimal = hex_to_uint64(packet[rfid_packet_animal]);
    snprintf(reading.rfidID, sizeof(reading.rfidID), "%03u %012llu", reading.countryID, reading.tagID);
    #ifdef OTR_DEBUG
        Serial.print("Tag ID: ");
        Serial.println(reading.tagID);
        Serial.print("Country ID: ");
        Serial.println(reading.countryID);
        Serial.print("Is Animal: ");
        Serial.println(reading.isAnimal);
        Serial.print("RFID ID: ");
        Serial.println(reading.rfidID);
    #endif
    return;
}

uint64_t WL_134A_RFID:: hex_to_uint64(const char c) {
    if (c >= '0' && c <= '9') {
        return c - '0';
    }
    if (c >= 'a' && c <= 'f') {
        return c - 'a' + 10;
    }
    if (c >= 'A' && c <= 'F') {
        return c - 'A' + 10;
    }
    return 0;
}

WL_134A_RFID::rfidRead WL_134A_RFID:: getReading() const {
    return reading;
}

void RFIDReader::activate() {
    digitalWrite(config.RFIDPowerPin, HIGH);
    readerStatus = 1;
    //updateScanIcon();
}
void RFIDReader::deactivate() {
    digitalWrite(config.RFIDPowerPin, LOW);
    readerStatus = 0;
    updateScanIcon();
}

void RFIDReader::toggle() {
    readerStatus = !readerStatus;
    
    if(readerStatus) {
        activate();
    } else {
        deactivate();
    }
}


void RFIDReader::updateScanIcon() {
    const lv_img_dsc_t* image;
    extern const lv_img_dsc_t ui_img_contactless_png;
    extern const lv_img_dsc_t ui_img_contactless_off_png;
    Serial.print("Update icon: ");
    Serial.println(readerStatus);
    if(readerStatus) {
        Serial.println("Reader active");
        image = &ui_img_contactless_png;
    } else {
        Serial.println("Reader inactive");
        image = &ui_img_contactless_off_png;
    }
    lv_img_set_src(ui_Main_TopPanelScan,image);
    //lv_img_set_src(ui_Scan_TopPanelScan,image);
}

String RFIDReader::scan() {
    
    scanResult = rfidreader.monitorSerial();
    if (scanResult != "") {
        if (validateRFID(scanResult.c_str())) {
            #ifdef OTR_DEBUG
                Serial.print("RFIDREADER:scan - Valid RFID! ");
                Serial.println(scanResult);
            #endif
            successfulScan(scanResult);
            
            return scanResult;
        } else {
            if (scanError == "Duplicate")   {
                #ifdef OTR_DEBUG
                    Serial.println("RFIDREADER:scan - Duplicate scan!");
                #endif
                duplicateScan();
            }
            return "";
        }
    }
    return "";

}