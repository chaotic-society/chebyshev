
#include "chebyshev.h"
#include <cmath>
using namespace ch;


int main(int argc, char const *argv[]) {

	// Setup benchmarking
	benchmark::setup("chebyshev");

		// Register a function to be benchmarked
		benchmark::request("std::sqrt", REAL_LAMBDA(std::sqrt), uniform_generator(0, 1000));

		// Register a function to be benchmarked
		benchmark::request("std::exp", REAL_LAMBDA(std::exp), uniform_generator(-10, 10));

		// Or using a simple macro
		BENCHMARK(std::exp, -10, 10);

	// Stop benchmarking and exit
	benchmark::terminate();
}
