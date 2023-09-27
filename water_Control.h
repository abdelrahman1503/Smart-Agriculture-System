#ifndef _WATER_CONTROL_h
#define _WATER_CONTROL_h

#include "WiFi_MQTT.h"

extern int moistureAlertThreshold;
extern int moisturePumpThreshold;
extern int waterLevelThreshold;
extern int moistureUpperThreshold;
extern bool isPumpOn;
extern unsigned long pumpOnTime;
extern const unsigned long pumpAutoOffDelay; 

void sendMoistureAlert(int moistureLevel);
void sendWaterLevelAlert();
void sendPumpStatus(bool isOn);

#endif