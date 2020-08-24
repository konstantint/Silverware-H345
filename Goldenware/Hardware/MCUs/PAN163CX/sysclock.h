// PAN159 system clock initialization and wall-time clock functions.
//
// License: MIT
// Author: konstantint
// Based on the work of silverx and other contributors to the Silverware project.

#pragma once
#include "PN020Series.h"

volatile uint32_t _systick_count;

// SysTick IRQ invokes this once every ms
extern "C" void SysTick_Handler() {
	_systick_count++;
}

// General system initialization & realtime clock
class SysClock {
	static constexpr uint32_t SYS_CLOCK_FREQ_HZ = 48000000; // 48MHZ
	static constexpr uint32_t _cycles_per_systick = SYS_CLOCK_FREQ_HZ / 1000;
public:
	inline SysClock() {
		SYS_UnlockReg();
		CLK_InitHIRC();
		SystemCoreClockUpdate();
		SYS_LockReg();

		// Set up time counting
		_systick_count = 0;

		// Set up the interrupt to fire once every millisecond.
		SysTick_Config(_cycles_per_systick);
	}

	// Busy loop for a given number of usecs (not super-precise)
	inline void delay_us(uint32_t usec) {
		// We divide by 10 in addition because that seems to produce more or less correct results
		// Apparently the empty while loop below actually takes about 10 cycles per iteration.
		uint32_t ops_per_usec = SYS_CLOCK_FREQ_HZ / 1000000 / 10;

		volatile uint32_t count;
		count = usec * ops_per_usec;
		while (count--)
			;
	}

	// Time in microseconds since the initialization of the wall clock
	// Overflows once every 4294seconds.
	inline uint32_t micros() {
		return _systick_count * 1000
				+ (_cycles_per_systick - SysTick->VAL) * 1000
						/ _cycles_per_systick;
	}

	// Returns the PLL clock frequency
	// Interestingly, it is not exactly equal to SysClockFreqHz.
	// Not sure whether it makes some of the computations above imprecise.
	inline uint32_t get_pll_clock_frequency() {
		return CLK_GetPLLClockFreq();
	}
};
