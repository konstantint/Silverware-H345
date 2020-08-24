#include "xn297_ble_util.h"

///////////////////
// nRF24 BLE Utility Functions
// https://github.com/lijunhw/nRF24_BLE/blob/master/Arduino/nRF24_BLE_advertizer_demo/nRF24_BLE_advertizer_demo.ino

void btLeCrc(const uint8_t* data, uint8_t len, uint8_t* dst) {
	// implementing CRC with LFSR
	uint8_t v, t, d;
	while (len--) {
		d = *data++;
		for (v = 0; v < 8; v++, d >>= 1) {
			t = dst[0] >> 7;
			dst[0] <<= 1;
			if (dst[1] & 0x80)
				dst[0] |= 1;
			dst[1] <<= 1;
			if (dst[2] & 0x80)
				dst[1] |= 1;
			dst[2] <<= 1;
			if (t != (d & 1)) {
				dst[2] ^= 0x5B;
				dst[1] ^= 0x06;
			}
		}
	}
}

void btLeWhiten(uint8_t* data, uint8_t len, uint8_t whitenCoeff) {
	// Implementing whitening with LFSR
	uint8_t m;
	while (len--) {
		for (m = 1; m; m <<= 1) {
			if (whitenCoeff & 0x80) {
				whitenCoeff ^= 0x11;
				(*data) ^= m;
			}
			whitenCoeff <<= 1;
		}
		data++;
	}
}

constexpr uint8_t btLeWhitenStart(uint8_t chan) {
	//the value we actually use is what BT'd use left shifted one...makes our life easier
	return swapbits(chan) | 2;
}

// Precomputed whitening sequence for BLE advertising channel 37 (2402Mhz)
constexpr uint8_t ble_whiten_37[] = { 0x8D, 0xd2, 0x57, 0xa1, 0x3d, 0xa7, 0x66,
		0xb0, 0x75, 0x31, 0x11, 0x48, 0x96, 0x77, 0xf8, 0xe3, 0x46, 0xe9, 0xab,
		0xd0, 0x9e, 0x53, 0x33, 0xd8, 0xba, 0x98, 0x08, 0x24, 0xcb, 0x3b, 0xfc,
		0x71, 0xa3, 0xf4, 0x55, 0x68, 0xCF, 0xA9, 0x19, 0x6C, 0x5D, 0x4C };

void btLePacketEncode(uint8_t* packet, uint8_t len, uint8_t chan) {
	// Assemble the packet to be transmitted
	// Length is of packet, including crc. pre-populate crc in packet with initial crc value!
	uint8_t i, dataLen = len - 3;

	packet[len - 3] = 0x55; //CRC start value: 0x555555
	packet[len - 2] = 0x55;
	packet[len - 1] = 0x55;

	btLeCrc(packet, dataLen, packet + dataLen);

	for (i = 0; i < 3; i++, dataLen++)
		packet[dataLen] = swapbits(packet[dataLen]);

	if (chan == 37) {
		// faster array based whitening
		for (i = 0; i < len; i++)
			packet[i] ^= ble_whiten_37[i];
	} else
		// lfsr based
		btLeWhiten(packet, len, btLeWhitenStart(chan));
}

BleAdvertisingPacket::BleAdvertisingPacket(const MacAddress& mac_addr):
	std::array<uint8_t, 42>(), pos(0) {
	// First two bytes: Header + Payload length
	(*this)[pos++] = 0x42; // TxAddr random (0x40) + ADV_NONCONN_IND (0x02)
	pos++; // We set the payload length byte later.
	// NB: Payload length excludes CRC and is limited to 37.
	std::copy(mac_addr.begin(), mac_addr.end(), &this->data()[pos]);
	pos += 6;

	(*this)[pos++] = 2;    // Length=2
	(*this)[pos++] = 0x01; // Type = Flags
	(*this)[pos++] = 0x06; // Content = (LE-only, general discoverable mode)
}


