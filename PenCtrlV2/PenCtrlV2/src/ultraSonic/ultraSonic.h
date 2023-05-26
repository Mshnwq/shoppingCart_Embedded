#include <Arduino.h>

const int numReadings = 5;  // Number of readings to average
const int readingsDelay = 5;

struct Sensor {
  int trigPin;       // Trigger pin of the sensor
  int echoPin;       // Echo pin of the sensor
  long readings[numReadings];  // Array to store distance readings
  int currentIndex;  // Index of the current reading
  long total;        // Total sum of the readings
  int maxRange;      // Maximum range in centimeters
};

Sensor createSensor(int trigPin, int echoPin, int maxDis);
void initializePins(Sensor& sensor);
long measureAverageDistance(Sensor& sensor);
