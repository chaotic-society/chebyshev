
///
/// @file benchmark.h Benchmark module.
///

#ifndef CHEBYSHEV_BENCHMARK_H
#define CHEBYSHEV_BENCHMARK_H

#include "benchmark/timer.h"


namespace chebyshev {

	/// @namespace chebyshev::benchmark Benchmark module.
	namespace benchmark {


		/// @class benchmark_state
		/// Global state of the benchmark module.
		// struct benchmark_state {

		// 	/// Whether to print benchmark results
		// 	/// to standard output.
		// 	bool quiet = false;

		// 	/// Name of the module currently being benchmarked
		// 	std::string moduleName;

		// 	/// Default number of iterations
		// 	unsigned int defaultIterations = CHEBYSHEV_BENCHMARK_ITER;

		// 	/// Default number of runs
		// 	unsigned int defaultRuns = CHEBYSHEV_BENCHMARK_RUNS;

		// 	/// Output file for the current module
		// 	std::ofstream outputFile;

		// 	/// Relative or absolute path to output folder
		// 	std::string outputFolder = "";

		// 	/// Benchmark state.results
		// 	std::vector<benchmark_result> results;

		// 	/// Number of failed benchmarks
		// 	unsigned int failedBenchmarks = 0;

		// 	/// Target benchmarks marked for execution
		// 	/// (all benchmarks will be executed if empty)
		// 	std::map<std::string, bool> pickedBenchmarks;
			
		// } state;


	}

}

#endif
