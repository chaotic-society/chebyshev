
#include "chebyshev.h"
using namespace chebyshev;


double f(double x) {
	return x * std::sqrt(x);
}

double g(double x) {
	return x * std::sqrt(x + 1E-10);
}


int main(int argc, char const *argv[]) {

	benchmark::setup("example", argc, argv);

		benchmark::benchmark("f(x)", f);
		benchmark::benchmark("g(x)", g);

	benchmark::terminate();
}
