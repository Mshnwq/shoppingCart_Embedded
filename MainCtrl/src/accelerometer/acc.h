#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

extern Adafruit_MPU6050 mpu;

// Initial setup for the mpu module
// return true if setup is complete
// false otherwise
boolean mpuSetup();

// used inside loop to check if there is a movement
float checkMovement();
