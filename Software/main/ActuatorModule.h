#ifndef ACTUATOR_MODULE_H
#define ACTUATOR_MODULE_H

#include "AlephServo.h"
#include "AlephMotor.h"

class ActuatorModule {
public:
    AlephServo servo;
    AlephMotor motor;

    ActuatorModule(int servo_pin = 25, int servo_channel = 0,
                   int motor_pwm = 32, int motor_channel = 8);
};

#endif // ACTUATOR_MODULE_H
