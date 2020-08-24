// A list of conditional compilation #defines used in various places around the code
//
// License: MIT
// Author: konstantint

#pragma once

// Include MPU6050 temperature sensor-related code?
// (It is probably meant to temperature-correct sensor readings, but this is not implemented)
#define MPU6050_USE_TEMPERATURE_SENSOR

// Include serial debug code?
// NB: Sometimes, for reasons beyond my understanding, the chip hallucinates UART input.
// If it hallucinates the input character '?' (which can happen in flight), it will switch into
// debug mode (i.e. stop listening to RX and start listening to commands on the serial interface).
// Hence for actual flying it is better to have SERIAL_DEBUG disabled
#define SERIAL_DEBUG

// Have the drone play a tune if it is idling for too long?
#define ENABLE_MUSIC
