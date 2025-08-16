#include <Arduino.h>
#include "SensorModule.h"
#include "WebModule.h"

const char* WIFI_SSID = "Vitel Tonnet-2.4Ghz";     // Replace with your WiFi SSID
const char* WIFI_PASSWORD = "crazyivan42";  // Replace with your WiFi password

SensorModule sensor_module;
WebModule web_module(WIFI_SSID, WIFI_PASSWORD, sensor_module);

void setup() {
  Serial.begin(115200);
  delay(200);

  if (!sensor_module.begin()) {
    Serial.println("Failed to initialize all sensors. Check wiring and addresses.");
    while (1) delay(10);
  }

  if (!web_module.begin()) {
    Serial.println("Failed to initialize web server. Check WiFi credentials.");
    while (1) delay(10);
  }
}

void loop() {
  web_module.update();
  sensor_module.updateGPSData();  // Update GPS data continuously
  
  static unsigned long lastPrint = 0;
  if (millis() - lastPrint >= 1000) {  // Print every second
    sensor_module.readMPUData();
    sensor_module.printSensorData();
    lastPrint = millis();
  }
}
