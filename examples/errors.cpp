
///
/// @file errors.h Example program for error checking.
///

#include "chebyshev.h"
#include <cmath>
using namespace ch;


double f(double x) {
	return std::sqrt(x);
}


int main(int argc, char const *argv[]) {

	// Setup error checking
	err::setup("chebyshev");

		// Make an assert
		err::assert(sqrt(4) == 2, "sqrt(4) is 2");

		// Check errno value after function call
		err::check_errno(f, -1, EDOM);

	// Stop error checking
	err::terminate();
}
