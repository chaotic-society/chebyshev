
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

double almost_zero(double x) {
	return 1E-10 * random::uniform(-1, 1);
}


int main(int argc, char const *argv[]) {

	// Setup the precision testing environment
	auto ctx = prec::prec_context("example", argc, argv);
	

	// Set the output file for the prec module
	ctx.settings.outputFiles = { "example_prec.csv" };


	// Estimate errors on g(x) on [0, 100]
	ctx.estimate("g(x)", g, f, prec::interval(0, 100));


	// Check that two values are equal up to a tolerance
	ctx.equals("f(1) = 1", f(1), 1, 1E-04);


	// Check that two values are equal up to a tolerance
	ctx.equals("g(1) = 1", g(1), 1, 1E-02);


	// Construct options from the test interval and estimator
	auto opt = prec::estimate_options<double, double>(
		prec::interval(1.0, 10.0),
		prec::estimator::quadrature1D<double>()
	);


	// Precision test an involution
	ctx.involution("inverse(x)", inverse, opt);


	// Precision test an idempotent function
	ctx.idempotence("absolute(x)", absolute, opt);


	// Precision test an homogeneous function
	ctx.homogeneous("almost_zero(x)", almost_zero, opt);

	// You can use ctx.terminate() to print the results,
	// or leave it to the destructor to do it automatically.
}
