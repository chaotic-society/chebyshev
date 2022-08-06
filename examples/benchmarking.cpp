
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

		// Custom benchmarks can be used too
		benchmark::custom_request("custom",
			[](unsigned int n, unsigned int m) {

				__volatile__ Real c = 0;
				Real tot = 0;

				for (unsigned int i = 0; i < n; ++i) {
					timer t = timer();

					for (unsigned int j = 1; j <= m; ++j)
						c += std::sqrt(i / (double) j);

					tot += t();
				}

				return benchmark::benchmark_result(tot, n, m);
			}, 500000, 4);

	// Stop benchmarking and exit
	benchmark::terminate();
}
