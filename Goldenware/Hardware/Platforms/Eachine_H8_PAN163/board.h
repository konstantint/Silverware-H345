// Definition of the Eachine H8 quadcopter board (PAN163CX-based) hardware, suitable for control algorithms.
//
// License: MIT
// Author: konstantint
// Based on the work of silverx and other contributors to the Silverware project.

#pragma once
#include "motors.h"
#include "../../MCUs/PAN163CX/gpio.h"
#include "../../MCUs/PAN163CX/sysclock.h"
#include "../../../Common/errors.h"


// Eachine H8 (PAN163-based board)
struct EachineH8Base {
	// MCU: PAN163CX
	SysClock clock;

	// LED
	OutputPin<5, 1> led;

	// Motors
	Motors motors;

	inline EachineH8Base() :
			clock(),
			led(false),
			motors() {
	}

	inline void set_motors(const QuadcopterMotorPowers& motor_powers) {
		for (auto i : ALL_MOTORS)
			motors.set_power(i, motor_powers[i]);
	}

	// Run self-diagnostic test(s)
	inline ErrorCode self_test() {
		return ErrorCode::E_SUCCESS;
	}
};
