#pragma once
#include <array>
#include <cstdint>
#include "tx_util.h"
#include "../Hardware/Peripherals/xn297.h"
#include "xn297_ble_util.h"


template<typename Xn297,
		 int Xn297AddressSize = 5  // We can use 4 or 5-byte addresses, depending on
		 	 	 	 	 	 	   // what the RX protocol is using.
		 >
class BLE {
	Xn297& _xn;
	uint32_t _adv_cnt = 0; // Number of times the beacon was sent

public:
	TxManager<Xn297> tx_manager;

	BLE(Xn297& xn) :
			_xn(xn),
			tx_manager(_xn, 100000, 10000) {

		// Set TX address to BLE advertising 0x8E89BED6
		// Remember that both bit and byte orders are reversed for BLE packet format
		// In addition, Xn297 scrambles bits when sending, so we need to apply an extra
		// unscrambling procedure:
		// https://www.deviationtx.com/forum/protocol-development/3368-jd-395-cx-10?screenMode=none&start=180
		constexpr int shift = 5 - Xn297AddressSize; // 1 or 0
		constexpr static uint8_t tx_addr[5] = {
			swapbits(0x8E) ^ xn297_scramble[4 - shift],
			swapbits(0x89) ^ xn297_scramble[3 - shift],
			swapbits(0xBE) ^ xn297_scramble[2 - shift],
			swapbits(0xD6) ^ xn297_scramble[1 - shift],
			swapbits(0xAA) ^ xn297_scramble[0]  // When using 5-byte addresses we fill this with a preamble sequence
		};
		_xn.write_buf(TX_ADDR, tx_addr, 5);

		_xn.write_reg(EN_AA, 0);	// Auto-Ack disabled
		_xn.write_reg(EN_RXADDR, 1); // Pipe 0 only
		_xn.set_data_rate_and_max_tx_power(0);
		_xn.write_reg(SETUP_RETR, 0); // no retransmissions ( redundant?)
		_xn.write_reg(SETUP_AW, Xn297AddressSize - 2); // address size (4 bytes for ble)
		_xn.set_software_ce_and_64_byte_payload();
	}

	void send(BleAdvertisingPacket& packet) {
		// Note: Advertising packets may be frequency hopped between BT channels 37,38 and 39
		// (2, 26 and 80 in xn297 terms). However, we can also just use one.
		// This makes it possible to use faster whitening code
		_xn.write_reg(RF_CH, 2);

		packet.finalize();
		packet.xn297_unwhiten(Xn297AddressSize);

		_xn.command(FLUSH_TX);
		_xn.power_up(false, false, false, true);
		_xn.write_buf(W_TX_PAYLOAD, packet.data(), packet.length());
	}
};

