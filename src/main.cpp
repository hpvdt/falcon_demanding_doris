#include <Arduino.h>

#ifdef ATTINY_CORE
#include "hx711.hpp"
#endif
#include "onewire.hpp"

/* Pin allocations
    PB0 - Resistive dividers for address
    PB1 - One Wire TX
    PB2 - Data from HX711
    PB3 - One Wire RX
    PB4 - Clock to HX711
    PB5 - RESET
*/

const int owAddMain = 2;
const int owAddTest = 0b1010; // Hardcoded address for spar boards.

#ifdef ARD_NANO
#warning "COMPILING FOR ARDUINO NANO"
const int owTX  = 3;
const int owRX  = 2;
const int owAdd = owAddMain;
#else
const int owTX  = PB1;
const int owRX  = PB3;
const int owAdd = owAddTest;
#endif


void setup() {
#ifdef ARD_NANO
    setupOneWire(owRX, owTX, owAdd, false);
    Serial.begin(115200);
    delay(100);
    Serial.println("RUNNING THE TEST STATION CODE");
    Serial.println("MISSES / TESTS");

    int32_t rec;
    requestOneWire(owAddTest, &rec);
    Serial.println(rec);
#else
    setupOneWire(owRX, owTX, owAdd, true);
    setupHX();
#endif
}

void loop() {
#ifdef ARD_NANO
    static unsigned int count = 0;
    static unsigned int missed = 0;

    int32_t rec = 0;
    if (requestOneWire(owAddTest, &rec) == false) missed++;
    count++;

    Serial.print(missed);
    Serial.print("/");
    Serial.println(count);
    delay(5);
#else
#ifdef ATTINY_CORE
    if (readyHX())setPayload(readHX());
    else delay(5);
#endif
#endif
}

