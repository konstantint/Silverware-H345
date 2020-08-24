// JJRC H345 battery voltage information reading over an ADC pin.
// It seems that battery voltage in H345 is connected to
// ADC channel 1 via 1/4 divider.
// The ADC is also capable of reading band-gap voltage (1.24) on channel 7.
// We use this to calibrate our readings.
//
// License: MIT
// Author: konstantint
// Based on the work of silverx and other contributors to the Silverware project.

#pragma once
#include "../../MCUs/PAN159CY/adc.h"

template<typename Adc> class Battery {
	Adc& _adc;
	float _scale = 0.010853391f; // We actually initialize it in constructor.
public:
	inline explicit Battery(Adc& adc) :
			_adc(adc) {
		// Calibrate scale via band-gap voltage reading
		ADC_CONFIG_CH7(ADC, ADC_CH7_BGP); // Map channel 7 to internal reference voltage (1.24V)
		float bgv = (float) _adc.read_channel(7); // Read BGP (= 457 in my tests)
		_scale = 1.24 / bgv * 4;
	}

	inline float get_voltage() {
		return _scale * (float) _adc.read_channel(1);
	}
};
