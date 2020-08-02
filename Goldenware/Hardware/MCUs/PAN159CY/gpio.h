// Interface to the PAN159 GPIO pins.
//
// License: MIT
// Author: konstantint

#pragma once
#include "Mini58Series.h"

template<unsigned int Port, unsigned int Pin> class OutputPin {
public:
	GPIO_T* const port_register = (GPIO_T*) (P0_BASE + 0x40 * Port);

	inline explicit OutputPin(bool init_state) {
		GPIO_SetMode(port_register, 1 << Pin, GPIO_MODE_OUTPUT);
		set(init_state);
	}

	inline void set(bool state) {
		GPIO_PIN_ADDR(Port, Pin) = state;
	}

	inline bool get() {
		return GPIO_PIN_ADDR(Port, Pin);
	}

	inline void toggle() {
		GPIO_PIN_ADDR(Port, Pin) = 1 - GPIO_PIN_ADDR(Port, Pin);
	}
};
