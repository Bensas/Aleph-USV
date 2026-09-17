#ifndef ALEPH_SERVO_H
#define ALEPH_SERVO_H

#include <Arduino.h>

class AlephServo {
private:
    const int pwm_pin;
    const int ledc_channel;
    int current_position;
    bool initialized;

    static const int MIN_POSITION = 0;
    static const int MAX_POSITION = 180;

    static const int LEDC_HZ = 50;
    static const int LEDC_RES = 16;

    static const int SERVO_MIN_MICROSECONDS = 1000;
    static const int SERVO_MAX_MICROSECONDS = 2000;

    uint32_t microsecondsToDutyCycle(int us) const;
    int angleToUs(int angle) const;

public:
    AlephServo(int pwm_pin = 25, int channel = 0);

    bool begin();
    bool isInitialized() const { return initialized; }

    void setPosition(int angle);
    int getPosition() const;

    void center();

    void detach();
    void attach();
};

#endif // ALEPH_SERVO_H
