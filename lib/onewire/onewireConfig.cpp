#include <stdint.h>
#include "onewireConfig.hpp"

const uint8_t OW_BIT_PERIOD = 55;        // Period for each bit in us (needs to be at least thrice `OW_PULSE_PERIOD`)
const uint8_t OW_PULSE_PERIOD = 15;      // Minimum period the pulse is held for a bit on one wire bus
const uint8_t OW_ADDRESS_WIDTH = 4;      // Number of bits for device addresses on one wire bus
const uint8_t OW_DATA_WIDTH = 24;        // Width of data response on one wire bus

const uint8_t OW_NUM_ATTEMPTS = 3;       // Maximum number of automatic retries in one wire exchange
const uint16_t OW_TIMEOUT_COMMS = 2000;  // Timeout after sending a request in us for one wire protocol