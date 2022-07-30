
#include "chebyshev.h"
#include <cmath>
using namespace ch;


int main(int argc, char const *argv[]) {

	// Setup error checking
	err::setup("chebyshev");

		// Make an assert
		err::assert(sqrt(4) == 2, "sqrt(4) is 2");

		// Or using a simple macro
		AUTOASSERT(sqrt(9) == 3);

		// Check errno value after function call
		err::check_errno(REAL_LAMBDA(std::sqrt), -1, EDOM);

	// Stop error checking
	err::terminate();
}
