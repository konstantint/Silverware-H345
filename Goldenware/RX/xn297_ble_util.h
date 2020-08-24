#pragma once
#include <algorithm>
#include <array>
#include <cstdint>

typedef std::array<uint8_t, 6> MacAddress;

void btLePacketEncode(uint8_t* packet, uint8_t len, uint8_t chan);

// scrambling sequence for xn297
constexpr uint8_t xn297_scramble[] = { 0xe3, 0xb1, 0x4b, 0xea, 0x85, 0xbc, 0xe5,
		0x66, 0x0d, 0xae, 0x8c, 0x88, 0x12, 0x69, 0xee, 0x1f, 0xc7, 0x62, 0x97,
		0xd5, 0x0b, 0x79, 0xca, 0xcc, 0x1b, 0x5d, 0x19, 0x10, 0x24, 0xd3, 0xdc,
		0x3f, 0x8e, 0xc5, 0x2f };

// bit reversed of above
constexpr uint8_t xn297_scramble_rev[] = { 0xc7, 0x8d, 0xd2, 0x57, 0xa1, 0x3d, 0xa7,
		0x66, 0xb0, 0x75, 0x31, 0x11, 0x48, 0x96, 0x77, 0xf8, 0xe3, 0x46, 0xe9,
		0xab, 0xd0, 0x9e, 0x53, 0x33, 0xd8, 0xba, 0x98, 0x8, 0x24, 0xcb, 0x3b,
		0xfc, 0x71, 0xa3, 0xf4, 85, 104, 207, 169, 25, 108, 93, 76, 4, 146, 229,
		29 };

// Convenient creator class for filling BLE packets
struct BleAdvertisingPacket: std::array<uint8_t, 42> {
	uint8_t pos;

	BleAdvertisingPacket(const MacAddress& mac_addr);

	// Set up an Eddystone Telemetry packet:
	// 	https://github.com/google/eddystone/blob/master/protocol-specification.md
	// 	https://github.com/google/eddystone/tree/master/eddystone-tlm
	// NB: Temperature = -128 denotes "no data"
	void eddystone_tlm(uint32_t time_us, const char name[4], float vbatt, uint32_t adv_cnt = 0, float temp = -128.0);

	// Set up an Eddystone URL packet
	// https://github.com/google/eddystone/tree/master/eddystone-url
	// url_scheme = 0x00 (http://www.), 0x01 (https://www.), 0x02 (http://), 0x03 (https://)
	// length = 1..17
	void eddystone_url(uint8_t tx_power, uint8_t url_scheme, const char* url, uint8_t url_length);

	inline void finalize() {
		// Finalize packet for transmission
		pos += 3; // CRC appended by btLePacketEncode
		btLePacketEncode(this->data(), pos, 37);
	}

	// Undo the whitening that xn297 performs on the packet
	inline void xn297_unwhiten(int address_offset) {
		for (uint8_t i = 0; i < length(); ++i) {
			data()[i] = data()[i] ^ xn297_scramble_rev[i + address_offset];
		}
	}

	inline uint8_t length() const {
		return pos;
	}
};


// from https://graphics.stanford.edu/~seander/bithacks.html#ReverseByteWith32Bits
// reverse the bit order in a single byte
constexpr uint8_t swapbits(uint8_t a){
	unsigned int b = a;
	b = ((b * 0x0802LU & 0x22110LU) | (b * 0x8020LU & 0x88440LU)) * 0x10101LU >> 16;
	return (uint8_t)b;
}

