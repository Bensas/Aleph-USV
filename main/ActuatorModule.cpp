#include "ActuatorModule.h"

ActuatorModule::ActuatorModule(int pin) 
    : servo_pin(pin), current_position(90), servo_initialized(false) {
}

bool ActuatorModule::begin() {
    servo.attach(servo_pin);
    delay(50);
    
    servo.write(90);
    current_position = 90;
    delay(500);
    
    servo_initialized = true;
    Serial.println("Servo initialized on pin " + String(servo_pin) + " at center position (90°)");
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
    servo.write(angle);
    current_position = angle;
    
    Serial.println("Servo moved to " + String(angle) + "°");
}

int ActuatorModule::getPosition() const {
    return current_position;
}

void ActuatorModule::center() {
    setPosition(90);
}

void ActuatorModule::detach() {
    if (servo_initialized) {
        servo.detach();
        Serial.println("Servo detached from pin " + String(servo_pin));
    }
}

void ActuatorModule::attach() {
    if (servo_initialized) {
        servo.attach(servo_pin);
        Serial.println("Servo re-attached to pin " + String(servo_pin));
    }
}
