#include "endianness.h"

void io::switch_endianness(uint16_t* n) {
	uint16_t b0 = *n & 0xFF00;
	b0 = b0 >> 8;

	uint16_t b1 = *n & 0x00FF;
	b1 = b1 << 8;

	*n = b0 | b1;
}

void io::switch_endianness(uint32_t* n) {
	uint32_t b0 = *n & 0xFF000000;
	b0 = b0 >> 24;

	uint32_t b1 = *n & 0x00FF0000;
	b1 = b1 >> 8;

	uint32_t b2 = *n & 0x0000FF00;
	b2 = b2 << 8;

	uint32_t b3 = *n & 0x000000FF;
	b3 = b3 << 24;

	*n = b0 | b1 | b2 | b3;
}

void io::switch_endianness(uint64_t* n) {
	uint64_t b0 = *n & 0xFF00000000000000;
	b0 = b0 >> 56;

	uint64_t b1 = *n & 0x00FF000000000000;
	b1 = b1 >> 40;

	uint64_t b2 = *n & 0x0000FF0000000000;
	b2 = b2 >> 24;

	uint64_t b3 = *n & 0x000000FF00000000;
	b3 = b3 >> 8;

	uint64_t b4 = *n & 0x00000000FF000000;
	b4 = b4 << 8;

	uint64_t b5 = *n & 0x0000000000FF0000;
	b5 = b5 << 24;
   
	uint64_t b6 = *n & 0x000000000000FF00;
	b6 = b6 << 40;

	uint64_t b7 = *n & 0x00000000000000FF;
	b7 = b7 << 56;

	*n = b0 | b1 | b2 | b3 | b4 | b5 | b6 | b7;
}
