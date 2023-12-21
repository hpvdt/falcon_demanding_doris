#ifndef ONEWIRE_CONFIG_HEADER_H
#define ONEWIRE_CONFIG_HEADER_H

#include <stdint.h>

extern const uint8_t bitPeriod; // Period for each bit in us (needs to be at least thrice `pulsePeriod`)
extern const uint8_t pulsePeriod; // Minimum period the pulse is held for a bit 
extern const uint8_t addressWidth; // Number of bits for device addresses
extern const uint8_t dataWidth;

extern const uint8_t numberAttempts;
extern const uint16_t timeoutComms; // Timeout after sending a request in us

#endif