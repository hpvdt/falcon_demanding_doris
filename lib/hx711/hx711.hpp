#ifndef HX711_HEADER_H
#define HX711_HEADER_H

#include <Arduino.h>

#ifndef ATTINY_CORE
#warning "HX711 Library not meant for chips other than the ATtiny85!"
void setupHX() __attribute__ ((unavailable));
bool readyHX() __attribute__ ((unavailable));
int32_t readHX() __attribute__ ((unavailable));

#else
void setupHX();
bool readyHX();
unt32_t readHX();
#endif
#endif