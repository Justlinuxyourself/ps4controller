package com.carcontroller.ps4

import android.app.PendingIntent
import android.content.BroadcastReceiver
import android.content.Context
import android.content.Intent
import android.content.IntentFilter
import android.hardware.usb.UsbDevice
import android.hardware.usb.UsbManager
import android.os.Bundle
import android.view.InputDevice
import android.view.KeyEvent
import android.view.MotionEvent
import androidx.appcompat.app.AppCompatActivity
import com.carcontroller.ps4.databinding.ActivityMainBinding
import kotlin.math.abs
import kotlin.math.max

class MainActivity : AppCompatActivity() {
    private lateinit var binding: ActivityMainBinding
    private lateinit var usbManager: UsbManager
    private lateinit var usbCommunicator: UsbCommunicator
    private var isControllerConnected = false
    private var isUsbConnected = false
    
    // Joystick deadzone threshold
    private val DEADZONE = 0.2f
    
    // Speed calculation variables
    private var currentSpeed = 0
    private var lastCommand = "None"

    companion object {
        private const val ACTION_USB_PERMISSION = "com.carcontroller.ps4.USB_PERMISSION"
    }

    private val usbReceiver = object : BroadcastReceiver() {
        override fun onReceive(context: Context, intent: Intent) {
            when (intent.action) {
                ACTION_USB_PERMISSION -> {
                    synchronized(this) {
                        val device = intent.getParcelableExtra<UsbDevice>(UsbManager.EXTRA_DEVICE)
                        if (intent.getBooleanExtra(UsbManager.EXTRA_PERMISSION_GRANTED, false)) {
                            device?.let {
                                connectToArduino(it)
                            }
                        }
                    }
                }
                UsbManager.ACTION_USB_DEVICE_DETACHED -> {
                    isUsbConnected = false
                    updateConnectionStatus()
                }
            }
        }
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)

        usbManager = getSystemService(Context.USB_SERVICE) as UsbManager
        usbCommunicator = UsbCommunicator()

