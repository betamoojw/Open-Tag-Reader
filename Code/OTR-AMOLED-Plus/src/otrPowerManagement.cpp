#include "otrPowerManagement.h"
#include "LilyGo_AMOLED.h"

extern LilyGo_Class amoled;

void ppmInit(void)
{
     //PPM init
     amoled.BQ.setChargeTargetVoltage(4208);
     amoled.BQ.setPrechargeCurr(64);
     amoled.BQ.setChargerConstantCurr(832);
     amoled.BQ.enableMeasure();
     amoled.BQ.enableCharge();
     
     #ifdef OTR_DEBUG
         Serial.println("PPM Init");
         Serial.print("Charging Status: "); Serial.println(amoled.BQ.isCharging() ? "YES" : "NO");
         Serial.print("USB Plug In: "); Serial.println(amoled.BQ.isVbusIn() ? "YES" : "NO");
         Serial.print("Charge State: "); Serial.println(amoled.BQ.getChargeStatusString());
     #endif   
}

void ppmCheckBattery(void) {
    amoled.BQ.enableBatLoad();
    delay(10);
    #ifdef OTR_DEBUG
        Serial.print("Charging Status: "); Serial.println(amoled.BQ.isCharging() ? "YES" : "NO");
        Serial.print("USB Plug In: "); Serial.println(amoled.BQ.isVbusIn() ? "YES" : "NO");
        Serial.print("Charge State: "); Serial.println(amoled.BQ.getChargeStatusString());
        Serial.print("Battery Voltage: "); Serial.print(amoled.BQ.getBattVoltage()); Serial.println(" mV");
    #endif
    amoled.BQ.disableBatLoad();
}

void printPowerStatistics(void) {
    // If the battery does not exist, the data may be confused when the charging function is turned on.
    // The chip cannot determine whether the battery exists
    Serial.println("Power           VBUS    VBAT   SYS       VbusStatus      ");
    Serial.println("Status          (mV)    (mV)   (mV)   (HEX)     String   ");
    Serial.println("---------------------------------------------------------------");
    Serial.print(amoled.BQ.isVbusIn() ? "Connected" : "Disconnect"); Serial.print("\t");
    Serial.print(amoled.BQ.getVbusVoltage()); Serial.print("\t");
    Serial.print(amoled.BQ.getBattVoltage()); Serial.print("\t");
    Serial.print(amoled.BQ.getSystemVoltage()); Serial.print("\t");
    Serial.print("0x");
    Serial.print(amoled.BQ.getBusStatus(), HEX); Serial.print("\t");
    Serial.println(amoled.BQ.getBusStatusString()); 
    Serial.println("_______________________________________________________________");

    Serial.println(" ChargeStatus                                           ");
    Serial.println("(HEX)   String                                          ");
    Serial.println("---------------------------------------------------------------");
    Serial.print("0x");
    Serial.print(amoled.BQ.chargeStatus(), HEX); Serial.print("\t");
    Serial.println(amoled.BQ.getChargeStatusString());
    Serial.println("_______________________________________________________________");
    
    Serial.println("    TargetVoltage ChargeCurrent      Precharge       ");
    Serial.println("        (mV)           (mA)            (mA)         ");
    Serial.println("---------------------------------------------------------------");
    Serial.print("\t");Serial.print(amoled.BQ.getChargeTargetVoltage()); Serial.print("\t\t");
    Serial.print(amoled.BQ.getChargeCurrent()); Serial.print("\t\t");
    Serial.println(amoled.BQ.getPrechargeCurr());
    Serial.println("_______________________________________________________________");
    Serial.println("  NTCStatus         ");
    Serial.println("(HEX)    String       ");
    Serial.println("---------------------------------------------------------------");
    Serial.print("0x");
    Serial.print(amoled.BQ.getNTCStatus()); Serial.print("\t");
    Serial.println(amoled.BQ.getNTCStatusString()); 
    Serial.println("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");

}

bool checkChargeStatus()    {
    bool chargeStatus = amoled.BQ.isCharging();
    #ifdef OTR_DEBUG
        Serial.print("checkChargeStatusCharge Status: ");
        Serial.println(chargeStatus ? "Charging" : "Not Charging");
    #endif
    return chargeStatus;
}

int getBatteryVoltage() {
    int voltage = amoled.BQ.getBattVoltage();
    #ifdef OTR_DEBUG
        Serial.print("getBatteryVoltage - Battery Voltage: ");
        Serial.print(voltage); Serial.println("mV");
    #endif
    return voltage;
}