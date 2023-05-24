#include <HX711_ADC.h>
#if defined(ESP8266) || defined(ESP32) || defined(AVR)
#include <EEPROM.h>
#endif
#ifndef SCALE_H
#define SCALE_H

// // scale pins:
extern const int DOUT_PIN;     // mcu > HX711 dout pin
extern const int SCK_PIN;      // mcu > HX711 sck pin
// extern int NUM_READINGS; // This is the number of readings that will be used to get an average weight

// // HX711 constructor:
extern HX711_ADC LoadCell;

// extern int CV_ADDRESS; // EEPROM address
// unsigned long t; // timestamp

/*
* scaleSetup: function used to initialize scale variables
*/
boolean scaleSetup();

/*
 * calibrate: used to calculate calibration parameter
 * and store it in the EEPROM
 */
boolean scaleCalibrate();

/*
 * scaleReadings: used to get the current readings
 * from the scale.
 */
float scaleReadings();

/*
 * chageSavedCalFactor: used to change the calibration
 * value and save it in the EEPROM address.
 */
void chageScaleSavedCalFactor();
#endif