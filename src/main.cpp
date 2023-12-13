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

void setup() {
    setupOneWire(PB3, PB1, 6);
}

void loop() {
    
}

