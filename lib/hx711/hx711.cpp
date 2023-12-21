#include "hx711.hpp"

#include <Arduino.h>

const uint8_t DOUT = PB2;
const uint8_t PDSCK = PB4;

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
    return ((PINB & (1 << DOUT)) == 0);
}

/**
 * @brief Reads value from HX711
 * @note Always sets HX711 to read with a gain of 128 on channel A
 * @warning Disables interrupts during read
 * 
 * @return Reading, not sign extended
 */
uint32_t readHX() {
    uint32_t reading = 0L;

    uint8_t clkLow = PORTB & ~(1 << PDSCK);
    uint8_t clkHigh = PORTB | (1 << PDSCK);

    noInterrupts();
    // Generate and clock in data for 25 pulses 
    // (so next read is with a gain of 128 on channel A)
    for (uint8_t i = 0; i < 25; i++) {
        reading = reading << 1;
        PORTB = clkHigh;
        bool temp = ((PINB & (1 << DOUT)) != 0); // Read in DOUT pin
        reading = reading + temp;
        PORTB = clkLow;
    }
    interrupts();

    // Shift back down one since data was only actually present the first 24 cycles
    reading = reading >> 1; 

    return reading;
}