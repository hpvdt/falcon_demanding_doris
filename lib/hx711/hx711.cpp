#include "hx711.hpp"

#include <Arduino.h>

const uint8_t DOUT = PIN_PA6;
const uint8_t PDSCK = PIN_PA7;



/**
 * @brief Sets up the HX711 interface
 * 
 */
void setupHX() {

    pinMode(DOUT, INPUT_PULLUP);
    pinMode(PDSCK, OUTPUT);

    digitalWrite(PDSCK, LOW); // Set low, otherwise HX711 will sleep
}

/**
 * @brief Checks if there is data available from HX711
 * 
 * @return True if HX711 has data ready.
 */
bool readyHX() {
    return (digitalReadFast(DOUT) == 0);
}

/**
 * @brief Reads value from HX711
 * 
 * \param clocks Number of clocks to read (25 for next reading to be 128 gain on channel A, 26 for 32 gain channel B)
 * 
 * @warning Disables interrupts during read
 * 
 * @return Reading, not sign extended
 */
int32_t readHX(uint8_t clocks) {

    int32_t reading = 0L;

    const uint8_t clk_mask = (1 << digital_pin_to_bit_position[PDSCK]);
    const uint8_t input_shift = digital_pin_to_bit_position[DOUT];

    noInterrupts();
    // Generate and clock in data for required number of pulses 
    // (25 so next read is with a gain of 128 on channel A, 26 for gain of 32 on channel B)
    for (uint8_t i = 0; i < clocks; i++) {
        reading = reading << 1;
        PORTA.OUTSET = clk_mask;
        uint8_t temp = (VPORTA.IN >> input_shift) & 1; // Read in DOUT pin
        reading = reading | temp;
        PORTA.OUTCLR = clk_mask;
    }
    interrupts();

    // Shift back down since data was only actually present the first 24 cycles
    reading = reading >> (clocks - 24); 

    // Sign extension based on leading bit
    if ((reading & 0x800000L) != 0) reading = reading | 0xFF000000L;

    return reading;
}