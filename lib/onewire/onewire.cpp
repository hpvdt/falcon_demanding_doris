#include <Arduino.h>

#include "onewire.hpp"

const uint8_t bitPeriod = 30; // Period for each bit in us
const uint8_t addressLength = 4; // Number of bits for device addresses
const uint8_t dataWidth = 24;

uint8_t pinRX, pinTX;
volatile uint8_t oneWireAddress;

void setupOneWire(uint8_t RX, uint8_t TX, uint8_t address) {
    pinRX = RX;
    pinTX = TX;
    pinMode(pinRX, INPUT);
    pinMode(pinTX, OUTPUT);
    digitalWrite(pinTX, LOW);

    oneWireAddress = address;

    attachInterrupt(digitalPinToInterrupt(pinRX), handleOneWireInput, FALLING);
}

/* Handling bus messages


*/
void handleOneWireInput() {

}