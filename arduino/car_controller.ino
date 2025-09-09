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
  - L9110 2-Channel Motor Driver
  - Left Motor: pins 5, 6 (A-IA, A-IB)
  - Right Motor: pins 9, 10 (B-IA, B-IB)
  - No additional components needed
  
  Author: Arduino Car Controller
  Compatible with Android app via USB OTG
*/

// L9110 Motor Driver pins (PWM capable for analog control)
const int leftMotorA = 5;   // Left motor A-IA pin
const int leftMotorB = 6;   // Left motor A-IB pin
const int rightMotorA = 9;  // Right motor B-IA pin
const int rightMotorB = 10; // Right motor B-IB pin

// Speed settings for analog control
int currentSpeed = 180;  // Default speed (0-255)
int slowSpeed = 80;      // Slower speed for differential turning
int fastSpeed = 200;     // Faster speed for differential turning

// Variables for command processing
char receivedCommand = 'S';
unsigned long lastCommandTime = 0;
const unsigned long commandTimeout = 1000; // Stop after 1 second of no commands

void setup() {
  // Initialize serial communication
  Serial.begin(9600);
  
  // Initialize motor control pins (all PWM capable)
  pinMode(leftMotorA, OUTPUT);
  pinMode(leftMotorB, OUTPUT);
  pinMode(rightMotorA, OUTPUT);
  pinMode(rightMotorB, OUTPUT);
  
  // Start with motors stopped
  stopMotors();
  
  Serial.println("PS4 Arduino Car Controller Ready! (L9110 + Differential Steering)");
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
  // Both motors forward at same speed
  analogWrite(leftMotorA, currentSpeed);
  analogWrite(leftMotorB, 0);
  
  analogWrite(rightMotorA, currentSpeed);
  analogWrite(rightMotorB, 0);
  
  Serial.print("Moving Forward - Speed: ");
  Serial.println(currentSpeed);
}

void moveBackward() {
  // Both motors backward at same speed
  analogWrite(leftMotorA, 0);
  analogWrite(leftMotorB, currentSpeed);
  
  analogWrite(rightMotorA, 0);
  analogWrite(rightMotorB, currentSpeed);
  
  Serial.print("Moving Backward - Speed: ");
  Serial.println(currentSpeed);
}

void turnLeft() {
  // Differential steering: Left motor slow forward, Right motor fast forward
  analogWrite(leftMotorA, slowSpeed);
  analogWrite(leftMotorB, 0);
  
  analogWrite(rightMotorA, fastSpeed);
  analogWrite(rightMotorB, 0);
  
  Serial.print("Turning Left - Left: ");
  Serial.print(slowSpeed);
  Serial.print(", Right: ");
  Serial.println(fastSpeed);
}

void turnRight() {
  // Differential steering: Left motor fast forward, Right motor slow forward
  analogWrite(leftMotorA, fastSpeed);
  analogWrite(leftMotorB, 0);
  
  analogWrite(rightMotorA, slowSpeed);
  analogWrite(rightMotorB, 0);
  
  Serial.print("Turning Right - Left: ");
  Serial.print(fastSpeed);
  Serial.print(", Right: ");
  Serial.println(slowSpeed);
}

void stopMotors() {
  // Stop both motors
  analogWrite(leftMotorA, 0);
  analogWrite(leftMotorB, 0);
  
  analogWrite(rightMotorA, 0);
  analogWrite(rightMotorB, 0);
  
  Serial.println("Motors Stopped");
}

// Function to adjust speed (can be called from serial commands if needed)
void setSpeed(int speed) {
  if (speed >= 0 && speed <= 255) {
    currentSpeed = speed;
    slowSpeed = speed * 0.4;  // Slow wheel is 40% of normal speed
    fastSpeed = speed * 0.9;  // Fast wheel is 90% of normal speed
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