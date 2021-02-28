//Development
const bool develmode = false;

//Identification
#include <ArduinoUniqueID.h>
String id = "";

//Network
#include <SPI.h>
#include <WiFiNINA.h>

char ssid[] = "telenet-0575928";      // Wifi SSID
char pass[] = "7Ytca4tEvshy";       // Wifi password

int status = WL_IDLE_STATUS;

WiFiSSLClient client;


//Sensing
const int sensorPin = A0;
const int dryVal = 880;
const int dryValNano = 700;
const int dryValNano10M = 485;
const int wetVal = 674;
const int wetValNano = 230;
const int wetValNano10M = 110;





void setup() {
  //Network
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }
  connectToAP();    // Connect to Wifi Access Point
  printWifiStatus(true, true);

  //Idenitifcation
  id = readID();
}

void loop() {
  //Sensing
  int sensorVal = analogRead(sensorPin);
  if (develmode) {
    Serial.print("Sensor Value: ");
    Serial.println(sensorVal);
  }
  int moisture = map(sensorVal, dryValNano10M, wetValNano10M, 0, 100); 
  if (moisture < 0) {moisture = 0;}
  if (moisture > 100) {moisture = 100;}

  Serial.println();
  printID();
  Serial.print("Moisture: ");
  Serial.println(moisture);
  printWifiStatus(false, true);
  Serial.println();
  

  //Serial.println();
  if (!develmode) {delay(900);}
  delay(100);
}


//Network
void printWifiStatus(bool showSsid, bool showIp) {
  if (showSsid) {
    Serial.print("SSID: ");
    Serial.println(WiFi.SSID());
  }
  if (showIp) {
    IPAddress ip = WiFi.localIP(); // Device IP address
    Serial.print("IP Address: ");
    Serial.println(ip);
  }
}

//Network
void connectToAP() {
  // Try to connect to Wifi network
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network
    status = WiFi.begin(ssid, pass);

    // wait 1 second for connection:
    delay(1000);
    Serial.println("Connected...");
  }
}

//Identification
String readID() {
  String id = "";
  for(size_t i = 0; i < UniqueIDsize; i++)
    id = id + UniqueID[i];
  return id;
}
void printID() {
  Serial.print("ID: ");
  Serial.println(id);
}
