// RX-related common definitions.
//
// License: MIT
// Author: konstantint, silverx
// Based on the work of silverx and other contributors to the Silverware project.

#pragma once
#include <cstdint>

enum RxMode
	: char {
		RX_BIND = 0, RX_NORMAL, RX_FAILSAFE
};

enum MainChannels
	: char {
		CH_AIL = 0, CH_ELE, CH_RUD, CH_THR,

		// Alternative names, for convenience
	CH_ROLL = 0,
	CH_PITCH = 1,
	CH_YAW = 2
};

// For enumeration convenience:
constexpr static MainChannels ROLL_PITCH_YAW[3] = {CH_ROLL, CH_PITCH, CH_YAW};

// Channel ID order taken from Silverware's rc_bayang.h
enum AuxChannels
	: char {
		CH_FLIP = 0,
	CH_EXPERT,
	CH_HEADFREE,
	CH_RTH,
	CH_AUX1,
	CH_AUX2,
	CH_INV,
	CH_VID,
	CH_PIC
};

constexpr int NUM_AUX_CHANNELS = 16;

struct RxData {
	RxMode rx_mode;
	float rx[4]; 				// Value between -1.0 and 1.0
	char aux[NUM_AUX_CHANNELS]; // Values 0 or 1 typically.

	// Latest packet received (for debugging)
	uint8_t packet_size;
	const uint8_t* packet_bytes;
};
