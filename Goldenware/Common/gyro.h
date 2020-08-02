// Wrapper around the MPU6050 chip to provide calibration and filtering of gyro readouts.
//
// License: MIT
// Author: konstantint
// Based on the work of silverx and other contributors to the Silverware project.

#pragma once
#include <cmath>
#include <ratio>
#include "filter.h"
#include "loop.h"
#include "point3d.h"

// Wrapper around the Gyro chip, that provides calibration
// and filtering.
// Board must provide:
//   - clock
//   - led
//   - read_gyro()
template<typename Board> class GyroReader {
	static constexpr uint32_t CALIBRATION_MAX_TIME = 60000000; // 60s max attempts to calibrate gyro
	static constexpr uint32_t CALIBRATION_ATTEMPT_TIME = 2000000; // 2000ms calibration
	static constexpr uint32_t LOOP_TIME = 1000; // .. in 1ms loops

	Board& _board;

	// Filtered gyro value
	LPFArray<Point3d<float>, std::ratio<1, 2>> _gyro_value;

public:
	Point3d<float> gyro_offset; // Calibrated offset

	inline GyroReader(Board& board) :
			_board(board), gyro_offset(), _gyro_value() {
	}

	// Wait for a stable set of Gyro readings and save the averages.
	// Return true if calibration succeeds (i.e. no shaking happens within the period)
	bool calibrate() {
		// Average ~500 recent samples
		LPFArray<Point3d<float>, std::ratio<499, 500>> _gyro_cal;

		auto calibration_start_time = _board.clock.micros();
		auto attempt_start_time = calibration_start_time;

		int led_brightness = 0;
		while (true) {
			LoopGuard<LOOP_TIME, decltype(_board.clock)> _loop(_board.clock); // Ensure 1ms loops

			// Have we been trying for too long?
			auto& cur_time = _loop.loop_started;
			int total_elapsed = cur_time - calibration_start_time;
			if (total_elapsed < 0)
				total_elapsed = CALIBRATION_MAX_TIME;
			if (total_elapsed >= CALIBRATION_MAX_TIME) {
				_board.led.set(false);
				return false; // Calibrating for too long
			}

			// Have we collected enough data?
			if (cur_time - attempt_start_time >= CALIBRATION_ATTEMPT_TIME) {
				gyro_offset = _gyro_cal;
				_board.led.set(false);
				return true;
			}

			// Imitate gradual light-up on the LED pin
			// Not sure who invented this trick, but it's normal not to undestand how it works at first sight.
			constexpr int GLOW_TIME = 62500;
			if ((led_brightness & 1)
					^ ((cur_time - attempt_start_time) % GLOW_TIME
							> (GLOW_TIME >> 1))) {
				led_brightness = (led_brightness + 1) & 0xF;
			}
			blink_tick(_board.led, _loop.loop_started >> 10, 0xF,
					led_brightness);

			// Collect a new data point
			Point3d<float> new_value = _board.read_gyro();

			// If the gyro reading jumps too high, restart attempt
			constexpr float TOL = 4.0; // deg/s
			if (fabsf(new_value.x() - _gyro_cal.value().x()) > TOL
					|| fabsf(new_value.y() - _gyro_cal.value().y()) > TOL
					|| fabsf(new_value.z() - _gyro_cal.value().z()) > TOL) {
				// If the value jumped too much, restart attempt
				attempt_start_time = cur_time;
				_gyro_cal = Point3d<float>(0, 0, 0);
			} else {
				// Otherwise, update the current running average
				_gyro_cal.update(new_value);
			}
		}
	}

	inline Point3d<float> read() {
		_gyro_value.update(_board.read_gyro() - gyro_offset);
		return _gyro_value.value();
	}

	inline const Point3d<float>& value() {
		return _gyro_value.value();
	}
};
