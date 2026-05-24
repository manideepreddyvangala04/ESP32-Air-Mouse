# ESP32-C3 BLE Air Mouse

A wearable, gesture-controlled mouse built with an ESP32-C3 Super Mini and an MPU6050 Gyroscope. This device uses Bluetooth Low Energy (BLE) to act as a native computer mouse without needing any extra software or drivers on the PC.

## Features
* **Native BLE HID:** Connects directly to Windows, Mac, Linux, or Android via standard Bluetooth.
* **Dynamic Cursor Acceleration:** Slow wrist movements give precise control, while fast movements multiply the speed to cross the screen quickly.
* **Virtual Scroll Wheel:** Holding the Smart Button freezes the cursor and turns physical vertical wrist tilts into digital scrolling.
* **Absolute Recenter:** Clicking the Smart Button snaps the cursor back to the center of the screen to fix any physical drift.
* **Auto-Calibration:** On startup, the device takes multiple gyroscope readings to establish a resting zero and eliminate hardware drift.

## Hardware Requirements
* Microcontroller: ESP32-C3 Super Mini
* Sensor: MPU6050 (Gyroscope/Accelerometer)
* Inputs: 3x Tactile Push Buttons
* Power: 3.7V LiPo Battery or USB-C power source

## Wiring Schematic
*Note: The ESP32-C3 uses 3.3V logic. Do not connect the MPU6050 to a 5V source.*

| Component      | ESP32-C3 Pin   |   Notes                       |
|----------------|----------------|-------------------------------|
| MPU6050 VCC    | `3.3V`         | Main power                    |
| MPU6050 GND    | `GND`          | Common ground                 |
| MPU6050 SDA    | `GPIO 8`       | I2C Data                      |
| MPU6050 SCL    | `GPIO 9`       | I2C Clock                     |
| Left Click     | `GPIO 2`       | Wire button diagonally to GND |
| Right Click    | `GPIO 3`       | Wire button diagonally to GND |
| Smart Button   | `GPIO 4`       | Wire button diagonally to GND |


*(Note: Internal pull-up resistors are used; no external resistors are needed.)*

## Software Setup Instructions

### 1. Arduino IDE Configuration
Install the ESP32 board definitions and select these settings under the **Tools** menu:
* **Board:** `ESP32C3 Dev Module`
* **USB CDC On Boot:** `Enabled` *(Crucial: Prevents USB lockout after flashing)*
* **Flash Mode:** `QIO`
* **Flash Size:** `4MB (32Mb)`

### 2. Required Libraries
Install via the Arduino Library Manager:
* `Adafruit MPU6050`
* `Adafruit Unified Sensor`
* `ESP32-BLE-Mouse` (by T-vK)

## Usage Guide
1. **Calibration:** On power-up, place the device perfectly flat and still for 6 seconds.
2. **Pairing:** Open your device's Bluetooth settings, search for "ESP32 Air Mouse", and pair.
3. **Controls:**
   * **Tilt Left/Right:** Move cursor horizontally
   * **Tilt Up/Down:** Move cursor vertically
   * **Left/Right Buttons:** Standard mouse clicks
   * **Smart Button (Hold):** Scroll mode (tilt up/down to scroll)
   * **Smart Button (Click):** Recenter cursor to the middle of the screen
