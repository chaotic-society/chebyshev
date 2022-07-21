
///
/// @file interval.h An interval on the real numbers
///

namespace chebyshev {

	/// @class interval An interval on the real numbers
	struct interval {

		// [a, b]

		long double a;
		long double b;

		interval() : a(0), b(1) {}

		interval(long double l, long double r) : a(l), b(r) {}
	};

}
