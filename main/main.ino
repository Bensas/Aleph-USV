#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>

// WiFi credentials
const char* ssid = "Vitel Tonnet-2.4Ghz";
const char* password = "crazyivan42";

// Pin definitions
#define SERVO_PIN 27
#define MOTOR_IN1 26
#define MOTOR_IN2 25

Servo myServo;
WebServer server(80);

// Current states
int servoAngle = 90; // Default to middle
String motorState = "stop";

void handleRoot() {
  String html = "<html><head><title>ESP32 Control</title></head><body>";
  html += "<h2>Servo Control</h2>";
  html += "<form action='/setServo' method='get'>";
  html += "Angle (0-180): <input type='number' name='angle' min='0' max='180' value='" + String(servoAngle) + "'>";
  html += "<input type='submit' value='Set Servo'>";
  html += "</form>";
  html += "<h2>Motor Control</h2>";
  html += "<form action='/setMotor' method='get'>";
  html += "<button name='action' value='forward'>Forward</button> ";
  html += "<button name='action' value='backward'>Backward</button> ";
  html += "<button name='action' value='stop'>Stop</button>";
  html += "</form>";
  html += "<p>Current Servo Angle: " + String(servoAngle) + "</p>";
  html += "<p>Motor State: " + motorState + "</p>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}

void handleSetServo() {
  if (server.hasArg("angle")) {
    int angle = server.arg("angle").toInt();
    if (angle >= 0 && angle <= 180) {
      servoAngle = angle;
      myServo.write(servoAngle);
    }
  }
  server.sendHeader("Location", "/");
  server.send(303);
}

void handleSetMotor() {
  if (server.hasArg("action")) {
    String action = server.arg("action");
    if (action == "forward") {
      digitalWrite(MOTOR_IN1, HIGH);
      digitalWrite(MOTOR_IN2, LOW);
      motorState = "forward";
    } else if (action == "backward") {
      digitalWrite(MOTOR_IN1, LOW);
      digitalWrite(MOTOR_IN2, HIGH);
      motorState = "backward";
    } else {
      digitalWrite(MOTOR_IN1, LOW);
      digitalWrite(MOTOR_IN2, LOW);
      motorState = "stop";
    }
  }
  server.sendHeader("Location", "/");
  server.send(303);
}

void setup() {
  Serial.begin(115200);
  myServo.attach(SERVO_PIN);
  myServo.write(servoAngle);
  pinMode(MOTOR_IN1, OUTPUT);
  pinMode(MOTOR_IN2, OUTPUT);
  digitalWrite(MOTOR_IN1, LOW);
  digitalWrite(MOTOR_IN2, LOW);

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/setServo", handleSetServo);
  server.on("/setMotor", handleSetMotor);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}
