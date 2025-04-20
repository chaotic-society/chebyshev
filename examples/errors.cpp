
///
/// @file errors.h Example program for error checking.
///

#include "chebyshev.h"
#include <cmath>
using namespace ch;


double f(double x) {
	return std::sqrt(x);
}


double g(double x) {

	if(x < 0)
		throw std::runtime_error("My error");

	return 1;
}


int main(int argc, char const *argv[]) {

	// Setup error checking
	auto ctx = err::make_context("example", argc, argv);


	// Set the output file for the err module
	ctx.settings.outputFiles = { "example_err.csv" };


	// Make an assert
	ctx.assert("std::sqrt", std::sqrt(4) == 2, "sqrt(4) is 2");


	// Check errno value after function call
	ctx.check_errno("f(x)", f, -1, EDOM);


	// Check that a function throws an exception
	ctx.check_exception("g(x)", g, -1);


	// Check that a function throws an exception,
	// controlling its type.
	ctx.check_exception<std::runtime_error>("g(x)", g, -2);
}
