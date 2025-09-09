# Arduino Car Controller Code

This Arduino code is designed to work with the PS4 Car Controller Android app via USB OTG connection.

## Hardware Setup

### Required Components
- Arduino Uno/Nano/Pro Mini
- L9110 2-Channel Motor Driver
- 2x DC Motors (for left and right wheels)
- Power supply for motors (6-12V depending on motors)
- Jumper wires
- Chassis/frame for the car

### Wiring Diagram

#### L9110 2-Channel Motor Driver
```
Arduino Uno -> L9110 Motor Driver
Pin 5  -> A-IA (Left motor control A - PWM)
Pin 6  -> A-IB (Left motor control B - PWM)
Pin 9  -> B-IA (Right motor control A - PWM)
Pin 10 -> B-IB (Right motor control B - PWM)
GND    -> GND
5V     -> VCC

L9110 -> Motors
OA+, OA- -> Left Motor
OB+, OB- -> Right Motor

L9110 -> Power Supply
VMS -> Positive terminal (2.5-12V)
GND -> Negative terminal
```

### Pin Configuration
- **Left Motor Control**: Pins 5, 6 (A-IA, A-IB)
- **Right Motor Control**: Pins 9, 10 (B-IA, B-IB)
- **L9110 2-Channel Motor Driver required**

## Features

### Command Processing
The Arduino receives single character commands via USB Serial:
- `'F'` - Move Forward
- `'B'` - Move Backward
- `'L'` - Turn Left
- `'R'` - Turn Right
- `'S'` - Stop

### Analog Speed Control
- Uses PWM (analogWrite) for smooth speed control on L9110
- Differential steering for smooth turns
- Speed differential creates natural turning motion

### Safety Features
- **Timeout Protection**: Motors stop automatically if no command received for 1 second
- **Unknown Command Handling**: Stops motors for safety on unrecognized commands

### Speed Control
- **Default Speed**: 180 (out of 255 maximum)
- **Differential Turning**: Slow wheel at 40%, fast wheel at 90%
- **Adjustable**: Speed can be modified in code or via serial commands

## Installation

1. **Connect Hardware**: Wire the Arduino to L9110 motor driver and motors
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
int slowSpeed = 80;      // Slower wheel for turning (0-255)
int fastSpeed = 200;     // Faster wheel for turning (0-255)
```

### Changing Pin Configuration
Update the pin definitions at the top of the code:
```cpp
const int leftMotorA = 5;   // Left motor A-IA pin
const int leftMotorB = 6;   // Left motor A-IB pin
// ... etc
```

### Motor Direction
If motors spin in wrong direction, swap the motor wires or modify the code:
```cpp
// In moveForward() function, change HIGH/LOW values
analogWrite(leftMotorA, 0);         // Was currentSpeed
analogWrite(leftMotorB, currentSpeed); // Was 0
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
This sets the speed to 150 and automatically adjusts differential speeds.

### Feedback
The Arduino sends status messages back to the app:
- Command acknowledgments
- Error messages
- Timeout notifications
- Speed information

## Compatible Hardware

### Tested Motor Drivers
- L9110 2-Channel Motor Driver (optimized for this)
- L298N Dual H-Bridge (with pin changes)
- L293D Motor Shield (with pin changes)

### Tested Arduino Boards
- Arduino Uno R3
- Arduino Nano
- Arduino Pro Mini
- ESP32 (with minor modifications)

## Power Requirements

- **Arduino**: 5V via USB or external supply
- **L9110**: 2.5-12V motor supply (VMS pin)
- **Motors**: Match L9110 voltage rating (typically 6-12V)
- **Current**: L9110 can handle up to 800mA per channel