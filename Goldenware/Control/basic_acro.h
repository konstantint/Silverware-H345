// Basic Acro flight controller.
// Ensures roll/pitch/yaw rates follow stick inputs using PID control.
//
// License: MIT
// Author: konstantint
// Based on the work of silverx and other contributors to the Silverware project.

#pragma once
#include "pid.h"
#include "../Common/gyro.h"
#include "../RX/common.h"

// Controllable must provide:
//  clock
//  rx
//  led
//  read_gyro()
//  set_motors()
//  ::acro_pids
template<typename Controllable> class BasicAcro {
public:
	constexpr static float MAX_RATE = 360.0f;

	Controllable& board;
	GyroReader<Controllable> gyro; // Filtered, calibrated gyro reading
	PidController pids[3];         // Roll/Pitch/Yaw pid controllers
	LPFArray<QuadcopterMotorPowers, std::ratio<0, 10>> motors;// Filtered motor output

	BasicAcro(Controllable& board_) :
			board(board_), gyro(board), pids(), motors() {
		for (auto ch : ROLL_PITCH_YAW)
			pids[ch] = PidController(Controllable::acro_pids[ch]);
	}

	bool calibrate() {
		return gyro.calibrate();
	}

	bool sense() {
		gyro.read();
		if (!board.rx.recv())
			return false;
		if (board.rx.data().rx_mode != RX_NORMAL)
			return false;
		return true;
	}

	bool decide() {
		const auto& rx = board.rx.data().rx;
		float throttle = rx[CH_THR];

		for (auto ch : ROLL_PITCH_YAW)
			pids[ch].update(rx[ch] * MAX_RATE - gyro.value()[ch]);

		QuadcopterMotorPowers new_motors;
		new_motors.set_rpyt(pids[CH_ROLL].output(), pids[CH_PITCH].output(),
				pids[CH_YAW].output(), throttle);
		motors.update(new_motors);

		// Override PID decisions if we have 0 throttle and centered sticks (or a failsafe)
		if (on_ground()) {
			motors = QuadcopterMotorPowers({0, 0, 0, 0});
			// Reset PIDs to avoid error accumulation
			for (auto ch : ROLL_PITCH_YAW) pids[ch].reset();
		}
		return true;
	}

	bool act() {
		board.set_motors(motors.value());
		return true;
	}

private:
	bool on_ground() {
		auto& rxd = board.rx.data();
		return (rxd.rx_mode == RX_FAILSAFE
				|| (rxd.rx[CH_THR] < 0.001f
					&& (fabsf(rxd.rx[CH_ROLL]) < 0.2f
					&& fabsf(rxd.rx[CH_PITCH]) < 0.2f
					&& fabsf(rxd.rx[CH_YAW]) < 0.2f)));
	};
};
