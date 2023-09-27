#include "water_Control.h"

int moistureAlertThreshold  = 800;
int moisturePumpThreshold   = 900;
int waterLevelThreshold     = 120;
int moistureUpperThreshold  = 100;
bool isPumpOn = false;
unsigned long pumpOnTime = 0;
const unsigned long pumpAutoOffDelay = 60000; // 1 minute

void sendPumpStatus(bool isOn) 
{
  if (isOn) 
  {
    mqttClient.publish(pumpTopic, "on");
  } 
  else 
  {
    mqttClient.publish(pumpTopic, "off");
  }
}

void sendWaterLevelAlert() 
{
  mqttClient.publish(waterLevelTopic, "Water level is low!");
}

void sendMoistureAlert(int moistureLevel) 
{
  mqttClient.publish(moistureTopic, "Soil moisture level is low");
}