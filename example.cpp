#include "src/precision.h"
using namespace chebyshev;


Real f(Real x) {
	return x * x;
}


Real f_a(Real x) {
	return x * x + 0.1;
}


int main(int argc, char const *argv[]) {

	// Setup the precision testing environment
	prec::setup("chebyshev");

	// Print pretty header
	prec::header();

	// Estimate errors on f_a on [0, 10]
	prec::estimate("f", f, f_a, interval(0, 10));

	// Estimate errors on f_a on multiple intervals
	prec::estimate("f", f, f_a, {
		interval(0, 1),
		interval(1, 10),
		interval(10, 100),
		interval(100, 1000)
	});

	// Stop testing
	prec::terminate();	
}
