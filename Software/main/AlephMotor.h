#ifndef ALEPH_MOTOR_H
#define ALEPH_MOTOR_H

#include <Arduino.h>

class AlephMotor {
private:
    const int pwm_pin;
    const int ledc_channel;
    int current_speed;
    bool initialized;

    static const int LEDC_HZ = 50;          // 50 Hz for ESC
    static const int LEDC_RES = 16;         // 16-bit resolution
    static const int MAX_SPEED = 100;       // 0-100% throttle
    static const int ESC_MIN_MICROSECONDS = 1000; // Minimum pulse width (stopped)
    static const int ESC_MAX_MICROSECONDS = 2000; // Maximum pulse width (full throttle)

    uint32_t microsecondsToDutyCycle(int us) const;

public:
    AlephMotor(int pwm_pin = 32, int channel = 8);

    bool begin();
    bool isInitialized() const { return initialized; }

    void setSpeed(int speed);  // Range: 0 to 100 (percentage throttle)
    int getSpeed() const;
    void stop();
};

#endif // ALEPH_MOTOR_H
