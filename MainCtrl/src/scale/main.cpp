#include "main.h"
// pins:
const int DOUT_PIN = 16;     // mcu > HX711 dout pin
const int SCK_PIN = 17;      // mcu > HX711 sck pin
const int NUM_READINGS = 30; // This is the number of readings that will be used to get an average weight

const int CV_ADDRESS = 0; // EEPROM address
unsigned long t = 0;      // timestamp
boolean scaleSetup(){
    unsigned long stabilizingtime = 2500; // preciscion right after power-up can be improved by adding a few seconds of stabilizing time
    LoadCell.begin(); // why I forgot this :(
    boolean do_Tare = true;               // set this to false if you don't want tare to be performed in the next step
    LoadCell.start(stabilizingtime, do_Tare);
    if (LoadCell.getTareTimeoutFlag() || LoadCell.getSignalTimeoutFlag())
    {
        Serial.println("Timeout, check MCU>HX711 wiring and pin designations");
        while (1); // Why you used this?
        return false; // scale setup failed 
    }
    else
    {
        LoadCell.setCalFactor(1.00); // user set calibration value (float), initial value 1.0 may be used for this sketch
        Serial.println("Scale Setup is complete");
    }
    while (!LoadCell.update()); 
    scaleCalibrate(); // start calibration procedure
    return true;
}

boolean scaleCalibrate(){
    Serial.println("***");
    Serial.println("Start calibration:");
    Serial.println("Place the load cell an a level stable surface.");
    Serial.println("Remove any load applied to the load cell.");
    Serial.println("Send 't' from serial monitor to set the tare offset.");
    boolean _resume = true;
    while (_resume)
    {
        LoadCell.update();
        if (Serial.available() > 0)
        {
            char inByte = Serial.read();
            if (inByte == 't')
                LoadCell.tareNoDelay();
        }
        if (LoadCell.getTareStatus() == true)
        {
            Serial.println("Tare complete");
            _resume = false;
        }
    }
    Serial.println("Now, place your known mass on the loadcell.");
    Serial.println("Then send the weight of this mass (i.e. 100.0) from serial monitor.");

    float known_mass = 0;
    _resume = true;
    while (_resume)
    {
        LoadCell.update();
        if (Serial.available() > 0)
        {
            known_mass = Serial.parseFloat();
            if (known_mass != 0)
            {
                Serial.print("Known mass is: ");
                Serial.println(known_mass);
                _resume = false;
            }
        }
    }
    // LoadCell.refreshDataSet();                                      // refresh the dataset to be sure that the known mass is measured correct
    float newCalibrationValue = LoadCell.getNewCalibration(known_mass); // get the new calibration value    Serial.print("New calibration value has been set to: ");
    Serial.print(newCalibrationValue);
    Serial.println(", use this as calibration value (calFactor) in your project sketch.");
    Serial.print("Save this value to EEPROM adress ");
    Serial.print(CV_ADDRESS);
    Serial.println("? y/n");

    _resume = true;
    while (_resume)
    {
        if (Serial.available() > 0)
        {
            char inByte = Serial.read();
            if (inByte == 'y')
            {
                #if defined(ESP8266) || defined(ESP32)
                    EEPROM.begin(512);
                #endif
                    EEPROM.put(CV_ADDRESS, newCalibrationValue);
                #if defined(ESP8266) || defined(ESP32)
                    EEPROM.commit();
                #endif
                EEPROM.get(CV_ADDRESS, newCalibrationValue);
                Serial.print("Value ");
                Serial.print(newCalibrationValue);
                Serial.print(" saved to EEPROM address: ");
                Serial.println(CV_ADDRESS);
                _resume = false;
            }
            else if (inByte == 'n')
            {
                Serial.println("Value not saved to EEPROM");
                _resume = false;
            }
        }else{
            _resume = false; // break from the loop
            return false; // Calibration failed
        }
    }
    Serial.println("End calibration");
    Serial.println("***");
    Serial.println("To re-calibrate, send 'r' from serial monitor.");
    Serial.println("For manual edit of the calibration value, send 'c' from serial monitor.");
    Serial.println("***");
    return true; // clibration succeed
}

float scaleReadings()
{
    static boolean newDataReady = 0;
    // check for new data/start next conversion:
    if (LoadCell.update())
        newDataReady = true;

    // get smoothed value from the dataset:
    if (newDataReady)
    {
        float data = LoadCell.getData();
        return data;
    }{
        Serial.println("No data available at ADC.");
        return 0;
    }

    // TODO: make another function for the following lines: 

    // // receive command from serial terminal
    // if (Serial.available() > 0)
    // {
    //     char inByte = Serial.read();
    //     if (inByte == 't')
    //         LoadCell.tareNoDelay(); // tare
    //     else if (inByte == 'r')
    //         scaleCalibrate(); // calibrate
    //     else if (inByte == 'c')
    //         chageScaleSavedCalFactor(); // edit calibration value manually
    // }
    // // check if last tare operation is complete
    // if (LoadCell.getTareStatus() == true)
    // {
    //     Serial.println("Tare complete");
    // }
}

void chageScaleSavedCalFactor()
{
    float oldCalibrationValue = LoadCell.getCalFactor();
    boolean _resume = true;
    Serial.println("***");
    Serial.print("Current scale calibration value is: ");
    Serial.println(oldCalibrationValue);
    Serial.println("Now, send the new value from Serial monitor, i.e. 696.0");
    float newCalibrationValue;
    while (_resume)
    {
        if (Serial.available() > 0)
        {
            newCalibrationValue = Serial.parseFloat();
            if (newCalibrationValue != 0)
            {
                Serial.print("New calibration value is: ");
                Serial.println(newCalibrationValue);
                LoadCell.setCalFactor(newCalibrationValue);
                _resume = false;
            }
        }
    }
    _resume = true;
    Serial.print("Save this value to EEPROM adress ");
    Serial.print(CV_ADDRESS);
    Serial.println("? y/n");
    while (_resume)
    {
        if (Serial.available() > 0)
        {
            char inByte = Serial.read();
            if (inByte == 'y')
            {
                #if defined(ESP8266) || defined(ESP32)
                    EEPROM.begin(512);
                #endif
                    EEPROM.put(CV_ADDRESS, newCalibrationValue);
                #if defined(ESP8266) || defined(ESP32)
                    EEPROM.commit();
                #endif
                EEPROM.get(CV_ADDRESS, newCalibrationValue);
                Serial.print("Value ");
                Serial.print(newCalibrationValue);
                Serial.print(" saved to EEPROM address: ");
                Serial.println(CV_ADDRESS);
                _resume = false;
            }
            else if (inByte == 'n')
            {
                Serial.println("Value not saved to EEPROM");
                _resume = false;
            }
        }
    }
    Serial.println("End change calibration value");
    Serial.println("***");
}