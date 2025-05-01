
///
/// @file errors.h Example program for benchmarks.
///

#include "chebyshev.h"
#include <cmath>

using namespace ch;


double f(double x) {
	return x * std::sqrt(x);
}


double g(double x) {
	return std::atan(x * x);
}


unsigned int h(unsigned int n) {
	
	if(n == 0)
		return 0;
	else
		return n + h(n - 1);
}



int main(int argc, char const *argv[]) {

	// Setup benchmarking
	auto ctx = benchmark::make_context("example", argc, argv);

	// Set the output file for the benchmark module
	ctx.settings.outputFiles = { "examples/benchmark.csv" };


	// Set options for multiple benchmarks
	// with a benchmark_options structure,
	// specialized for functions taking in doubles
	auto opt = benchmark::benchmark_options<double>(
		10, 	// Runs
		1E+06,	// Iterations
		benchmark::generator::uniform1D(0, 1000) // Input generator
	);

	// Benchmark the given functions
	ctx.benchmark("f(x)", f, opt);
	ctx.benchmark("g(x)", g, opt);

	// You may need to specify the input type
	// of your function if it isn't deduced.

	// Specify parameters directly
	ctx.benchmark<unsigned int>(
		"h(n)", h,
		benchmark::generator::discrete1D(0, 1000), // Input generator
		10,		// Runs
		1000	// Iterations
	);
}
