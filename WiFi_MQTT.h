#ifndef _WIFI_MQTT_h
#define _WIFI_MQTT_h

#include <PubSubClient.h>
#include <WiFi.h>

#define MOISTURESENSORPIN     36
#define WATERLEVELSENSORPIN   34
#define WATERPUMPPIN          15

extern const char* mqttServer;
extern const char* waterLevelTopic;
extern const char* moistureTopic;
extern const char* pumpTopic;
extern const int   mqttPort;

extern WiFiClient espClient;
extern PubSubClient mqttClient;

void connectToWiFi();
void connectToMQTT();
void checkConnection();
void callback(char* topic, byte* payload, unsigned int length);

#endif 
