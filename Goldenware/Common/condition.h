// Utility class for tracking time a particular condition was continuously true.
//
// License: MIT
// Author: konstantint

#pragma once
#include <functional>

class Condition {
public:
	bool valid;
	uint32_t valid_since;
	uint32_t valid_for;

	void update(bool value, uint32_t time) {
		if (value) {
			if (!valid) {
				valid = true;
				valid_since = time;
				valid_for = 0;
			}
			valid_for = time - valid_since; // NB: Can overflow to negative.
		}
		else {
			valid = false;
			valid_since = 0;
			valid_for = 0;
		}
	}
};
