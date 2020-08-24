// Generic interface to the MPU6050 (or compatible) I2C six axis accelerometer/gyro chip
// See e.g. https://cdn.sparkfun.com/datasheets/Sensors/Accelerometers/RM-MPU-6000A.pdf
// Also: https://invensense.tdk.com/wp-content/uploads/2015/02/MPU-6000-Datasheet1.pdf
//
// License: MIT
// Author: konstantint, silverx
// Based on the work of silverx and other contributors to the Silverware project.

#pragma once
#include <algorithm>
#include <array>
#include "../../Common/defines.h"
#include "../../Common/point3d.h"

enum class Mpu6050GyroRange {
	D250 = 0, D500 = 1, D1000 = 2, D2000 = 3
};

enum class Mpu6050AccRange {
	G2 = 0, G4 = 1, G8 = 2, G16 = 3
};

template<Mpu6050GyroRange GyroRange, Mpu6050AccRange AccRange>
struct Mpu6050Data {
	//	Each 16-bit accelerometer measurement has a full scale defined in ACCEL_FS (Register 28). For
	//	each full scale setting, the accelerometers’ sensitivity per LSB in ACCEL_xOUT is shown in the table
	//	below.
	//  ACCEL_FS Range Sensitivity
	//	0        ±2g 	16384 LSB/g
	//	1 		 ±4g 	 8192 LSB/g
	//	2 		 ±8g 	 4096 LSB/g
	//	3 		 ±16g 	 2048 LSB/g
	Point3d<int16_t> acc;

	//	Each 16-bit gyroscope measurement has a full scale defined in FS_SEL (Register 27). For each full
	//	scale setting, the gyroscopes’ sensitivity per LSB in GYRO_xOUT is shown in the table below:
	//  FS_SEL  Range       Sensitivity
	//	0       ± 250  °/s  131 LSB/°/s
	//	1       ± 500  °/s  65.5 LSB/°/s
	//	2       ± 1000 °/s  32.8 LSB/°/s
	//	3       ± 2000 °/s  16.4 LSB/°/s
	Point3d<int16_t> gyro;

#ifdef MPU6050_USE_TEMPERATURE_SENSOR
	// Temperature in C = (TEMP_OUT Register Value as a signed quantity)/340 + 36.53
	int16_t temp;

	// Temperature in Celsius
	inline float temp_c() {
		// Actually every MPU6050 clone has its own calibration parameters,
		// these are from the original MPU6050 datasheet
		// TODO: Make the code chip-specific
		return (float)temp / 340.0f + 36.53;
	}
#endif

	// Acceleration in Gs (assuming max sensitivity setting)
	inline Point3d<float> acc_g() {
		if (AccRange == Mpu6050AccRange::G2)
			return acc.cast<float>() / 16384.0;
		if (AccRange == Mpu6050AccRange::G4)
			return acc.cast<float>() / 8192.5;
		if (AccRange == Mpu6050AccRange::G8)
			return acc.cast<float>() / 4096.8;
		else
			// G16
			return acc.cast<float>() / 2048.0;
	}

	// Gyro readings in degrees per second (assuming max sensitivity setting)
	inline Point3d<float> gyro_dps() {
		if (GyroRange == Mpu6050GyroRange::D250)
			return gyro.cast<float>() / 131.0;
		if (GyroRange == Mpu6050GyroRange::D500)
			return gyro.cast<float>() / 65.5;
		if (GyroRange == Mpu6050GyroRange::D1000)
			return gyro.cast<float>() / 32.8;
		else
			// D2000
			return gyro.cast<float>() / 16.4;
	}
};

// Registers
constexpr uint8_t R_PWR_MGMT_1 = 0x6B;
constexpr uint8_t R_GYRO_CONFIG = 0x1B;
constexpr uint8_t R_ACCEL_CONFIG = 0x1C;
constexpr uint8_t R_WHO_AM_I = 0x75;
constexpr uint8_t R_CONFIG = 0x1A;
constexpr uint8_t R_ACCEL_DATA = 0x3B;
constexpr uint8_t R_TEMP_DATA = 0x41;
constexpr uint8_t R_GYRO_DATA = 0x43;

template<typename I2c, typename Clock,
		// Default = Max range / min sensitivity for both gyro and accelerometer
		Mpu6050GyroRange GyroRange = Mpu6050GyroRange::D2000,
		Mpu6050AccRange AccRange = Mpu6050AccRange::G16>
