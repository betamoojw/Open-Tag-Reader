// OPEN TAG READER
// Lilygo T-Display-S3 AMOELD PLUS
// C.Chad 2025


#include "otrTime.h"
#include <LV_Helper.h>
#include <lvgl.h>
#include "ui/ui.h"
#include "LilyGo_AMOLED.h"

extern LilyGo_Class amoled;

bool pmFlag;
int year;
int month;
int date;
int hour;
int minute;
int second;


void initTime(void)   {
    //Read RTC and same to system
    struct tm timeinfo;
    time_t now = amoled.hwClockRead();
    
    struct timeval tv;
    tv.tv_sec = now;
    tv.tv_usec = 0;
    settimeofday(&tv, NULL);
    setTimeOnTopPanel();
    
}

char* timeStampRTC() {
    // Return RTC time and date
    struct tm timeinfo;
    amoled.getDateTime(&timeinfo);
    static char timeStampStr[20];
    strftime(timeStampStr, 20, "%Y-%m-%d %H:%M:%S", &timeinfo);
    #ifdef OTR_DEBUG
        Serial.print("timeStampRTC: ");
        Serial.println(timeStampStr);
    #endif
    return timeStampStr;
}

char* timeStampSystem() {
    struct tm timeinfo;
    time_t now;
    time(&now);
    localtime_r(&now, &timeinfo);
    static char timeStampStr[20];
    strftime(timeStampStr, 20, "%Y-%m-%d %H:%M:%S", &timeinfo);
    #ifdef OTR_DEBUG
        Serial.print("timeStampSystem: ");
        Serial.println(timeStampStr);
    #endif
    return timeStampStr;
}

void getTopPanelTime(char timestr[7]) {
    struct tm timeinfo;
    time_t now;
    time(&now);
    localtime_r(&now, &timeinfo);
    strftime(timestr, 7, "%H:%M", &timeinfo);
    #ifdef OTR_DEBUG
        Serial.println(timestr);
    #endif
}

void getDateTime(char dateStr[11], char timeStr[12]) {
    // Return RTC time and date
    struct tm timeinfo;
    amoled.getDateTime(&timeinfo);
    strftime(dateStr, 11, "%d/%m/%Y", &timeinfo);
    strftime(timeStr, 12, "%l:%M %p", &timeinfo);

    #ifdef OTR_DEBUG
        Serial.print("getDateTime: ");
        Serial.print(dateStr);
        Serial.print(" ");
        Serial.println(timeStr);
    #endif
}

void getNow(int &hour, int &minute, bool &ampm, int &day, int &month, int &year) {
    // Return current time and date
    struct tm timeinfo;
    amoled.getDateTime(&timeinfo);

    hour = timeinfo.tm_hour % 12; // 0-11
    if (hour == 0) hour = 12;
    minute = timeinfo.tm_min;
    ampm = (timeinfo.tm_hour < 12) ? 0 : 1; // 0 for AM, 1 for PM
    day = timeinfo.tm_mday;
    month = timeinfo.tm_mon + 1; // 1-12
    year = timeinfo.tm_year + 1900; // 4-digit year
}

String getSessionDate() {
    struct tm timeinfo;
    time_t now;
    time(&now);
    localtime_r(&now, &timeinfo);
    char tmpStr[8];
    strftime(tmpStr, 8, "%y%m%d", &timeinfo);
    #ifdef OTR_DEBUG
        Serial.println(tmpStr);
    #endif
    return String(tmpStr);
    
}

void setTimeManual(int &hour, int &minute, bool &ampm) {
    struct tm timeinfo;
    amoled.getDateTime(&timeinfo);
    timeinfo.tm_hour = hour;
    if(ampm) {
        if (timeinfo.tm_hour != 12)  {
            timeinfo.tm_hour += 12;
        }
    } else {    //AM
        if (timeinfo.tm_hour == 12)  {
            timeinfo.tm_hour = 0;
        }    
    }
    timeinfo.tm_min = minute;
    timeinfo.tm_sec = 0;
    // Update RTC
    amoled.setDateTime(timeinfo.tm_year,
        timeinfo.tm_mon + 1,
        timeinfo.tm_mday,
        timeinfo.tm_hour,
        timeinfo.tm_min,
        timeinfo.tm_sec);

    // Update system time
    mktime(&timeinfo);
    struct timeval tv;
    tv.tv_sec = mktime(&timeinfo);
    tv.tv_usec = 0;
    settimeofday(&tv, NULL);
    
}

void setDateManual(int &day, int &month, int &year) {
    struct tm timeinfo;
    amoled.getDateTime(&timeinfo);
    timeinfo.tm_year = year;
    timeinfo.tm_mon = month;
    timeinfo.tm_mday = day;

    // Update the RTC time
    amoled.setDateTime(timeinfo.tm_year,
                       timeinfo.tm_mon,
                       timeinfo.tm_mday,
                       timeinfo.tm_hour,
                       timeinfo.tm_min,
                       timeinfo.tm_sec);

    // Update the system time
    mktime(&timeinfo);
    struct timeval tv;
    tv.tv_sec = mktime(&timeinfo);
    tv.tv_usec = 0;
    settimeofday(&tv, NULL);    

}

