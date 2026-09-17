#include "AlephServo.h"

AlephServo::AlephServo(int pwm_pin, int channel)
    : pwm_pin(pwm_pin), ledc_channel(channel), current_position(90), initialized(false) {
}

bool AlephServo::begin() {
    ledcSetup(ledc_channel, LEDC_HZ, LEDC_RES);
    ledcAttachPin(pwm_pin, ledc_channel);
    delay(50);

    initialized = true;
    setPosition(90);
    delay(500);

    Serial.println("Servo initialized on pin " + String(pwm_pin) + " (LEDC channel " + String(ledc_channel) + ") at center position (90°)");
    return true;
}

void AlephServo::setPosition(int angle) {
    if (!initialized) {
        Serial.println("ERROR: Servo not initialized");
        return;
    }

    angle = constrain(angle, MIN_POSITION, MAX_POSITION);

    int pulseWidth = angleToUs(angle);
    uint32_t duty = microsecondsToDutyCycle(pulseWidth);
    ledcWrite(ledc_channel, duty);

    current_position = angle;

    Serial.println("Servo moved to " + String(angle) + "° (pulse: " + String(pulseWidth) + "µs, duty: " + String(duty) + ")");
}

int AlephServo::getPosition() const {
    return current_position;
}

void AlephServo::center() {
    setPosition(90);
}

void AlephServo::detach() {
    if (initialized) {
        ledcDetachPin(pwm_pin);
        Serial.println("Servo detached from pin " + String(pwm_pin));
    }
}

void AlephServo::attach() {
    if (initialized) {
        ledcAttachPin(pwm_pin, ledc_channel);
        Serial.println("Servo re-attached to pin " + String(pwm_pin) + " (LEDC channel " + String(ledc_channel) + ")");
    }
}

uint32_t AlephServo::microsecondsToDutyCycle(int us) const {
    const uint32_t period_us = 1000000UL / LEDC_HZ;  // e.g. 1e6 / 50 = 20000 µs
    const uint32_t max_duty = (1UL << LEDC_RES) - 1;
    return (uint32_t)((uint64_t)us * max_duty / period_us);
}

int AlephServo::angleToUs(int angle) const {
    return map(angle, MIN_POSITION, MAX_POSITION, SERVO_MIN_MICROSECONDS, SERVO_MAX_MICROSECONDS);
}
