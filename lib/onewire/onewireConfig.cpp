#include <stdint.h>
#include "onewireConfig.hpp"

const uint8_t bitPeriod = 65; // Period for each bit in us (needs to be at least thrice `pulsePeriod`)
const uint8_t pulsePeriod = 15; // Minimum period the pulse is held for a bit 
const uint8_t addressWidth = 4; // Number of bits for device addresses
const uint8_t dataWidth = 24;

const uint8_t numberAttempts = 3;
const uint16_t timeoutComms = 2000; // Timeout after sending a request in us