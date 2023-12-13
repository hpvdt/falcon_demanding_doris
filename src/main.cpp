#include <Arduino.h>

// read HX711

/* Pin allocations
    PB0 - Resistive dividers for address
    PB1 - One Wire TX
    PB2 - Data from HX711
    PB3 - One Wire RX
    PB4 - Clock to HX711
    PB5 - RESET
*/

void setup() {
  // put your setup code here, to run once:
  pinMode(3, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(3, HIGH); 
  delay(50); 
  digitalWrite(3, LOW); 
  delay(50); 
}

