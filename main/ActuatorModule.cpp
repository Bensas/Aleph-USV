#include "ActuatorModule.h"

ActuatorModule::ActuatorModule(int pin, int channel) 
    : servo_pin(pin), ledc_channel(channel), current_position(90), servo_initialized(false) {
}

bool ActuatorModule::begin() {

    ledcSetup(ledc_channel, LEDC_HZ, LEDC_RES);
    ledcAttachPin(servo_pin, ledc_channel);
    delay(50);
    
    setPosition(90);
    delay(500);
    
    servo_initialized = true;
    Serial.println("Servo initialized on pin " + String(servo_pin) + " (LEDC channel " + String(ledc_channel) + ") at center position (90°)");
    return true;
}

void ActuatorModule::setPosition(int angle) {
    if (!servo_initialized) {
        Serial.println("ERROR: Servo not initialized");
        return;
    }

    if (angle < MIN_POSITION || angle > MAX_POSITION) {
      Serial.println("WARNING: Servo angle " + String(angle) + "° out of range (" + 
                    String(MIN_POSITION) + "-" + String(MAX_POSITION) + "°). Constraining.");
    }
    
    angle = constrain(angle, MIN_POSITION, MAX_POSITION);
    
    int pulseWidth = angleToUs(angle);
    uint32_t duty = microsecondsToDutyCycle(pulseWidth);
    ledcWrite(ledc_channel, duty);
    
    current_position = angle;
    
    Serial.println("Servo moved to " + String(angle) + "° (pulse: " + String(pulseWidth) + "µs, duty: " + String(duty) + ")");
}

int ActuatorModule::getPosition() const {
    return current_position;
}

void ActuatorModule::center() {
    setPosition(90);
}

void ActuatorModule::detach() {
    if (servo_initialized) {
        ledcDetachPin(servo_pin);
        Serial.println("Servo detached from pin " + String(servo_pin));
    }
}

void ActuatorModule::attach() {
    if (servo_initialized) {
        ledcAttachPin(servo_pin, ledc_channel);
        Serial.println("Servo re-attached to pin " + String(servo_pin) + " (LEDC channel " + String(ledc_channel) + ")");
    }
}

uint32_t ActuatorModule::microsecondsToDutyCycle(int us) const {
    const uint32_t period_us = 1000000UL / LEDC_HZ;  // e.g. 1e6 / 50 = 20000 µs
    const uint32_t max_duty = (1UL << LEDC_RES) - 1;
    return (uint32_t)((uint64_t)us * max_duty / period_us);
}

// Helper function: Convert an angle (0–180°) → pulse width (µs)
int ActuatorModule::angleToUs(int angle) const {
    return map(angle, MIN_POSITION, MAX_POSITION, SERVO_MIN_MICROSECONDS, SERVO_MAX_MICROSECONDS);
}
