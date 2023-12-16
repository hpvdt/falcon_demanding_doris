#include <Arduino.h>

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
const int owAddTest = 0b1010;

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
    Serial.println("RUNNING THE MAIN STATION CODE");

    int32_t rec;
    requestOneWire(owAddTest, &rec);
    Serial.println(rec);
#else
    setupOneWire(owRX, owTX, owAdd, true);
#endif
}

void loop() {
#ifdef ARD_NANO
    int32_t rec = 0;
    requestOneWire(owAddTest, &rec);
    Serial.println(rec);
    delay(5000);
#else
    setPayload(0b101010);
    delay(20);
#endif
}

