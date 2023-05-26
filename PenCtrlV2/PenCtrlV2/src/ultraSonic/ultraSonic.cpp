#include <Arduino.h>
#include "ultraSonic.h"

// const int numReadings = 5;  // Number of readings to average

// struct Sensor {
//   int trigPin;       // Trigger pin of the sensor
//   int echoPin;       // Echo pin of the sensor
//   long readings[numReadings];  // Array to store distance readings
//   int currentIndex;  // Index of the current reading
//   long total;        // Total sum of the readings
//   int maxRange;      // Maximum range in centimeters
// };

// Sensor sensor1 = {2, 3, {0}, 0, 0, 100};  // Example sensor configuration
// Sensor sensor2 = {4, 5, {0}, 0, 0, 150};  // Example sensor configuration

Sensor createSensor(int trigPin, int echoPin, int maxDis){
    Sensor initSensor = {trigPin, echoPin, {0}, 0, 0, maxDis};  //
    return initSensor;

}
// void setup() {
//   Serial.begin(9600);  // Initialize serial communication at 9600 baud rate
//   initializePins(sensor1);
//   initializePins(sensor2);
// }

// void loop() {
//   long averageDistance1 = measureAverageDistance(sensor1);
//   long averageDistance2 = measureAverageDistance(sensor2);
  
//   // Print the average distances to the serial monitor
//   Serial.print("Average Distance from Sensor 1: ");
//   Serial.print(averageDistance1);
//   Serial.println(" cm");
  
//   Serial.print("Average Distance from Sensor 2: ");
//   Serial.print(averageDistance2);
//   Serial.println(" cm");
  
//   delay(1000);  // Delay for 1 second before taking the next distance measurement
// }

long measureDistance(int trigPin, int echoPin) {
  // Send a short pulse to the trigger pin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Measure the duration of the echo pulse
  long duration = pulseIn(echoPin, HIGH);
  
  return duration;
}

void initializePins(Sensor& sensor) {
  pinMode(sensor.trigPin, OUTPUT);
  pinMode(sensor.echoPin, INPUT);
}

long measureAverageDistance(Sensor& sensor) {
  // Perform multiple distance measurements and calculate the average distance
    long duration;
    sensor.total = 0;
    sensor.currentIndex = 0;
  for (int i = 0; i < numReadings; i++) {

    duration = measureDistance(sensor.trigPin, sensor.echoPin);
    
    // Check if the measured distance is within the maximum range
    if (duration != 0 && (duration <= sensor.maxRange * 2 * 29.1)) {
      // Calculate the distance based on the speed of sound
      long distance = duration * 0.0343 / 2;
      // Update the readings array and total sum
      sensor.readings[sensor.currentIndex] = distance;
      sensor.total = sensor.total + sensor.readings[sensor.currentIndex];
      sensor.currentIndex = (sensor.currentIndex + 1) % numReadings;
    }
    
    vTaskDelay(readingsDelay/portTICK_PERIOD_MS);  // Delay between individual distance measurements
  }
  
  // Calculate the average distance
  long averageDistance = sensor.total / numReadings;
  
  return averageDistance;
}
