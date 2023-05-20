#include <Arduino.h>
#include <NewPing.h>

#define SONAR_NUM 6      // Number of sensors.
#define MAX_DISTANCE 40 // Maximum distance (in cm)to ping
#define oddPin 16      // Number of sensors.
#define evenPin 17 // Maximum distance (in cm) to ping
#define avgQuantity 3
#define errorPin 26
#define sucessPin 27
#define releasePin 15
int mode;

NewPing sonar[SONAR_NUM] = {   // Sensor object array
  // Each sensor's trigger pin, echo pin, and max distance to ping
  NewPing(oddPin, 19, MAX_DISTANCE), 
  NewPing(evenPin, 14, MAX_DISTANCE),
  NewPing(oddPin, 18, MAX_DISTANCE), 
  NewPing(evenPin, 12, MAX_DISTANCE),
  NewPing(oddPin, 5, MAX_DISTANCE), 
  NewPing(evenPin, 13, MAX_DISTANCE),
};

// Zone Reading variables
long zone1S1_Reading = sonar[0].ping_cm(); 
long zone1S2_Reading = sonar[1].ping_cm();
long zone2S1_Reading = sonar[2].ping_cm();
long zone2S2_Reading = sonar[3].ping_cm();
long zone3S1_Reading = sonar[4].ping_cm();
long zone3S2_Reading = sonar[5].ping_cm();

// Zones Logic
boolean zone1NoPen = (zone1S1_Reading == 0 && zone1S2_Reading == 0);  // Logic where no penetration is detected if no reading from parallel sensors 
boolean zone2NoPen = (zone2S1_Reading == 0 && zone2S2_Reading == 0);
boolean zone3NoPen = (zone3S1_Reading == 0 && zone3S2_Reading == 0);
boolean nextZone = false;

// Funiction to get the average of the sensor based on pre determined number of readings. In this case 3.
long getAvgReadings(int sensorNo){
  long sensorAvg = 0;                     // Varaoble of the average
  int i = 0;                              // Variable for iteration
  while(i<avgQuantity){
    // if(sonar[sensorNo].ping_cm() == 0) {  // Case where reading is 0
    //   sensorAvg = 0;
    //   break;
    // }
    sensorAvg += sonar[sensorNo].ping_cm(); // Add average
    i++;                                    // Iterate 
  }
  return (sensorAvg/avgQuantity);           // Calculate average
}


void updateReadings(){
  delay(30);                            // Delay to make sure no interfernce between sensors
  zone1S1_Reading = getAvgReadings(0);  // Store the reading of the ultrasonic sensor
  delay(30);
  zone1S2_Reading = getAvgReadings(1);
  delay(30);
  zone2S1_Reading = getAvgReadings(2);
  delay(30);
  zone2S2_Reading = getAvgReadings(3);
  delay(30);
  zone3S1_Reading = getAvgReadings(4);
  delay(30);
  zone3S2_Reading = getAvgReadings(5);
  
}
void showReadings(){
  Serial.print(0);
  Serial.print("=");
  Serial.print(zone1S1_Reading);
  Serial.print("cm   ");
  Serial.print(1);
  Serial.print("=");
  Serial.print(zone1S2_Reading);
  Serial.print("cm   ");
  Serial.print(2);
  Serial.print("=");
  Serial.print(zone2S1_Reading);
  Serial.print("cm   ");
  Serial.print(3);
  Serial.print("=");
  Serial.print(zone2S2_Reading);
  Serial.print("cm   ");
  Serial.print(4);
  Serial.print("=");
  Serial.print(zone3S1_Reading);
  Serial.print("cm   ");
  Serial.print(5);
  Serial.print("=");
  Serial.print(zone3S2_Reading);
  Serial.print("cm   ");
  Serial.println();

}

void normalMode(){
  while(true){
    updateReadings();
    showReadings() ;
    if(!zone1NoPen || !zone2NoPen || !zone3NoPen){
      digitalWrite(errorPin, HIGH);
    }
    if(zone1NoPen && zone2NoPen && zone3NoPen){
      digitalWrite(errorPin, LOW);
    }
    if(mode != 1) {
      break;
    }
  }
}

void scaleMode(){
  while(true){
    updateReadings();
    showReadings() ;
        if(!zone2NoPen || !zone3NoPen){
          digitalWrite(errorPin, HIGH);
          break;
        }
        if(zone2NoPen && zone3NoPen){
          digitalWrite(errorPin, LOW);
        }
        if(zone1NoPen && !zone2NoPen && zone3NoPen){
          nextZone = true;
        }
    if(nextZone){
      while(true){
        updateReadings();
        showReadings() ;
        if(!zone1NoPen || (!zone2NoPen&&!zone3NoPen)){
          digitalWrite(errorPin, HIGH);
        }
        if(zone1NoPen && ((!zone2NoPen&&zone3NoPen) || (zone2NoPen&&!zone3NoPen))){
          digitalWrite(errorPin, LOW);
        }
        if(zone1NoPen && zone2NoPen && zone3NoPen){
          digitalWrite(sucessPin, HIGH);
          delay(50);
          digitalWrite(sucessPin, LOW);
          break;
        } 
      }
    }
    if(nextZone) {
      nextZone = false;
      break;
    }
  }
}


void setup() {
  Serial.begin(9600); // Open serial monitor at 115200 baud to see ping results.
  delay(75);
}

void loop() {
  updateReadings();
  showReadings() ;
  if(digitalRead(releasePin)){
    digitalWrite(errorPin, LOW);
  }
  switch (mode)
  {
  case 1:
    normalMode();
    break;
  case 2:
    scaleMode();
    break;
  default:
    break;
  }
}