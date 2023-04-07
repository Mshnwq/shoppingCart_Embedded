#include "lock.h"
#include <Arduino.h>

void activateLock(){
    digitalWrite(2, HIGH);
    Serial.println("Lock is activated!");
}

void deactivateLock()
{
    digitalWrite(2, LOW);
    Serial.println("Lock is deactivated!");
}

void checkLockStatus(){
    Serial.println("Lock status is: Active");
}