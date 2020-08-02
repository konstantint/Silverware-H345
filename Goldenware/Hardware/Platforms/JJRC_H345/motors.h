// Controller for the JJRC H345 quadcopter motors (over the hardware PWM drivers).
//
// License: MIT
// Author: konstantint
// Based on the work of silverx and other contributors to the Silverware project.

#pragma once
#include "Mini58Series.h"
#include "../../MCUs/PAN159CY/pwm.h"
#include "../../../Common/maths.h"
#include "../../../Common/motors.h"

// -------- Motors on the JJRC345 are attached to the following pins / PWM drivers -------
// #1: 2.4 (PWM0 ch 2): Back right, CW
// #2: 2.5 (PWM0 ch 3): Front right, CCW
// #3: 1.4 (PWM0 ch 4): Back left, CCW
// #4: 1.2 (PWM0 ch 0): Front left, CW

class Motors: private Pwm {
// We run the PWM at 30KHz - at 20KHz or lower motors make annoying audible sounds.
	constexpr static uint32_t PWM_FREQUENCY = 30000;

public:
	inline Motors() :
			Pwm() {
		SYS_UnlockReg();
		SYS->P2_MFP = (SYS->P2_MFP & ~SYS_MFP_P25_Msk) | SYS_MFP_P25_PWM0_CH3;
		SYS->P2_MFP = (SYS->P2_MFP & ~SYS_MFP_P24_Msk) | SYS_MFP_P24_PWM0_CH2;
		SYS->P1_MFP = (SYS->P1_MFP & ~SYS_MFP_P14_Msk) | SYS_MFP_P14_PWM0_CH4;
		SYS->P1_MFP = (SYS->P1_MFP & ~SYS_MFP_P12_Msk) | SYS_MFP_P12_PWM0_CH0;
		SYS_LockReg();
		PWM_EnableOutput(PWM, 0b11101);
		PWM_Start(PWM, 0b011101);
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
			ch = 2;
			break;
		case FRONT_RIGHT:
			ch = 3;
			break;
		case BACK_LEFT:
			ch = 4;
			break;
		case FRONT_LEFT:
			ch = 0;
			break;
		}
		clip(power, 0.0f, 1.0f);
		set(ch, freq, (uint8_t) (power * 100));
	}
};

