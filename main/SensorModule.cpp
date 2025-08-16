#include "SensorModule.h"

SensorModule::SensorModule(int sda_pin, int scl_pin, uint8_t bmp_address, uint8_t mpu_address, float sea_level)
    : i2c_sda(sda_pin), i2c_scl(scl_pin), bmp_addr(bmp_address), mpu_addr(mpu_address), 
      sea_level_hpa(sea_level), bmp_initialized(false), mpu_initialized(false), gps_initialized(false) {
    // Initialize GPS data structure
    gps_data.valid = false;
    gps_data.latitude = 0.0;
    gps_data.longitude = 0.0;
    gps_data.altitude = 0.0;
    gps_data.speed = 0.0;
    gps_data.satellites = 0;
    gps_data.time = "";
    gps_data.date = "";
}

bool SensorModule::begin() {
    Wire.begin(i2c_sda, i2c_scl);
    delay(50);

    bool bmp_init = initializeBMP280();
    bool mpu_init = initializeMPU6050();
    bool gps_init = initializeGPS();

    bmp_initialized = bmp_init;
    mpu_initialized = mpu_init;
    gps_initialized = gps_init;

    return bmp_init || mpu_init || gps_init;
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

bool SensorModule::initializeGPS() {
    Serial2.begin(9600, SERIAL_8N1, 17, 16);  // RX pin 16, TX pin 17
    delay(100);
    
    Serial.println("GPS initialized using Serial2 with TinyGPS++ library (RX: GPIO16, TX: GPIO17)");
    return true; // GPS doesn't have a direct way to verify connection, so assume success
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

void SensorModule::updateGPSData() {
    while (Serial2.available()) {
        char c = Serial2.read();
        // Serial.print(c);
        if (gps.encode(c)) {
            updateGPSDataFromLibrary();
        }
    }
}

GPSData SensorModule::getGPSData() const {
    return gps_data;
}

bool SensorModule::isGPSDataValid() const {
    return gps_data.valid;
}

double SensorModule::getLatitude() const {
    return gps_data.latitude;
}

double SensorModule::getLongitude() const {
    return gps_data.longitude;
}

double SensorModule::getGPSAltitude() const {
    return gps_data.altitude;
}

float SensorModule::getSpeed() const {
    return gps_data.speed;
}

int SensorModule::getSatellites() const {
    return gps_data.satellites;
}

String SensorModule::getGPSTime() const {
    return gps_data.time;
}

String SensorModule::getGPSDate() const {
    return gps_data.date;
}

void SensorModule::updateGPSDataFromLibrary() {
    gps_data.valid = gps.location.isValid();
    
    if (gps_data.valid) {
        gps_data.latitude = gps.location.lat();
        gps_data.longitude = gps.location.lng();
    } else {
        gps_data.latitude = 13.37;
        gps_data.longitude = 13.37;
    }
    
    if (gps.altitude.isValid()) {
        gps_data.altitude = gps.altitude.meters();
    } else {
        gps_data.altitude = 13.37;
    }
    
    if (gps.speed.isValid()) {
        gps_data.speed = gps.speed.knots();
    } else {
        gps_data.speed = 13.37;
    }
    
    if (gps.satellites.isValid()) {
        gps_data.satellites = gps.satellites.value();
    } else {
        gps_data.satellites = -1337;
    }
    
    if (gps.time.isValid()) {
        char timeBuffer[10];
        sprintf(timeBuffer, "%02d%02d%02d", gps.time.hour(), gps.time.minute(), gps.time.second());
        gps_data.time = String(timeBuffer);
    } else {
        gps_data.time = "1337:00:00";
    }
    
    if (gps.date.isValid()) {
        char dateBuffer[8];
        sprintf(dateBuffer, "%02d%02d%02d", gps.date.day(), gps.date.month(), gps.date.year() % 100);
        gps_data.date = String(dateBuffer);
    } else {
        gps_data.date = "13/37";
    }
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

    // Print GPS data
    Serial.print("GPS  Valid: "); Serial.print(gps_data.valid ? "Yes" : "No");
    Serial.print("  Sats: "); Serial.println(gps_data.satellites);
    if (gps_data.valid) {
        Serial.print("Lat: "); Serial.print(gps_data.latitude, 6);
        Serial.print("  Lon: "); Serial.print(gps_data.longitude, 6);
        Serial.print("  Alt: "); Serial.print(gps_data.altitude); Serial.println(" m");
        Serial.print("Speed: "); Serial.print(gps_data.speed); Serial.print(" knots  ");
        Serial.print("Time: "); Serial.print(gps_data.time);
        Serial.print("  Date: "); Serial.println(gps_data.date);
    }

    Serial.println("-----------------------------");
}
