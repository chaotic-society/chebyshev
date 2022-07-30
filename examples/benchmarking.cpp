
#include "chebyshev.h"
#include <cmath>
using namespace ch;


int main(int argc, char const *argv[]) {

	// Setup benchmarking
	benchmark::setup("chebyshev");

		// Register a function to be benchmarked
		benchmark::request("std::sqrt", REAL_LAMBDA(std::sqrt), uniform_generator(0, 1000));

		benchmark::request("std::cbrt", REAL_LAMBDA(std::cbrt), uniform_generator(0, 1000));

		benchmark::request("std::exp", REAL_LAMBDA(std::exp), uniform_generator(-100, 10));


	// Stop benchmarking and exit
	benchmark::terminate();
}
