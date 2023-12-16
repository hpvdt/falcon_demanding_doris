#include <Arduino.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "onewire.hpp"

void handleOneWireInput(); // Since it is only meant to be used as an interrupt it is locally scoped

const uint8_t bitPeriod = 50; // Period for each bit in us
const uint8_t addressWidth = 4; // Number of bits for device addresses
const uint8_t delayLag = 6;
const uint8_t dataWidth = 24;
const bool lineActive = HIGH;

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
#ifdef ARD_NANO
    if (isListener) {
        attachInterrupt(digitalPinToInterrupt(pinRX), handleOneWireInput, CHANGE);
        interrupts();
    }
#else
    if (isListener) {
        cli();                  // Disable interrupts during setup
        PCMSK |= (1 << pinRX);  // Enable interrupt handler (ISR) for our chosen interrupt pin
        GIMSK |= (1 << PCIE);   // Enable PCINT interrupt in the general interrupt mask
        sei(); 
    }
#endif
}

// Call the handler in an interrupt service routine
ISR(PCINT0_vect) {
  handleOneWireInput();
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
        delayMicroseconds(bitPeriod - delayLag);
        addressIn = addressIn << 1;
        if (digitalRead(pinRX) == lineActive) addressIn = addressIn + 1;
    }

    // If address is not matched then just sit out the rest of the transaction
    if (addressIn != oneWireAddress) {
        delayMicroseconds(dataWidth * bitPeriod - delayLag);
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

        delayMicroseconds(bitPeriod - delayLag);
    }

    digitalWrite(pinTX, LOW); // Release line
}

/**
 * @brief Requests and receives data from device on the one wire bus
 * 
 * @param targetAdd Address of the unit of interest
 * @param destination Pointer to location to store response from target
 */
void requestOneWire(uint8_t targetAdd, int32_t *destination) {

    *(destination) = 0; // Reset destination value
    noInterrupts(); // Don't want it catching it's own messages

    // Pull line down for a half period to get attention of all devices
    // Half period so that it line has more time to settle between bits
    digitalWrite(pinTX, HIGH);
    delayMicroseconds(bitPeriod / 2 - delayLag);

    // Send out address
    const int32_t maskedBitToSend = 1 << (addressWidth - 1);
    for (uint8_t i = 0; i < addressWidth; i++) {
        bool makeLineActive = ((maskedBitToSend & (targetAdd << i)) != 0);

        // Since the output gets inverter due to the inverter the opposite line active state is written out
        if (makeLineActive) digitalWrite(pinTX, !lineActive);
        else digitalWrite(pinTX, lineActive);

        delayMicroseconds(bitPeriod - delayLag);
    }
    digitalWrite(pinTX, LOW); // Release line to called device

    for (uint8_t i = 0; i < dataWidth; i++) {
        *(destination) = *(destination) << 1;
        if (digitalRead(pinRX) == lineActive) *(destination) = *(destination) + 1;
        delayMicroseconds(bitPeriod - delayLag);
    }

    // Extend sign by prefixing ones as needed
    if (*(destination) & (1L << (dataWidth - 1))) {
        *(destination) = *(destination) | (0xFFFFFFFF << (dataWidth - 1));
    }
}

void setPayload(int32_t newPayload) {
    noInterrupts();
    oneWirePayload = newPayload;
    interrupts();
}