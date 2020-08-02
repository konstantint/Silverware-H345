// Vector of motor powers
//
// License: MIT
// Author: konstantint

#pragma once
#include <array>
#include "maths.h"

enum QuadcopterMotorId
	: char {
		BACK_RIGHT = 0, FRONT_RIGHT = 1, BACK_LEFT = 2, FRONT_LEFT = 3

};

// For convenient range-for loops
static constexpr QuadcopterMotorId ALL_MOTORS[4] = { BACK_RIGHT, FRONT_RIGHT,
		BACK_LEFT, FRONT_LEFT };

// Four motor powers in a single vector
struct QuadcopterMotorPowers: public std::array<float, 4> {
	typedef std::array<float, 4> base_t;

	QuadcopterMotorPowers() :
			base_t() {
	}
	QuadcopterMotorPowers(base_t arr) :
			base_t(arr) {
	}

	// Set motor powers based on requested roll/pitch/yaw and throttle powers
	void set_rpyt(float roll, float pitch, float yaw, float throttle) {
		(*this)[FRONT_RIGHT] = throttle - roll - pitch + yaw;
		(*this)[FRONT_LEFT] = throttle + roll - pitch - yaw;
		(*this)[BACK_RIGHT] = throttle - roll + pitch - yaw;
		(*this)[BACK_LEFT] = throttle + roll + pitch + yaw;

		//for (int i = 0; i < 4; i++) clip((*this)[i], 0.0f, 1.0f);
	}
};
