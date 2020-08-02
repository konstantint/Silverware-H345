// printf extensions.
//
// License: MIT
// Author: konstantint

#include "printf.h"
#include <cstdio>

// See: https://stackoverflow.com/questions/12703307/arm-none-eabi-gcc-printing-float-number-using-printf
void printf(float v) {
	char sign = v < 0 ? '-' : ' ';
	v = v < 0 ? -v : v;
	int int_part = (int) v;
	float pow = 1000000;
	int frac_part = (int) ((v - (float) int_part) * pow);
	printf("%c%i.%06i", sign, int_part, frac_part);
}

void printf(const Point3d<float>& point) {
	printf("(");
	printf(point.x());
	printf(", ");
	printf(point.y());
	printf(", ");
	printf(point.z());
	printf(")");
}

void printf(const RxData& rxdata) {
	printf("RXMode: %i\n", rxdata.rx_mode);
	printf("RX: ");
	for (int i = 0; i < 4; i++) {
		printf(rxdata.rx[i]);
		printf(" ");
	}
	printf("\nAux: ");
	for (int i = 0; i < NUM_AUX_CHANNELS; i++)
		printf("%i ", rxdata.aux[i]);
	printf("\nPacket: ");
	for (int i = 0; i < rxdata.packet_size; i++)
		printf("%02x ", (unsigned char) rxdata.packet_bytes[i]);
	printf("\n");
}
