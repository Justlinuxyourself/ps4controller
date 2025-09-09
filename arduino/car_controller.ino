/*
  PS4 Arduino Car Controller
  Compatible with the PS4 Car Controller Android App
  
  This code receives single character commands via USB Serial:
  'F' - Forward
  'B' - Backward
  'L' - Left
  'R' - Right
  'S' - Stop
  
  Hardware connections:
  - Motor Driver (L298N or similar)
  - Left Motor: pins 5, 6
  - Right Motor: pins 9, 10
  - Enable pins: 3, 11 (for speed control)
  
  Author: Arduino Car Controller
  Compatible with Android app via USB OTG
*/

// Motor control pins
const int leftMotorPin1 = 5;   // Left motor direction pin 1
const int leftMotorPin2 = 6;   // Left motor direction pin 2
const int rightMotorPin1 = 9;  // Right motor direction pin 1
const int rightMotorPin2 = 10; // Right motor direction pin 2

// Enable pins for speed control (PWM)
const int leftMotorEnable = 3;  // Left motor speed control
const int rightMotorEnable = 11; // Right motor speed control

// Speed settings
int currentSpeed = 200;  // Default speed (0-255)
int turnSpeed = 150;     // Speed for turning (slightly slower)

// Status LED (built-in)
const int statusLED = 13;

// Variables for command processing
char receivedCommand = 'S';
unsigned long lastCommandTime = 0;
const unsigned long commandTimeout = 1000; // Stop after 1 second of no commands

void setup() {
  // Initialize serial communication
  Serial.begin(9600);
  
  // Initialize motor control pins
  pinMode(leftMotorPin1, OUTPUT);
  pinMode(leftMotorPin2, OUTPUT);
  pinMode(rightMotorPin1, OUTPUT);
  pinMode(rightMotorPin2, OUTPUT);
  pinMode(leftMotorEnable, OUTPUT);
  pinMode(rightMotorEnable, OUTPUT);
  
  // Initialize status LED
  pinMode(statusLED, OUTPUT);
  
  // Start with motors stopped
  stopMotors();
  
  // Blink LED to indicate ready
  for(int i = 0; i < 3; i++) {
    digitalWrite(statusLED, HIGH);
    delay(200);
    digitalWrite(statusLED, LOW);
    delay(200);
  }
  
  Serial.println("PS4 Arduino Car Controller Ready!");
  Serial.println("Waiting for commands...");
}

void loop() {
  // Check for incoming serial commands
  if (Serial.available() > 0) {
    receivedCommand = Serial.read();
    lastCommandTime = millis();
    
    // Turn on status LED when receiving commands
    digitalWrite(statusLED, HIGH);
    
    // Process the command
    processCommand(receivedCommand);
    
    // Send acknowledgment back to app
    Serial.print("Received: ");
    Serial.println(receivedCommand);
  }
  
  // Safety feature: stop motors if no command received for timeout period
  if (millis() - lastCommandTime > commandTimeout && receivedCommand != 'S') {
    stopMotors();
    receivedCommand = 'S';
    digitalWrite(statusLED, LOW);
    Serial.println("Timeout - Motors stopped");
  }
  
  // Small delay to prevent overwhelming the serial buffer
  delay(10);
}

void processCommand(char command) {
  switch(command) {
    case 'F':
    case 'f':
      moveForward();
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
      // Unknown command - stop for safety
      stopMotors();
      Serial.print("Unknown command: ");
      Serial.println(command);
      break;
  }
}

void moveForward() {
  // Left motor forward
  digitalWrite(leftMotorPin1, HIGH);
  digitalWrite(leftMotorPin2, LOW);
  analogWrite(leftMotorEnable, currentSpeed);
  
  // Right motor forward
  digitalWrite(rightMotorPin1, HIGH);
  digitalWrite(rightMotorPin2, LOW);
  analogWrite(rightMotorEnable, currentSpeed);
  
  Serial.println("Moving Forward");
}

void moveBackward() {
  // Left motor backward
  digitalWrite(leftMotorPin1, LOW);
  digitalWrite(leftMotorPin2, HIGH);
  analogWrite(leftMotorEnable, currentSpeed);
  
  // Right motor backward
  digitalWrite(rightMotorPin1, LOW);
  digitalWrite(rightMotorPin2, HIGH);
  analogWrite(rightMotorEnable, currentSpeed);
  
  Serial.println("Moving Backward");
}

void turnLeft() {
  // Left motor backward (or stopped)
  digitalWrite(leftMotorPin1, LOW);
  digitalWrite(leftMotorPin2, HIGH);
  analogWrite(leftMotorEnable, turnSpeed);
  
  // Right motor forward
  digitalWrite(rightMotorPin1, HIGH);
  digitalWrite(rightMotorPin2, LOW);
  analogWrite(rightMotorEnable, turnSpeed);
  
  Serial.println("Turning Left");
}

void turnRight() {
  // Left motor forward
  digitalWrite(leftMotorPin1, HIGH);
  digitalWrite(leftMotorPin2, LOW);
  analogWrite(leftMotorEnable, turnSpeed);
  
  // Right motor backward (or stopped)
  digitalWrite(rightMotorPin1, LOW);
  digitalWrite(rightMotorPin2, HIGH);
  analogWrite(rightMotorEnable, turnSpeed);
  
  Serial.println("Turning Right");
}

void stopMotors() {
  // Stop both motors
  digitalWrite(leftMotorPin1, LOW);
  digitalWrite(leftMotorPin2, LOW);
  analogWrite(leftMotorEnable, 0);
  
  digitalWrite(rightMotorPin1, LOW);
  digitalWrite(rightMotorPin2, LOW);
  analogWrite(rightMotorEnable, 0);
  
  Serial.println("Motors Stopped");
}

// Function to adjust speed (can be called from serial commands if needed)
void setSpeed(int speed) {
  if (speed >= 0 && speed <= 255) {
    currentSpeed = speed;
    turnSpeed = speed * 0.75; // Turn speed is 75% of normal speed
    Serial.print("Speed set to: ");
    Serial.println(speed);
  }
}

// Optional: Function to handle speed commands (format: "SPEED:200")
void handleSpeedCommand(String command) {
  if (command.startsWith("SPEED:")) {
    int speed = command.substring(6).toInt();
    setSpeed(speed);
  }
}