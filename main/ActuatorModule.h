#ifndef ACTUATOR_MODULE_H
#define ACTUATOR_MODULE_H

#include <Arduino.h>

class ActuatorModule {
private:
    const int servo_pin;
    const int ledc_channel;
    int current_position;
    bool servo_initialized;
    
    static const int MIN_POSITION = 0;
    static const int MAX_POSITION = 180;
    
    static const int LEDC_HZ = 50;
    static const int LEDC_RES = 16;
    

    static const int SERVO_MIN_MICROSECONDS = 1000;
    static const int SERVO_MAX_MICROSECONDS = 2000;
    
    uint32_t microsecondsToDutyCycle(int us) const;
    int angleToUs(int angle) const;

public:
    ActuatorModule(int pin = 25, int channel = 0);
    
    bool begin();
    bool isServoInitialized() const { return servo_initialized; }
    
    void setPosition(int angle);
    int getPosition() const;
    
    void center();
    
    void detach();
    void attach();
};

#endif // ACTUATOR_MODULE_H
