#include <Arduino.h>
#include "SensorModule.h"

SensorModule sensor_module;

void setup() {
  Serial.begin(115200);
  delay(200);

  if (!sensor_module.begin()) {
    Serial.println("Failed to initialize all sensors. Check wiring and addresses.");
    while (1) delay(10);
  }
}

void loop() {
  sensor_module.readMPUData();
  sensor_module.printSensorData();
  
  delay(1000);
}
