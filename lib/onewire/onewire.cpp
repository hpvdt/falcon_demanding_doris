#include <Arduino.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "onewire.hpp"

void handleOneWireInput(); // Since it is only meant to be used as an interrupt it is locally scoped
void sendData(uint32_t data, uint8_t width);

const uint8_t bitPeriod = 65; // Period for each bit in us (needs to be at least thrice `pulsePeriod`)
const uint8_t pulsePeriod = 15; // Minimum period the pulse is held for a bit 
const uint8_t addressWidth = 4; // Number of bits for device addresses
const uint8_t dataWidth = 24;

volatile uint8_t pinRX, pinTX;
volatile uint8_t oneWireAddress;
volatile int32_t oneWirePayloadOut;
volatile int32_t oneWirePayloadIn;
volatile bool oneWireMessageReceived;
volatile bool oneWireListener;

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
    oneWireListener = isListener;
#ifdef ATTINY_CORE
    noInterrupts();         // Disable interrupts during setup
    PCMSK |= (1 << pinRX);  // Enable interrupt handler (ISR) for our chosen interrupt pin
    GIMSK |= (1 << PCIE);   // Enable PCINT interrupt in the general interrupt mask
    interrupts();
#else
    attachInterrupt(digitalPinToInterrupt(pinRX), handleOneWireInput, CHANGE);
#endif
}

#ifdef ATTINY_CORE
// Call the handler in an interrupt service routine
ISR(PCINT0_vect) {
  handleOneWireInput();
}
#endif

/**
 * @brief Handles potential requests from the one wire bus
 * 
 * @note Meant to be an interrupt
 * @warning Blocks for the entirety of a transmission to host if responding
 */
void handleOneWireInput() {
    static unsigned long lastEdge = 0; // Store previous edge timestamp
    unsigned long present = micros();

#ifdef ATTINY_CORE
    bool reading = PINB & (1 << pinRX);
#else
    bool reading = digitalRead(pinRX);
#endif
    unsigned long delta = present - lastEdge;

    static uint8_t bitCount = 0;
    static uint8_t ignoreCount = 0;
    static uint32_t tempData = 0;

    // Too short since last edge, ignore. Probably setting up the next actual edge
    if (delta < (3 * pulsePeriod)) return;
    else lastEdge = present;

    // See if the edge is late (new message or timeout)
    if (delta > (2 * bitPeriod)) {
        bitCount = 0;
        ignoreCount = 0;
        tempData = 0;
        return;
    }

    // Are we ignoring data edges? (From other responders)
    if (ignoreCount != 0) {
        ignoreCount--;
        return;
    }
    
    // Process the edge
    tempData = (tempData << 1) + reading;
    bitCount++;
    
    // Process depending on if it's a caller or not
    if (oneWireListener) {
        // If there's an address check for a match
        if (bitCount == addressWidth) {

            if (tempData == oneWireAddress) sendData(oneWirePayloadOut, dataWidth);
            else {
                // Ignore the other device's response
                ignoreCount = dataWidth;
            }

            // Reset for next message
            bitCount = 0;
            tempData = 0;
        }
    }
    else {
        // If awaiting a response
        if (bitCount == dataWidth) {
            oneWireMessageReceived = true;

            // Extend sign by prefixing ones as needed prior to recording it
            if (tempData & (1L << (dataWidth - 1))) {
                oneWirePayloadIn = tempData | (0xFFFFFFFF << (dataWidth - 1));
            }
            else oneWirePayloadIn = tempData; 

            // Reset for next message
            bitCount = 0;
            ignoreCount = 0;
            tempData = 0;
        }
    }
}

/**
 * @brief Requests and receives data from device on the one wire bus
 * 
 * @warning Leaves interrupts enabled once completed
 * 
 * @param targetAdd Address of the unit of interest
 * @param destination Pointer to location to store response from target
 * 
 * @return True if message was received
 */
bool requestOneWire(uint8_t targetAdd, int32_t *destination) {

    noInterrupts(); // Don't want it catching it's own messages
    oneWireMessageReceived = false; // Clear received flag

    // Pull line down for a half period to get attention of all devices
    digitalWrite(pinTX, HIGH);
    delayMicroseconds(pulsePeriod);

    // Send out address
    sendData(targetAdd, addressWidth);

    // Read data in from line
    unsigned long timeoutMark = millis() + 10;
    while (!oneWireMessageReceived && (millis() < timeoutMark)) {
        //delayMicroseconds(1000);
    }

    if (oneWireMessageReceived) *(destination) = oneWirePayloadIn; 
    //else *(destination) = 0;

    return oneWireMessageReceived;
}

/**
 * @brief Send data over one wire interface
 * 
 * @note Shifts data out MSB first. Positive dominant edges are for 1.
 * 
 * @warning Leaves interrupts enabled on completion
 * 
 * @param data Payload to send
 * @param width The width of the data to send in bits
 */
void sendData(uint32_t data, uint8_t width) {
    noInterrupts(); // Don't want interrupts to catch outgoing message

#ifdef ATTINY_CORE
    // Port values to set TX without changing other pins
    uint8_t outputLow = PORTB & ~(1 << pinTX);
    uint8_t outputHigh = PORTB | (1 << pinTX);
#endif

    for (uint8_t i = width; i > 0; i--) {
        uint32_t mask = 1L << (i - 1); // Needs the `1L` otherwise mask will be 16 bits wide

        bool currentBit = ((mask & data) != 0);
#ifdef ATTINY_CORE
        // Probably the ATtiny85
        if (currentBit == true) {
            PORTB = outputHigh;
            delayMicroseconds(bitPeriod - pulsePeriod);
            PORTB = outputLow;
            delayMicroseconds(pulsePeriod);
        }
        else {
            PORTB = outputLow;
            delayMicroseconds(bitPeriod - pulsePeriod);
            PORTB = outputHigh;
            delayMicroseconds(pulsePeriod);
        }
#else
        digitalWrite(pinTX, currentBit);
        delayMicroseconds(bitPeriod - pulsePeriod);
        digitalWrite(pinTX, !currentBit);
        delayMicroseconds(pulsePeriod);
#endif
    }

    digitalWrite(pinTX, LOW); // Release line
    interrupts();
}

/**
 * @brief Set the one wire response payload
 * 
 * @param newPayload What to repsond with next one wire query
 */
void setPayload(int32_t newPayload) {
    noInterrupts();
    oneWirePayloadOut = newPayload;
    interrupts();
}