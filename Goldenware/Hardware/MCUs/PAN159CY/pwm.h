// Interface to the PAN159 PWM module.
//
// License: MIT
// Author: konstantint

#pragma once
#include "Mini58Series.h"

class Pwm {
public:
	inline Pwm() {
		// PWM module initialization (everything except PWM_EnableOutput and PWM_Start)
		CLK_EnableModuleClock(PWMCH01_MODULE);
		CLK_EnableModuleClock(PWMCH23_MODULE);
		CLK_EnableModuleClock(PWMCH45_MODULE);
		CLK_SetModuleClock(PWMCH01_MODULE, CLK_CLKSEL1_PWMCH01SEL_HCLK, 0);
		CLK_SetModuleClock(PWMCH23_MODULE, CLK_CLKSEL1_PWMCH23SEL_HCLK, 0);
		CLK_SetModuleClock(PWMCH45_MODULE, CLK_CLKSEL2_PWMCH45SEL_HCLK, 0);
		for (int ch = 0; ch < 6; ch++)
			PWM_ConfigOutputChannel(PWM, ch, 1000, 0);
	}

	inline void set(uint8_t channel, uint32_t freq, uint8_t duty) {
		PWM_ConfigOutputChannel(PWM, channel, freq, duty);
	}
};