// If name[0] = 0x00, the name is not included in the packet
void BleAdvertisingPacket::eddystone_tlm(uint32_t time_us, const char name[4], float vbatt, uint32_t adv_cnt, float temp) {
	(*this)[1] = 6+3+4+18 + (name[0] == 0x00 ? 0 : 6); // Payload length = 31 (no name) or 37 (with name, max)

	// Field types see at:
	// https://www.bluetooth.com/specifications/assigned-numbers/generic-access-profile/
	(*this)[pos++] = 3;  	  // Length=3
	(*this)[pos++] = 0x03;  // Type = GAP_ADTYPE_16BIT_COMPLETE
	(*this)[pos++] = 0xAA;  // Eddystone ID - 16 bit 0xFEAA
	(*this)[pos++] = 0xFE;  // Eddystone ID

	if (name[0] != 0x00) {
		// We can only fit 4 character-name in what remains from the Eddystone-TLM packet
		(*this)[pos++] = 5;  	  // Length=5
		(*this)[pos++] = 0x08;    // Type = GAP_ADTYPE_LOCAL_NAME_SHORT
		(*this)[pos++] = name[0];
		(*this)[pos++] = name[1];
		(*this)[pos++] = name[2];
		(*this)[pos++] = name[3];
	}

	(*this)[pos++] = 17;  // Length = 17
	(*this)[pos++] = 0x16;  // Type = GAP_ADTYPE_SERVICE_DATA
	(*this)[pos++] = 0xAA;  // Eddystone ID
	(*this)[pos++] = 0xFE;  // Eddystone ID
	(*this)[pos++] = 0x20;  // Eddystone TLM frame type
	(*this)[pos++] = 0x00;  // TLM version

	// Battery voltage is the current battery charge in millivolts, expressed as 1 mV per bit.
	// If not supported (for example in a USB-powered beacon) the value should be zeroed.
	int vbatt_i = vbatt * 1000.0;
	(*this)[pos++] = vbatt_i >> 8;
	(*this)[pos++] = vbatt_i;

	// Beacon temperature is the temperature in degrees Celsius
	// sensed by the beacon and expressed in a signed 8.8 fixed-point notation.
	// If not supported the value should be set to 0x8000, -128 °C.
	int temp_i = temp * (1<<8);
	(*this)[pos++] = temp_i >> 8;
	(*this)[pos++] = temp_i;

	// ADV_CNT is the running count of advertisement frames of all types emitted by the beacon since
	// power-up or reboot, useful for monitoring performance metrics that scale per broadcast frame.
	// If this value is reset (e.g. on reboot), the current time field must also be reset.
	(*this)[pos++] = adv_cnt >> 24;
	(*this)[pos++] = adv_cnt >> 16;
	(*this)[pos++] = adv_cnt >> 8;
	(*this)[pos++] = adv_cnt;

	// SEC_CNT is a 0.1 second resolution counter that represents time since
	// beacon power-up or reboot. If this value is reset (e.g. on a reboot), the ADV count field
	// must also be reset.
	uint32_t time_i = time_us / 100000;
	(*this)[pos++] = time_i >> 24;
	(*this)[pos++] = time_i >> 16;
	(*this)[pos++] = time_i >> 8;
	(*this)[pos++] = time_i;
}

void BleAdvertisingPacket::eddystone_url(uint8_t tx_power, uint8_t url_scheme, const char* url, uint8_t url_length) {
	(*this)[1] = 20 + url_length;

	(*this)[pos++] = 3;  	  // Length=3
	(*this)[pos++] = 0x03;  // Type = GAP_ADTYPE_16BIT_COMPLETE
	(*this)[pos++] = 0xAA;  // Eddystone ID - 16 bit 0xFEAA
	(*this)[pos++] = 0xFE;  // Eddystone ID

	(*this)[pos++] = 6+url_length;
	(*this)[pos++] = 0x16;  // Type = GAP_ADTYPE_SERVICE_DATA
	(*this)[pos++] = 0xAA;  // Eddystone ID
	(*this)[pos++] = 0xFE;  // Eddystone ID
	(*this)[pos++] = 0x10;  // Eddystone URL frame type
	(*this)[pos++] = tx_power;
	(*this)[pos++] = url_scheme;
	std::copy(&url[0], &url[url_length], &this->data()[pos]);
	pos += url_length;
}
