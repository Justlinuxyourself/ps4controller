/*
  Advanced PS4 Arduino Car Controller
  Enhanced version with speed control, sensor integration, and advanced features
  
  This version includes:
  - Variable speed control from Android app
  - Ultrasonic sensor for obstacle avoidance
  - Servo motor for camera/sensor pan
  - LED indicators
  - Enhanced safety features
  
  Hardware connections:
  - Motor Driver (L298N): pins 3, 5, 6, 9, 10, 11
  - Ultrasonic Sensor (HC-SR04): pins 7, 8
  - Servo Motor: pin 2
  - Status LEDs: pins 12, 13
  
  Commands from app:
  'F' - Forward, 'B' - Backward, 'L' - Left, 'R' - Right, 'S' - Stop
  'SPEED:xxx' - Set speed (0-255)
  'SERVO:xxx' - Set servo angle (0-180)
*/

#include <Servo.h>

// Motor control pins
const int leftMotorPin1 = 5;
const int leftMotorPin2 = 6;
const int rightMotorPin1 = 9;
const int rightMotorPin2 = 10;
const int leftMotorEnable = 3;
const int rightMotorEnable = 11;

// Ultrasonic sensor pins
const int trigPin = 7;
const int echoPin = 8;

// Servo motor
Servo cameraServo;
const int servoPin = 2;

// LED indicators
const int statusLED = 13;      // Built-in LED
const int obstacleWarningLED = 12; // External LED for obstacle warning

// Speed and control variables
int currentSpeed = 180;
int turnSpeed = 120;
int minSpeed = 80;
int maxSpeed = 255;

// Safety and sensor variables
const int obstacleThreshold = 20; // cm
bool obstacleAvoidanceEnabled = true;
unsigned long lastCommandTime = 0;
const unsigned long commandTimeout = 1500;

// Command processing
String inputString = "";
bool stringComplete = false;
char lastCommand = 'S';

void setup() {
  Serial.begin(9600);
  
  // Initialize motor pins
  pinMode(leftMotorPin1, OUTPUT);
  pinMode(leftMotorPin2, OUTPUT);
  pinMode(rightMotorPin1, OUTPUT);
  pinMode(rightMotorPin2, OUTPUT);
  pinMode(leftMotorEnable, OUTPUT);
  pinMode(rightMotorEnable, OUTPUT);
  
  // Initialize sensor pins
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
  // Initialize LED pins
  pinMode(statusLED, OUTPUT);
  pinMode(obstacleWarningLED, OUTPUT);
  
  // Initialize servo
  cameraServo.attach(servoPin);
  cameraServo.write(90); // Center position
  
  // Start with motors stopped
  stopMotors();
  
  // Startup sequence
  startupSequence();
  
  Serial.println("Advanced PS4 Arduino Car Controller Ready!");
  Serial.println("Features: Speed Control, Obstacle Avoidance, Servo Control");
  Serial.println("Commands: F/B/L/R/S, SPEED:xxx, SERVO:xxx, AVOID:ON/OFF");
}

void loop() {
  // Handle serial input
  handleSerialInput();
  
  // Process complete commands
  if (stringComplete) {
    processCommand(inputString);
    inputString = "";
    stringComplete = false;
  }
  
  // Safety timeout check
  if (millis() - lastCommandTime > commandTimeout && lastCommand != 'S') {
    stopMotors();
    lastCommand = 'S';
    digitalWrite(statusLED, LOW);
    Serial.println("TIMEOUT: Motors stopped for safety");
  }
  
  // Obstacle detection (only when moving forward)
  if (obstacleAvoidanceEnabled && lastCommand == 'F') {
    checkObstacles();
  }
  
  delay(50); // Small delay for stability
}

void handleSerialInput() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    
    if (inChar == '\n' || inChar == '\r') {
      stringComplete = true;
    } else {
      inputString += inChar;
    }
    
    // Handle single character commands immediately
    if (inputString.length() == 1 && 
        (inChar == 'F' || inChar == 'B' || inChar == 'L' || inChar == 'R' || inChar == 'S')) {
      stringComplete = true;
    }
  }
}

void processCommand(String command) {
  lastCommandTime = millis();
  digitalWrite(statusLED, HIGH);
  
  // Handle single character commands
  if (command.length() == 1) {
    char cmd = command.charAt(0);
    processMovementCommand(cmd);
    return;
  }
  
  // Handle multi-character commands
  if (command.startsWith("SPEED:")) {
    handleSpeedCommand(command);
  } else if (command.startsWith("SERVO:")) {
    handleServoCommand(command);
  } else if (command.startsWith("AVOID:")) {
    handleAvoidanceCommand(command);
  } else {
    Serial.println("Unknown command: " + command);
  }
}

void processMovementCommand(char command) {
  lastCommand = command;
  
  switch(command) {
    case 'F':
    case 'f':
      if (!checkObstacleAhead()) {
        moveForward();
      } else {
        stopMotors();
        Serial.println("OBSTACLE DETECTED: Forward movement blocked");
      }
      break;
      
    case 'B':
    case 'b':
      moveBackward();
      break;
      
    case 'L':
    case 'l':
      turnLeft();
      break;
      
    case 'R':
    case 'r':
      turnRight();
      break;
      
    case 'S':
    case 's':
      stopMotors();
      digitalWrite(statusLED, LOW);
      break;
      
    default:
      stopMotors();
      Serial.println("Unknown movement command: " + String(command));
      break;
  }
}

