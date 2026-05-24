#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <BleMouse.h>

BleMouse bleMouse("ESP32 Air Mouse", "CustomTech", 100);
Adafruit_MPU6050 mpu;

// PIN DEFINITIONS 
const int btnLeft       = 12; 
const int btnRight      = 14; 
const int btnScrollMode = 27; // Hold to scroll

// GYROSCOPE VARIABLES 
float gyroOffsetY = 0.0; 
float gyroOffsetZ = 0.0;
float smoothedX = 0.0;
float smoothedY = 0.0;

//  TUNING DIALS

const float filterAlpha = 0.25;       // Smoothness (lower = smoother)
const float baseSensitivity = 16.0;   // Normal walking speed
const float accelMultiplier = 4.0;    // Flick/jump multiplier
const float scrollSensitivity = 0.5;  // Scroll wheel speed

void setup() {
  Serial.begin(115200);
  bleMouse.begin();

  if (!mpu.begin()) {
    while (1) { delay(10); } 
  }
  
  mpu.setGyroRange(MPU6050_RANGE_250_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  pinMode(btnLeft, INPUT_PULLUP);
  pinMode(btnRight, INPUT_PULLUP);
  pinMode(btnScrollMode, INPUT_PULLUP);


  // UPGRADED DEEP CALIBRATION ROUTINE

  Serial.println("Calibrating Gyro... DO NOT MOVE DEVICE!");
  
  // Give the internal silicon 3 full seconds to warm up and stabilize
  delay(3000); 

  // Take 500 micro-measurements to find a mathematically perfect zero
  sensors_event_t a, g, temp;
  for (int i = 0; i < 500; i++) { 
    mpu.getEvent(&a, &g, &temp);
    gyroOffsetY += g.gyro.y; 
    gyroOffsetZ += g.gyro.z; 
    delay(5); 
  }
  gyroOffsetY /= 500.0;
  gyroOffsetZ /= 500.0;
  
  Serial.println("Calibration Complete!");
}

void loop() {
  if (bleMouse.isConnected()) {
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp); 

    // Pure Gyro Velocity
    float rawX = g.gyro.z - gyroOffsetZ; 
    float rawY = g.gyro.y - gyroOffsetY; 

    // Smooth the data
    smoothedX = (rawX * filterAlpha) + (smoothedX * (1.0 - filterAlpha));
    smoothedY = (rawY * filterAlpha) + (smoothedY * (1.0 - filterAlpha));

    // Dynamic Acceleration Engine
    float dynamicSenseX = baseSensitivity + (abs(smoothedX) * accelMultiplier);
    float dynamicSenseY = baseSensitivity + (abs(smoothedY) * accelMultiplier);

    int moveX = -smoothedX * dynamicSenseX;  
    int moveY = smoothedY * dynamicSenseY; 
    int scrollAmount = 0;

    // Upgraded Deadzone (Kills the microscopic right-side drift)
    if (abs(moveX) < 4) moveX = 0; // Slightly higher shield on X-axis for the leak
    if (abs(moveY) < 2) moveY = 0;

    // Scroll Clutch Logic
    if (digitalRead(btnScrollMode) == LOW) {
      scrollAmount = -moveY * scrollSensitivity; 
      moveX = 0;
      moveY = 0; 
    }

    // 6. The 8-Bit Speed Limit (Prevents the cursor from reversing)
    moveX = constrain(moveX, -127, 127);
    moveY = constrain(moveY, -127, 127);
    scrollAmount = constrain(scrollAmount, -127, 127);

    // 7. Move the mouse
    if (moveX != 0 || moveY != 0 || scrollAmount != 0) {
      bleMouse.move(moveX, moveY, scrollAmount);
    }

    // BUTTON LOGIC 
    if (digitalRead(btnLeft) == LOW) {
      if (!bleMouse.isPressed(MOUSE_LEFT)) bleMouse.press(MOUSE_LEFT);
    } else {
      if (bleMouse.isPressed(MOUSE_LEFT)) bleMouse.release(MOUSE_LEFT);
    }

    if (digitalRead(btnRight) == LOW) {
      if (!bleMouse.isPressed(MOUSE_RIGHT)) bleMouse.press(MOUSE_RIGHT);
    } else {
      if (bleMouse.isPressed(MOUSE_RIGHT)) bleMouse.release(MOUSE_RIGHT);
    }

    delay(10); 
  }
}