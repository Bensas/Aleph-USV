#include "WebModule.h"

WebModule::WebModule(const char* wifi_ssid, const char* wifi_password, SensorModule& sensor_module)
    : ssid(wifi_ssid)
    , password(wifi_password)
    , server(80)
    , sensors(sensor_module) {
}

bool WebModule::begin() {
    // Connect to WiFi
    Serial.print("Connecting to WiFi");
    WiFi.begin(ssid, password);
    
    // Wait for connection with timeout
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
        delay(500);
        Serial.print(".");
        attempts++;
    }
    
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("\nFailed to connect to WiFi!");
        return false;
    }
    
    Serial.println("\nConnected to WiFi");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    
    // Setup server routes
    server.on("/", [this]() { handleRoot(); });
    server.on("/data", [this]() { handleData(); });
    server.onNotFound([this]() { handle404(); });
    
    // Start server
    server.begin();
    Serial.println("HTTP server started");
    return true;
}

void WebModule::update() {
    server.handleClient();
}

void WebModule::handleRoot() {
    server.send(200, "text/html", generateHTML());
}

void WebModule::handleData() {
    server.send(200, "application/json", generateJSON());
}

void WebModule::handle404() {
    server.send(404, "text/plain", "Not found");
}

String WebModule::generateHTML() {
    String html = R"(
<!DOCTYPE html>
<html>
<head>
    <title>ESP32 Sensor Dashboard</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
        body { font-family: Arial, sans-serif; margin: 20px; }
        .container { max-width: 800px; margin: 0 auto; }
        .sensor-box {
            border: 1px solid #ddd;
            padding: 15px;
            margin: 10px 0;
            border-radius: 5px;
        }
        .value { font-weight: bold; }
    </style>
</head>
<body>
    <div class="container">
        <h1>ESP32 Sensor Dashboard</h1>
        <div class="sensor-box">
            <h2>GPS (GY-GPS6MV2)</h2>
            <p>Status: <span id="gps-status" class="value">--</span></p>
            <p>Satellites: <span id="gps-satellites" class="value">--</span></p>
            <p>Latitude: <span id="gps-lat" class="value">--</span>°</p>
            <p>Longitude: <span id="gps-lon" class="value">--</span>°</p>
            <p>Altitude: <span id="gps-altitude" class="value">--</span> m</p>
            <p>Speed: <span id="gps-speed" class="value">--</span> knots</p>
            <p>Time: <span id="gps-time" class="value">--</span></p>
            <p>Date: <span id="gps-date" class="value">--</span></p>
        </div>
        <div class="sensor-box">
            <h2>BMP280</h2>
            <p>Temperature: <span id="bmp-temp" class="value">--</span> °C</p>
            <p>Pressure: <span id="bmp-pressure" class="value">--</span> hPa</p>
            <p>Altitude: <span id="bmp-altitude" class="value">--</span> m</p>
        </div>
        <div class="sensor-box">
            <h2>MPU6050</h2>
            <p>Temperature: <span id="mpu-temp" class="value">--</span> °C</p>
            <h3>Acceleration (m/s²)</h3>
            <p>X: <span id="acc-x" class="value">--</span></p>
            <p>Y: <span id="acc-y" class="value">--</span></p>
            <p>Z: <span id="acc-z" class="value">--</span></p>
            <h3>Gyroscope (rad/s)</h3>
            <p>X: <span id="gyro-x" class="value">--</span></p>
            <p>Y: <span id="gyro-y" class="value">--</span></p>
            <p>Z: <span id="gyro-z" class="value">--</span></p>
        </div>
    </div>
    <script>
        function updateValues() {
            fetch('/data')
                .then(response => response.json())
                .then(data => {
                    // Update GPS data
                    document.getElementById('gps-status').textContent = data.gps.valid ? 'Valid Fix' : 'No Fix';
                    document.getElementById('gps-satellites').textContent = data.gps.satellites;
                    document.getElementById('gps-lat').textContent = data.gps.valid ? data.gps.latitude.toFixed(6) : '--';
                    document.getElementById('gps-lon').textContent = data.gps.valid ? data.gps.longitude.toFixed(6) : '--';
                    document.getElementById('gps-altitude').textContent = data.gps.valid ? data.gps.altitude.toFixed(1) : '--';
                    document.getElementById('gps-speed').textContent = data.gps.valid ? data.gps.speed.toFixed(1) : '--';
                    document.getElementById('gps-time').textContent = data.gps.time || '--';
                    document.getElementById('gps-date').textContent = data.gps.date || '--';
                    
                    // Update BMP280 data
                    document.getElementById('bmp-temp').textContent = data.bmp.temperature.toFixed(2);
                    document.getElementById('bmp-pressure').textContent = data.bmp.pressure.toFixed(2);
                    document.getElementById('bmp-altitude').textContent = data.bmp.altitude.toFixed(2);
                    
                    // Update MPU6050 data
                    document.getElementById('mpu-temp').textContent = data.mpu.temperature.toFixed(2);
                    document.getElementById('acc-x').textContent = data.mpu.acceleration.x.toFixed(3);
                    document.getElementById('acc-y').textContent = data.mpu.acceleration.y.toFixed(3);
                    document.getElementById('acc-z').textContent = data.mpu.acceleration.z.toFixed(3);
                    document.getElementById('gyro-x').textContent = data.mpu.gyro.x.toFixed(3);
                    document.getElementById('gyro-y').textContent = data.mpu.gyro.y.toFixed(3);
                    document.getElementById('gyro-z').textContent = data.mpu.gyro.z.toFixed(3);
                })
                .catch(error => console.error('Error fetching data:', error));
        }
        
        // Update values every second
        setInterval(updateValues, 1000);
        // Initial update
        updateValues();
    </script>
</body>
</html>
)";
    return html;
}

