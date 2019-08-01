#pragma once

#include "read.h"
#include <cstdint>

namespace io {
	uint64_t read_variable_length_integer(std::istream& in);
}
