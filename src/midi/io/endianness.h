#pragma once
#include <cstdint>

namespace io {
	void switch_endianness(uint16_t* n);
	void switch_endianness(uint32_t* n);
	void switch_endianness(uint64_t* n);
}