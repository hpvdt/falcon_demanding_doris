#include <Arduino.h>

#ifndef ARD_NANO
#include "hx711.hpp"
#endif
#include "onewire.hpp"

// Both LEDs are PWM enabled
const pin_size_t LED_ORANGE = PIN_PA3;
const pin_size_t LED_RED = PIN_PB0; // Usually not installed

const int OW_MAIN_ADDRESS = 2;
const int OW_TEST_ADDRESS = 0b1010; // Hardcoded address for spar boards.

const uint8_t TORSION_ADDR_CUTOFF = 12; // Addresses of this and above will read for torsion (channel B)

#ifdef ARD_NANO
#warning "COMPILING FOR ARDUINO NANO"
const int OW_TX_PIN = 3;
const int OW_RX_PIN = 2;
#else
const pin_size_t OW_TX_PIN = PIN_PA2;
const pin_size_t OW_RX_PIN = PIN_PA1;
#endif

uint8_t clocks_for_reading = 25; // Clocks to use for HX711 read (25 - Channel A, 26 - Channel B)

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
    uint8_t pin_setting = PORT_PULLUPEN_bm | PORT_INVEN_bm; // Invert so shorts are read as 1
    PORTC.PIN0CTRL = pin_setting;
    PORTC.PIN1CTRL = pin_setting;
    PORTC.PIN2CTRL = pin_setting;
    PORTC.PIN3CTRL = pin_setting;
    delayMicroseconds(1000); // Just to ensure their values stabilize
    index = VPORTC.IN & 0xF;

    if (index >= TORSION_ADDR_CUTOFF) clocks_for_reading = 26;   // Torsion
    else clocks_for_reading = 25;                               // Strain
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

    Serial.begin(9600);
    // Serial.printf("\nAddress is %d, clocks %d", ow_address, clocks_for_reading);
    // delay(5000);
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
    if (readyHX()) {
        int32_t payload = readHX(clocks_for_reading);
        setPayload(payload);
        Serial.println(payload);
    }
    else delay(5);

    // Light up when scanned
    digitalWriteFast(LED_ORANGE, millis() < led_timeout);
#endif
}

