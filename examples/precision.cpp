#include "chebyshev.h"
using namespace ch;


Real f(Real x) {
	return x * x;
}


Real f_a(Real x) {
	return x * x + sqrt(x) * 0.1 + 0.001;
}


int main(int argc, char const *argv[]) {

	// // Setup the precision testing environment
	prec::setup("chebyshev");

		// Estimate errors on f_a on [0, 10]
		prec::estimate("f_a", f_a, f, interval(0, 10));

		// Estimate errors on f_a on multiple intervals
		prec::estimate("f_a", f_a, f, {
			interval(0, 1),
			interval(1, 10),
			interval(10, 100),
			interval(100, 1000)
		});

		// Check that two values are almost equal
		prec::equals("f_a", f_a(1), 1, 0.2);

		// Check multiple pairs of values
		prec::equals("f_a", {
			{f_a(1), 1},
			{f_a(2), 4},
			{f_a(3), 9}
		});

		// Or more simply, if the function is the same:
		prec::equals("f_a", f_a, {
			{1, 1},
			{2, 4},
			{3, 9}
		});

	// Stop precision testing
	prec::terminate();
}
