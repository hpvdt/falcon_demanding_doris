#include <Arduino.h>

#include "onewire.hpp"

void handleOneWireInput(); // Since it is only meant to be used as an interrupt it is locally scoped

const uint8_t bitPeriod = 50; // Period for each bit in us
const uint8_t addressWidth = 4; // Number of bits for device addresses
const uint8_t dataWidth = 24;
const bool lineActive = LOW;

uint8_t pinRX, pinTX;
volatile uint8_t oneWireAddress;
volatile int32_t oneWirePayload;

/**
 * @brief Setups up a one wire interface
 * 
 * @param RX Pin for reading one wire bus
 * @param TX Pin connected to one wire transistor
 * @param address Address for device (default is 0)
 * @param isListener Set to true if device is listener (default is true). If a listener, it will set the handler interrupt routine up.
 */
void setupOneWire(uint8_t RX, uint8_t TX, uint8_t address, bool isListener) {
    pinRX = RX;
    pinTX = TX;
    pinMode(pinRX, INPUT);
    pinMode(pinTX, OUTPUT);
    digitalWrite(pinTX, LOW);

    oneWireAddress = address;

    if (isListener) attachInterrupt(digitalPinToInterrupt(pinRX), handleOneWireInput, FALLING);
}

/**
 * @brief Handles potential requests from the one wire bus
 * 
 * @note Meant to be an interrupt
 * @warning Blocks for the entirety of a transmission even if not the requested unit!
 */
void handleOneWireInput() {
    uint8_t addressIn = 0;

    for (uint8_t i = 0; i < addressWidth; i++) {
        delayMicroseconds(bitPeriod);
        addressIn = addressIn << 1;
        if (digitalRead(pinRX) == lineActive) addressIn = addressIn + 1;
    }

    // If address is not matched then just sit out the rest of the transaction
    if (addressIn != oneWireAddress) {
        delayMicroseconds(dataWidth * bitPeriod);
        return;
    }

    // Delay to allow other units to catch address before the response might confuse them collecting their address
    delayMicroseconds(bitPeriod / 2);

    // Send out the data
    const int32_t maskedBitToSend = 1L << (dataWidth - 1);

    for (uint8_t i = 0; i < dataWidth; i++) {
        bool makeLineActive = ((maskedBitToSend & (oneWirePayload << i)) != 0);

        // Since the output gets inverter due to the inverter the opposite line active state is written out
        if (makeLineActive) digitalWrite(pinTX, !lineActive);
        else digitalWrite(pinTX, lineActive);

        delayMicroseconds(bitPeriod);
    }
}

/**
 * @brief Requests and receives data from device on the one wire bus
 * 
 * @param targetAdd Address of the unit of interest
 * @param destination Pointer to location to store response from target
 */
void requestOneWire(uint8_t targetAdd, int32_t *destination) {

    *(destination) = 0; // Reset destination value

    // Pull line down for a half period to get attention of all devices
    // Half period so that it line has more time to settle between bits
    digitalWrite(pinTX, HIGH);
    delayMicroseconds(bitPeriod / 2);

    // Send out address
    const int32_t maskedBitToSend = 1 << (addressWidth - 1);
    for (uint8_t i = 0; i < addressWidth; i++) {
        bool makeLineActive = ((maskedBitToSend & (targetAdd << i)) != 0);

        // Since the output gets inverter due to the inverter the opposite line active state is written out
        if (makeLineActive) digitalWrite(pinTX, !lineActive);
        else digitalWrite(pinTX, lineActive);

        delayMicroseconds(bitPeriod);
    }

    for (uint8_t i = 0; i < dataWidth; i++) {
        *(destination) = *(destination) << 1;
        if (digitalRead(pinRX) == lineActive) *(destination) = *(destination) + 1;
        delayMicroseconds(bitPeriod);
    }

    // Extend sign by prefixing ones as needed
    if (*(destination) & (1L << (dataWidth - 1))) {
        *(destination) = *(destination) | (0xFFFFFFFF << (dataWidth - 1));
    }
}