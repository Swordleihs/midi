#pragma once
#include "util/tagged.h"

namespace midi {

	struct __declspec(empty_bases) Channel :
		tagged<uint8_t, Channel>,
		equality<Channel>,
		show_value<Channel, int>
	{
		using tagged::tagged;
	};

	struct __declspec(empty_bases) Instrument :
		tagged<uint8_t, Instrument>,
		equality<Instrument>,
		show_value<Instrument, int>
	{
		using tagged::tagged;
	};

	struct __declspec(empty_bases) NoteNumber :
		tagged<uint8_t, NoteNumber>,
		ordered<NoteNumber>,
		show_value<NoteNumber, int>
	{
		using tagged::tagged;
	};

	struct __declspec(empty_bases)Time :
		tagged<uint64_t, Time>,
		ordered<Time>,
		show_value<Time, int>
	{
		using tagged::tagged;
	};

	struct __declspec(empty_bases)Duration :
		tagged<uint64_t, Duration>,
		ordered<Duration>,
		show_value<Duration, int>
	{
		using tagged::tagged;
	};

	Time operator + (const Time t, const Duration d);
	Time operator + (const Duration d, const Time t);
	Time& operator += (Time& t, const Duration d);
	
	Duration operator + (const Duration d0, const Duration d1);
	Duration operator - (const Time t0, const Time t1);
	Duration operator - (const Duration d0, const Duration d1);
	Duration& operator += (Duration& d0, Duration d1);
	Duration& operator -= (Duration& d0, Duration d1); 
}
