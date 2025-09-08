package com.carcontroller.ps4

import android.hardware.usb.UsbDevice
import android.hardware.usb.UsbDeviceConnection
import android.hardware.usb.UsbManager
import android.util.Log
import com.felhr.usbserial.UsbSerialDevice
import com.felhr.usbserial.UsbSerialInterface

class UsbCommunicator {
    private var usbDevice: UsbDevice? = null
    private var usbConnection: UsbDeviceConnection? = null
    private var serialPort: UsbSerialDevice? = null
    private var isConnected = false

    companion object {
        private const val TAG = "UsbCommunicator"
        private const val BAUD_RATE = 9600
    }

    private val serialCallback = UsbSerialInterface.UsbReadCallback { data ->
        val receivedData = String(data)
        Log.d(TAG, "Received from Arduino: $receivedData")
    }

    fun connect(usbManager: UsbManager, device: UsbDevice): Boolean {
        return try {
            usbDevice = device
            usbConnection = usbManager.openDevice(device)
            
            if (usbConnection == null) {
                Log.e(TAG, "Failed to open USB connection")
                return false
            }

            serialPort = UsbSerialDevice.createUsbSerialDevice(device, usbConnection)
            
            if (serialPort == null) {
                Log.e(TAG, "Failed to create serial device")
                return false
            }

            if (serialPort!!.open()) {
                serialPort!!.setBaudRate(BAUD_RATE)
                serialPort!!.setDataBits(UsbSerialInterface.DATA_BITS_8)
                serialPort!!.setStopBits(UsbSerialInterface.STOP_BITS_1)
                serialPort!!.setParity(UsbSerialInterface.PARITY_NONE)
                serialPort!!.setFlowControl(UsbSerialInterface.FLOW_CONTROL_OFF)
                serialPort!!.read(serialCallback)
                
                isConnected = true
                Log.d(TAG, "USB Serial connection established")
                true
            } else {
                Log.e(TAG, "Failed to open serial port")
                false
            }
        } catch (e: Exception) {
            Log.e(TAG, "Error connecting to USB device", e)
            false
        }
    }

    fun sendData(data: String): Boolean {
        return try {
            if (isConnected && serialPort != null) {
                serialPort!!.write(data.toByteArray())
                Log.d(TAG, "Sent to Arduino: $data")
                true
            } else {
                Log.w(TAG, "Not connected, cannot send data")
                false
            }
        } catch (e: Exception) {
            Log.e(TAG, "Error sending data", e)
            false
        }
    }

    fun disconnect() {
        try {
            isConnected = false
            serialPort?.close()
            usbConnection?.close()
            Log.d(TAG, "USB connection closed")
        } catch (e: Exception) {
            Log.e(TAG, "Error disconnecting", e)
        }
    }

    fun isConnected(): Boolean = isConnected
}