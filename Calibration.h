#ifndef CALIBRATION_H
#define CALIBRATION_H

#include <Adafruit_MPU6050.h>

void performCalibration(Adafruit_MPU6050 &mpu, float &offY, float &offZ) {
    Serial.println("Calibrating Gyro... DO NOT MOVE DEVICE!");
    delay(3000); 

    sensors_event_t a, g, temp;
    for (int i = 0; i < 500; i++) { 
        mpu.getEvent(&a, &g, &temp);
        offY += g.gyro.y; 
        offZ += g.gyro.z; 
        delay(5); 
    }
    offY /= 500.0;
    offZ /= 500.0;
    Serial.println("Calibration Complete!");
}

#endif
