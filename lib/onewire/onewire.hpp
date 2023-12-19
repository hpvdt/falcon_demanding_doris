#ifndef ONEWIRE_HEADER_H
#define ONEWIRE_HEADER_H

#include <Arduino.h>

void setupOneWire(uint8_t RX, uint8_t TX, uint8_t address = 0, bool isListener = true);
bool requestOneWire(uint8_t targetAdd, int32_t *destination);
void setPayload(int32_t newPayload);

#endif