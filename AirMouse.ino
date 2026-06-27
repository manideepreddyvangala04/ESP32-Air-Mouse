#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <BleMouse.h>
#include "Config.h"
#include "Calibration.h"

BleMouse bleMouse("ESP32 Air Mouse", "CustomTech", 100);
Adafruit_MPU6050 mpu;

float gyroOffsetY = 0.0, gyroOffsetZ = 0.0;
float smoothedX = 0.0, smoothedY = 0.0;

void setup() {
  Serial.begin(115200);
  bleMouse.begin();

  if (!mpu.begin()) while (1) delay(10);
  
  mpu.setGyroRange(MPU6050_RANGE_250_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  pinMode(btnLeft, INPUT_PULLUP);
  pinMode(btnRight, INPUT_PULLUP);
  pinMode(btnScrollMode, INPUT_PULLUP);

  performCalibration(mpu, gyroOffsetY, gyroOffsetZ);
}

void loop() {
  if (bleMouse.isConnected()) {
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp); 

    float rawX = g.gyro.z - gyroOffsetZ; 
    float rawY = g.gyro.y - gyroOffsetY; 

    smoothedX = (rawX * filterAlpha) + (smoothedX * (1.0 - filterAlpha));
    smoothedY = (rawY * filterAlpha) + (smoothedY * (1.0 - filterAlpha));

    int moveX = -smoothedX * (baseSensitivity + (abs(smoothedX) * accelMultiplier));
    int moveY = smoothedY * (baseSensitivity + (abs(smoothedY) * accelMultiplier));
    
    // Deadzone and constraints
    if (abs(moveX) < 4) moveX = 0;
    if (abs(moveY) < 2) moveY = 0;

    int scrollAmount = (digitalRead(btnScrollMode) == LOW) ? -moveY * scrollSensitivity : 0;
    if (scrollAmount != 0) { moveX = 0; moveY = 0; }

    bleMouse.move(constrain(moveX, -127, 127), constrain(moveY, -127, 127), constrain(scrollAmount, -127, 127));

    // Button Handling
    if (digitalRead(btnLeft) == LOW != bleMouse.isPressed(MOUSE_LEFT)) 
        digitalRead(btnLeft) == LOW ? bleMouse.press(MOUSE_LEFT) : bleMouse.release(MOUSE_LEFT);
    
    if (digitalRead(btnRight) == LOW != bleMouse.isPressed(MOUSE_RIGHT)) 
        digitalRead(btnRight) == LOW ? bleMouse.press(MOUSE_RIGHT) : bleMouse.release(MOUSE_RIGHT);

    delay(10); 
  }
}
