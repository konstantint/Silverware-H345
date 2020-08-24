// Generic interface to the XN297 radio chip over an SPI bus.
//
// XN297L English datasheet: http://www.panchip.com/static/upload/file/20190916/1568621331607821.pdf
//
// "XN297L is compatible with the XN297, an early version of XN297L.
//  With a fewer external passive components than the XN297, XN297L improves the RF
//  specification margins for meeting RF regulatory standards."
//
// NB: X297 is a clone of nRF24L01+ with an extra command (ACTIVATE) and
// three additional registers (DEMOD_CAL, RF_CAL, BB_CAL)
//
// License: MIT
// Author: silverx, conversion to C++ by konstantint
// Based on the work of silverx and other contributors to the Silverware project.

#pragma once
#include <cstdint>

// registers
#define CONFIG      0x00
#define EN_AA       0x01
#define EN_RXADDR   0x02
#define SETUP_AW    0x03
#define SETUP_RETR  0x04
#define RF_CH       0x05
#define RF_SETUP    0x06
#define R_STATUS    0x07
#define OBSERVE_TX  0x08
#define CD          0x09
#define RX_ADDR_P0  0x0A
#define RX_ADDR_P1  0x0B
#define RX_ADDR_P2  0x0C
#define RX_ADDR_P3  0x0D
#define RX_ADDR_P4  0x0E
#define RX_ADDR_P5  0x0F
#define TX_ADDR     0x10
#define RX_PW_P0    0x11
#define RX_PW_P1    0x12
#define RX_PW_P2    0x13
#define RX_PW_P3    0x14
#define RX_PW_P4    0x15
#define RX_PW_P5    0x16
#define FIFO_STATUS 0x17
#define R_FEATURE 	0x1d

// Bit positions
#define MASK_RX_DR  6
#define MASK_TX_DS  5
#define MASK_MAX_RT 4
#define EN_CRC      3
#define CRCO        2
#define PWR_UP      1
#define PWR_DOWN    0
#define PRIM_RX     0
#define ENAA_P5     5
#define ENAA_P4     4
#define ENAA_P3     3
#define ENAA_P2     2
#define ENAA_P1     1
#define ENAA_P0     0
#define ERX_P5      5
#define ERX_P4      4
#define ERX_P3      3
#define ERX_P2      2
#define ERX_P1      1
#define ERX_P0      0
#define AW          0
#define ARD         4
#define ARC         0
#define PLL_LOCK    4
#define RF_DR       3
#define RF_PWR      1
#define LNA_HCURR   0        
#define RX_DR       6
#define TX_DS       5
#define MAX_RT      4
#define RX_P_NO     1
#define TX_FULL     0
#define PLOS_CNT    4
#define ARC_CNT     0
#define TX_REUSE    6
#define FIFO_FULL   5
#define TX_EMPTY    4
#define RX_FULL     1
#define RX_EMPTY    0

// address width (bytes)
#define AW_3B 1
#define AW_4B 2
#define AW_5B 3

// commands
#define R_REGISTER    0x00
#define W_REGISTER    0x20
#define REGISTER_MASK 0x1F
#define ACTIVATE      0x50
#define R_RX_PAYLOAD  0x61
#define W_TX_PAYLOAD  0xA0
#define CE_FSPI_ON	  0xFD
#define FLUSH_TX      0xE1
#define FLUSH_RX      0xE2
#define REUSE_TX_PL   0xE3
#define NOP           0xFF

// bitrate
#define BITRATE_1M 		0x0
#define BITRATE_2M 		0x1
#define BITRATE_250K 	0x2


// The different types of Xn297 chip have
// different commands that are incompatible
// (e.g. setting a data rate for Xn297 and Xn297L is different)
// Seemingly Xn297-compatible nRF datasheet:
//   https://www.sparkfun.com/datasheets/Components/nRF24L01_prelim_prod_spec_1_2.pdf
// Xn297L datasheet:
//   http://www.panchip.com/static/upload/file/20190916/1568621331607821.pdf
enum class Xn297Type {
	Xn297,
	Xn297L
};


template<Xn297Type XnType, typename Spi> class Xn297 {
	Spi& _spi;
public:
	constexpr static Xn297Type xn297_type = XnType;

	inline explicit Xn297(Spi& spi) :
			_spi(spi) {
	}

	void write_reg(uint8_t reg, uint8_t val) {
		_spi.cson();
		_spi.sendrecv(reg | W_REGISTER);
		_spi.sendrecv(val);
		_spi.csoff();
	}

	char read_reg(uint8_t reg) {
		_spi.cson();
		_spi.sendrecv(reg);
		char val = _spi.sendrecv(0x00);
		_spi.csoff();
		return val;
	}

	void write_buf(uint8_t reg, const uint8_t *data, int size) {
		_spi.cson();
		_spi.sendrecv(reg | W_REGISTER);
		for (int i = 0; i < size; i++)
			_spi.sendrecv(data[i]);
		_spi.csoff();
	}

	void read_buf(uint8_t reg, uint8_t *data, int size) {
		_spi.cson();
		_spi.sendrecv(reg);
		for (int i = 0; i < size; i++)
			data[i] = _spi.sendrecv(0x00);
		_spi.csoff();
	}

	int command(uint8_t cmd) {
		_spi.cson();
		int status = _spi.sendrecv(cmd);
		_spi.csoff();
		return status;
	}

	bool self_test() {
		// We can self-test by reading the RX_ADDR_P5 register
		// It must be equal to 0xC6 on reset
		return read_reg(RX_ADDR_P5) == 0xC6;
	}

	// Sets the highest TX power and a given data rate
	// Rate:
	//   0 = 1Mbps
	//   1 = 2Mbps
	//   3 = 250Kbps (X297L only)
	inline void set_data_rate_and_max_tx_power(char rate) {
		if (XnType == Xn297Type::Xn297)
			write_reg(RF_SETUP, (uint8_t)(0b100111 + (rate << 6)));
		else
			write_reg(RF_SETUP, (uint8_t)(0b111 + (rate << 3)));
	}

	// Sets the software CE pin for Xn297 and enables 64 byte payloads
	inline void set_software_ce_and_64_byte_payload() {
		if (XnType == Xn297Type::Xn297L) {
			write_reg(R_FEATURE, 0b00111000); // 64 byte payload, software CE
			write_reg(CE_FSPI_ON, 0x00); // Internal CE high command followed by 0x00
		}
	}

	inline void power_up(bool rx, bool crc_2bytes,  bool crc_enabled, bool stb3) {
		uint8_t value = rx | (0b10) | (crc_2bytes << 2) | (crc_enabled << 3);
		if (XnType == Xn297Type::Xn297L) value |= (stb3 << 7);
		write_reg(CONFIG, value);
	}

	inline bool tx_fifo_empty() {
		return (read_reg(FIFO_STATUS) & (1 << TX_EMPTY));
	}

	inline bool rx_fifo_empty() {
		return (read_reg(FIFO_STATUS) & (1 << RX_EMPTY));
	}
};
