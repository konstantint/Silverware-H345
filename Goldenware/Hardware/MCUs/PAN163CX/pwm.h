// Interface to the PAN159 PWM module.
//
// License: MIT
// Author: konstantint

#pragma once
#include "PN020Series.h"

class Pwm {
public:
	inline Pwm() {
		// PWM module initialization (everything except PWM_EnableOutput and PWM_Start)
		CLK_EnableModuleClock(PWMCH01_MODULE);
		CLK_EnableModuleClock(PWMCH23_MODULE);
		CLK_EnableModuleClock(PWMCH45_MODULE);
		CLK_EnableModuleClock(PWMCH67_MODULE);
		for (int ch = 0; ch < 8; ch++)
			PWM_ConfigOutputChannel(PWM, ch, 1000, 0);
	}

	inline void set(uint8_t channel, uint32_t freq, uint8_t duty) {
		PWM_ConfigOutputChannel(PWM, channel, freq, duty);
	}
};
