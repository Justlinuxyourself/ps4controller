# PS4 Arduino Car Controller

An Android app that controls an Arduino-based robotic car using a PS4 controller via USB OTG connection.

## Features

- **PS4 Controller Support**: Full analog stick control with variable speed
- **USB OTG Communication**: Direct communication with Arduino via USB
- **Variable Speed Control**: Speed changes based on joystick position (0-255 range)
- **Real-time Status**: Shows connection status for both USB and controller
- **Safety Features**: Automatic stop when app closes or connection lost

## Hardware Requirements

- Android phone with USB OTG support
- USB OTG adapter/cable
- Arduino (Uno/Nano/etc.) with your car control code
- PS4 DualShock controller
- Robotic car with motor driver (H-Bridge)

## Control Mapping

- **Left Analog Stick (Y-axis)**: Forward/Backward movement
- **Right Analog Stick (X-axis)**: Left/Right turning
- **D-Pad**: Backup digital controls (full speed)
- **Speed**: Varies from 0-255 based on stick position

## Arduino Compatibility

This app is designed to work with your existing Arduino code. It sends single character commands:
- `'F'` - Forward
- `'B'` - Backward  
- `'L'` - Left
- `'R'` - Right
- `'S'` - Stop

## Setup Instructions

1. **Build the App**: Open in Android Studio and build the APK
2. **Connect Hardware**: 
   - Connect PS4 controller to phone (wired or Bluetooth)
   - Connect Arduino to phone via USB OTG
3. **Upload Arduino Code**: Use your existing car control code
4. **Launch App**: The app will request USB permissions
5. **Start Controlling**: Use the PS4 controller to drive your car!

## Permissions

- `USB_PERMISSION`: Required for Arduino communication
- `USB_HOST`: Required for USB OTG functionality

## Technical Details

- **USB Communication**: 9600 baud serial communication
- **Deadzone**: 0.2 threshold to prevent drift
- **Update Rate**: Real-time joystick input processing
- **Safety**: Automatic stop command on app close

## Troubleshooting

1. **USB Not Connected**: Check OTG adapter and Arduino connection
2. **Controller Not Detected**: Ensure PS4 controller is properly paired/connected
3. **Car Not Responding**: Verify Arduino code matches expected commands
4. **Permission Issues**: Manually grant USB permission in app settings

## Building

To build this project:

```bash
./gradlew assembleDebug
```

The APK will be generated in `app/build/outputs/apk/debug/`