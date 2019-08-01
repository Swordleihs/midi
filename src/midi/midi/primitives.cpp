#include "primitives.h"

namespace midi {

	Time operator + (const Time t, const Duration d) {
		return Time(value(t) + value(d));
	}

	Time operator + (const Duration d, const Time t) {
		return Time(value(d) + value(t));
	}

	Time& operator += (Time& t, const Duration d) {
		value(t) += value(d);
		return t;
	}

	Duration operator + (const Duration d0, const Duration d1) {
		return Duration(value(d0) + value(d1));
	}

	Duration operator - (const Time t0, const Time t1) {
		return Duration(value(t0) - value(t1));
	}

	Duration operator - (const Duration d0, const Duration d1) {
		return Duration(value(d0) - value(d1));
	}

	Duration& operator += (Duration& d0, Duration d1) {
		value(d0) += value(d1);
		return d0;
	}

	Duration& operator -= (Duration& d0, Duration d1) {
		value(d0) -= value(d1);
		return d0;
	}

}