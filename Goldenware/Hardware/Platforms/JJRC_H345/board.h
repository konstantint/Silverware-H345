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
#include "../../../Common/point3d.h"
#include "../../../Control/pid.h"
#include "../../../RX/bayang.h"


class JjrcH345 {
public:
	// Chip hardware
	SysClock<FREQ_50MHZ> clock;
	Serial<115200> serial;
	Adc adc;
	Battery<Adc> battery;
	I2c i2c;
	Rfspi rfspi;

	// Peripherals
	Mpu6050<I2c, decltype(clock)> mpu6050;	// Accelerometer/Gyro
	Xn297<Rfspi> xn297;		// Radio chip

	// LED
	OutputPin<2, 6> led;

	// Motors
	Motors motors;

	// RX Protocol
	BayangRX<decltype(xn297), decltype(clock)> rx;

	// Default Acro PID config
	// NB: PIDs from Silverware's E011 config actually work out reasonably,
	// however we changed the scale of values and the PID algorithm to the point
	// where we needed to recalibrate anew.
	constexpr static QuadcopterPidConfigs acro_pids = {
		{0.0030, 0.000, 0.00003, 0.001, 5000, 8},
		{0.0030, 0.000, 0.00003, 0.001, 5000, 8},
		{0.0030, 0.000, 0.00003, 0.001, 5000, 8}
	};

	inline JjrcH345() :
			clock(), serial(), adc(), battery(adc), i2c(400000), rfspi(2000000), mpu6050(
					i2c, clock, 0x68), xn297(rfspi), rx(xn297, clock), led(
					false), motors() {
	}

	// Control interface

	// Gyro output vector is in deg/s, where
	// x, y, z = roll, pitch, yaw, matching the stick directions
	inline Point3d<float> read_gyro() {
		mpu6050.read_gyro();
		return mpu6050.data().gyro_dps() * (-1); // Flip roll/pitch/yaw axes to match
	}

	inline void set_motors(const QuadcopterMotorPowers& motor_powers) {
		for (auto i : ALL_MOTORS)
			motors.set_power(i, motor_powers[i]);
	}

};

// Needed until C++17.
// https://en.cppreference.com/w/cpp/language/static
constexpr QuadcopterPidConfigs JjrcH345::acro_pids;
