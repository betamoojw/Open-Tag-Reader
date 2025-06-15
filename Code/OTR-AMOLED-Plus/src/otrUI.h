// OPEN TAG READER
// Lilygo T-Display-S3 AMOELD PLUS
// C.Chad 2025


#ifndef OTRUI_H
#define OTRUI_H

#include <Arduino.h>
#include <lvgl.h>
#include "ui/ui.h"
#include "otrPowerManagement.h"
#include "otrTime.h"
#include "otrNetwork.h"
#include "otrFeedback.h"


void setupHomeButton(void);
void createHomeButtonTimer(void);
void homeButtonLongPress(void);
void homeButtonShortPress(void);
void createLvglTimers(void);
void oneSecondTimer(void);
void setTimeOnTopPanel(void);
void setDateTimeOnTimeScreen(void);
void setDateTimeRollers(void);
void updateDayRoller(void);
void getDateTimeRollers(int &hour, int &minute, bool &ampm, int &day, int &month, int &year);
void setTimeOrDateManualPressed(void);
void blankScreen(void);
void toggleBlankScreen(void);
void endBlankScreen(void);
void updateBatteryImage(void);
void updateTopPanel(void);
void updateIPAddress(String ipAddressStr);
void updateWiFiImage(void);
void successfulScan(String scanResult);
void duplicateScan(void);
void displayFileSystemContents(void) ;
void displayStorageInfo(void);



#endif