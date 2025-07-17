/**
 *  OPEN TAG READER
 * 
 *  Initial test of board with no RFID Reader board attached
 *  Goals
 *  - Basic Screen 
 * 
 *  Lilygo T-Display-S3 AMOELD PLUS
 * 
 *  C.Chad 2025
 */

#include <LilyGo_AMOLED.h>
#include <LV_Helper.h>
#include <lvgl.h>
#include "ui/ui.h"
#include "otrFileHandling.h"
#include "otrPowerManagement.h"
#include "otrUI.h"
#include "otrTime.h"
#include "otrNetwork.h"
#include "otrWebInterface.h"
#include "otrData.h"
#include "otrScanning.h"
#include "otrFeedback.h"

LilyGo_Class amoled;
uint8_t btnPin = 0;
uint8_t rotation = 1;
extern RFIDReader rfidreader;
LED led;
VIBRATE vibrate;
BUZZER buzzer;
RECORDS records;
TAGS tags;
ANIMALS animals;



//CONFIG #################################
bool readerBoard =0;  // 0 for Priority 1 (ASCII Output), 1 for WL134A
bool readerSerial = 1; // 0 for Serial0, 1 for Serial1 
// #######################################


void setup()
{
    #ifdef OTR_DEBUG
        uint32_t startMillis = millis();
    #endif
    

    amoled.beginAMOLED_191_SPI();

    beginLvglHelper(amoled);
    amoled.setRotation(rotation);
    lv_disp_drv_t *drv = lv_disp_get_default()->driver;
    drv->hor_res = amoled.width();
    drv->ver_res = amoled.height();
    lv_disp_drv_update(lv_disp_get_default(), drv);
    #ifdef OTR_DEBUG
        uint32_t bootMillis = millis();
    #endif
    
    ui_init();
    
    #ifdef OTR_DEBUG
        uint32_t uiMillis = millis();

        uint32_t bootTime = bootMillis - startMillis;
        uint32_t uiLoadTime = uiMillis - bootMillis;
        char bootTimeString[60];
        sprintf(bootTimeString, "Boot time = %dms nUI Load Time = %dms", bootTime, uiLoadTime);
        Serial.println(bootTimeString);
        //lv_label_set_text(ui_Main_Label_RFID, bootTimeString);
        
        
    #endif

    //Time
    initTime();
    updateTopPanel();
    
    //Serial
    Serial.begin(115200);


    //PPM init
    ppmInit();
    

    //Filesystem Init
    initFileSystem();
    tags.readTagsActive();
    

    
    //OTR UI
    setupHomeButton();
    createLvglTimers();


    //RFID Reader
    rfidreader.begin();

    //LED
    led.init();

    //Vibrate
    vibrate.begin();

    //Buzzer
    buzzer.init();

    //TESTING
    // buzzer.successTone();
    // deleteFileSD("/Sheep/sessions/_1.csv");

    
    

    //Network
    createAccessPoint();
    startWebServer();
}


void loop()
{

    lv_task_handler();
    delay(5);
    if(rfidreader.scan() != "") {   // reads incoming serial and checks if it is a valid tag
                                    // if valid scan
        //Serial.println(rfidreader.scan());
        timeStampSystem();
        Serial.println("Scan result" + rfidreader.scanResult);
        tags.resetCurrentTag();
        animals.resetCurrentAnimal();
    

    } else  {
        // Check if tag and animal details are populated and matching scan result
        if (rfidreader.scanResult == "") {
            return;
        } else if (tags.currentTag.RFID == rfidreader.scanResult) {
            // tag details already populated
            // check if animal details are populated
            if (animals.currentAnimal.rfid == rfidreader.scanResult) {
                // animal details already populated
                return;
            } else {
                //retrieve animal details
            }
        } else {
            //retrieve tag details
            if (!tags.knownFlag && !tags.isNew) {
                //search for tag - set flags
                tags.activeFlag = tags.isTagActive(rfidreader.scanResult);
                if (tags.activeFlag) {
                    tags.knownFlag = true;
                }  else {
                    tags.knownFlag = tags.isTagKnown(rfidreader.scanResult);
                    if (!tags.knownFlag) {
                        tags.isNew = true;
                    }
                }  
                return;
            } else if (tags.activeFlag) {
                tags.currentTag = tags.getActiveTagDetails(rfidreader.scanResult);
            } else if (!tags.activeFlag && tags.knownFlag && !tags.isNew) {
                tags.currentTag = tags.getTagDetails(rfidreader.scanResult);                
            } else if (tags.isNew)  {
                //create new tag
                tags.currentTag.RFID = rfidreader.scanResult;
                tags.currentTag.Status = "Active";
                tags.addTag(); // add tag to tags file
                tags.knownFlag = true;
                tags.isNew = false; // reset new tag flag
                //todo: add tag functionality - tagging mode - create new animals

            }

            if (!tags.activeFlag && !tags.isNew) {
                // check if tag is active
                tags.activeFlag = tags.isTagActive(rfidreader.scanResult);
                if (tags.activeFlag) {
                    // tag is active - get tag details
                    tags.currentTag = tags.getActiveTagDetails(rfidreader.scanResult);
                } else {
                    // tag is not active - check if tag is known
                    if (tags.isTagKnown(rfidreader.scanResult)) {
                        // tag is known - get tag details
                        tags.currentTag = tags.getTagDetails(rfidreader.scanResult);
                    } else {
                        // tag is not known - activate tag
                        tags.isNew = true; // set flag to indicate new tag
                    }
                }

            }
        }
         //check tag is active
        if (!tags.activeFlag && !tags.isNew)   {
            tags.activeFlag = tags.isTagActive(rfidreader.scanResult);
        }
        if(tags.isTagActive(rfidreader.scanResult))     {
            // tag Active - find associated records
            TAGS::Tags tagDetails = tags.getActiveTagDetails(rfidreader.scanResult);
            
        } else {
            // check tag is known
            if(tags.isTagKnown(rfidreader.scanResult)) {
                // tag known -get tag details
                TAGS::Tags tagDetails = tags.getTagDetails(rfidreader.scanResult);
                if(tagDetails.Status == "Unused") {
                    // Activate tag
                } else if(tagDetails.Status == "Inactive") {
                    // Tag either belongs to Dead or Sold animal
                }

            } else {
                // tag not known - Activate Tag
            }
        }
    }
    
    
}
