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

LilyGo_Class amoled;
uint8_t btnPin = 0;
uint8_t rotation = 1;
extern RFIDReader rfidreader;

//CONFIG
bool readerBoard =0;  // 0 for Priority 1 (ASCII Output), 1 for WL134A
bool readerSerial = 0; // 0 for Serial0, 1 for Serial1 
//


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
        sprintf(bootTimeString, "Boot time =\n%dms\nUI Load Time =\n%dms", bootTime, uiLoadTime);
        lv_label_set_text(ui_Main_Label_Testing, bootTimeString);
        
        
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

    

    
    //OTR UI
    setupHomeButton();
    createLvglTimers();


    //RFID Reader
    rfidreader.begin();


    //Network
    createAccessPoint();
    startWebServer();
}


void loop()
{

    lv_task_handler();
    delay(5);
    if(rfidreader.scan() != "") {
        Serial.println(rfidreader.scan());
        successfulScan();
        timeStampSystem();

    }
    
    
}
