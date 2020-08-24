// Definition of the JJRC H345 quadcopter board hardware, suitable for control algorithms.
//
// License: MIT
// Author: konstantint
// Based on the work of silverx and other contributors to the Silverware project.

#pragma once
#include "battery.h"
#include "motors.h"
#include "../../MCUs/PAN159CY/adc.h"
#include "../../MCUs/PAN159CY/i2c.h"
#include "../../MCUs/PAN159CY/gpio.h"
#include "../../MCUs/PAN159CY/rfspi.h"
#include "../../MCUs/PAN159CY/serial.h"
#include "../../MCUs/PAN159CY/sysclock.h"
#include "../../Peripherals/mpu6050.h"
#include "../../Peripherals/xn297.h"
#include "../../../Common/errors.h"
#include "../../../Common/point3d.h"
#include "../../../Control/pid.h"
#include "../../../RX/bayang.h"
#include "../../../RX/xn297_ble_beacon.h"


class JjrcH345Base {
public:
	// Chip hardware
	SysClock<FREQ_50MHZ> clock;
	Serial<115200> serial;
	Adc adc;
	Battery<Adc> battery;
	I2c i2c;
	Rfspi rfspi;

	// Peripherals
	Mpu6050<I2c, decltype(clock)> imu;	// Accelerometer/Gyro
	Xn297<Xn297Type::Xn297L, Rfspi> xn297;		// Radio chip

	// LED
	OutputPin<2, 6> led;

	// Motors
	Motors motors;

	inline JjrcH345Base() :
			clock(), serial(), adc(), battery(adc), i2c(400000), rfspi(2000000), imu(
					i2c, clock, 0x68), xn297(rfspi), led(
					false), motors() {
	}

	// Control interface

	// Gyro output vector is in deg/s, where
	// x, y, z = roll, pitch, yaw, matching the stick directions
	inline Point3d<float> read_gyro() {
		imu.read_gyro();
		return imu.data().gyro_dps() * (-1); // Flip roll/pitch/yaw axes to match
	}

	inline void set_motors(const QuadcopterMotorPowers& motor_powers) {
		for (auto i : ALL_MOTORS)
			motors.set_power(i, motor_powers[i]);
	}

	// Run self-diagnostic test(s)
	inline ErrorCode self_test() {
		// TODO: recover lost self test logic (call device self tests)
		return ErrorCode::E_SUCCESS;
	}
};


class JjrcH345: public JjrcH345Base {
public:
	// RX Protocol
	BayangRX<decltype(xn297), decltype(clock)> rx;
	BLE<decltype(xn297)> ble;

	// Default Acro PID config
	// NB: PIDs from Silverware's E011 config actually work out reasonably,
	// however we changed the scale of values and the PID algorithm to the point
	// where we needed to recalibrate anew.
	constexpr static QuadcopterPidConfigs acro_pids = {
		{0.0030, 0.000, 0.00003, 0.001, 5000, 8},
		{0.0030, 0.000, 0.00003, 0.001, 5000, 8},
		{0.0030, 0.000, 0.00003, 0.001, 5000, 8}
	};
	constexpr static char name[4] = {'H','3','4','5'};

	inline JjrcH345():
		JjrcH345Base(), rx(xn297, clock), ble(xn297) {}
};

// Needed until C++17.
// https://en.cppreference.com/w/cpp/language/static
constexpr QuadcopterPidConfigs JjrcH345::acro_pids;
constexpr char JjrcH345::name[4];
