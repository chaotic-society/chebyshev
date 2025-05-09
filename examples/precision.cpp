
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

double inverse(double x) {
	return 1.0 / x;
}

double absolute(double x) {
	return std::abs(x) - 1E-09;
}

double noise(random::random_source& rnd, double x) {
	return 1E-10 * rnd.uniform(-1, 1);
}


int main(int argc, char const *argv[]) {

	// Setup the precision testing environment
	prec::prec_context ctx ("example", argc, argv);
	

	// Set the output file for the prec module
	ctx.settings.outputFiles = { "examples/precision.csv" };

	ctx.settings.defaultIterations = 1E+06;


	// Estimate errors on g(x) on [0, 100]
	ctx.estimate("g(x)", g, f, prec::interval(0, 100));


	// Check that two values are equal up to a tolerance
	ctx.equals("f(1) = 1", f(1), 1, 1E-04);


	// Check that two values are equal up to a tolerance
	ctx.equals("g(1) = 1", g(1), 1, 1E-02);


	// Construct options from the test interval and estimator
	auto opt = prec::estimate_options<double, double>(
		prec::interval(1.0, 10.0),					// Interval of estimation
		prec::estimator::quadrature1D<double>(),	// Estimator
		1E-02,	// Tolerance
		1E+06	// Iterations
	);


	// Precision test an involution
	ctx.involution("inverse(x)", inverse, opt);


	// Precision test an idempotent function
	ctx.idempotence("absolute(x)", absolute, opt);

	// Get a source of random numbers
	random::random_source rnd = ctx.random->get_rnd();
	auto almost_zero = [rnd](double x) {
		random::random_source r = rnd;
		return noise(r, x);
	};

	// Precision test an homogeneous function
	ctx.homogeneous("almost_zero(x)", almost_zero, opt);

	// You can use ctx.terminate() to print the results,
	// or leave it to the destructor to do it automatically.
	// ctx.terminate();
}
