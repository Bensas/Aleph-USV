#ifndef WEB_MODULE_H
#define WEB_MODULE_H

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include "SensorModule.h"

class WebModule {
private:
    const char* ssid;
    const char* password;
    WebServer server;
    SensorModule& sensors;  // Reference to sensor module for data access
    
    void handleRoot();
    void handleData();
    void handle404();
    
    String generateHTML();
    String generateJSON();
    
public:
    WebModule(const char* wifi_ssid, const char* wifi_password, SensorModule& sensor_module);
    
    bool begin();
    void update();
    
    bool isWiFiConnected() const { return WiFi.status() == WL_CONNECTED; }
    IPAddress getIP() const { return WiFi.localIP(); }
};

#endif // WEB_MODULE_H
