
#include "chebyshev.h"
using namespace chebyshev;


double f(double x) {
	return x * std::sqrt(x);
}

double g(double x) {
	return x * std::sqrt(x + 1E-10);
}


int main(int argc, char const *argv[]) {


	// Precision estimation
	prec::setup("example", argc, argv);

		prec::equals("1 = 1", 1 + 1, 2);

		prec::estimate("f(x)", f, g, prec::interval(0, 100));

	prec::terminate(false);


	// Benchmarks
	benchmark::setup("example", argc, argv);

		benchmark::benchmark("f(x)", f);
		benchmark::benchmark("g(x)", g);

	benchmark::terminate(false);


	// Error checking
	err::setup("example", argc, argv);

		err::check_errno(f, -1, EDOM);

	err::terminate();
}