void setDateTimeManual(int hour, int minute, bool ampm, int day, int month, int year) {
    // Set time
    struct tm timeinfo;
    timeinfo.tm_hour = hour;
    if(ampm) {
        if (timeinfo.tm_hour != 12)  {
            timeinfo.tm_hour += 12;
        }
    } else {    //AM
        if (timeinfo.tm_hour == 12)  {
            timeinfo.tm_hour = 0;
        }    
    }
    timeinfo.tm_min = minute;
    timeinfo.tm_sec = 0;
    timeinfo.tm_year = year;
    timeinfo.tm_mon = month;
    timeinfo.tm_mday = day;
    // Update the RTC time
    amoled.setDateTime(timeinfo.tm_year,
        timeinfo.tm_mon,
        timeinfo.tm_mday,
        timeinfo.tm_hour,
        timeinfo.tm_min,
        timeinfo.tm_sec);

    // Update the system time
    mktime(&timeinfo);
    struct timeval tv;
    tv.tv_sec = mktime(&timeinfo);
    tv.tv_usec = 0;
    settimeofday(&tv, NULL);
    #ifdef OTR_DEBUG
        char timestr[20];
        strftime(timestr, 20, "%Y-%m-%d %H:%M:%S", &timeinfo);
        Serial.print("DateTime Set Manually: ");
        Serial.println(timestr);
    #endif
}

// void updateTimeToScreen()   {
//     char timeClock[5];
//     DateTime now = rtc.now();
//     if (now.isPM()) {
//         sprintf(timeClock, "%d:%02d", now.hour() - 12, now.minute());
//     } else {
//         sprintf(timeClock, "%d:%02d", now.hour(), now.minute());
//     }
//     lv_label_set_text_fmt(ui_Main_TopPanelTime, "%s", timeClock);
//     lv_label_set_text_fmt(ui_Scan_TopPanelTime, "%s", timeClock);
  
    
// }

// void updateDateToScreen()   {
//     char dateStr[11];
//     DateTime now = rtc.now();
//     sprintf(dateStr, "%d/%d/%d", now.day(), now.month(), now.year());
//     Serial.println(dateStr);
//     lv_label_set_text_fmt(ui_Time_Label_Date, "%s", dateStr);
// }

// void get_RTC_temperature() {
//     Serial.printf(" Temperature: %.1f °C\r\n", rtc.getTemperature());
// }
// void clock_timer(lv_timer_t * timer) {
//     updateTimeToScreen();
//     updateBatteryImage();
// }
// DateTime stringToDateTime(String dateStr) {
//     int day = dateStr.substring(0, dateStr.indexOf('/')).toInt();
//     int month = dateStr.substring(dateStr.indexOf('/') + 1, dateStr.lastIndexOf('/')).toInt();
//     int year = dateStr.substring(dateStr.lastIndexOf('/') + 1).toInt();
//     return DateTime(year, month, day);
// }
// String dateTimeToString(DateTime dt) {
//     char dateStr[11];
//     sprintf(dateStr, "%02d/%02d/%04d", dt.day(), dt.month(), dt.year());
//     return String(dateStr);
// }

// String timeStampToString(DateTime dt) {
//     char dateStr[20];
//     sprintf(dateStr, "%02d/%02d/%04d %02d:%02d:%02d", dt.day(), dt.month(), dt.year(), dt.hour(), dt.minute(), dt.second());
//     return String(dateStr);
// }

// DateTime stringToTimeStamp(String tmStmp) {
//     int day = tmStmp.substring(0, tmStmp.indexOf('/')).toInt();
//     int month = tmStmp.substring(tmStmp.indexOf('/') + 1, tmStmp.lastIndexOf('/')).toInt();
//     int year= tmStmp.substring(tmStmp.lastIndexOf('/') + 1, 4).toInt();
//     int hour = tmStmp.substring(tmStmp.indexOf(' ') + 1, tmStmp.indexOf(':')).toInt();
//     int minute = tmStmp.substring(tmStmp.indexOf(':') + 1, tmStmp.lastIndexOf(':')).toInt();
//     int second = tmStmp.substring(tmStmp.lastIndexOf(':') + 1).toInt();
//     return DateTime(year, month, day, hour, minute, second);
// }

// String dateToSessionFormat(DateTime dt) {
//     char datestr[10];
//     int year = dt.year()-2000;
//     sprintf(datestr, "%02d%02d%02d", year, dt.month(), dt.day());
//     Serial.println(datestr);
//     return String(datestr);
// }

// String ageFromDate(String dateStr) {
//     DateTime dt = stringToDateTime(dateStr);
//     DateTime now = rtc.now();
//     int ageInMonths = (now.year() - dt.year()) * 12 + (now.month() - dt.month());
//     if (ageInMonths < 18) {
//         return String(ageInMonths) + " mths";
//     } else {
//         float ageInYears = ageInMonths / 12;
//         return String(ageInYears, 1) + " yrs ";
//     }
// }

// String whpSafeDate(String dateStr, String whp) {
//     DateTime dt = stringToDateTime(dateStr);
//     int daysToAdd = whp.toInt();
        
//     DateTime newDt = dt + TimeSpan(daysToAdd, 0, 0, 0);
//     return dateTimeToString(newDt);
// }