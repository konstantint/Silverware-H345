/*
The MIT License (MIT)

Copyright (c) 2016 silverx

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/


#include <stdbool.h>
#include "project.h"
#include "drv_hw_i2c.h"
#include "drv_time.h"
#include "config.h"

#define HW_I2C_ADDRESS SOFTI2C_GYRO_ADDRESS 

// pins for hw i2c , select one only
// select pins PB6 and PB7
// OR select pins PA9 and PA10
//#define HW_I2C_PINS_PB67
//#define HW_I2C_PINS_PA910


// digital filter 0 - 15 ( 0 - off )
#define HW_I2C_DIGITAL_FILTER 15


// 100Khz (slow)
//#define HW_I2C_TIMINGREG 0x10805e89

// 200Khz	
//#define HW_I2C_TIMINGREG 0x2060083e

// 400khz (fast)
//#define HW_I2C_TIMINGREG 0x00901850

// 1000Khz (fast+)
//#define HW_I2C_TIMINGREG 0x00700818

// 1000+Khz (overclock i2c)
//#define HW_I2C_TIMINGREG 0x00400615

// 100Khz (slow) (overclock 64Mhz)
//#define HW_I2C_TIMINGREG 0x10d05880

// 400khz (fast) (overclock 64Mhz)
//#define HW_I2C_TIMINGREG 0x00c0216c

// 1000Khz (fast+) (overclock 64Mhz)
//#define HW_I2C_TIMINGREG 0x00900b22


#ifdef HW_I2C_SPEED_FAST_OC
// 1000+Khz (overclock i2c)
#define HW_I2C_TIMINGREG 0x00400615
#endif

#ifdef HW_I2C_SPEED_FAST2
// 1000Khz (fast+)
#define HW_I2C_TIMINGREG 0x00700818
#endif

#ifdef HW_I2C_SPEED_FAST
// 400khz (fast)
#define HW_I2C_TIMINGREG 0x00901850
#endif

#ifdef HW_I2C_SPEED_SLOW1
// 200Khz	
#define HW_I2C_TIMINGREG 0x2060083e
#endif

#ifdef HW_I2C_SPEED_SLOW2
// 100Khz (slow)
#define HW_I2C_TIMINGREG 0x10805e89
#endif


#ifndef HW_I2C_PINS_PA910
#ifndef HW_I2C_PINS_PB67
#define HW_I2C_PINS_PB67
#endif
#endif

// default if not set
#ifndef HW_I2C_TIMINGREG
// 400khz (fast)
#define HW_I2C_TIMINGREG 0x00901850
#endif

extern int liberror;

void hw_i2c_init( void)
{
	    CLK_EnableModuleClock(I2C0_MODULE);

	    SYS->P3_MFP &= ~SYS_MFP_P34_Msk;
	    SYS->P3_MFP |= SYS_MFP_P34_I2C0_SDA;
	    SYS->P3_MFP &= ~SYS_MFP_P35_Msk;
	    SYS->P3_MFP |= SYS_MFP_P35_I2C0_SCL;

	    I2C_Open(I2C0, 400000);
}


// Helpers
bool i2c_start(uint8_t slave_id, bool prepare_to_receive) {
	uint8_t recv_bit = (uint8_t)prepare_to_receive;

	I2C_START(I2C0);
	I2C_WAIT_READY(I2C0);
	I2C_SET_DATA(I2C0, (uint8_t)(slave_id << 1) | recv_bit);
	I2C_SET_CONTROL_REG(I2C0, I2C_SI);
	I2C_WAIT_READY(I2C0);

	uint8_t status = I2C_GET_STATUS(I2C0);
	if (prepare_to_receive)
		return status == 0x40; // SLA+R has been transmitted; ACK has been received.
	else
		return status == 0x18; // SLA+W has been transmitted; ACK has been received.
}

void i2c_stop() {
		I2C_SET_CONTROL_REG(I2C0, I2C_STO | I2C_SI);
}

bool i2c_tx_byte(uint8_t data) {
		I2C_SET_DATA(I2C0, data);
		I2C_SET_CONTROL_REG(I2C0, I2C_SI);
		I2C_WAIT_READY(I2C0);
		return I2C_GET_STATUS(I2C) == 0x28; // Data byte in I2DAT has been transmitted; ACK has been received.
}

void hw_i2c_writereg( int reg ,int data)
{
		int status = i2c_start(SOFTI2C_GYRO_ADDRESS, 0);
		if (!status) goto done; // Sorry, Edsger

		status = i2c_tx_byte(reg);
		if (!status) goto done;

		status = i2c_tx_byte(data);
	done:
		i2c_stop();
}



int hw_i2c_readdata( int reg, int *data, int size )
{
		int status = i2c_start(SOFTI2C_GYRO_ADDRESS, 0);
		if (!status) goto done;

		status = i2c_tx_byte(reg);
		if (!status) goto done;
		i2c_stop();

		status = i2c_start(SOFTI2C_GYRO_ADDRESS, 1);
		if (!status) goto done;

	    uint8_t i;
	    for (i = 0; i < size - 1; i++) {
	        I2C_SET_CONTROL_REG(I2C, I2C_SI | I2C_AA);
	        I2C_WAIT_READY(I2C);
	        data[i] = I2C_GET_DATA(I2C);
	    }
	    I2C_SET_CONTROL_REG(I2C, I2C_SI);
	    I2C_WAIT_READY(I2C);
	    data[i] = I2C_GET_DATA(I2C);

	done:
		i2c_stop();
		return status;	
}



int hw_i2c_readreg( int reg )
{
	int data;
	hw_i2c_readdata( reg, &data, 1 );
	return data;
}
































