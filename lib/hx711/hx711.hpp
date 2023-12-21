#ifndef HX711_HEADER_H
#define HX711_HEADER_H

#include <Arduino.h>

#ifndef ATTINY_CORE
#error "HX711 Library not meant for chips other than the ATtiny85!"
#endif

void setupHX();
bool readyHX();
uint32_t readHX();

#endif