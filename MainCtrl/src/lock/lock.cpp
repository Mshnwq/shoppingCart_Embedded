#include "lock.h"
#include <Arduino.h>

void activateLock(){
    digitalWrite(26, HIGH);
    Serial.println("Lock is activated!");
}

void activateLockWithBuz()
{
    digitalWrite(26, HIGH);
    digitalWrite(27, HIGH);
    Serial.println("Lock and buzzer are activated!");
}

void deactivateLock()
{
    digitalWrite(26, LOW);
    Serial.println("Lock is deactivated!");
}
void deactivateLockWithBuz()
{
    digitalWrite(26, LOW);
    digitalWrite(27, LOW);
    Serial.println("Lock and buzzer are activated!");
}

void checkLockStatus(){
    Serial.println("Lock status is: Active");
}