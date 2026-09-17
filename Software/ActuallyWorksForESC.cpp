
// Configuration
const int escPin = 32;
const int freq = 50;           // 50Hz for standard ESC PWM
const int ledChannel = 0;      // PWM channel 0
const int resolution = 16;     // 16-bit precision

// Duty cycle calculations for 16-bit at 50Hz
const uint32_t MIN_DUTY = 3277;  // ~1000us (Bottom position)
const uint32_t MAX_DUTY = 6554;  // ~2000us (Top position)

void setThrottle(uint32_t microseconds) {
    // Convert microseconds to 16-bit duty cycle
    uint32_t duty = (microseconds * 65536) / 20000;
    ledcWrite(ledChannel, duty);
}

void setup() {
    Serial.begin(115200);

    // Initialize LEDC PWM
    ledcSetup(ledChannel, freq, resolution);
    ledcAttachPin(escPin, ledChannel);

    Serial.println("--- DYS Manual Arming Sequence ---");
    
    // 1. Start at Bottom Position (Manual: [cite: 50])
    Serial.println("Sending Low Signal to Arm...");
    setThrottle(1000); 
    
    // 2. Wait for self-detection (Manual: [cite: 49, 53])
    // You should hear the battery cell count beeps here 
    delay(5000); 
    
    Serial.println("If beeping stopped, ESC is ready.");
}

void loop() {
    // Slow ramp to test movement
    for(int i = 1000; i <= 2000; i += 2) {
        setThrottle(i);
        delay(20);
    }
    delay(1000);
    setThrottle(1000);
    delay(2000);
}
