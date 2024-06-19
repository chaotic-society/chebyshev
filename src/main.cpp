
#include "chebyshev.h"
using namespace chebyshev;


double f(double x) {
	return x * x;
}

double g(double x) {
	return x * (x + 1E-10);
}


int main(int argc, char const *argv[]) {

	prec::setup("example", argc, argv);

		prec::equals("1 + 1 = 2", 1 + 1, 2);

		prec::estimate("f(x)", f, g, prec::interval(0, 100));

	prec::terminate();
}
