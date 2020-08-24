// Error codes
//
// License: MIT
// Author: konstantint

#pragma once

enum class ErrorCode : char {
	E_SUCCESS = 0,
	E_LOW_BATTERY = 2,
	E_NO_GYRO = 4,
	E_CLOCK = 5,
	E_LOOP_TIME = 6,
	E_I2C = 7,
	E_I2C_MAIN = 8,
	E_NO_RADIO = 9,
};