        setupUsbCommunication()
        updateConnectionStatus()
    }

    override fun onResume() {
        super.onResume()
        registerReceiver(usbReceiver, IntentFilter().apply {
            addAction(ACTION_USB_PERMISSION)
            addAction(UsbManager.ACTION_USB_DEVICE_DETACHED)
        })
        checkForControllers()
    }

    override fun onPause() {
        super.onPause()
        unregisterReceiver(usbReceiver)
    }

    override fun onDestroy() {
        super.onDestroy()
        usbCommunicator.disconnect()
        sendCommand('S', 0) // Stop the car when app closes
    }

    private fun setupUsbCommunication() {
        val deviceList = usbManager.deviceList
        for (device in deviceList.values) {
            if (device.vendorId == 0x2341 || device.productId == 0x0043) { // Arduino Uno identifiers
                requestUsbPermission(device)
                return
            }
        }
        
        // If no Arduino found, try to connect to any available device
        deviceList.values.firstOrNull()?.let { device ->
            requestUsbPermission(device)
        }
    }

    private fun requestUsbPermission(device: UsbDevice) {
        val permissionIntent = PendingIntent.getBroadcast(
            this, 0, Intent(ACTION_USB_PERMISSION), PendingIntent.FLAG_IMMUTABLE
        )
        usbManager.requestPermission(device, permissionIntent)
    }

    private fun connectToArduino(device: UsbDevice) {
        if (usbCommunicator.connect(usbManager, device)) {
            isUsbConnected = true
            updateConnectionStatus()
            // Send initial stop command
            sendCommand('S', 0)
        }
    }

    private fun checkForControllers() {
        val inputDeviceIds = InputDevice.getDeviceIds()
        isControllerConnected = false
        
        for (deviceId in inputDeviceIds) {
            val inputDevice = InputDevice.getDevice(deviceId)
            if (inputDevice != null && inputDevice.sources and InputDevice.SOURCE_GAMEPAD == InputDevice.SOURCE_GAMEPAD) {
                isControllerConnected = true
                break
            }
        }
        updateConnectionStatus()
    }

    private fun updateConnectionStatus() {
        runOnUiThread {
            binding.usbStatusText.text = "USB: ${if (isUsbConnected) "Connected" else "Disconnected"}"
            binding.usbStatusText.setTextColor(if (isUsbConnected) 0xFF4CAF50.toInt() else 0xFFFF6B6B.toInt())
            
            binding.controllerStatusText.text = "PS4 Controller: ${if (isControllerConnected) "Connected" else "Disconnected"}"
            binding.controllerStatusText.setTextColor(if (isControllerConnected) 0xFF4CAF50.toInt() else 0xFFFF6B6B.toInt())
            
            binding.speedText.text = "Speed: $currentSpeed%"
            binding.commandText.text = "Last Command: $lastCommand"
        }
    }

    override fun onGenericMotionEvent(event: MotionEvent): Boolean {
        if (event.source and InputDevice.SOURCE_JOYSTICK == InputDevice.SOURCE_JOYSTICK &&
            event.action == MotionEvent.ACTION_MOVE) {
            processJoystickInput(event)
            return true
        }
        return super.onGenericMotionEvent(event)
    }

    override fun onKeyDown(keyCode: Int, event: KeyEvent): Boolean {
        if (event.source and InputDevice.SOURCE_GAMEPAD == InputDevice.SOURCE_GAMEPAD) {
            // Handle D-pad input as backup controls
            when (keyCode) {
                KeyEvent.KEYCODE_DPAD_UP -> {
                    sendCommand('F', 200)
                    return true
                }
                KeyEvent.KEYCODE_DPAD_DOWN -> {
                    sendCommand('B', 200)
                    return true
                }
                KeyEvent.KEYCODE_DPAD_LEFT -> {
                    sendCommand('L', 200)
                    return true
                }
                KeyEvent.KEYCODE_DPAD_RIGHT -> {
                    sendCommand('R', 200)
                    return true
                }
            }
        }
        return super.onKeyDown(keyCode, event)
    }

    override fun onKeyUp(keyCode: Int, event: KeyEvent): Boolean {
        if (event.source and InputDevice.SOURCE_GAMEPAD == InputDevice.SOURCE_GAMEPAD) {
            when (keyCode) {
                KeyEvent.KEYCODE_DPAD_UP,
                KeyEvent.KEYCODE_DPAD_DOWN,
                KeyEvent.KEYCODE_DPAD_LEFT,
                KeyEvent.KEYCODE_DPAD_RIGHT -> {
                    sendCommand('S', 0)
                    return true
                }
            }
        }
        return super.onKeyUp(keyCode, event)
    }

    private fun processJoystickInput(event: MotionEvent) {
        // Get left stick Y axis for forward/backward
        val leftY = event.getAxisValue(MotionEvent.AXIS_Y)
        // Get right stick X axis for left/right
        val rightX = event.getAxisValue(MotionEvent.AXIS_Z)

        // Calculate movement based on stick positions
        val forwardBackward = if (abs(leftY) > DEADZONE) -leftY else 0f // Invert Y axis
        val leftRight = if (abs(rightX) > DEADZONE) rightX else 0f

        // Determine primary movement direction and speed
        val maxInput = max(abs(forwardBackward), abs(leftRight))
        currentSpeed = (maxInput * 255).toInt() // Convert to 0-255 range for Arduino

        when {
            // Forward/Backward movement has priority
            abs(forwardBackward) > abs(leftRight) && abs(forwardBackward) > DEADZONE -> {
                if (forwardBackward > 0) {
                    sendCommand('F', currentSpeed)
                } else {
                    sendCommand('B', currentSpeed)
                }
            }
            // Left/Right movement
            abs(leftRight) > DEADZONE -> {
                if (leftRight > 0) {
                    sendCommand('R', currentSpeed)
                } else {
                    sendCommand('L', currentSpeed)
                }
            }
            // No significant input - stop
            else -> {
                currentSpeed = 0
                sendCommand('S', 0)
            }
        }

        updateConnectionStatus()
    }

    private fun sendCommand(direction: Char, speed: Int) {
        if (isUsbConnected) {
            // Send the direction command
            usbCommunicator.sendData(direction.toString())
            
            // Update the last command for display
            lastCommand = when (direction) {
                'F' -> "Forward (Speed: $speed)"
                'B' -> "Backward (Speed: $speed)"
                'L' -> "Left (Speed: $speed)"
                'R' -> "Right (Speed: $speed)"
                'S' -> "Stop"
                else -> "Unknown"
            }
            
            // Convert speed to percentage for display
            currentSpeed = (speed * 100) / 255
        }
    }
}