String WebModule::generateJSON() {
    // Update MPU and GPS data
    sensors.readMPUData();
    sensors.updateGPSData();
    
    // Create JSON string with all sensor data
    String json = "{";
    
    // BMP280 data
    json += "\"bmp\":{";
    json += "\"temperature\":" + String(sensors.readBMPTemperature()) + ",";
    json += "\"pressure\":" + String(sensors.readBMPPressure()) + ",";
    json += "\"altitude\":" + String(sensors.readBMPAltitude());
    json += "},";
    
    // MPU6050 data
    json += "\"mpu\":{";
    json += "\"temperature\":" + String(sensors.getMPUTemperature()) + ",";
    json += "\"acceleration\":{";
    json += "\"x\":" + String(sensors.getAccelX()) + ",";
    json += "\"y\":" + String(sensors.getAccelY()) + ",";
    json += "\"z\":" + String(sensors.getAccelZ());
    json += "},";
    json += "\"gyro\":{";
    json += "\"x\":" + String(sensors.getGyroX()) + ",";
    json += "\"y\":" + String(sensors.getGyroY()) + ",";
    json += "\"z\":" + String(sensors.getGyroZ());
    json += "}";
    json += "},";
    
    // GPS data
    json += "\"gps\":{";
    json += "\"valid\":" + String(sensors.isGPSDataValid() ? "true" : "false") + ",";
    json += "\"latitude\":" + String(sensors.getLatitude(), 6) + ",";
    json += "\"longitude\":" + String(sensors.getLongitude(), 6) + ",";
    json += "\"altitude\":" + String(sensors.getGPSAltitude()) + ",";
    json += "\"speed\":" + String(sensors.getSpeed()) + ",";
    json += "\"satellites\":" + String(sensors.getSatellites()) + ",";
    json += "\"time\":\"" + sensors.getGPSTime() + "\",";
    json += "\"date\":\"" + sensors.getGPSDate() + "\"";
    json += "}";
    json += "}";
    
    return json;
}
