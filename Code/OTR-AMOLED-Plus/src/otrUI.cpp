// OPEN TAG READER
// Lilygo T-Display-S3 AMOELD PLUS
// C.Chad 2025


#include "otrUI.h"
#include "LilyGo_AMOLED.h"


extern LilyGo_Class amoled;

static uint8_t lastBri =0;

extern const lv_img_dsc_t ui_img_battery_full_png;
extern const lv_img_dsc_t ui_img_battery_5_bar_png;
extern const lv_img_dsc_t ui_img_battery_4_bar_png;
extern const lv_img_dsc_t ui_img_battery_3_bar_png;
extern const lv_img_dsc_t ui_img_battery_2_bar_png;
extern const lv_img_dsc_t ui_img_battery_1_bar_png;
extern const lv_img_dsc_t ui_img_battery_alert_png;
extern const lv_img_dsc_t ui_img_battery_charging_full_png;
int counter = 0;

void five_sec_timer_cb(lv_timer_t * timer) {
    
}

void fifteen_sec_timer_cb(lv_timer_t * timer2) {
    //update time etc
    updateTopPanel();    
    
}

void one_minute_timer_cb(lv_timer_t * timer3) {
    //update time etc
    
}

void led_timer_cb(lv_timer_t * timer4) {
    //
}

void createLvglTimers(void) {
    lv_timer_t * timer = lv_timer_create(five_sec_timer_cb,5000,NULL);
    lv_timer_t * timer2 = lv_timer_create(fifteen_sec_timer_cb,15000,NULL);
    lv_timer_t * timer3 = lv_timer_create(one_minute_timer_cb,60000,NULL);
    lv_timer_t * timer4 = lv_timer_create(led_timer_cb,50,NULL);
}



void setupHomeButton(void) {
    amoled.setHomeButtonCallback([](void *ptr) {
        #ifdef OTR_DEBUG
            Serial.println("Home key pressed!");
        #endif
        static uint32_t checkMs = 0;
        

        if (millis() > checkMs) {
            _ui_screen_change(&ui_Main, LV_SCR_LOAD_ANIM_FADE_IN, 500, 0, &ui_Main_screen_init);
            toggleBlankScreen();
        }
        checkMs = millis() + 200;
        

        lv_timer_create([](lv_timer_t *t) {
            lv_timer_del(t);
        }, 2000, NULL);
    }, NULL);    
}

void setTimeOnTopPanel(void) {
    char timestr[6];
    getTopPanelTime(timestr);
    lv_label_set_text_fmt(ui_Main_TopPanelTime, "%s", timestr);
}

void setDateTimeOnTimeScreen(void) {
    char dateStr[11];
    char timeStr[12];
    getDateTime(dateStr, timeStr);
    lv_label_set_text_fmt(ui_Time_Label_Date, "%s", dateStr);
    lv_label_set_text_fmt(ui_Time_Label_Time, "%s", timeStr);
    lv_refr_now(lv_disp_get_default());
}

void setDateTimeRollers(void) {
    int hour, minute, day, month, year;
    bool ampm;
    getNow(hour, minute, ampm, day, month, year);
    lv_roller_set_selected(ui_Time_RollerHour, hour-1,LV_ANIM_ON);
    lv_roller_set_selected(ui_Time_RollerMinute, minute,LV_ANIM_ON);
    lv_roller_set_selected(ui_Time_RollerAMPM, ampm,LV_ANIM_ON);
    lv_roller_set_selected(ui_Date_RollerDay, day-1,LV_ANIM_ON);
    lv_roller_set_selected(ui_Date_RollerMonth, month-1,LV_ANIM_ON);
    lv_roller_set_selected(ui_Date_RollerYear, year-2025,LV_ANIM_ON);

}

void getDateTimeRollers(int &hour, int &minute, bool &ampm, int &day, int &month, int &year) {
    // Return values from date rollers
    hour = lv_roller_get_selected(ui_Time_RollerHour) + 1;
    minute = lv_roller_get_selected(ui_Time_RollerMinute);
    ampm = lv_roller_get_selected(ui_Time_RollerAMPM);
    day = lv_roller_get_selected(ui_Date_RollerDay) + 1;
    month = lv_roller_get_selected(ui_Date_RollerMonth) + 1;
    year = lv_roller_get_selected(ui_Date_RollerYear) + 125;
}

