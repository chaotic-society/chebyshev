
#include "chebyshev.h"
using namespace chebyshev;


int main(int argc, char const *argv[]) {

	prec::setup("example", argc, argv);

		prec::equals("Test 1", 1, 1);

		prec::equation_options<double> opt {};
		opt.distance = prec::distance::abs_distance<double>;
		opt.tolerance = 0;

		prec::equals<double>("Test 2", 1, 1 + 1E-12, opt);

	prec::terminate();
}
