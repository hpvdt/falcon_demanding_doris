#include <Arduino.h>



void setup() {
  // put your setup code here, to run once:
  pinMode(PB3, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(PB3, HIGH); 
  delay(500); 
  digitalWrite(PB3, LOW); 
  delay(500); 
}

