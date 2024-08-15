#include <Arduino.h>

#ifdef ATTINY_CORE
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
const int ADDRESS_PIN = PB0;
const int OW_TX_PIN = PB1;
const int OW_RX_PIN = PB3;
#endif

/**
 * \brief Get the OneWire address for the board 
 * 
 * \note On ATtiny85-based boards this is done using a resistive divider
 * 
 * \return Address for the device to respond to
 */
uint8_t get_ow_address() {
#ifdef __AVR_ATtiny85__
    const uint16_t CUTOFF_MARKS[] = {51, 153, 245, 350, 456, 570, 670, 782, 870, 985};

    pinMode(ADDRESS_PIN, INPUT); // Just in case someone's been fooling around
    delay(5); // Allow pin to settle if it was previous driving
    uint16_t adc_reading = analogRead(ADDRESS_PIN);

    int8_t index;
    for (int8_t index = 0; index < (sizeof(CUTOFF_MARKS) / sizeof(CUTOFF_MARKS[0])); index++) {
        if (adc_reading < CUTOFF_MARKS[index]) return index;
    }
    return index;
#else
    return OW_TEST_ADDRESS;
#endif
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
#ifdef ATTINY_CORE
    if (readyHX())setPayload(readHX());
    else delay(5);
#endif
#endif
}

