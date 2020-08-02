// Basic PID controller implementation.
//
// License: MIT
// Author: konstantint
// Based on the work of silverx and other contributors to the Silverware project.

#pragma once
#include <array>
#include "../Common/maths.h"

struct PidControllerConfig {
	float K_p, K_i, K_d, dt, int_e_abslimit, output_abslimit;
};

// Basic PID controller for a single float value with a fixed loop time
class PidController {
	float _int_e; // \sum e (without dt)
	float _e_prev; // previous error value
	float _int_e_abslimit; // limit value for the magnitude int_e term (to prevent I windup)
	float _output_abslimit; // limit output
	float _output;

public:
	float K_p;
	float K_i_dt; // K_i * dt
	float K_d_div_dt; // K_d / dt

	// Needed to be able to create an array of these
	inline PidController() {
	}

	inline PidController(const PidControllerConfig& config) :
			K_p(config.K_p), K_i_dt(config.K_i * config.dt),
			K_d_div_dt(config.K_d / config.dt),
			_int_e_abslimit(config.int_e_abslimit),
			_output_abslimit(config.output_abslimit),
			_int_e(0), _e_prev(0), _output(0) {
	}

	void update(float error) {
		_int_e += error;
		clip(_int_e, -_int_e_abslimit, _int_e_abslimit);

		_output = K_p * error + K_i_dt * _int_e
				+ K_d_div_dt * (error - _e_prev);
		clip(_output, -_output_abslimit, _output_abslimit);
		_e_prev = error;
	}

	inline float output() {
		return _output;
	}

	void reset() {
		_int_e = _e_prev = _output = 0.0;
	}
};

// Roll-Pitch-Yaw config in a single structure for convenient storage
typedef PidControllerConfig QuadcopterPidConfigs[3];
