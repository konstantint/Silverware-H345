// Bayang RX protocol implementation.
//
// License: MIT
// Author: silverx, conversion to C++ by konstantint
// Based on the work of silverx and other contributors to the Silverware project.

#pragma once
#include "common.h"
#include "../Hardware/Peripherals/xn297.h"


constexpr int FAILSAFETIME = 1000000; // one second
// Note that during binding the transmitter sends bind packets for longer than 1 second
// Therefore we may briefly failsafe on binding.

template<typename Xn297, typename Clock> class BayangRX {
	Xn297& _xn;
	Clock& _clock;
	RxData _data;

	unsigned long lastrxtime = 0;
	unsigned long bindtime = 0;
	unsigned long failsafetime = 0;
	unsigned long secondtimer = 0;
	unsigned int skipchannel = 0;
	int lastrxchan = 0;
	int timingfail = 0;

	// packet period in uS
	static constexpr int PACKET_PERIOD = 3300;
	static constexpr int PACKET_OFFSET = 500;
	// how many times to hop ahead if no reception
	static constexpr int HOPPING_NUMBER = 4;

	int chan = 0;
	uint8_t rfchannel[4];
	char lastaux[NUM_AUX_CHANNELS];
	char auxchange[NUM_AUX_CHANNELS];
	uint8_t rxdata[15];
	uint8_t rxaddress[5] = { 0, 0, 0, 0, 0 };

	bool _packet_available() {
		//TODO:
		//return !_xn.rx_fifo_empty();
		int status = _xn.read_reg(R_STATUS);
		return (status & 0b00001110) != 0b00001110; // RX FIFO not empty
	}

	void _next_channel() {
		chan++;
		if (chan > 3)
			chan = 0;
		_xn.write_reg(RF_CH, rfchannel[chan]);
	}

	static float _decode_input(const uint8_t* data) {
		float val = (float) (data[0] & 0b11) * 256.0f
				+ (float) (uint8_t) data[1];
		return (val - 512.0) * 0.001953125; // /512
	}

	bool _decode_packet() {
		if (rxdata[0] == 0xA5) {
			int sum = 0;
			for (int i = 0; i < 14; i++) {
				sum += rxdata[i];
			}
			if ((sum & 0xFF) == rxdata[14]) {
				_data.rx[CH_AIL] = _decode_input(&rxdata[4]);
				_data.rx[CH_ELE] = _decode_input(&rxdata[6]);
				_data.rx[CH_RUD] = _decode_input(&rxdata[10]);
				_data.rx[CH_THR] = (float) ((float) (rxdata[8] & 0b11) * 256.0f
						+ (uint8_t) rxdata[9]) * 0.000976562; // /1024
				_data.aux[CH_INV] = (rxdata[3] & 0x80) ? 1 : 0; // inverted flag
				_data.aux[CH_VID] = (rxdata[2] & 0x10) ? 1 : 0;
				_data.aux[CH_PIC] = (rxdata[2] & 0x20) ? 1 : 0;
				_data.aux[CH_FLIP] = (rxdata[2] & 0x08) ? 1 : 0;
				_data.aux[CH_EXPERT] = (rxdata[1] == 0xfa) ? 1 : 0;
				_data.aux[CH_HEADFREE] = (rxdata[2] & 0x02) ? 1 : 0;
				_data.aux[CH_RTH] = (rxdata[2] & 0x01) ? 1 : 0;	// rth channel

				for (int i = 0; i < NUM_AUX_CHANNELS - 2; i++) {
					auxchange[i] = 0;
					if (lastaux[i] != _data.aux[i])
						auxchange[i] = 1;
					lastaux[i] = _data.aux[i];
				}
				return true; // valid packet
			}
			return false; // sum fail
		}
		return false; // first byte different
	}

public:

	BayangRX(Xn297& xn, Clock& clock) :
			_xn(xn), _clock(clock), _data() {
		_data.rx_mode = RX_BIND;
		_data.packet_size = 15;
		_data.packet_bytes = rxdata;

		_xn.write_buf(RX_ADDR_P0, rxaddress, 5);
		_xn.write_reg(EN_AA, 0);	// aa disabled
		_xn.write_reg(EN_RXADDR, 1); // pipe 0 only
		_xn.write_reg(RX_PW_P0, 15); // payload size
		_xn.write_reg(SETUP_RETR, 0); // no retransmissions (redundant?)
		_xn.write_reg(SETUP_AW, 3); // address size (5 bits)
		_xn.set_data_rate_and_max_tx_power(0);
		_xn.set_software_ce_and_64_byte_payload();

		_xn.command(FLUSH_RX);
		_xn.write_reg(RF_CH, 0);  // bind on channel 0
		_xn.power_up(true, true, true, true); // Power up in RX mode, 2byte CRC
	}

	bool recv() {
		bool retval = false;
		unsigned long time = _clock.micros();

		if (_packet_available()) {
			if (_data.rx_mode == RX_BIND) {
				_xn.read_buf(R_RX_PAYLOAD, rxdata, 15);
				if (rxdata[0] == 0xA4) {	// bind packet
					rfchannel[0] = rxdata[6];
					rfchannel[1] = rxdata[7];
					rfchannel[2] = rxdata[8];
					rfchannel[3] = rxdata[9];

					rxaddress[0] = rxdata[1];
					rxaddress[1] = rxdata[2];
					rxaddress[2] = rxdata[3];
					rxaddress[3] = rxdata[4];
					rxaddress[4] = rxdata[5];

					_xn.write_buf(RX_ADDR_P0, rxaddress, 5);
					_xn.write_reg(RF_CH, rfchannel[chan]);// Set channel frequency
					_data.rx_mode = RX_NORMAL;
					retval = true;
					bindtime = time;
				}
			} else { // normal mode
				_next_channel();
				_xn.read_buf(R_RX_PAYLOAD, rxdata, 15);
				if (_decode_packet()) {
					skipchannel = 0;
					timingfail = 0;
					lastrxchan = chan;
					lastrxtime = time;
					failsafetime = lastrxtime;
					_data.rx_mode = RX_NORMAL;
					retval = true;
				}
			}		// end normal rx mode
		}			// end packet received

		// sequence period 12000
		if (time - lastrxtime > (HOPPING_NUMBER * PACKET_PERIOD + 1000)
				&& _data.rx_mode != RX_BIND) {
			//  channel with no reception
			lastrxtime = time;
			// set channel to last with reception
			if (!timingfail)
				chan = lastrxchan;
			// advance to next channel
			_next_channel();
			// set flag to discard packet timing
			timingfail = 1;
		}

		if (!timingfail && skipchannel < HOPPING_NUMBER + 1
				&& _data.rx_mode != RX_BIND) {
			unsigned int temp = time - lastrxtime;

			if (temp > 1000
					&& (temp + (PACKET_OFFSET)) / ((int) PACKET_PERIOD)
							>= (skipchannel + 1)) {
				_next_channel();
				skipchannel++;
			}
		}

		if (time - failsafetime > FAILSAFETIME && _data.rx_mode == RX_NORMAL) { //  failsafe
		// If we just bound, ignore this (as the TX may still be sending binding packets simply)
			if (time - bindtime > 2 * FAILSAFETIME) {
				_data.rx_mode = RX_FAILSAFE;
				for (int i = 0; i < 4; i++)
					_data.rx[i] = 0;
			}
		}

		return retval;
	}

	RxData& data() {
		return _data;
	}
};
