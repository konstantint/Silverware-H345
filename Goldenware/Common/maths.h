// Math utility code
//
// License: MIT
// Author: konstantint

#pragma once

// FYI: This is not called "math.h" to avoid confusion with the system header of the same name

template<typename T> inline void clip(T& value, T min, T max) {
	if (value < min)
		value = min;
	else if (value > max)
		value = max;
}
