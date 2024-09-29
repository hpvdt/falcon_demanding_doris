#include <Arduino.h>

#ifndef ARD_NANO
#include "hx711.hpp"
#endif
#include "onewire.hpp"

/* Pin allocations
    PB0 - Resistive dividers for address
    PB1 - One Wire TX
    PB2 - Data from HX711
    PB3 - One Wire RX
    PB4 - Clock to HX711
    PB5 - RESET
*/

const int OW_MAIN_ADDRESS = 2;
const int OW_TEST_ADDRESS = 0b1010; // Hardcoded address for spar boards.

#ifdef ARD_NANO
#warning "COMPILING FOR ARDUINO NANO"
const int OW_TX_PIN = 3;
const int OW_RX_PIN = 2;
#else
const int OW_TX_PIN = PIN_PA2;
const int OW_RX_PIN = PIN_PA1;
#endif

/**
 * \brief Get the OneWire address for the board 
 * 
 * \note On ATtiny85-based boards this is done using a resistive divider
 * 
 * \return Address for the device to respond to
 */
uint8_t get_ow_address() {
    int8_t index;
#ifdef ARD_NANO
    index = OW_TEST_ADDRESS;
#else
    // Configure address pins for digital input with pullups
    PORTC.DIRCLR = 0xF;
    PORTC.PIN0CTRL = PIN_PULLUP_ON;
    PORTC.PIN1CTRL = PIN_PULLUP_ON;
    PORTC.PIN2CTRL = PIN_PULLUP_ON;
    PORTC.PIN3CTRL = PIN_PULLUP_ON;
    delay(10); // Just to ensure their values stabilize
    index = PORTC.IN & 0xF;
#endif
    return index;
}

void setup() {
#ifdef ARD_NANO
    setupOneWire(OW_RX_PIN, OW_TX_PIN, OW_MAIN_ADDRESS, false);
    Serial.begin(115200);
    delay(100);
    Serial.println("RUNNING THE TEST STATION CODE");
    Serial.println("MISSES / TESTS");

    int32_t rec;
    Serial.println(rec);
#else
    uint8_t ow_address = get_ow_address();
    setupOneWire(OW_RX_PIN, OW_TX_PIN, ow_address, true);
    setupHX();
#endif
}

void loop() {
#ifdef ARD_NANO
    static unsigned int count = 0;
    static unsigned int missed = 0;

    int32_t rec = 0;
    if (requestOneWire(OW_TEST_ADDRESS, &rec) == false) missed++;
    count++;

    Serial.print(missed);
    Serial.print("/");
    Serial.println(count);
    delay(5);
#else
    if (readyHX())setPayload(readHX());
    else delay(5);
#endif
}