void updateDayRoller(void) {
    int month = lv_roller_get_selected(ui_Date_RollerMonth) + 1;
    int year = lv_roller_get_selected(ui_Date_RollerYear) + 2025;
    int day = lv_roller_get_selected(ui_Date_RollerDay);
    int max_days;
    switch (month) {
        case 1: //Jan
            max_days = 31;
            break;
        case 2: //Feb
            if ((year % 4 == 0 && year %100 != 0) || year % 400 == 0) {
                max_days = 29;  //leap year
            } else {
                max_days = 28;
            }
            break;
        case 3: // March
            max_days = 31;
            break;
        case 4: // April
            max_days = 30;
            break;
        case 5: // May
            max_days = 31;
            break;
       case 6: // June
            max_days = 30;
            break;
        case 7: // July
            max_days = 31;
            break;
        case 8: // August
            max_days = 31;
            break;
        case 9: // September
            max_days = 30;
            break;
        case 10: // October
            max_days = 31;
            break;
        case 11: // November
            max_days = 30;
            break;
        case 12: // December
            max_days = 31;
            break;
        default:
            // handle invalid month
            break;
    }
    std::string options;
    for (int i = 1; i <= max_days; i++) {
        options += std::to_string(i) + "\n";
    }
    lv_roller_set_options(ui_Date_RollerDay, options.c_str(),
        LV_ROLLER_MODE_NORMAL);
    lv_roller_set_selected(ui_Date_RollerDay,day,LV_ANIM_ON);
}

void setTimeOrDateManualPressed(void) {
    int hour, minute, day, month, year;
    bool ampm;
    getDateTimeRollers(hour, minute, ampm, day, month, year);
    setDateTimeManual(hour, minute, ampm, day, month, year);
}

void blankScreen(void) {
    if (amoled.getBrightness()) {
        lastBri = amoled.getBrightness();
        amoled.setBrightness(0);  //turn screen "off"
    } 
}

void toggleBlankScreen(void) {
    if (amoled.getBrightness()) {
        lastBri = amoled.getBrightness();
        amoled.setBrightness(0);  //turn screen "off"
    } else {
        amoled.setBrightness(lastBri);
    }
}

void endBlankScreen(void)   {
    if (!amoled.getBrightness()) {
        amoled.setBrightness(lastBri);
    }
}

void updateBatteryImage(void)   {
    int voltage = getBatteryVoltage();
    bool chargeStatus = checkChargeStatus();
    const lv_img_dsc_t* image;


    if (voltage >=3950 && voltage <= 4500) {
        image = &ui_img_battery_full_png;
    } else if (voltage >= 3800 && voltage < 3950) {
        image = &ui_img_battery_5_bar_png;
    } else if (voltage >= 3650 && voltage < 3800) {
        image = &ui_img_battery_4_bar_png;
    } else if (voltage >= 3500 && voltage < 3650) {
        image = &ui_img_battery_3_bar_png;
    } else if (voltage >= 3300 && voltage < 3500) {
        image = &ui_img_battery_2_bar_png;
    } else if (voltage >= 3200 && voltage < 3300) {
        image = &ui_img_battery_1_bar_png;
    } else {
        image = &ui_img_battery_alert_png;
    }
    if (chargeStatus) { 
        image = &ui_img_battery_charging_full_png;
    }
    lv_img_set_src(ui_Main_TopPanelBattery, image);

    #ifdef OTR_DEBUG
        Serial.print("Update Battery Image - V = ");
        Serial.print(voltage); 
        Serial.print(" Charge status = ");
        Serial.println(chargeStatus);
        // char batteryStatusStr[70];
        // sprintf(batteryStatusStr, "Battery = \n%dmV\nCharge \nStatus =%d", voltage, chargeStatus);
        // lv_label_set_text(ui_Main_Label_Testing, batteryStatusStr);
        // lv_obj_set_style_text_align(ui_Main_Label_Testing, LV_TEXT_ALIGN_CENTER, 0);
    #endif

}

void updateTopPanel(void)   {
    setTimeOnTopPanel();
    updateBatteryImage();
    lv_refr_now(lv_disp_get_default());
    updateWiFiImage();
}

void updateIPAddress(String ipAddressStr)   {
    String ipStr = "IP Address: \n" + ipAddressStr;
    lv_label_set_text(ui_Main_LabelBotton, ipStr.c_str());
    lv_refr_now(lv_disp_get_default());
}

void updateWiFiImage(void)   {
    if (isValidIPAvailable())   {
        // Hotspot on
        lv_img_set_src(ui_Main_TopPanelScan, &ui_img_wifi_tethering_png);
    } else {
        // Hotspot off
        lv_img_set_src(ui_Main_TopPanelScan, &ui_img_wifi_tethering_off_png);
    }
}

void successfulScan(void)   {
    lv_obj_set_style_bg_color(ui_Main_Panel1, lv_color_hex(0x32CD32), LV_PART_MAIN | LV_STATE_DEFAULT);
    counter++;
    lv_label_set_text(ui_Main_Counter, String(counter).c_str());
    lv_obj_clear_flag(ui_Main_Counter, LV_OBJ_FLAG_HIDDEN);
    lv_refr_now(lv_disp_get_default());
    lv_timer_create([](lv_timer_t *t) {
        lv_obj_set_style_bg_color(ui_Main_Panel1, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);;
        lv_timer_del(t);
    }, 5000, NULL);

}