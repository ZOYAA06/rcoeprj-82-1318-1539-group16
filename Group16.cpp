#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>

const char* ssid = "A";
const char* password = "12345678";

WebServer server(80);
Servo baseServo;
Servo shoulderServo;
Servo elbowServo;
Servo clutcherServo;
Servo gripperServo;  // New gripper servo

int baseAngle = 0;
int shoulderAngle = 90;
int elbowAngle = 0;
int clutcherAngle = 90;
int gripperAngle = 0;  // New gripper angle

void moveServoSmooth(Servo &servo, int currentAngle, int targetAngle) {
  int step = (targetAngle > currentAngle) ? 1 : -1;
  for (int angle = currentAngle; angle != targetAngle; angle += step) {
    servo.write(angle);
    delay(10); // Adjust this delay for speed control
  }
  servo.write(targetAngle);
}

void handleRoot() {
  String html = "<html><body><h2>Robotic Arm Control</h2>"
                "<h3>Base</h3>"
                "<input type='range' min='0' max='180' value='" + String(baseAngle) + "' onchange='sendAngle(this.value, \"base\")' /><br>"
                "<h3>Shoulder</h3>"
                "<input type='range' min='0' max='180' value='" + String(shoulderAngle) + "' onchange='sendAngle(this.value, \"shoulder\")' /><br>"
                "<h3>Elbow</h3>"
                "<input type='range' min='0' max='180' value='" + String(elbowAngle) + "' onchange='sendAngle(this.value, \"elbow\")' /><br>"
                "<h3>Clutcher Rotate</h3>"
                "<input type='range' min='0' max='180' value='" + String(clutcherAngle) + "' onchange='sendAngle(this.value, \"clutcher\")' /><br>"
                "<h3>Gripper (Pick/Drop)</h3>"
                "<input type='range' min='0' max='180' value='" + String(gripperAngle) + "' onchange='sendAngle(this.value, \"gripper\")' /><br>"
                "<script>"
                "function sendAngle(val, joint) {"
                " fetch('/set?' + joint + '=' + val);"
                "}"
                "</script></body></html>";
  server.send(200, "text/html", html);
}

void handleSet() {
  if (server.hasArg("base")) {
    int target = server.arg("base").toInt();
    target = constrain(target, 0, 180);
    moveServoSmooth(baseServo, baseAngle, target);
    baseAngle = target;
    Serial.println("Base angle: " + String(baseAngle));
  }

  if (server.hasArg("shoulder")) {
    int target = server.arg("shoulder").toInt();
    target = constrain(target, 0, 180);
    moveServoSmooth(shoulderServo, shoulderAngle, target);
    shoulderAngle = target;
    Serial.println("Shoulder angle: " + String(shoulderAngle));
  }

  if (server.hasArg("elbow")) {
    int target = server.arg("elbow").toInt();
    target = constrain(target, 0, 180);
    moveServoSmooth(elbowServo, elbowAngle, target);
    elbowAngle = target;
    Serial.println("Elbow angle: " + String(elbowAngle));
  }

  if (server.hasArg("clutcher")) {
    int target = server.arg("clutcher").toInt();
    target = constrain(target, 0, 180);
    moveServoSmooth(clutcherServo, clutcherAngle, target);
    clutcherAngle = target;
    Serial.println("Clutcher angle: " + String(clutcherAngle));
  }

  if (server.hasArg("gripper")) {
    int target = server.arg("gripper").toInt();
    target = constrain(target, 0, 180);
    moveServoSmooth(gripperServo, gripperAngle, target);
    gripperAngle = target;
    Serial.println("Gripper angle: " + String(gripperAngle));
  }

  server.send(200, "text/plain", "OK");
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected! IP address: ");
  Serial.println(WiFi.localIP());

  baseServo.setPeriodHertz(50);
  shoulderServo.setPeriodHertz(50);
  elbowServo.setPeriodHertz(50);
  clutcherServo.setPeriodHertz(50);
  gripperServo.setPeriodHertz(50);  // New servo

  baseServo.attach(13, 500, 2400);
  shoulderServo.attach(12, 500, 2400);
  elbowServo.attach(14, 500, 2400);
  clutcherServo.attach(27, 500, 2400);
  gripperServo.attach(26, 500, 2400);  // Attach to GPIO 26

  baseServo.write(baseAngle);
  shoulderServo.write(shoulderAngle);
  elbowServo.write(elbowAngle);
  clutcherServo.write(clutcherAngle);
  gripperServo.write(gripperAngle);  // Initialize gripper

  server.on("/", handleRoot);
  server.on("/set", handleSet);
  server.begin();
  Serial.println("Server started");
}

void loop() {
  server.handleClient();
}