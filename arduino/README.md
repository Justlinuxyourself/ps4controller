# Arduino Car Controller Code

This Arduino code is designed to work with the PS4 Car Controller Android app via USB OTG connection.

## Hardware Setup

### Required Components
- Arduino Uno/Nano/Pro Mini
- Motor Driver (L298N recommended) OR direct motor control
- 2x DC Motors (for left and right wheels)
- Power supply for motors (6-12V depending on motors)
- Jumper wires
- Chassis/frame for the car

### Wiring Diagram

#### Option 1: With L298N Motor Driver
```
Arduino Uno -> L298N Motor Driver
Pin 5  -> IN1 (Left motor direction/speed - PWM)
Pin 6  -> IN2 (Left motor direction/speed - PWM)
Pin 9  -> IN3 (Right motor direction/speed - PWM)
Pin 10 -> IN4 (Right motor direction/speed - PWM)
GND    -> GND
5V     -> VCC (Logic power)
ENA    -> 5V (always enabled)
ENB    -> 5V (always enabled)

L298N -> Motors
OUT1, OUT2 -> Left Motor
OUT3, OUT4 -> Right Motor

L298N -> Power Supply
VIN -> Positive terminal (6-12V)
GND -> Negative terminal
```

#### Option 2: Direct Motor Control (Small Motors Only)
```
Arduino Uno -> Motors (via transistors/MOSFETs for larger motors)
Pin 5  -> Left Motor Terminal 1 (PWM)
Pin 6  -> Left Motor Terminal 2 (PWM)
Pin 9  -> Right Motor Terminal 1 (PWM)
Pin 10 -> Right Motor Terminal 2 (PWM)
GND    -> Motor Ground/Common
```

### Pin Configuration
- **Left Motor Control**: Pins 5, 6 (PWM analog control)
- **Right Motor Control**: Pins 9, 10 (PWM analog control)
- **No additional components required**

## Features

### Command Processing
The Arduino receives single character commands via USB Serial:
- `'F'` - Move Forward
- `'B'` - Move Backward
- `'L'` - Turn Left
- `'R'` - Turn Right
- `'S'` - Stop

### Analog Speed Control
- Uses PWM (analogWrite) for smooth speed control
- No separate enable pins needed
- Direct speed control on motor pins

### Safety Features
- **Timeout Protection**: Motors stop automatically if no command received for 1 second
- **Unknown Command Handling**: Stops motors for safety on unrecognized commands

### Speed Control
- **Default Speed**: 180 (out of 255 maximum)
- **Turn Speed**: 120 (75% of normal speed for better control)
- **Adjustable**: Speed can be modified in code or via serial commands

## Installation

1. **Connect Hardware**: Wire the Arduino to motors (with or without motor driver)
2. **Upload Code**: Open `car_controller.ino` in Arduino IDE and upload to your board
3. **Test Connection**: Open Serial Monitor (9600 baud) to see status messages
4. **Connect to App**: Use USB OTG cable to connect Arduino to Android phone

## Usage

1. Power on the Arduino and motors
2. Connect to Android phone via USB OTG
3. Launch the PS4 Car Controller app
4. Grant USB permissions when prompted
5. Connect PS4 controller to phone
6. Use analog sticks to control the car!

## Customization

### Adjusting Speed
Modify these variables in the code:
```cpp
int currentSpeed = 180;  // Normal driving speed (0-255)
int turnSpeed = 120;     // Turning speed (0-255)
```

### Changing Pin Configuration
Update the pin definitions at the top of the code:
```cpp
const int leftMotorPin1 = 5;   // Left motor direction pin 1
const int leftMotorPin2 = 6;   // Left motor direction pin 2
// ... etc
```

### Motor Direction
If motors spin in wrong direction, swap the motor wires or modify the code:
```cpp
// In moveForward() function, change HIGH/LOW values
analogWrite(leftMotorPin1, 0);         // Was currentSpeed
analogWrite(leftMotorPin2, currentSpeed); // Was 0
```

## Troubleshooting

### Car doesn't move
- Check power supply to motors
- Verify wiring connections
- Test motors directly with battery
- Check Serial Monitor for error messages
- Ensure PWM pins (5, 6, 9, 10) are used

### Car moves in wrong direction
- Swap motor wire connections
- Or modify direction logic in code

### Erratic movement
- Check for loose connections
- Ensure adequate power supply
- Verify ground connections
- Make sure motors can handle the PWM frequency

### No communication with app
- Check USB OTG connection
- Verify Arduino is powered
- Check Serial Monitor for "Ready" message
- Try different USB cable

## Serial Commands for Testing

You can test the Arduino without the app using Serial Monitor:
- Send `F` for forward
- Send `B` for backward
- Send `L` for left
- Send `R` for right
- Send `S` for stop

## Advanced Features

### Speed Control via Serial
The code includes optional speed control. Send commands like:
```
SPEED:150
```
This sets the speed to 150 (out of 255).

### Feedback
The Arduino sends status messages back to the app:
- Command acknowledgments
- Error messages
- Timeout notifications
- Speed information

## Compatible Hardware

### Tested Motor Drivers
- L298N Dual H-Bridge
- L293D Motor Shield
- TB6612FNG Motor Driver
- Direct connection (small motors only)

### Tested Arduino Boards
- Arduino Uno R3
- Arduino Nano
- Arduino Pro Mini
- ESP32 (with minor modifications)

## Power Requirements

- **Arduino**: 5V via USB or external supply
- **Motors**: 6-12V depending on motor specifications
- **Current**: Ensure motor driver can handle your motor current draw