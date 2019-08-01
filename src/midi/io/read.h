#pragma once

#include "logging.h"
#include <istream>

namespace io {

	template<typename T>
	void read_to(std::istream& in, T* buffer, size_t n = 1)
	{
		in.read(reinterpret_cast<char*>(buffer), sizeof(T) * n);
		CHECK(!in.fail()) << __FUNCTION__ << " has failed.";
	}

	template<typename T, typename std::enable_if<std::is_fundamental<T>::value, T>::type* = nullptr>
	T read(std::istream& in)
	{
		T t;
		read_to(in, &t);
		return t;
	}

	template<typename T>
	std::unique_ptr<T[]> read_array(std::istream& in, size_t n)
	{
		std::unique_ptr<T[]> array = std::make_unique<T[]>(n);
		read_to(in, array.get(), n);
		return array;
	}

}
