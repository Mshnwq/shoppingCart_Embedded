#include "acc.h"
Adafruit_MPU6050 mpu;
boolean mpuSetup(){
    // Try to initialize!
    if (!mpu.begin())
    {
        Serial.println("Failed to find MPU6050 chip");
        return false;
    }
    Serial.println("MPU6050 Found!");

    // setupt motion detection
    mpu.setHighPassFilter(MPU6050_HIGHPASS_0_63_HZ);
    mpu.setMotionDetectionThreshold(1);
    mpu.setMotionDetectionDuration(20);
    mpu.setInterruptPinLatch(true); // Keep it latched.  Will turn off when reinitialized.
    mpu.setInterruptPinPolarity(true);
    mpu.setMotionInterrupt(true);
    mpu.setAccelerometerRange(MPU6050_RANGE_8_G);

    Serial.println("");
    delay(100);
    return true;
}

float checkMovement(){
    sensors_event_t accel_event;
    sensors_event_t gyro_event;
    sensors_event_t temp_event;
    mpu.getEvent(&accel_event, &gyro_event, &temp_event);

    float accel_magnitude = sqrt(accel_event.acceleration.x * accel_event.acceleration.x + accel_event.acceleration.y * accel_event.acceleration.y + accel_event.acceleration.z * accel_event.acceleration.z);
    // Serial.print("Acceleration Magnitude: ");
    // Serial.println(accel_magnitude);

    // if (accel_magnitude > 10)
    // {
    //     Serial.println("Movement detected!");
    // }
    return accel_magnitude;
}