void moveForward() {
  digitalWrite(leftMotorPin1, HIGH);
  digitalWrite(leftMotorPin2, LOW);
  analogWrite(leftMotorEnable, currentSpeed);
  
  digitalWrite(rightMotorPin1, HIGH);
  digitalWrite(rightMotorPin2, LOW);
  analogWrite(rightMotorEnable, currentSpeed);
  
  Serial.println("Moving Forward - Speed: " + String(currentSpeed));
}

void moveBackward() {
  digitalWrite(leftMotorPin1, LOW);
  digitalWrite(leftMotorPin2, HIGH);
  analogWrite(leftMotorEnable, currentSpeed);
  
  digitalWrite(rightMotorPin1, LOW);
  digitalWrite(rightMotorPin2, HIGH);
  analogWrite(rightMotorEnable, currentSpeed);
  
  Serial.println("Moving Backward - Speed: " + String(currentSpeed));
}

void turnLeft() {
  digitalWrite(leftMotorPin1, LOW);
  digitalWrite(leftMotorPin2, HIGH);
  analogWrite(leftMotorEnable, turnSpeed);
  
  digitalWrite(rightMotorPin1, HIGH);
  digitalWrite(rightMotorPin2, LOW);
  analogWrite(rightMotorEnable, turnSpeed);
  
  Serial.println("Turning Left - Speed: " + String(turnSpeed));
}

void turnRight() {
  digitalWrite(leftMotorPin1, HIGH);
  digitalWrite(leftMotorPin2, LOW);
  analogWrite(leftMotorEnable, turnSpeed);
  
  digitalWrite(rightMotorPin1, LOW);
  digitalWrite(rightMotorPin2, HIGH);
  analogWrite(rightMotorEnable, turnSpeed);
  
  Serial.println("Turning Right - Speed: " + String(turnSpeed));
}

void stopMotors() {
  digitalWrite(leftMotorPin1, LOW);
  digitalWrite(leftMotorPin2, LOW);
  analogWrite(leftMotorEnable, 0);
  
  digitalWrite(rightMotorPin1, LOW);
  digitalWrite(rightMotorPin2, LOW);
  analogWrite(rightMotorEnable, 0);
  
  digitalWrite(obstacleWarningLED, LOW);
  Serial.println("Motors Stopped");
}

bool checkObstacleAhead() {
  if (!obstacleAvoidanceEnabled) return false;
  
  long distance = getDistance();
  
  if (distance < obstacleThreshold && distance > 0) {
    digitalWrite(obstacleWarningLED, HIGH);
    return true;
  } else {
    digitalWrite(obstacleWarningLED, LOW);
    return false;
  }
}

void checkObstacles() {
  if (checkObstacleAhead()) {
    stopMotors();
    lastCommand = 'S';
    Serial.println("EMERGENCY STOP: Obstacle detected at " + String(getDistance()) + "cm");
  }
}

long getDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  long duration = pulseIn(echoPin, HIGH, 30000); // 30ms timeout
  if (duration == 0) return 999; // No echo received
  
  long distance = duration * 0.034 / 2;
  return distance;
}

void handleSpeedCommand(String command) {
  int speed = command.substring(6).toInt();
  
  if (speed >= minSpeed && speed <= maxSpeed) {
    currentSpeed = speed;
    turnSpeed = speed * 0.7; // Turn speed is 70% of normal speed
    Serial.println("Speed set to: " + String(speed));
  } else {
    Serial.println("Invalid speed. Range: " + String(minSpeed) + "-" + String(maxSpeed));
  }
}

void handleServoCommand(String command) {
  int angle = command.substring(6).toInt();
  
  if (angle >= 0 && angle <= 180) {
    cameraServo.write(angle);
    Serial.println("Servo angle set to: " + String(angle));
  } else {
    Serial.println("Invalid servo angle. Range: 0-180");
  }
}

void handleAvoidanceCommand(String command) {
  String setting = command.substring(6);
  
  if (setting == "ON" || setting == "on") {
    obstacleAvoidanceEnabled = true;
    Serial.println("Obstacle avoidance: ENABLED");
  } else if (setting == "OFF" || setting == "off") {
    obstacleAvoidanceEnabled = false;
    digitalWrite(obstacleWarningLED, LOW);
    Serial.println("Obstacle avoidance: DISABLED");
  } else {
    Serial.println("Invalid avoidance setting. Use ON or OFF");
  }
}

void startupSequence() {
  // LED startup sequence
  for (int i = 0; i < 3; i++) {
    digitalWrite(statusLED, HIGH);
    digitalWrite(obstacleWarningLED, HIGH);
    delay(200);
    digitalWrite(statusLED, LOW);
    digitalWrite(obstacleWarningLED, LOW);
    delay(200);
  }
  
  // Servo sweep
  for (int angle = 90; angle <= 180; angle += 10) {
    cameraServo.write(angle);
    delay(50);
  }
  for (int angle = 180; angle >= 0; angle -= 10) {
    cameraServo.write(angle);
    delay(50);
  }
  for (int angle = 0; angle <= 90; angle += 10) {
    cameraServo.write(angle);
    delay(50);
  }
  
  Serial.println("Startup sequence complete");
}