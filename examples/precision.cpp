
///
/// @file precision.cpp Example program for precision testing.
///

#include "chebyshev.h"
using namespace ch;


double f(double x) {
	return x * std::sqrt(x);
}


double g(double x) {
	return x * std::sqrt(x + 1E-12);
}


int main(int argc, char const *argv[]) {

	// Setup the precision testing environment
	prec::setup("chebyshev", argc, argv);

		// Estimate errors on g(x) on [0, 1000]
		prec::estimate("g(x)", g, f, prec::interval(0, 100));

		// Check that two values are equal up to a tolerance
		prec::equals("f(1) = 1", f(1), 1, 1E-04);

		// Check that two values are equal up to a tolerance
		prec::equals("g(1) = 1", g(1), 1, 1E-02);

	// Stop precision testing
	prec::terminate();
}
