#include <Arduino.h>
#include <NewPing.h>
#include "main.h"
#include "wifi/wifi.h"
#include "mqtt/mqtt.h"

// char cartToken[63] = "/cart/AN1kVAUYNynaPvk6nmyS3D6a36R42B2R0kQ338rcM7ERqF2O5GrERSco";
#define SONAR_NUM 6      // Number of sensors.
#define MAX_DISTANCE 40 // Maximum distance (in cm)to ping
#define oddPin 16      // Number of sensors.
#define evenPin 17 // Maximum distance (in cm) to ping
#define avgQuantity 3
#define errorPin 26
#define sucessPin 27
#define releasePin 15
#define checkAgainDelayMs 1000
#define delaySuccess 2
#define delayError 2
#define delayQuantum 300


long delayCheck = 400;
// bool secondCheck = true;
// long checkTime = millis();
// bool alreadyChecking = false;

 SemaphoreHandle_t readWritePennMutex;
 SemaphoreHandle_t errorPinMutex;
 SemaphoreHandle_t modeWRMutex;

NewPing sonar[SONAR_NUM] = {   // Sensor object array
  // Each sensor's trigger pin, echo pin, and max distance to ping
  NewPing(oddPin, 19, MAX_DISTANCE), 
  NewPing(evenPin, 14, 35),
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

// confirms penetration

// int checkAgain(int CheckTime){
//   if(secondCheck){
//     checkTime = millis();
//     secondCheck = false;
//     alreadyChecking = true;
//   }
//   if((checkTime+CheckTime) < millis()){
//     alreadyChecking = false;
//     return 1;
//   }else{
//     return 0;
//   }
// }
// int errorStatus = 0; // 0 = no error, 1 = error detected
// Funiction to get the average of the sensor based on pre determined number of readings. In this case 3.
long getAvgReadings(int sensorNo){
  long sensorAvg = 0;                     // Varaoble of the average
  int i = 0;                              // Variable for iteration
  while(i<avgQuantity){
    if(sonar[sensorNo].ping_cm() == 0) {  // Case where reading is 0
      sensorAvg = 0;
      break;
    }
    sensorAvg += sonar[sensorNo].ping_cm(); // Add average
    i++;                                    // Iterate 
  }
  return (sensorAvg/avgQuantity);           // Calculate average
}


// void updateReadings(){                      
//   zone1S1_Reading = round(getAvgReadings(0));  // Store the reading of the ultrasonic sensor
//   delay(30);                                   // Delay to make sure no interfernce between sensors
//   zone1S2_Reading = round(getAvgReadings(1));
//   delay(30);
//   zone2S1_Reading = round(getAvgReadings(2));
//   delay(30);
//   zone2S2_Reading = round(getAvgReadings(3));
//   delay(30);
//   zone3S1_Reading = round(getAvgReadings(4));
//   delay(30);
//   zone3S2_Reading = round(getAvgReadings(5));
//   delay(30);
//   // Zones Logic
//   zone1NoPen = (zone1S1_Reading == 0 && zone1S2_Reading == 0);  // Logic where no penetration is detected if no reading from parallel sensors 
//   zone2NoPen = (zone2S1_Reading == 0 && zone2S2_Reading == 0);
//   zone3NoPen = (zone3S1_Reading == 0 && zone3S2_Reading == 0);
// }
void showReadings(){
  Serial.print("#");
  Serial.print("L1=");
  Serial.print(zone1S1_Reading);
  Serial.print("R1=");
  Serial.print(zone1S2_Reading);
  Serial.print("L2=");
  Serial.print(zone2S1_Reading);
  Serial.print("R2=");
  Serial.print(zone2S2_Reading);
  Serial.print("L3=");
  Serial.print(zone3S1_Reading);
  Serial.print("R3=");
  Serial.print(zone3S2_Reading);
  Serial.print(".");
  Serial.println();

}

void updateReadings(){              
    zone1S1_Reading = round(getAvgReadings(0));  // Store the reading of the ultrasonic sensor
    delay(30);                                   // Delay to make sure no interfernce between sensors
    zone1S2_Reading = round(getAvgReadings(1));
    delay(30);
    zone2S1_Reading = round(getAvgReadings(2));
    delay(30);
    zone2S2_Reading = round(getAvgReadings(3));
    delay(30);
    zone3S1_Reading = round(getAvgReadings(4));
    delay(30);
    zone3S2_Reading = round(getAvgReadings(5));
    delay(30);
    // Zones Logic
    zone1NoPen = (zone1S1_Reading == 0 && zone1S2_Reading == 0);  // Logic where no penetration is detected if no reading from parallel sensors 
    zone2NoPen = (zone2S1_Reading == 0 && zone2S2_Reading == 0);
    zone3NoPen = (zone3S1_Reading == 0 && zone3S2_Reading == 0);
    showReadings();
}
void mQttTaskParallel(void * parameters){
  for(;;){
    mqttClient.loop();
  }
  vTaskDelete(NULL);
}
// void showReadings(){
//   Serial.print(0);
//   Serial.print("=");
//   Serial.print(zone1S1_Reading);
//   Serial.print(1);
//   Serial.print("=");
//   Serial.print(zone1S2_Reading);
//   Serial.print(2);
//   Serial.print("=");
//   Serial.print(zone2S1_Reading);
//   Serial.print(3);
//   Serial.print("=");
//   Serial.print(zone2S2_Reading);
//   Serial.print(4);
//   Serial.print("=");
//   Serial.print(zone3S1_Reading);
//   Serial.print(5);
//   Serial.print("=");
//   Serial.print(zone3S2_Reading);
//   Serial.println();

// }


void normalMode(){
  int err = 0;
  while(true){
    Serial.print("error status = ");
    Serial.println(errorStatus);
    Serial.print("mode is: ");
    Serial.println(mode);
    delay(50);
    updateReadings();
    if(!zone1NoPen || !zone2NoPen || !zone3NoPen){
      // delay(delayCheck);
      err++;
    // } else {
      // if(!errorStatus){
      //       //publish mqtt
      //       Serial.println("fail 2 penn");
      //       errorStatus = 1;
      //       publishMqtt(1, 0);
      //     }
    } else {
      err =0;
    }
      if (err== delayError) {
      // updateReadings();
        // if((!zone1NoPen && (!zone2NoPen || !zone3NoPen) || (!zone2NoPen && (!zone1NoPen || !zone3NoPen)) || (!zone3NoPen && (!zone1NoPen || !zone2NoPen)))){
        if(!errorStatus){
              //publish mqtt
              Serial.println("fail penn");
              errorStatus = 1;
              publishMqtt(1, 0);
            }
      }
    //   delay(delayCheck);
    //   updateReadings();
    //   if(!zone1NoPen || !zone2NoPen || !zone3NoPen){
    //     if(!errorStatus){
    //       //publish mqtt
    //       errorStatus = 1;
    //       publishMqtt(1);
    //       Serial.println("mqtt published");
    //     }
    //   }
    //   else{
    //   }
    // }
    // else{
    // }
    if(mode != 1){
      break;  
    }
  }
}
  // while(true){
  //   updateReadings();
  //   showReadings();
  //   if(!zone1NoPen || !zone2NoPen || !zone3NoPen){
  //     if(checkAgain()){
  //       if(!zone1NoPen || !zone2NoPen || !zone3NoPen){
  //         if(!errorStatus){
  //           //publish mqtt
  //           publishMqtt(1);
  //           errorStatus = 1;
  //         }
  //       }
  //       secondCheck = true;
  //     } else{
  //       secondCheck = true;
  //     }
  //     // digitalWrite(26, HIGH);
  //     // publish mqtt error

  //     // Serial.println("should be high"); 
  //   }
  //   // if(zone1NoPen && zone2NoPen && zone3NoPen){
  //   //   digitalWrite(26, LOW);
  //   //    Serial.println("should be low");
  //   // }
  //   if(mode != 0) {
  //     break;
  //   }
  // }
// }

void scaleMode(){
  int err =0;
  while(true){
    Serial.print("error status = ");
    Serial.println(errorStatus);
    Serial.print("mode is: ");
    Serial.println(mode);
    delay(delayQuantum);
    updateReadings();
    if(!zone2NoPen || !zone3NoPen){
       // delay(delayCheck);
    
      err++;
    // } else {
      // if(!errorStatus){
      //       //publish mqtt
      //       Serial.println("fail 2 penn");
      //       errorStatus = 1;
      //       publishMqtt(1, 0);
      //     }
    } else {
      err =0;
    }
      if (err == delayError) {
      // updateReadings();
        // if((!zone1NoPen && (!zone2NoPen || !zone3NoPen) || (!zone2NoPen && (!zone1NoPen || !zone3NoPen)) || (!zone3NoPen && (!zone1NoPen || !zone2NoPen)))){
          if(!errorStatus){
            //publish mqtt
            Serial.println("fail 2 penn");
            errorStatus = 1;
            publishMqtt(1, 0);
          }
        // } else{
        }
    //   delay(delayCheck);
    //   updateReadings();
    //   if(!zone2NoPen || !zone3NoPen){
    //     if(!errorStatus){
    //       //publish mqtt
    //       errorStatus = 1;
    //       publishMqtt(1);
          
    //     }
    //   } else{
    //   }
    // } else{
    // }

    if(mode != 2){
      break;  
    }
  }

}

void removeItem(){
  int out1 = 0;
  int out2 = 0;
  int in = 0;
  while(true){
    Serial.print("error status = ");
    Serial.println(errorStatus);
    Serial.print("mode is: ");
    Serial.println(mode);
    updateReadings();
    if((!zone1NoPen || !zone2NoPen || !zone3NoPen)){
      Serial.print(" in if ");
      while(true){
        Serial.print("in second loop ");
        Serial.print("error status = ");
        Serial.println(errorStatus);
        Serial.print("mode is: ");
        Serial.println(mode);
        delay(delayQuantum);
        updateReadings();
        if((!zone1NoPen && (!zone2NoPen || !zone3NoPen) || (!zone2NoPen && (!zone1NoPen || !zone3NoPen)) || (!zone3NoPen && (!zone1NoPen || !zone2NoPen)))){
          out1++;
          out2=0;
          in=0;
        } else {
          out1=0;
        }

        if(out1 == delayError){
              if(!errorStatus){
              //publish mqtt
              Serial.println("fail 2 penn");
              errorStatus = 1;
              publishMqtt(1);
        }
        }
        //   if((!zone1NoPen && (!zone2NoPen || !zone3NoPen) || (!zone2NoPen && (!zone1NoPen || !zone3NoPen)) || (!zone3NoPen && (!zone1NoPen || !zone2NoPen)))){
        //     Serial.println("fail 2 pen 3"); 
        //     if(!errorStatus){
        //       //publish mqtt
        //       Serial.println("fail 2 penn");
        //       errorStatus = 1;
        //       publishMqtt(1);
        //     }
        //   } else {
        //   }
        // } else {
        // }
        // updateReadings();
        if(zone1NoPen && zone2NoPen && zone3NoPen){
          out2++;
          out1=0;
          in=0;
        } else {
          out2=0;
        }

        if(out2 == 3){
              if(!errorStatus){
              Serial.println("fail no penn");
              errorStatus = 1;
              publishMqtt(1);
            }
        }
        //   Serial.println("fail no penn 1"); 
        //   delay(delayCheck);
        //   updateReadings();
        //   if(zone1NoPen && zone2NoPen && zone3NoPen){
        //     Serial.println("fail no pen 2"); 
        //     if(!errorStatus){
        //       Serial.println("fail no penn");
        //       errorStatus = 1;
        //       publishMqtt(1);
        //     }
        //   } else {
        //   }   
        // } else{
        // }
        // updateReadings();
        if(!zone1NoPen && zone2NoPen && zone3NoPen){
          in++;
          out1=0;
          out2=0;
        } else {
          in=0;
        }

        if(in == delaySuccess){
           if(!errorStatus){
              Serial.println("sucess remove");
              publishMqtt(0, 1);
              Serial.println("Removing success MQTT sent");
              mode = 2;
              break;
            }

        }
          
        //   Serial.println("Sucess 1"); 
        //     delay(delayCheck);
        //     updateReadings();
        //   if(!zone1NoPen && zone2NoPen && zone3NoPen){
        //     if(!errorStatus){
        //     Serial.println("sucess remove");
        //       publishMqtt(0, 1);
        //       Serial.println("Removing success MQTT sent");
        //       break;
        //     }
        //   } else {
        //   }
        // } else{
        // }
        // if(zone1NoPen && zone2NoPen && zone3NoPen){
        //   // publish mqtt success
        //   publishMqtt(1);
        //   mode = 0;
        //   break;
        // }
          if(mode != 4){
            break;  
          }
        }
      } else{
      }
      if(mode != 4){
        break;  
      }
  }
  // while(true){
  //   updateReadings();
  //   if((!zone1NoPen && (!zone2NoPen || !zone3NoPen) || (!zone2NoPen && (!zone1NoPen || !zone3NoPen)) || (!zone3NoPen && (!zone1NoPen || !zone2NoPen)))){
  //     if(checkAgain()){
  //       if((!zone1NoPen && (!zone2NoPen || !zone3NoPen) || (!zone2NoPen && (!zone1NoPen || !zone3NoPen)) || (!zone3NoPen && (!zone1NoPen || !zone2NoPen)))){
  //         if(!errorStatus){
  //           //publish mqtt
  //           publishMqtt(1);
  //           errorStatus = 1;
  //         }
  //       }
  //       secondCheck = true;
  //     } else{
  //       secondCheck = true;
  //     }
  //   if(mode != 3){
  //     break;
  //   }
  // }
}


void movingMode(){
  // bool out = false;
  int out = 0;
  int in = 0;
  while(true){
    Serial.print("error status = ");
    Serial.println(errorStatus);
    Serial.print("mode is: ");
    Serial.println(mode);
    delay(delayQuantum);
    updateReadings();
    if((!zone1NoPen && (!zone2NoPen || !zone3NoPen) || (!zone2NoPen && (!zone1NoPen || !zone3NoPen)) || (!zone3NoPen && (!zone1NoPen || !zone2NoPen)))){
      // delay(delayCheck);
    
      in++;
      out = 0;
    // } else {
      // if(!errorStatus){
      //       //publish mqtt
      //       Serial.println("fail 2 penn");
      //       errorStatus = 1;
      //       publishMqtt(1, 0);
      //     }
    } else {
      in =0;
    }
      if (in == delayError) {
      // updateReadings();
        // if((!zone1NoPen && (!zone2NoPen || !zone3NoPen) || (!zone2NoPen && (!zone1NoPen || !zone3NoPen)) || (!zone3NoPen && (!zone1NoPen || !zone2NoPen)))){
          if(!errorStatus){
            //publish mqtt
            Serial.println("fail 2 penn");
            errorStatus = 1;
            publishMqtt(1, 0);
          }
        // } else{
        }
    // } else{
    // }  
    // updateReadings();
    if(zone1NoPen && zone2NoPen && zone3NoPen){
      out++;
      in = 0;
    } else {
      out = 0;
    }
    //   updateReadings();
    //     if(zone1NoPen && zone2NoPen && zone3NoPen){
    //       Serial.println("Sucess");
    //       publishMqtt(0);
    //       mode = 0;
    //       break;
    //     } else{
    //     }
    // } else{
    // }
    // if(zone1NoPen && zone2NoPen && zone3NoPen){
    //   // publish mqtt success
    //   publishMqtt(0);
    //   mode = 0;
    //   break;
    
    if(out == 4) {
      Serial.println("Sucess");
          publishMqtt(0);
          mode = 0;
          break;
    }
    if(mode != 3){
    break;  
    }
  }
  // while(true){
  //       updateReadings();
  //       showReadings() ;
  //     if((!zone1NoPen && (!zone2NoPen || !zone3NoPen) || (!zone2NoPen && (!zone1NoPen || !zone3NoPen)) || (!zone3NoPen && (!zone1NoPen || !zone2NoPen)))){
  //       if(checkAgain()){
  //         if((!zone1NoPen && (!zone2NoPen || !zone3NoPen) || (!zone2NoPen && (!zone1NoPen || !zone3NoPen)) || (!zone3NoPen && (!zone1NoPen || !zone2NoPen)))){
  //           if(!errorStatus){
  //             //publish mqtt
  //             publishMqtt(1);
  //             errorStatus = 1;
  //           }
  //         }
  //         secondCheck = true;
  //     } else{
  //         secondCheck = true;
  //     }
  //       // if(zone1NoPen && ((!zone2NoPen&&zone3NoPen) || (zone2NoPen&&!zone3NoPen))){
  //       //   digitalWrite(errorPin, LOW);
  //       // }
  //       if(zone1NoPen && zone2NoPen && zone3NoPen){
  //         // publish mqtt success
  //         publishMqtt(0);
  //         mode = 0;
  //         break;
  //       } 
  //       if (mode != 2)
  //         break;
  //     }
}

// void MainLoop(void * parameters) {
//   for(;;){
//     Serial.print("Mode is: ");
//     Serial.println(mode);
//     switch (mode)
//     {
//     case 0:
//       break;
//     case 1:
//       Serial.print("Mode is: ");
//       Serial.println(mode);
//       normalMode();
//       break;
//     case 2:
//       Serial.print("Mode is: ");
//       Serial.println(mode);
//       scaleMode();
//       break;
//     case 3:
//       Serial.print("Mode is: ");
//       Serial.println(mode);
//       movingMode();
//       break;
//     case 4:
//       Serial.print("Mode is: ");
//       Serial.println(mode);
//       removeItem();
//       break;
//     default:
//       break;
//     }
//   }
// }


void setup() {
  pinMode(errorPin, OUTPUT);
  // connect wifit
  Serial.begin(9600); // Open serial monitor at 115200 baud to see ping results.
  wifiSetup();
  mqttSetup();
  mode =0;
  
  xTaskCreatePinnedToCore(mQttTaskParallel, "MQTT Task", 4096, NULL, 1, NULL, 1);
  

  delay(75);
}

void loop(){
    Serial.print("Mode is: ");
    Serial.println(mode);
    updateReadings();
    switch (mode)
    {
    case 0:
      break;
    case 1:
      Serial.print("Mode is: ");
      Serial.println(mode);
      normalMode();
      break;
    case 2:
      Serial.print("Mode is: ");
      Serial.println(mode);
      scaleMode();
      break;
    case 3:
      Serial.print("Mode is: ");
      Serial.println(mode);
      movingMode();
      break;
    case 4:
      Serial.print("Mode is: ");
      Serial.println(mode);
      removeItem();
      break;
    default:
      break;
    }

}