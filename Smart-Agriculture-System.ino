#include <SD.h>
#include "OTA.h"
#include "water_Control.h"

#define SD_CS_PIN             5
#define SPEAKER_PIN           14

void playSound();


// Array to store the sound file names

char* soundFiles[] = {
  "sound1.wav",
  "sound2.wav",
  "sound3.wav"
};

const int numSoundFiles = sizeof(soundFiles) / sizeof(soundFiles[0]);

/*
TMRpcm tmrpcm;
*/
void setup() 
{
  Serial.begin(115200);
   if (!SD.begin(SD_CS_PIN)) {
    Serial.println("SD card initialization failed!");
  }
  connectToWiFi();
  OTASetup();
  mqttClient.setCallback(callback);
  connectToMQTT();
  /*
  tmrpcm.speakerPin = SPEAKER_PIN;
  tmrpcm.setVolume(5);
  */
  pinMode(WATERPUMPPIN, OUTPUT);
}

void loop() 
{
  checkConnection();
  OTALoop();
  int moistureLevel = analogRead(MOISTURESENSORPIN);
  String moisture = String(moistureLevel, 1);
  mqttClient.publish(moistureTopic, moisture.c_str());
  if (moistureLevel > moistureAlertThreshold)
  {
    sendMoistureAlert(moistureLevel);
    /*playSound();*/
  }
  if (moistureLevel > moisturePumpThreshold)
  {
    digitalWrite(WATERPUMPPIN, HIGH);
    isPumpOn = true;
    pumpOnTime = millis();
    sendPumpStatus(true);
    /*playSound();*/
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
  int waterLevel = analogRead(WATERLEVELSENSORPIN);
  String water = String(waterLevel, 1);
  mqttClient.publish(waterLevelTopic, water.c_str());
  if(waterLevel < waterLevelThreshold)
  {
    sendWaterLevelAlert();
  }
  mqttClient.loop();
  delay(1000);
}

void playSound()
{
      // Generate a random index for selecting a sound file
    int randomIndex = random(0, numSoundFiles);

    // Get the selected sound file name
    char* soundFile = soundFiles[randomIndex];

    // Play the sound file
    //tmrpcm.play(soundFile);
    delay(100); // Delay to allow audio playback
}