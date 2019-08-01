#include "vli.h"

uint8_t drop_first_bit(uint8_t u) {
	return (u & 0x7F);
}

bool significant_set(uint8_t u) {
	return (u >> 7) == 1;
}

uint64_t io::read_variable_length_integer(std::istream& in) {
	uint64_t result = 0;
	uint8_t byte = read<uint8_t>(in);

	while(significant_set(byte)){
		result = (result << 7) | drop_first_bit(byte);
		byte = read<uint8_t>(in);
	}

	return ((result << 7) | drop_first_bit(byte));	
}