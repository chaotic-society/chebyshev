#include "src/chebyshev.h"
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

		// Estimate errors on f_a on [0, 10]
		prec::estimate("f_a", f, f_a, interval(0, 10));

		// Estimate errors on f_a on multiple intervals
		prec::estimate("f_a", f, f_a, {
			interval(0, 1),
			interval(1, 10),
			interval(10, 100),
			interval(100, 1000)
		});

		// Check that two values are almost equals
		prec::equals("f_a", f_a(1), 1, 0.2);

		// Check multiple pairs of values
		prec::equals("f_a", {
			{f_a(1), 1},
			{f_a(2), 4},
			{f_a(3), 3}
		});

		// Or more simply, if the function is the same:
		prec::equals("f_a", f_a, {
			{1, 1},
			{2, 4},
			{3, 3}
		});

	// Stop precision testing
	prec::terminate(false);
	std::cout << std::endl;
	

	// Setup error checking
	err::setup("chebyshev");

		// Make an assert
		err::assert(sqrt(4) == 2, "sqrt(4) is 2");

		// Or using a simple macro
		AUTOASSERT(sqrt(9) == 3);

		// Check errno value after function call
		err::check_errno(REAL_LAMBDA(std::sqrt), -1, EDOM);

	// Stop error checking
	err::terminate(false);
	std::cout << std::endl;


	// Setup benchmarking
	benchmark::setup("chebyshev");

		// Register a function to be benchmarked
		benchmark::register_function("std::sqrt", REAL_LAMBDA(std::sqrt), uniform_generator(0, 1000));

		// Run all benchmarks
		benchmark::run();

	// Stop benchmarking and exit
	benchmark::terminate();
}
