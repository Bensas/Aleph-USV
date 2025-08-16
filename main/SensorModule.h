#ifndef SENSOR_MODULE_H
#define SENSOR_MODULE_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <TinyGPS++.h>

struct GPSData {
    bool valid;
    double latitude;
    double longitude;
    double altitude;
    float speed;
    int satellites;
    String time;
    String date;
};

class SensorModule {
private:
    const int i2c_sda;
    const int i2c_scl; 
    const uint8_t bmp_addr;
    const uint8_t mpu_addr;

    Adafruit_BMP280 bmp;
    Adafruit_MPU6050 mpu;
    TinyGPSPlus gps;
    
    sensors_event_t accel, gyro, temp;
    const float sea_level_hpa;
    GPSData gps_data;
    
    bool bmp_initialized;
    bool mpu_initialized;
    bool gps_initialized;

private:
    bool initializeBMP280();
    bool initializeMPU6050();
    bool initializeGPS();
    void updateGPSDataFromLibrary();

public:
    SensorModule(
      int sda_pin = 21,
      int scl_pin = 22, 
      uint8_t bmp_address = 0x76, 
      uint8_t mpu_address = 0x68,
      float sea_level = 1013.25
    );
    
    bool begin();
    bool isBMPInitialized() const { return bmp_initialized; }
    bool isMPUInitialized() const { return mpu_initialized; }
    bool isGPSInitialized() const { return gps_initialized; }
    
    float readBMPTemperature();    // Returns temperature in °C
    float readBMPPressure();       // Returns pressure in hPa
    float readBMPAltitude();       // Returns altitude in meters
    
    void readMPUData();            // Updates internal sensor data
    float getMPUTemperature();  // Returns MPU temperature in °C
    float getAccelX();       // Returns acceleration X in m/s²
    float getAccelY();       // Returns acceleration Y in m/s²
    float getAccelZ();       // Returns acceleration Z in m/s²
    float getGyroX();        // Returns gyro X in rad/s
    float getGyroY();        // Returns gyro Y in rad/s
    float getGyroZ();        // Returns gyro Z in rad/s
    
    void updateGPSData();           // Reads and parses GPS data
    GPSData getGPSData() const;     // Returns current GPS data
    bool isGPSDataValid() const;    // Returns if GPS has valid fix
    double getLatitude() const;     // Returns latitude
    double getLongitude() const;    // Returns longitude
    double getGPSAltitude() const;  // Returns GPS altitude
    float getSpeed() const;         // Returns speed in knots
    int getSatellites() const;      // Returns number of satellites
    String getGPSTime() const;      // Returns GPS time
    String getGPSDate() const;      // Returns GPS date
    
    void printSensorData();  // Prints all sensor data to Serial
};

#endif // SENSOR_MODULE_H
