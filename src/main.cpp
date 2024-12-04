#include <Arduino.h>
#include "hx711.hpp"
#include "onewire.hpp"

// Both LEDs are PWM enabled
const pin_size_t LED_GREEN = PIN_PA3;
const pin_size_t LED_RED = PIN_PB0; // Usually not installed

const uint8_t TORSION_ADDR_CUTOFF = 12; // Addresses of this and above will read for torsion (channel B)

const pin_size_t OW_TX_PIN = PIN_PA2;
const pin_size_t OW_RX_PIN = PIN_PA1;

uint8_t clocks_for_reading = 25; // Clocks to use for HX711 read (25 - Channel A, 26 - Channel B)

/**
 * \brief Get the OneWire address for the board 
 * 
 * \return Address for the device to respond to
 */
uint8_t get_ow_address() {
    int8_t index;

    // Configure address pins for digital input with pullups
    PORTC.DIRCLR = 0xF;
    uint8_t pin_setting = PORT_PULLUPEN_bm | PORT_INVEN_bm; // Invert so shorts are read as 1
    PORTC.PIN0CTRL = pin_setting;
    PORTC.PIN1CTRL = pin_setting;
    PORTC.PIN2CTRL = pin_setting;
    PORTC.PIN3CTRL = pin_setting;
    delayMicroseconds(1000); // Just to ensure their values stabilize
    index = VPORTC.IN & 0xF;

    if (index >= TORSION_ADDR_CUTOFF) clocks_for_reading = 26;  // Torsion
    else clocks_for_reading = 25;                               // Strain

    return index;
}

void setup() {
    uint8_t ow_address = get_ow_address();
    setupOneWire(OW_RX_PIN, OW_TX_PIN, ow_address, true);
    setupHX();

    Serial.begin(9600);

    pinMode(LED_GREEN, OUTPUT);
    pinMode(LED_RED, OUTPUT);
}

void loop() {
    if (readyHX()) {
        int32_t payload = readHX(clocks_for_reading);
        payload = ow_generate_test_data();
        setPayload(payload);
        Serial.println(payload);
    }
    else delay(5);

    // Light up when scanned
    digitalWriteFast(LED_GREEN, millis() < led_timeout);
}