class Mpu6050 {
	I2c& _i2c;
	Clock& _clock;
	Mpu6050Data<GyroRange, AccRange> _data;
	uint8_t _slave_id;

public:
	inline Mpu6050(I2c& i2c, Clock& clock, uint8_t slave_id) :
			_i2c(i2c), _clock(clock), _data(), _slave_id(slave_id) {

		// Reset all registers to 0 (in particular this sets max sensitivities for Gyro and Accel)
		device_reset();

		// This is important (otherwise the sensitivity setting is not respected)
		_clock.delay_us(40000);

		// Upon power up, the MPU-60X0 clock source defaults to the internal oscillator. However, it is highly
		// recommended that the device be configured to use one of the gyroscopes (or an external clock
		// source) as the clock reference for improved stability
		constexpr uint8_t TEMP_DIS_BIT = 1 << 3;
		constexpr uint8_t CLKSRC_PLL_GYRO_X = 1;

		// Set Gyro X as the oscillator.
		i2c.write_register(slave_id, R_PWR_MGMT_1, CLKSRC_PLL_GYRO_X
#ifndef MPU6050_USE_TEMPERATURE_SENSOR
				// .. and disable the temperature sensor
				| TEMP_DIS_BIT
#endif
				);

		// Configure Gyro and Accel sensitivity
		i2c.write_register(slave_id, R_GYRO_CONFIG, (int) GyroRange << 3);
		i2c.write_register(slave_id, R_ACCEL_CONFIG, (int) AccRange << 3);

		// Enable low-pass filtering for Gyro & Accelerometer
		// DBPL_CFG Acc_LP_Bandwidth_Hz Acc_LP_Delay_ms Gyro_Bandwidth_Hz Gyro_Delay_ms Gyro_Fs_kHz
		//        0                 260               0               256          0.98           8
		//        1                 184               2               188          1.9            1
		//        2 				 94 			  3				   98		   2.8			  1
		//        3 				 44 			4.9 			   42		   4.8			  1
		//        4 				 21 			8.5				   20		   8.3			  1
		//        5 				 10			   13.8				   10		  13.4			  1
		//        6 				  5 		   19.0				    5 		  18.6			  1
		i2c.write_register(slave_id, R_CONFIG, 3);

		// Silverware had this as well, not applicable to JJRC H345 (the only board tested so far)
		// int newboard = !(0x68 == i2c_readreg(117) );
		// if (newboard) i2c_writereg(29, ACC_LOW_PASS_FILTER);
	}

	void device_reset() {
		constexpr uint8_t DEVICE_RESET_BIT = 1 << 7;
		_i2c.write_register(_slave_id, R_PWR_MGMT_1, DEVICE_RESET_BIT);
	}

	// Read accelerometer, Gyro and temperature values
	bool read_all() {
		uint8_t buffer[14];
		bool status = _i2c.read_data(_slave_id, R_ACCEL_DATA, buffer, 14);

		_data.acc.x() = (int16_t) ((buffer[0] << 8) | buffer[1]);
		_data.acc.y() = (int16_t) ((buffer[2] << 8) | buffer[3]);
		_data.acc.z() = (int16_t) ((buffer[4] << 8) | buffer[5]);

		_data.gyro.x() = (int16_t)((buffer[8] << 8) | buffer[9]);
		_data.gyro.y() = (int16_t)((buffer[10] << 8) | buffer[11]);
		_data.gyro.z() = (int16_t)((buffer[12] << 8) | buffer[13]);

#ifdef MPU6050_USE_TEMPERATURE_SENSOR
		_data.temp = (int16_t)((buffer[6] << 8) | buffer[7]);
#endif

		return status;
	}

#ifdef MPU6050_USE_TEMPERATURE_SENSOR
	// Read temperature data
	bool read_temperature() {
		uint8_t buffer[6];
		bool status = _i2c.read_data(_slave_id, R_TEMP_DATA, buffer, 2);
		_data.temp = (int16_t)((buffer[0] << 8) | buffer[1]);
		return status;
	}
#endif

	// Read only the accelerometer values
	bool read_acc() {
		uint8_t buffer[6];
		bool status = _i2c.read_data(_slave_id, R_ACCEL_DATA, buffer, 6);

		_data.acc.x() = (int16_t) ((buffer[0] << 8) | buffer[1]);
		_data.acc.y() = (int16_t) ((buffer[2] << 8) | buffer[3]);
		_data.acc.z() = (int16_t) ((buffer[4] << 8) | buffer[5]);

		return status;
	}

	// Read only the gyro values
	bool read_gyro() {
		uint8_t buffer[6];
		bool status = _i2c.read_data(_slave_id, R_GYRO_DATA, buffer, 6);

		_data.gyro.x() = (int16_t) ((buffer[0] << 8) | buffer[1]);
		_data.gyro.y() = (int16_t) ((buffer[2] << 8) | buffer[3]);
		_data.gyro.z() = (int16_t) ((buffer[4] << 8) | buffer[5]);

		return status;
	}

	// Verify that the Gyro responds to a "whoami" request with one of known gyro board ids.
	// NB: There's actualy a real 'self-test' function built into MPU6050, but we don't use it here
	inline bool self_test() {
		constexpr static std::array<uint8_t, 4> known_gyros = {
				0x68 /* MPU6050 datasheet */, 0x98, 0x7D /* H345's M540 */, 0x72 };

		uint8_t who_am_i = _i2c.read_register(_slave_id, R_WHO_AM_I);
		return std::find(known_gyros.begin(), known_gyros.end(), who_am_i)
				!= known_gyros.end();
	}

	inline Mpu6050Data<GyroRange, AccRange>& data() {
		return _data;
	}
};
