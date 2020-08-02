// Interface to the PAN159 ADC converter.
// NB:
// PAN159CY has one ADC converter connected to 8 ADC channels:
// P5.3 ADC0
// P1.0 ADC1
// P1.2 ADC2
// P1.3 ADC3
// P1.4 ADC4
// P1.5 ADC5
// P5.0 ADC6
// P3.1 ADC7
// ADC7 can be remapped to internal band-gap voltage (min 1.20 typical 1.24 max 1.28 at temp 25C)
//
// License: MIT
// Author: konstantint

#pragma once
#include "Mini58Series.h"

class Adc {
public:
	inline Adc() {
		SYS_UnlockReg();
		CLK_EnableModuleClock(ADC_MODULE);

		// While using VBG as channel 7 source, ADC module clock must /b not exceed 300kHz
		// With a system clock frequency of 50MHz, 300KHz can be obtained with a divider of 200 or so.
		CLK_SetModuleClock(ADC_MODULE, CLK_CLKSEL1_ADCSEL_HIRC,
				CLK_CLKDIV_ADC(200));
		ADC_POWER_ON(ADC);

		SYS_LockReg();
	}

	uint32_t read_channel(uint8_t channel) {
		ADC_Open(ADC, 0 /* unused */, 0 /* unused */, 1 << channel);
		ADC_START_CONV(ADC);
		while (ADC_IS_BUSY(ADC))
			;
		return ADC_GET_CONVERSION_DATA(ADC, 0 /* unused */);
	}
};
