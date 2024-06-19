
#include "chebyshev.h"
using namespace chebyshev;


int main(int argc, char const *argv[]) {

	prec::setup("example", argc, argv);

		prec::equation_options<double> opt {};
		opt.distance = prec::distance::abs_distance<double>;
		opt.tolerance = 0;

		prec::equals("1=1", 1, 1);
		prec::equals<double>("1=1...", 1, 1 + 1E-12, opt);

	prec::terminate();
}
