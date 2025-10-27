#include "ActuatorModule.h"

ActuatorModule::ActuatorModule(int servo_pin, int servo_channel, 
                               int motor_pwm, int motor_stby, int motor_in1, int motor_in2, int motor_channel) 
    : servo_pin(servo_pin), ledc_channel(servo_channel), current_position(90), servo_initialized(false),
      motor_pwm_pin(motor_pwm), motor_standby_pin(motor_stby), 
      motor_in1_pin(motor_in1), motor_in2_pin(motor_in2), 
      motor_ledc_channel(motor_channel), current_motor_speed(0), motor_initialized(false) {
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

// ==================== DC MOTOR CONTROL (TB6612FNG) ====================

bool ActuatorModule::beginMotor() {
    // Initialize motor control pins
    pinMode(motor_in1_pin, OUTPUT);
    pinMode(motor_in2_pin, OUTPUT);
    pinMode(motor_standby_pin, OUTPUT);
    
    // Initialize to stopped state
    digitalWrite(motor_in1_pin, LOW);
    digitalWrite(motor_in2_pin, LOW);
    digitalWrite(motor_standby_pin, HIGH);  // Enable motor driver (active high)
    
    // Setup PWM for motor speed control
    ledcSetup(motor_ledc_channel, MOTOR_LEDC_HZ, MOTOR_LEDC_RES);
    ledcAttachPin(motor_pwm_pin, motor_ledc_channel);
    ledcWrite(motor_ledc_channel, 0);  // Start with 0 speed
    
    motor_initialized = true;
    current_motor_speed = 0;
    
    Serial.println("DC Motor initialized:");
    Serial.println("  PWM pin: " + String(motor_pwm_pin) + " (LEDC channel " + String(motor_ledc_channel) + ")");
    Serial.println("  STBY pin: " + String(motor_standby_pin));
    Serial.println("  IN1 pin: " + String(motor_in1_pin));
    Serial.println("  IN2 pin: " + String(motor_in2_pin));
    
    return true;
}

void ActuatorModule::setMotorSpeed(int speed) {
    if (!motor_initialized) {
        Serial.println("ERROR: Motor not initialized");
        return;
    }
    
    // Constrain speed to valid range
    if (speed < -MAX_MOTOR_SPEED || speed > MAX_MOTOR_SPEED) {
        Serial.println("WARNING: Motor speed " + String(speed) + " out of range (-" + 
                      String(MAX_MOTOR_SPEED) + " to " + String(MAX_MOTOR_SPEED) + "). Constraining.");
    }
    speed = constrain(speed, -MAX_MOTOR_SPEED, MAX_MOTOR_SPEED);
    
    // Set direction based on speed sign
    setMotorDirection(speed);
    
    // Set PWM duty cycle based on absolute speed value
    int pwm_value = abs(speed);
    ledcWrite(motor_ledc_channel, pwm_value);
    
    current_motor_speed = speed;
    
    String direction = (speed > 0) ? "FORWARD" : (speed < 0) ? "REVERSE" : "STOPPED";
    Serial.println("Motor speed set to " + String(speed) + " (" + direction + ", PWM: " + String(pwm_value) + ")");
}

void ActuatorModule::setMotorDirection(int speed) {
    if (speed > 0) {
        // Forward: IN1 = HIGH, IN2 = LOW
        digitalWrite(motor_in1_pin, HIGH);
        digitalWrite(motor_in2_pin, LOW);
    } else if (speed < 0) {
        // Reverse: IN1 = LOW, IN2 = HIGH
        digitalWrite(motor_in1_pin, LOW);
        digitalWrite(motor_in2_pin, HIGH);
    } else {
        // Stop/Brake: IN1 = LOW, IN2 = LOW
        digitalWrite(motor_in1_pin, LOW);
        digitalWrite(motor_in2_pin, LOW);
    }
}

int ActuatorModule::getMotorSpeed() const {
    return current_motor_speed;
}

void ActuatorModule::stopMotor() {
    if (!motor_initialized) {
        Serial.println("ERROR: Motor not initialized");
        return;
    }
    
    // Stop motor by setting speed to 0
    digitalWrite(motor_in1_pin, LOW);
    digitalWrite(motor_in2_pin, LOW);
    ledcWrite(motor_ledc_channel, 0);
    current_motor_speed = 0;
    
    Serial.println("Motor stopped");
}

void ActuatorModule::enableMotor() {
    if (!motor_initialized) {
        Serial.println("ERROR: Motor not initialized");
        return;
    }
    
    digitalWrite(motor_standby_pin, HIGH);
    Serial.println("Motor driver enabled");
}

void ActuatorModule::disableMotor() {
    if (!motor_initialized) {
        Serial.println("ERROR: Motor not initialized");
        return;
    }
    
    digitalWrite(motor_standby_pin, LOW);
    current_motor_speed = 0;
    Serial.println("Motor driver disabled (standby mode)");
}
