#include "WebModule.h"

WebModule::WebModule(const char* wifi_ssid, const char* wifi_password, SensorModule& sensor_module, ActuatorModule& actuator_module)
    : ssid(wifi_ssid)
    , password(wifi_password)
    , server(80)
    , sensor_module(sensor_module)
    , actuator_module(actuator_module) {
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
    server.on("/servo", [this]() { handleServo(); });
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

void WebModule::handleServo() {
    if (server.hasArg("angle")) {
        int angle = server.arg("angle").toInt();
        actuator_module.setPosition(angle);
        server.send(200, "application/json", "{\"status\":\"success\",\"angle\":" + String(actuator_module.getPosition()) + "}");
    } else {
        server.send(400, "application/json", "{\"status\":\"error\",\"message\":\"Missing angle parameter\"}");
    }
}

void WebModule::handle404() {
    server.send(404, "text/plain", "Not found");
}

String WebModule::generateHTML() {
    String html = R"END_HTML(
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
            <h2>Servo Control</h2>
            <p>Current Position: <span id="servo-position" class="value">90</span>°</p>
            <p>
                <label for="servo-slider">Servo Angle (0-180°):</label><br>
                <input type="range" id="servo-slider" min="0" max="180" value="90" style="width: 100%; margin: 10px 0;">
                <span id="slider-value" class="value">90</span>°
            </p>
            <p>
                <button onclick="centerServo()" style="padding: 10px 20px; margin: 5px;">Center (90°)</button>
            </p>
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
        
        // Servo control functions
        const slider = document.getElementById('servo-slider');
        const sliderValue = document.getElementById('slider-value');
        
        slider.addEventListener('input', function() {
            const angle = slider.value;
            sliderValue.textContent = angle;
            setServoPosition(angle);
        });
        
        function setServoPosition(angle) {
            fetch('/servo?angle=' + angle, { method: 'POST' })
                .then(response => response.json())
                .then(data => {
                    if (data.status === 'success') {
                        document.getElementById('servo-position').textContent = data.angle;
                    } else {
                        console.error('Servo error:', data.message);
                    }
                })
                .catch(error => console.error('Error setting servo position:', error));
        }
        
        function centerServo() {
            slider.value = 90;
            sliderValue.textContent = 90;
            setServoPosition(90);
        }
        
        setInterval(updateValues, 1000);
        updateValues();
    </script>
</body>
</html>
)END_HTML";
    return html;
}

String WebModule::generateJSON() {
    // Update MPU and GPS data
    sensor_module.readMPUData();
    sensor_module.updateGPSData();
    
    // Create JSON string with all sensor data
    String json = "{";
    
    // BMP280 data
    json += "\"bmp\":{";
    json += "\"temperature\":" + String(sensor_module.readBMPTemperature()) + ",";
    json += "\"pressure\":" + String(sensor_module.readBMPPressure()) + ",";
    json += "\"altitude\":" + String(sensor_module.readBMPAltitude());
    json += "},";
    
    // MPU6050 data
    json += "\"mpu\":{";
    json += "\"temperature\":" + String(sensor_module.getMPUTemperature()) + ",";
    json += "\"acceleration\":{";
    json += "\"x\":" + String(sensor_module.getAccelX()) + ",";
    json += "\"y\":" + String(sensor_module.getAccelY()) + ",";
    json += "\"z\":" + String(sensor_module.getAccelZ());
    json += "},";
    json += "\"gyro\":{";
    json += "\"x\":" + String(sensor_module.getGyroX()) + ",";
    json += "\"y\":" + String(sensor_module.getGyroY()) + ",";
    json += "\"z\":" + String(sensor_module.getGyroZ());
    json += "}";
    json += "},";
    
    // GPS data
    json += "\"gps\":{";
    json += "\"valid\":" + String(sensor_module.isGPSDataValid() ? "true" : "false") + ",";
    json += "\"latitude\":" + String(sensor_module.getLatitude(), 6) + ",";
    json += "\"longitude\":" + String(sensor_module.getLongitude(), 6) + ",";
    json += "\"altitude\":" + String(sensor_module.getGPSAltitude()) + ",";
    json += "\"speed\":" + String(sensor_module.getSpeed()) + ",";
    json += "\"satellites\":" + String(sensor_module.getSatellites()) + ",";
    json += "\"time\":\"" + sensor_module.getGPSTime() + "\",";
    json += "\"date\":\"" + sensor_module.getGPSDate() + "\"";
    json += "}";
    json += "}";
    
    return json;
}
