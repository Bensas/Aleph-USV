#include "AlephMotor.h"

AlephMotor::AlephMotor(int pwm_pin, int channel)
    : pwm_pin(pwm_pin), ledc_channel(channel), current_speed(0), initialized(false) {
}

uint32_t AlephMotor::microsecondsToDutyCycle(int us) const {
    // For 50Hz: period = 20ms = 20000µs
    // resolution=16 bits => max count = 65535
    // duty = microseconds / 20000 * 65535
    const uint32_t period_us = 1000000UL / LEDC_HZ;  // 20000 µs
    const uint32_t max_duty = (1UL << LEDC_RES) - 1; // 65535
    return (uint32_t)((uint64_t)us * max_duty / period_us);
}

bool AlephMotor::begin() {
    ledcSetup(ledc_channel, LEDC_HZ, LEDC_RES);
    ledcAttachPin(pwm_pin, ledc_channel);

    // Send minimum throttle to arm the ESC
    ledcWrite(ledc_channel, microsecondsToDutyCycle(ESC_MIN_MICROSECONDS));

    initialized = true;
    current_speed = 0;

    Serial.println("Arming ESC with " + String(ESC_MIN_MICROSECONDS) + "µs pulse...");
    delay(5000);  // Allow ESC to arm
    Serial.println("ESC armed and ready!");
    Serial.println("ESC Motor initialized on PWM pin " + String(pwm_pin) + " (LEDC channel " + String(ledc_channel) + ")");
    return true;
}

void AlephMotor::setSpeed(int speed) {
    if (!initialized) {
        Serial.println("AlephMotor::setSpeed() ERROR: Motor not initialized");
        return;
    }

    speed = constrain(speed, 0, 100);
    int pulse_us = map(speed, 0, 100, ESC_MIN_MICROSECONDS, ESC_MAX_MICROSECONDS);
    uint32_t duty = microsecondsToDutyCycle(pulse_us);
    ledcWrite(ledc_channel, duty);
    current_speed = speed;

    Serial.println("Motor speed set to " + String(speed) + "% (pulse: " + String(pulse_us) + "µs, duty: " + String(duty) + ")");
}