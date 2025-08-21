#ifndef WEB_MODULE_H
#define WEB_MODULE_H

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include "SensorModule.h"
#include "ActuatorModule.h"

class WebModule {
private:
    const char* ssid;
    const char* password;
    WebServer server;
    SensorModule& sensor_module;
    ActuatorModule& actuator_module;
    
    void handleRoot();
    void handleData();
    void handleServo();
    void handle404();
    
    String generateHTML();
    String generateJSON();
    
public:
    WebModule(const char* wifi_ssid, const char* wifi_password, SensorModule& sensor_module, ActuatorModule& actuator_module);
    
    bool begin();
    void update();
    
    bool isWiFiConnected() const { return WiFi.status() == WL_CONNECTED; }
    IPAddress getIP() const { return WiFi.localIP(); }
};

#endif // WEB_MODULE_H
