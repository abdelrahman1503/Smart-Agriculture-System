#include "OTA.h"
#include "water_Control.h"

void setup() 
{
  Serial.begin(115200);
  connectToWiFi();
  OTASetup();
  mqttClient.setCallback(callback);
  connectToMQTT();
  pinMode(WATERPUMPPIN, OUTPUT);
}

void loop() 
{
  checkConnection();
  if (!mqttClient.connected()) {
    reconnect();
  }
  OTALoop();
  int waterLevel = analogRead(WATERLEVELSENSORPIN);
  Serial.print("Water:");
  Serial.println(waterLevel);
  int waterPercent = map(waterLevel, 0, 4500, 0, 100);
  char waterOut[20];
  sprintf(waterOut, "%d", waterPercent);
  mqttClient.publish(waterLevelTopic, waterOut);
  if(waterLevel < waterLevelThreshold)
  {
    sendWaterLevelAlert();
  }
  int moistureLevel = analogRead(MOISTURESENSORPIN);
  Serial.print("Moisture:");
  Serial.println(moistureLevel);
  int moisturePercent = map(moistureLevel, 4500, 0, 0, 100);
  char moistureOut[20];
  sprintf(moistureOut, "%d", moisturePercent);
  mqttClient.publish(moistureTopic, moistureOut);
  if (moistureLevel > moistureAlertThreshold)
  {
    sendMoistureAlert(moistureLevel);
  }
  if (moistureLevel > moisturePumpThreshold)
  {
    digitalWrite(WATERPUMPPIN, HIGH);
    isPumpOn = true;
    pumpOnTime = millis();
    sendPumpStatus(true);
  }
  else 
  {
    if (isPumpOn)
    {
      unsigned long currentTime = millis();
      if ((currentTime - pumpOnTime >= pumpAutoOffDelay) && (moistureLevel < moistureUpperThreshold)) 
      {
        digitalWrite(WATERPUMPPIN, LOW);   // Deactivate water pump
        isPumpOn = false;
        sendPumpStatus(false);
      }
    }
  }
  mqttClient.loop();
  delay(2000);
}