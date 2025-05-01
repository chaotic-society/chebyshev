
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
	err::err_context ctx ("example", argc, argv);


	// Set the output file for the err module
	ctx.settings.outputFiles = { "examples/errors.csv" };


	// Make an assert, checking that an expression is true
	ctx.assert("std::sqrt", std::sqrt(4) == 2, "sqrt(4) is 2");


	// Check errno value after function call
	ctx.errno_value("f(x)", f, -1, EDOM);


	// Check that a function throws an exception
	ctx.throws("g(x)", g, -1);


	// Check that a function throws an exception
	// of the given type
	ctx.throws<std::runtime_error>("g(x)", g, -2);
}
