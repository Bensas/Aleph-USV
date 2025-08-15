#include "SensorModule.h"

SensorModule::SensorModule(int sda_pin, int scl_pin, uint8_t bmp_address, uint8_t mpu_address, float sea_level)
    : i2c_sda(sda_pin), i2c_scl(scl_pin), bmp_addr(bmp_address), mpu_addr(mpu_address), 
      sea_level_hpa(sea_level), bmp_initialized(false), mpu_initialized(false) {
}

bool SensorModule::begin() {
    Wire.begin(i2c_sda, i2c_scl);
    delay(50);

    bool bmp_initialized = initializeBMP280();
    bool mpu_initialized = initializeMPU6050();

    return bmp_initialized || mpu_initialized;
}

bool SensorModule::initializeBMP280() {
    bmp_initialized = bmp.begin(bmp_addr) || bmp.begin(0x77);
    if (!bmp_initialized) {
        Serial.println("ERROR: BMP280 not found at 0x76/0x77. Check SDO/CSB/SDA/SCL wiring.");
        return false;
    }
    
    bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,
                    Adafruit_BMP280::SAMPLING_X2,   // temperature
                    Adafruit_BMP280::SAMPLING_X16,  // pressure
                    Adafruit_BMP280::FILTER_X16,
                    Adafruit_BMP280::STANDBY_MS_125);
    
    Serial.println("BMP280 initialized over I2C");
    return true;
}

bool SensorModule::initializeMPU6050() {
    mpu_initialized = mpu.begin(mpu_addr);
    if (!mpu_initialized) {
        Serial.println("ERROR: MPU6050 not found at 0x68/0x69. Check AD0/SDA/SCL wiring.");
        return false;
    }
    
    mpu.setAccelerometerRange(MPU6050_RANGE_4_G);
    mpu.setGyroRange(MPU6050_RANGE_500_DEG);
    mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
    
    Serial.println("MPU6050 initialized over I2C");
    return true;
}

float SensorModule::readBMPTemperature() {
    return bmp.readTemperature();
}

float SensorModule::readBMPPressure(){
    return bmp.readPressure() / 100.0;  // Convert Pa to hPa
}

float SensorModule::readBMPAltitude() {
    return bmp.readAltitude(sea_level_hpa);
}

void SensorModule::readMPUData() {
    mpu.getEvent(&accel, &gyro, &temp);
}

float SensorModule::getMPUTemperature() {
    return temp.temperature;
}

float SensorModule::getAccelX() {
    return accel.acceleration.x;
}

float SensorModule::getAccelY() {
    return accel.acceleration.y;
}

float SensorModule::getAccelZ() {
    return accel.acceleration.z;
}

float SensorModule::getGyroX() {
    return gyro.gyro.x;
}

float SensorModule::getGyroY() {
    return gyro.gyro.y;
}

float SensorModule::getGyroZ() {
    return gyro.gyro.z;
}

void SensorModule::printSensorData() {
    // Print BMP280 data
    Serial.print("BMP  T: "); Serial.print(readBMPTemperature());  Serial.print(" °C  ");
    Serial.print("P: ");      Serial.print(readBMPPressure());     Serial.print(" hPa  ");
    Serial.print("Alt: ");    Serial.print(readBMPAltitude());     Serial.println(" m");

    // Print MPU6050 data
    Serial.print("MPU  T: "); Serial.print(getMPUTemperature()); Serial.println(" °C");
    Serial.print("Acc  x: "); Serial.print(getAccelX());
    Serial.print("  y: ");    Serial.print(getAccelY());
    Serial.print("  z: ");    Serial.println(getAccelZ());

    Serial.print("Gyro x: "); Serial.print(getGyroX());
    Serial.print("  y: ");    Serial.print(getGyroY());
    Serial.print("  z: ");    Serial.println(getGyroZ());

    Serial.println("-----------------------------");
}
