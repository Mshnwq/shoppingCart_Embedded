#include "lock.h"
#include <Arduino.h>

void activateLock(){
    digitalWrite(27, LOW);
    Serial.println("Lock is activated!");
}
void activateBuz(){
    digitalWrite(12, HIGH);
    Serial.println("Buzzer is activated!");
}
void deactivateBuz(){
    digitalWrite(12, LOW);
    Serial.println("Buzzer is deactivated!");
}

void activateLockWithBuz()
{
    digitalWrite(27, LOW); // LOCK
    digitalWrite(12, HIGH); // BUZZER
    Serial.println("Lock and buzzer are activated!");
}

void deactivateLock()
{
    digitalWrite(27, HIGH);
    Serial.println("Lock is deactivated!");
}
void deactivateLockWithBuz()
{
    digitalWrite(27, HIGH);
    digitalWrite(12, LOW);
    Serial.println("Lock and buzzer are activated!");
}

void checkLockStatus(){
    Serial.println("Lock status is: Active");
}