/*
  PS4 Arduino Car Controller
  Compatible with the PS4 Car Controller Android App
  
  This code receives single character commands via USB Serial with analog speed control:
  'F' - Forward
  'B' - Backward
  'L' - Left
  'R' - Right
  'S' - Stop
  
  Hardware connections:
  - Motor Driver (L298N or similar)
  - Left Motor: pins 5, 6 (PWM for analog control)
  - Right Motor: pins 9, 10 (PWM for analog control)
  - No additional components needed
  
  Author: Arduino Car Controller
  Compatible with Android app via USB OTG
*/

// Motor control pins (PWM capable for analog control)
const int leftMotorPin1 = 5;   // Left motor direction pin 1
const int leftMotorPin2 = 6;   // Left motor direction pin 2
const int rightMotorPin1 = 9;  // Right motor direction pin 1
const int rightMotorPin2 = 10; // Right motor direction pin 2

// Speed settings for analog control
int currentSpeed = 180;  // Default speed (0-255)
int turnSpeed = 120;     // Speed for turning (slightly slower)

// Variables for command processing
char receivedCommand = 'S';
unsigned long lastCommandTime = 0;
const unsigned long commandTimeout = 1000; // Stop after 1 second of no commands

void setup() {
  // Initialize serial communication
  Serial.begin(9600);
  
  // Initialize motor control pins (all PWM capable)
  pinMode(leftMotorPin1, OUTPUT);
  pinMode(leftMotorPin2, OUTPUT);
  pinMode(rightMotorPin1, OUTPUT);
  pinMode(rightMotorPin2, OUTPUT);
  
  // Start with motors stopped
  stopMotors();
  
  Serial.println("PS4 Arduino Car Controller Ready! (Analog Control)");
  Serial.println("Waiting for commands...");
}

void loop() {
  // Check for incoming serial commands
  if (Serial.available() > 0) {
    receivedCommand = Serial.read();
    lastCommandTime = millis();
    
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
  // Left motor forward (analog control)
  analogWrite(leftMotorPin1, currentSpeed);
  analogWrite(leftMotorPin2, 0);
  
  // Right motor forward (analog control)
  analogWrite(rightMotorPin1, currentSpeed);
  analogWrite(rightMotorPin2, 0);
  
  Serial.print("Moving Forward - Speed: ");
  Serial.println(currentSpeed);
}

void moveBackward() {
  // Left motor backward (analog control)
  analogWrite(leftMotorPin1, 0);
  analogWrite(leftMotorPin2, currentSpeed);
  
  // Right motor backward (analog control)
  analogWrite(rightMotorPin1, 0);
  analogWrite(rightMotorPin2, currentSpeed);
  
  Serial.print("Moving Backward - Speed: ");
  Serial.println(currentSpeed);
}

void turnLeft() {
  // Left motor backward for turning (analog control)
  analogWrite(leftMotorPin1, 0);
  analogWrite(leftMotorPin2, turnSpeed);
  
  // Right motor forward for turning (analog control)
  analogWrite(rightMotorPin1, turnSpeed);
  analogWrite(rightMotorPin2, 0);
  
  Serial.print("Turning Left - Speed: ");
  Serial.println(turnSpeed);
}

void turnRight() {
  // Left motor forward for turning (analog control)
  analogWrite(leftMotorPin1, turnSpeed);
  analogWrite(leftMotorPin2, 0);
  
  // Right motor backward for turning (analog control)
  analogWrite(rightMotorPin1, 0);
  analogWrite(rightMotorPin2, turnSpeed);
  
  Serial.print("Turning Right - Speed: ");
  Serial.println(turnSpeed);
}

void stopMotors() {
  // Stop both motors (analog control)
  analogWrite(leftMotorPin1, 0);
  analogWrite(leftMotorPin2, 0);
  
  analogWrite(rightMotorPin1, 0);
  analogWrite(rightMotorPin2, 0);
  
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