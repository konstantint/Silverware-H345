// Interface to the PAN163CX GPIO pins.
//
// License: MIT
// Author: konstantint

#pragma once
#include "PN020Series.h"

template<unsigned int Port, unsigned int Pin> class OutputPin {
public:
	GPIO_T* const port_register = (GPIO_T*) (P0_BASE + 0x40 * Port);

	inline explicit OutputPin(bool init_state) {
		// Set the whole port into GPIO mode.
		// Note that this is pretty dirty (e.g. it would conflict with any MFP
		// port settings made previously, including, say, external oscillator configs).
		//
		// The correct approach would be to clear only the relevant bits, i.e.:
		//   SYS->P5_MFP &= ~(SYS_MFP_P51_Msk
		// But coding it generically would require a long list of if-then-elses,
		// and it is not important for now.
		if constexpr (Port == 0) SYS->P0_MFP = 0;
		if constexpr (Port == 1) SYS->P1_MFP = 0;
		if constexpr (Port == 2) SYS->P2_MFP = 0;
		if constexpr (Port == 3) SYS->P3_MFP = 0;
		if constexpr (Port == 4) SYS->P4_MFP = 0;
		if constexpr (Port == 5) SYS->P5_MFP = 0;

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
