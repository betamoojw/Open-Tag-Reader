// otrPowerManagement.h
#ifndef OTRPOWERMANAGEMENT_H
#define OTRPOWERMANAGEMENT_H

#include <Arduino.h>

void ppmInit(void);
void ppmCheckBattery(void);
void printPowerStatistics(void);
bool checkChargeStatus();
int getBatteryVoltage();


#endif