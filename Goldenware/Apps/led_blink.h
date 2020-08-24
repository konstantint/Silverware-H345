// Simple LED blinker
//
// License: MIT
// Author: konstantint

#pragma once

template<typename Led, typename Clock> struct LedBlinkApp {
	Led& led;
	Clock& clock;

	inline LedBlinkApp(Led& led_, Clock& clock_) :
			led(led_), clock(clock_) {
	}

	inline int main() {
	    while (true) {
			clock.delay_us(500000);
			led.toggle();
		}
		return 0;
	}
};

