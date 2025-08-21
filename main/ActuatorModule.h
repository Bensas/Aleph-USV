#ifndef ACTUATOR_MODULE_H
#define ACTUATOR_MODULE_H

#include <Arduino.h>
#include <ESP32Servo.h>

class ActuatorModule {
private:
    const int servo_pin;
    Servo servo;
    int current_position;
    bool servo_initialized;
    
    // Servo constraints
    static const int MIN_POSITION = 0;
    static const int MAX_POSITION = 180;

public:
    ActuatorModule(int pin = 25);
    
    bool begin();
    bool isServoInitialized() const { return servo_initialized; }
    
    void setPosition(int angle);
    int getPosition() const;
    
    void center();
    
    void detach();
    void attach();
};

#endif // ACTUATOR_MODULE_H
