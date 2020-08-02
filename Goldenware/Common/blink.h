// Led blinking utilities.
//
// License: MIT
// Author: konstantint
// Based on the work of silverx and other contributors to the Silverware project.

#pragma once
template<typename Board> void blink(Board& board, int num = 1,
		uint32_t period_us = 200000) {
	for (int i = 0; i < num; i++) {
		board.led.set(true);
		board.clock.delay_us(period_us);
		board.led.set(false);
		board.clock.delay_us(period_us);
	}
}

// Infinite loop, cyclically blinking the given status code
template<typename Board> void blink_forever(Board& board, int status) {
	while (true) {
		blink(board, status);
		board.clock.delay_us(1000000);
	}
}

// Called once in a loop, enables led-on led-off blinking with a given cycle
template<typename Led> void blink_tick(Led& led, uint32_t time, uint32_t period,
		uint32_t on_period) {
	led.set((time % period) < on_period);
}
