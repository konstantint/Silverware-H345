// Initialization of the debug UART output on the ICE pins.
// Enables printf.
//
// License: MIT
// Author: konstantint

#pragma once
#include "Mini58Series.h"
#include "../../../Common/printf.h"

// Enables UART1 on P4.6 for RX (ICECLK otherwise) and P4.7 for TX (ICEDAT otherwise)
// This makes printf and scanf work (together with the relevant code in retarget.c)
template<int BaudRate> class Serial {
public:
	inline Serial() {
		CLK_EnableModuleClock(UART1_MODULE);
		SYS->P4_MFP = SYS_MFP_P46_UART1_RXD | SYS_MFP_P47_UART1_TXD;
		UART_Open(UART1, BaudRate);
		dprintf("Serial initialized\n");
	}

	inline int kbhit() {
		return !(UART1->FIFOSTS & UART_FIFOSTS_RXEMPTY_Msk);
	}
};
