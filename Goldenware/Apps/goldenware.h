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
#include "../Common/errors.h"
#include "../Common/blink.h"
#include "../Common/filter.h"
#include "../Common/gyro.h"
#include "../Common/loop.h"
#include "../Common/motors.h"
#include "../Common/printf.h"
#include "../RX/xn297_ble_beacon.h"
#include "../Sound/melodies.h"


//TODO

// The QuadcopterBoard type must provide the following fields:
//  - clock
//  - led
//  - battery
//  - read_gyro, set_motors and self_test methods.
// Controller must provide "calibrate", "sense", "decide" and "act"
template<typename QuadcopterBoard, typename Controller>
struct GoldenwareApp {
	static constexpr int LOOP_TIME = 1000; // 1ms per loop
	static constexpr float LOW_BATTERY_THRESHOLD = 3.3f;

	QuadcopterBoard& board;
	Controller controller;

	LPFValue<float, std::ratio<63, 64>> v_batt = 3.7; // Battery voltage

	inline GoldenwareApp(QuadcopterBoard& board_) :
			board(board_), controller(board_) {
		// Welcome blink
		blink(board);

		// Self-test
		ErrorCode self_test = board.self_test();
		if (self_test != ErrorCode::E_SUCCESS)
			failure(self_test);

		// Read battery voltage and stop on low battery
		for (int i = 0; i < 64; i++)
			v_batt.update(board.battery.get_voltage());
		if (v_batt < LOW_BATTERY_THRESHOLD)
			failure(ErrorCode::E_LOW_BATTERY);

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

			// BLE telemetry
			board.ble.tx_manager.act(_loop.loop_started, [&]() {
				static int adv_cnt = 0;
				static int p_type = 1;

				constexpr MacAddress mac_addr = {0x11, 0x22, 0x33, 0x44, 0x55, 0xF6};
				BleAdvertisingPacket packet(mac_addr);

				switch (p_type) {
				case 0:
					board.imu.read_temperature();
					packet.eddystone_tlm(_loop.loop_started, QuadcopterBoard::name, v_batt, ++adv_cnt,
										 board.imu.data().temp_c());
					break;
				case 1:
					// -22 dBm TX power
					packet.eddystone_url(-12, 0x02, "google.com/", 11);
					break;
				}
				p_type = (p_type + 1) % 2;
				board.ble.send(packet);
			});

			// Battery voltage & LED signaling
			v_batt.update(board.battery.get_voltage());
			led_signaling_tick(_loop.loop_started);
			if (_loop.elapsed() > 10 * LOOP_TIME) failure(ErrorCode::E_LOOP_TIME);

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

	void failure(ErrorCode status) {
		for (auto m : ALL_MOTORS) board.motors.set_power(m, 0.0);
		dprintf("Failure: %i\n", (int)status);
		blink_forever(board, (int)status);
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

		if (not_moving.valid_for > 120000000) {
			// Idling for 120 seconds? Play a tune!
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
