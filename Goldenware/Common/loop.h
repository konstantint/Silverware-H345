// Loop time tracking utility.
//
// License: MIT
// Author: konstantint

#pragma once

template<int LoopTime, typename Clock> class LoopGuard {
	Clock& _clock;
public:
	uint32_t loop_started;

	inline explicit LoopGuard(Clock& clock) :
			_clock(clock), loop_started(clock.micros()) {
	}

	// May be negative on time overflows
	inline int32_t elapsed() {
		return (int32_t) (_clock.micros() - loop_started);
	}

	inline ~LoopGuard() {
		volatile uint32_t cycles = 0;
		while (elapsed() > 0 && elapsed() < LoopTime)
			cycles++;
		//dprintf("Idle cycles: %i\n", cycles);
	}
};
