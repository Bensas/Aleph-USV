#include "ActuatorModule.h"

ActuatorModule::ActuatorModule(int servo_pin, int servo_channel,
                               int motor_pwm, int motor_channel)
    : servo(servo_pin, servo_channel),
      motor(motor_pwm, motor_channel) {
}
