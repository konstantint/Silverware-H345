// Sometimes we may also transmit stuff. This file contains utility code for that.
//
// License: MIT
// Author: konstantint, silverx
// Based on the work of silverx and other contributors to the Silverware project.

#pragma once
#include <functional>
#include "../Hardware/Peripherals/xn297.h"

// To send beacon info, we need to switch to transmit mode once in a while.
// This class encapsulates the logic for saving and restoring RX state
// as well as deciding when to transmit.
template <typename Xn297>
class TxManager {
	Xn297& _xn;
	const uint32_t _transmit_every_us;	// How often are we allowed to transmit
	const uint32_t _tx_timeout;			// How long do we let the Xn297 to stay in TX mode

	uint32_t _last_tx_time = 0;
	uint8_t _saved_rx_chan;
	uint8_t _saved_config;
public:
	bool transmitting;

	inline TxManager(Xn297& xn, uint32_t transmit_every_us, uint32_t tx_timeout):
		_transmit_every_us(transmit_every_us), _tx_timeout(tx_timeout), _xn(xn),
		transmitting(false) {}

	void act(uint32_t time_us, std::function<void()> tx_func) {
		if (!transmitting) {
			if (time_us - _last_tx_time > _transmit_every_us) {
				// Save Rx state
				_saved_rx_chan = _xn.read_reg(RF_CH);
				_saved_config = _xn.read_reg(CONFIG);
				tx_func();
				_last_tx_time = time_us;
				transmitting = true;
			}
		}
		else {
			if ((_xn.read_reg(FIFO_STATUS) & 0b00010000) ||
				 time_us - _last_tx_time > _tx_timeout) { // Done or timeout
				// Restore RX state
				_xn.command(FLUSH_TX);
				_xn.write_reg(RF_CH, _saved_rx_chan);
				_xn.write_reg(CONFIG, _saved_config);
				transmitting = false;
			}
		}
	}
};
