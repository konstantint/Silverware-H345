// Controller for the Eachine H8 quadcopter motors (over the hardware PWM drivers).
//
// License: MIT
// Author: konstantint
// Based on the work of silverx and other contributors to the Silverware project.

#pragma once
#include "PN020Series.h"
#include "../../MCUs/PAN163CX/pwm.h"
#include "../../../Common/maths.h"
#include "../../../Common/motors.h"

// -------- Motors on the H8 are attached to the following pins / PWM drivers -------
// #1: 5.7 (PWM0 ch 7): Back right, CW
// #2: 2.6 (PWM0 ch 4): Front right, CCW
// #3: 0.4 (PWM0 ch 5): Back left, CCW
// #4: 5.6 (PWM0 ch 6): Front left, CW

class Motors: private Pwm {
// We run the PWM at 30KHz - at 20KHz or lower motors make annoying audible sounds.
	constexpr static uint32_t PWM_FREQUENCY = 30000;

public:
	inline Motors() :
			Pwm() {
		SYS_UnlockReg();
	    SYS->P2_MFP |= SYS_MFP_P26_PWM0_CH4;
	    SYS->P5_MFP |= SYS_MFP_P56_PWM0_CH6|SYS_MFP_P57_PWM0_CH7;
	    SYS->P0_MFP |= SYS_MFP_P04_PWM0_CH5;
		SYS_LockReg();
		PWM_EnableOutput(PWM, 0b11110000);
		PWM_Start(PWM, 0b11110000);
	}

	// Power is 0 .. 1.0
	inline void set_power(QuadcopterMotorId motor_id, float power) {
		set_power(motor_id, PWM_FREQUENCY, power);
	}

	// Same as above, but allows to specify custom PWM frequency
	// Enables the amazing MusicMaker functionality.
	void set_power(QuadcopterMotorId motor_id, uint32_t freq, float power) {
		uint8_t ch = 0;
		switch (motor_id) {
		case BACK_RIGHT:
			ch = 7;
			break;
		case FRONT_RIGHT:
			ch = 4;
			break;
		case BACK_LEFT:
			ch = 5;
			break;
		case FRONT_LEFT:
			ch = 6;
			break;
		}
		clip(power, 0.0f, 1.0f);
		set(ch, freq, (uint8_t) (power * 100));
	}
};

