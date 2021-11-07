/**  
   WiFiClient wifiClient - creates a Wi-Fi client.
   MqttClient mqttClient(wifiClient) - connects the Wi-Fi client to the MQTT client.
   WiFi.begin(ssid, pass) - connects to local Wi-Fi network.
   mqttClient.connect(broker, port) - connects to broker (and port).
   mqttClient.poll() - keeps the connection alive, used in the loop().
   mqttClient.beginMessage(topic) - creates a new message to be published.
   mqttClient.print() - prints the content of message between the ().
   mqttClient.endMessage() - publishes the message to the broker.
   mqttClient.subscribe(topic) - subscribes to a topic.
   mqttClient.available() - checks if any messages are available from the topic.
   mqttClient.read() - reads the incoming messages.
 */

#include <ArduinoMqttClient.h>
#include <WiFiNINA.h>
#include "config.h"

//Connecting
char ssid[] = SSID;        // your network SSID (name)
char pass[] = PWD;    // your network password (use for WPA, or use as key for WEP)

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

const char broker[] = BROKER_IP;
int        port     = 1883;
const char moisture_topic[]  = "moisture";
const char temperature_topic[]  = "temperature";
const char light_topic[]  = "light";

const long interval = 8000;
unsigned long previousMillis = 0;


//Sensing
  //Moisture
const int sensorMoisturePin = A3;
const int dryVal = 880;
const int dryValNano = 700;
const int dryValNano10M = 520;
const int wetVal = 674;
const int wetValNano = 230;
const int wetValNano10M = 180;
  //Temperature
const int sensorTemperaturePin = A6;
  //Light
const int sensorLightPin = A0;
const int sensorLightLow = 0;
const int sensorLightHigh = 1023;




int count = 0;

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // attempt to connect to Wifi network:
  Serial.print("Attempting to connect to WPA SSID: ");
  Serial.println(ssid);
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    // failed, retry
    Serial.print(".");
    delay(5000);
  }

  Serial.println("You're connected to the network");
  Serial.println();

  Serial.print("Attempting to connect to the MQTT broker: ");
  Serial.println(broker);

  if (!mqttClient.connect(broker, port)) {
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());

    while (1);
  }

  Serial.println("You're connected to the MQTT broker!");
  Serial.println();
}

void loop() {
  // call poll() regularly to allow the library to send MQTT keep alive which
  // avoids being disconnected by the broker
  mqttClient.poll();

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    // save the last time a message was sent
    previousMillis = currentMillis;

    //record random value from A0, A1 and A2
    int moisture_value = measure_moisture();
    float temperature_value = measure_temperature();
    int light_value = measure_light();

    Serial.print("Sending message to topic: ");
    Serial.println(moisture_topic);
    Serial.println(moisture_value);

    Serial.print("Sending message to topic: ");
    Serial.println(temperature_topic);
    Serial.println(temperature_value);

    Serial.print("Sending message to topic: ");
    Serial.println(light_topic);
    Serial.println(light_value);

    // send message, the Print interface can be used to set the message contents
    mqttClient.beginMessage(moisture_topic);
    mqttClient.print(moisture_value);
    mqttClient.endMessage();

    mqttClient.beginMessage(temperature_topic);
    mqttClient.print(temperature_value);
    mqttClient.endMessage();

    mqttClient.beginMessage(light_topic);
    mqttClient.print(light_value);
    mqttClient.endMessage();

    Serial.println();
  }
}

int measure_moisture() {
  int sensorMoistureVal = analogRead(sensorMoisturePin);
  int moisture = map(sensorMoistureVal, dryValNano10M, wetValNano10M, 0, 100); 
  if (moisture < 0) {moisture = 0;}
  if (moisture > 100) {moisture = 100;}
  return moisture;
}

float measure_temperature() {
  int sensorTemperatureVal = analogRead(sensorTemperaturePin);
  float voltageTemp = (sensorTemperatureVal/1024.0) * 5.0;
  float temperature = (voltageTemp - .5) * 100;
  return temperature;
}

int measure_light() {
  int sensorLightVal = analogRead(sensorLightPin);
  int light = map(sensorLightVal, sensorLightLow, sensorLightHigh, 0, 100); 
  return light;
}
