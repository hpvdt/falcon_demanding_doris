#include <stdint.h>
#include "onewireConfig.hpp"

const uint8_t oneWireBitPeriod = 65;        // Period for each bit in us (needs to be at least thrice `oneWirePulsePeriod`)
const uint8_t oneWirePulsePeriod = 15;      // Minimum period the pulse is held for a bit on one wire bus
const uint8_t oneWireAddressWidth = 4;      // Number of bits for device addresses on one wire bus
const uint8_t oneWireDataWidth = 24;        // Width of data response on one wire bus

const uint8_t oneWireNumAttempts = 3;       // Maximum number of automatic retries in one wire exchange
const uint16_t oneWireTimeoutComms = 2000;  // Timeout after sending a request in us for one wire protocol