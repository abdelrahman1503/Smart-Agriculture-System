#include <WiFi.h>
#include <PubSubClient.h>

#define MOISTURESENSORPIN     36
#define WATERLEVELSENSORPIN   34
#define WATERPUMPPIN          15
//const char* ssid                    = "ESP32";
//const char* password                = "12345678";
const char* mqttServer              = "broker.hivemq.com";
const char* waterLevelTopic         = "ESP32/waterLevel";
const char* moistureTopic           = "ESP32/moisture";
const char* pumpTopic               = "ESP32/pump";
const int   mqttPort                = 1883;
int         moistureAlertThreshold  = 800;
int         moisturePumpThreshold   = 900;
int         waterLevelThreshold     = 120;
int         moistureUpperThreshold  = 100;

bool isPumpOn = false;
unsigned long pumpOnTime = 0;
const unsigned long pumpAutoOffDelay = 60000;  // 1 minute

WiFiClient espClient;
PubSubClient mqttClient(espClient);

void connectToMQTT();
void sendMoistureAlert(int moistureLevel);
void connectToWiFi();
void sendWaterLevelAlert();
void callback(char* topic, byte* payload, unsigned int length);
void sendPumpStatus(bool isOn);
void checkConnection();

void setup() 
{
  Serial.begin(115200);
  connectToWiFi();
  mqttClient.setCallback(callback);
  connectToMQTT();
  pinMode(WATERPUMPPIN, OUTPUT);
}

void loop() 
{
  checkConnection();
  int moistureLevel = analogRead(MOISTURESENSORPIN);
  String moisture = String(moistureLevel, 1);
  mqttClient.publish(moistureTopic, moisture.c_str());
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

void connectToWiFi() 
{
  WiFi.mode(WIFI_STA);  // Set ESP32 to station mode

  Serial.println("Scanning Wi-Fi networks...");
  int numNetworks = WiFi.scanNetworks();  // Scan for available networks

  if (numNetworks == 0) 
  {
    Serial.println("No networks found. Please check your Wi-Fi setup.");
    while (1) {
      delay(1000);
    }
  }

  Serial.println("Available networks:");
  for (int i = 0; i < numNetworks; i++) 
  {
    Serial.print(i + 1);
    Serial.print(": ");
    Serial.println(WiFi.SSID(i));  // Print the network name (SSID)
  }

  int selectedNetwork = -1;
  while (selectedNetwork < 0 || selectedNetwork >= numNetworks) 
  {
    Serial.print("Enter the number of the network you want to connect to (1-");
    Serial.print(numNetworks);
    Serial.print("): ");
    while (!Serial.available()) 
    {
      delay(100);
    }
    selectedNetwork = Serial.parseInt();
    Serial.println(selectedNetwork);

    if (selectedNetwork < 1 || selectedNetwork > numNetworks) 
    {
      Serial.println("Invalid network number. Please try again.");
    }
  }

  Serial.print("Selected network: ");
  Serial.println(WiFi.SSID(selectedNetwork - 1));

  String password;
  while (password.length() == 0) 
  {
    Serial.print("Enter the password for the selected network: ");
    while (!Serial.available()) 
    {
      delay(100);
    }
    password = Serial.readString();
    password.trim();

    if (password.length() == 0) 
    {
      Serial.println("Password cannot be empty. Please try again.");
    }
  }

  WiFi.begin(WiFi.SSID(selectedNetwork - 1).c_str(), password.c_str());  // Connect to the selected network

  Serial.print("Connecting to ");
  Serial.println(WiFi.SSID(selectedNetwork - 1));

  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("Wi-Fi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void connectToMQTT() 
{
  mqttClient.setServer(mqttServer, mqttPort);
  while (!mqttClient.connected()) 
  {
    Serial.print("Connecting to MQTT...");
    if (mqttClient.connect("ESP32Client")) 
    {
      Serial.println("Connected to MQTT");
    } 
    else 
    {
      Serial.print("Failed to connect to MQTT, rc=");
      Serial.println(mqttClient.state());
      delay(2000);
    }
  }
}

void sendMoistureAlert(int moistureLevel) 
{
  mqttClient.publish(moistureTopic, "Soil moisture level is low");
}

void sendWaterLevelAlert() 
{
  mqttClient.publish(waterLevelTopic, "Water level is low!");
}

void callback(char* topic, byte* payload, unsigned int length)
{
  String message = "";
  for (int i = 0; i < length; i++) 
  {
    message += (char)payload[i];
  }

  if (message == "on") 
  {
    digitalWrite(WATERPUMPPIN, HIGH);  // Turn on water pump
  } 
  else if (message == "off") 
  {
    digitalWrite(WATERPUMPPIN, LOW);   // Turn off water pump
  }
}

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

void checkConnection()
{
  if (WiFi.status() != WL_CONNECTED) 
  {
    Serial.println("Wi-Fi connection lost. Reconnecting...");
    WiFi.reconnect();
    while (WiFi.status() != WL_CONNECTED) 
    {
      delay(1000);
      Serial.print(".");
    }
    Serial.println("");
    Serial.println("Wi-Fi reconnected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }  
}
