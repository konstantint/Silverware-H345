// Interface to the PAN159 I2C driver.
//
// License: MIT
// Author: konstantint
// Based on the work of silverx and other contributors to the Silverware project.

#pragma once
#include "Mini58Series.h"

// Configure I2C0 at pins P3.4 and P3.5
class I2c {

	// Helpers
	bool _start(uint8_t slave_id, bool prepare_to_receive) {
		uint8_t recv_bit = (uint8_t) prepare_to_receive;

		I2C_START(I2C0);
		I2C_WAIT_READY(I2C0);
		I2C_SET_DATA(I2C0, (uint8_t )(slave_id << 1) | recv_bit);
		I2C_SET_CONTROL_REG(I2C0, I2C_SI);
		I2C_WAIT_READY(I2C0);

		uint8_t status = I2C_GET_STATUS(I2C0);
		if (prepare_to_receive)
			return status == 0x40; // SLA+R has been transmitted; ACK has been received.
		else
			return status == 0x18; // SLA+W has been transmitted; ACK has been received.
	}

	inline void _stop() {
		I2C_SET_CONTROL_REG(I2C0, I2C_STO | I2C_SI);
	}

	bool _tx_byte(uint8_t data) {
		I2C_SET_DATA(I2C0, data);
		I2C_SET_CONTROL_REG(I2C0, I2C_SI);
		I2C_WAIT_READY(I2C0);
		return I2C_GET_STATUS(I2C) == 0x28; // Data byte in I2DAT has been transmitted; ACK has been received.
	}

public:
	inline explicit I2c(uint32_t i2c_baudrate) {
		CLK_EnableModuleClock(I2C0_MODULE);

		SYS->P3_MFP &= ~SYS_MFP_P34_Msk;
		SYS->P3_MFP |= SYS_MFP_P34_I2C0_SDA;
		SYS->P3_MFP &= ~SYS_MFP_P35_Msk;
		SYS->P3_MFP |= SYS_MFP_P35_I2C0_SCL;

		I2C_Open(I2C0, i2c_baudrate);
	}

	bool write_register(uint8_t slave_id, uint8_t reg, uint8_t value) {
		bool status = _start(slave_id, false);
		if (!status)
			goto done;
		// Sorry, Edsger

		status = _tx_byte(reg);
		if (!status)
			goto done;

		status = _tx_byte(value);
		done: _stop();
		return status;
	}

	bool read_data(uint8_t slave_id, uint8_t reg, uint8_t *buffer,
			uint8_t size) {
		bool status = _start(slave_id, false);
		if (!status)
			goto done;

		status = _tx_byte(reg);
		if (!status)
			goto done;
		_stop();

		status = _start(slave_id, true);
		if (!status)
			goto done;

		uint8_t i;
		for (i = 0; i < size - 1; i++) {
			I2C_SET_CONTROL_REG(I2C, I2C_SI | I2C_AA);
			I2C_WAIT_READY(I2C);
			buffer[i] = I2C_GET_DATA(I2C);
		}
		I2C_SET_CONTROL_REG(I2C, I2C_SI);
		I2C_WAIT_READY(I2C);
		buffer[i] = I2C_GET_DATA(I2C);

		done: _stop();
		return status;
	}

	inline uint8_t read_register(uint8_t slave_id, uint8_t reg) {
		uint8_t data;
		read_data(slave_id, reg, &data, 1);
		return data;
	}
};
