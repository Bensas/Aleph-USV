// ESP32 + BMP280 + MPU6050 over I2C
// Libraries to install:
//  - Adafruit BMP280 Library
//  - Adafruit Unified Sensor
//  - Adafruit MPU6050

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

// ---- I2C pins (ESP32) ----
constexpr int I2C_SDA = 21;
constexpr int I2C_SCL = 22;

// ---- I2C addresses ----
// BMP280: SDO=GND -> 0x76 (yours), SDO=3V3 -> 0x77
// MPU6050: AD0=GND -> 0x68 (default), AD0=3V3 -> 0x69
constexpr uint8_t BMP_ADDR = 0x76;
constexpr uint8_t MPU_ADDR = 0x68;  // change to 0x69 if AD0 is HIGH

static const float SEA_LEVEL_HPA = 1013.25; // set to local sea-level pressure

Adafruit_BMP280 bmp;       // I2C
Adafruit_MPU6050 mpu;      // I2C

void setup() {
  Serial.begin(115200);
  delay(200);

  // Start I2C on explicit ESP32 pins
  Wire.begin(I2C_SDA, I2C_SCL);
  delay(10);

  // ----- BMP280 init -----
  bool bmp_ok = bmp.begin(BMP_ADDR) || bmp.begin(0x77);
  if (!bmp_ok) {
    Serial.println("ERROR: BMP280 not found at 0x76/0x77. Check SDO/CSB/SDA/SCL wiring.");
    while (1) delay(10);
  }

  // Reasonable, stable sampling config
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,
                  Adafruit_BMP280::SAMPLING_X2,   // temperature
                  Adafruit_BMP280::SAMPLING_X16,  // pressure
                  Adafruit_BMP280::FILTER_X16,
                  Adafruit_BMP280::STANDBY_MS_125);

  // ----- MPU6050 init -----
  // If your AD0 is HIGH, use: if (!mpu.begin(0x69)) { ... }
  if (!mpu.begin(MPU_ADDR)) {
    Serial.println("ERROR: MPU6050 not found at 0x68/0x69. Check AD0/SDA/SCL wiring.");
    while (1) delay(10);
  }

  // Configure ranges & filter (nice defaults)
  mpu.setAccelerometerRange(MPU6050_RANGE_4_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  Serial.println("BMP280 + MPU6050 initialized over I2C.");
}

void loop() {
  // ---- BMP280 reads ----
  float t_bmp   = bmp.readTemperature();         // °C
  float p_hpa   = bmp.readPressure() / 100.0;    // hPa
  float alt_m   = bmp.readAltitude(SEA_LEVEL_HPA);

  // ---- MPU6050 reads ----
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp); // a: m/s^2, g: rad/s, temp.temperature in °C

  // ---- Print nicely ----
  Serial.print("BMP  T: "); Serial.print(t_bmp);  Serial.print(" °C  ");
  Serial.print("P: ");      Serial.print(p_hpa);  Serial.print(" hPa  ");
  Serial.print("Alt: ");    Serial.print(alt_m);  Serial.println(" m");

  Serial.print("MPU  T: "); Serial.print(temp.temperature); Serial.println(" °C");
  Serial.print("Acc  x: "); Serial.print(a.acceleration.x);
  Serial.print("  y: ");    Serial.print(a.acceleration.y);
  Serial.print("  z: ");    Serial.println(a.acceleration.z);

  Serial.print("Gyro x: "); Serial.print(g.gyro.x);
  Serial.print("  y: ");    Serial.print(g.gyro.y);
  Serial.print("  z: ");    Serial.println(g.gyro.z);

  Serial.println("-----------------------------");
  delay(250);
}
