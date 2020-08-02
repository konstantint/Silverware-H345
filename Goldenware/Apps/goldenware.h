// The main quadcopter firmware app
//
// License: MIT
// Author: konstantint
// Based on the work of silverx and other contributors to the Silverware project.

#pragma once
#include <algorithm>
#include <cstdint>
#include <ratio>
#include "serial_debug.h"
#include "../Common/condition.h"
#include "../Common/defines.h"
#include "../Common/blink.h"
#include "../Common/filter.h"
#include "../Common/gyro.h"
#include "../Common/loop.h"
#include "../Common/motors.h"
#include "../Common/printf.h"
#include "../Sound/melodies.h"

enum BlinkCodes : char {
	E_LOW_BATTERY = 2,
	E_NO_GYRO = 4,
	E_CLOCK = 5,
	E_LOOP_TIME = 6,
	E_I2C = 7,
	E_I2C_MAIN = 8
};

// The QuadcopterBoard type must provide the following fields:
//  - clock
//  - led
//  - battery
//  - mpu6050
//  - read_gyro, read_rx and set_motors methods.

template<typename QuadcopterBoard, typename Controller>
struct GoldenwareApp {
	static constexpr int LOOP_TIME = 1000; // 1ms per loop
	static constexpr float LOW_BATTERY_THRESHOLD = 3.3f;

	QuadcopterBoard& board;
	Controller controller;

	LPFValue<float, std::ratio<63, 64>> v_batt = 3.7; // Battery voltage

	inline GoldenwareApp(QuadcopterBoard& board_) :
			board(board_), controller(board) {
		// Welcome blink
		blink(board);

		// Self-test
		if (!board.mpu6050.self_test())
			failure(E_NO_GYRO);

		// Read battery voltage and stop on low battery
		for (int i = 0; i < 64; i++)
			v_batt.update(board.battery.get_voltage());
		if (v_batt < LOW_BATTERY_THRESHOLD)
			failure(E_LOW_BATTERY);

		// Calibrate the Gyro
		if (controller.calibrate())
			blink(board, 5, 50000);
		else
			blink(board, 40, 50000);
	}

	// Main loop
	inline int main() {
		while (true) {
			LoopGuard<LOOP_TIME, decltype(board.clock)> _loop(board.clock);

			// Flight control logic
			controller.sense();
			controller.decide();
			controller.act();

			// Battery voltage & LED signaling
			v_batt.update(board.battery.get_voltage());
			led_signaling_tick(_loop.loop_started);
			if (_loop.elapsed() > 10 * LOOP_TIME) failure(E_LOOP_TIME);

			// Extra features
			serial_debug_app();
			play_music_when_idling();
		}
	}

	inline void led_signaling_tick(uint32_t time) {

		// Low battery: Blinking period 1 second
		if (v_batt < LOW_BATTERY_THRESHOLD)
			blink_tick(board.led, time, 1000000, 500000);

		// Binding: fast flashes
		else if (board.rx.data().rx_mode == RX_BIND)
			blink_tick(board.led, time, 100000, 75000);

		// Failsafe: quick longer flashes
		else if (board.rx.data().rx_mode == RX_FAILSAFE)
			blink_tick(board.led, time, 100000, 90000);

		// Normal mode, led determined by AUX1.
		else
			board.led.set(board.rx.data().aux[CH_FLIP]);
	}

	void failure(int status) {
		for (int i = 0; i < 4; i++)
			board.motors.set_power((QuadcopterMotorId) i, 0.0);
		dprintf("Failure: %i\n", status);
		blink_forever(board, status);
	}

	// ----------- Extras --------------
	inline void serial_debug_app() {
#ifdef SERIAL_DEBUG
		// Listen to UART
		if (board.serial.kbhit()) {
			char cmd;
			scanf("%c", &cmd);
			if (cmd == '?') {
				// Transition to the serial debug app
				dprintf(
						"Starting serial debug app. Press 'q' to go back to quadcopter mode.\n");
				SerialDebugApp<QuadcopterBoard, Controller> serial_debug(board, controller);
				serial_debug.main();
			}
		}
#endif
	}

#ifdef ENABLE_MUSIC
	int next_track = 0;
	Condition not_moving;
#endif
	inline void play_music_when_idling() {
#ifdef ENABLE_MUSIC
		auto is_not_moving = [&]() {
			auto& v = controller.gyro.value();
			return (*std::max_element(v.begin(), v.end()) < 5) &&
				   (*std::min_element(v.begin(), v.end()) > -5);
		};
		not_moving.update(is_not_moving(), board.clock.micros());

		if (not_moving.valid_for > 20000000) {
			// Idling for 20 seconds? Play a tune!
			board.led.set(true);
			SoundMaker<decltype(board.motors)> sound(board.motors);
			Melodies::song(sound, board.clock, next_track++);
			not_moving.valid_since = board.clock.micros();
			not_moving.valid_for = 0;
			board.led.set(false);
		}
#endif
	}
};
