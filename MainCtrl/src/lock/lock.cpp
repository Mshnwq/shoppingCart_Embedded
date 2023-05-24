#include "lock.h"
#include <Arduino.h>

void activateLock(){
    digitalWrite(13, HIGH);
    Serial.println("Lock is activated!");
}

void activateLockWithBuz()
{
    digitalWrite(13, HIGH);
    digitalWrite(12, HIGH);
    Serial.println("Lock and buzzer are activated!");
}

void deactivateLock()
{
    digitalWrite(13, LOW);
    Serial.println("Lock is deactivated!");
}
void deactivateLockWithBuz()
{
    digitalWrite(13, LOW);
    digitalWrite(12, LOW);
    Serial.println("Lock and buzzer are activated!");
}

void checkLockStatus(){
    Serial.println("Lock status is: Active");
}