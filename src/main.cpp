#include <Arduino.h>

// read HX711

/* 
  hx711: 
    PB1 - mosfet
    PB4 - attiny to hx711
    PB3 - output
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

