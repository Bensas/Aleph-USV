#ifndef ACTUATOR_MODULE_H
#define ACTUATOR_MODULE_H

#include <Arduino.h>

class ActuatorModule {
private:
    // Servo control
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
    
    // DC Motor control (TB6612FNG)
    const int motor_pwm_pin;
    const int motor_standby_pin;
    const int motor_in1_pin;
    const int motor_in2_pin;
    const int motor_ledc_channel;
    int current_motor_speed;
    bool motor_initialized;
    
    static const int MOTOR_LEDC_HZ = 1000;
    static const int MOTOR_LEDC_RES = 8;
    static const int MAX_MOTOR_SPEED = 255;
    
    void setMotorDirection(int speed);

public:
    ActuatorModule(int servo_pin = 25, int servo_channel = 0, 
                   int motor_pwm = 32, int motor_stby = 33, int motor_in1 = 27, int motor_in2 = 26, int motor_channel = 8);
    
    // Servo methods
    bool begin();
    bool isServoInitialized() const { return servo_initialized; }
    
    void setPosition(int angle);
    int getPosition() const;
    
    void center();
    
    void detach();
    void attach();
    
    // Motor methods
    bool beginMotor();
    bool isMotorInitialized() const { return motor_initialized; }
    
    void setMotorSpeed(int speed);  // Range: -255 to 255 (negative = reverse)
    int getMotorSpeed() const;
    void stopMotor();
    void enableMotor();
    void disableMotor();
};

#endif // ACTUATOR_MODULE_H
