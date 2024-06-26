
///
/// @file benchmark.h Benchmark module.
///

#ifndef CHEBYSHEV_BENCHMARK_H
#define CHEBYSHEV_BENCHMARK_H

#include <ctime>

#include "./core/random.h"
#include "./benchmark/timer.h"
#include "./benchmark/generator.h"
#include "./benchmark/benchmark_structures.h"


namespace chebyshev {


	/// @namespace chebyshev::benchmark Benchmark module.
	namespace benchmark {


		/// @class benchmark_state
		/// Global state of the benchmark module.
		struct benchmark_state {

			/// Whether to print benchmark results
			/// to standard output.
			bool quiet = false;

			/// Name of the module currently being benchmarked
			std::string moduleName;

			/// Default number of iterations
			unsigned int defaultIterations = CHEBYSHEV_BENCHMARK_ITER;

			/// Default number of runs
			unsigned int defaultRuns = CHEBYSHEV_BENCHMARK_RUNS;

			/// Relative or absolute path to output folder
			std::string outputFolder = "";

			/// Prefix to prepend to the filename, in addition
			/// to the module name.
			std::string filenamePrefix = "benchmark_";

			/// Suffix to append to the filename, in addition
			/// to the module name.
			std::string filenameSuffix = ".csv";

			/// Whether to output results to a file.
			bool outputToFile = true;

			/// Benchmark state.results
			std::vector<benchmark_result> results;

			/// Total number of benchmarks
			unsigned int totalBenchmarks = 0;

			/// Number of failed benchmarks
			unsigned int failedBenchmarks = 0;

			/// Target benchmarks marked for execution
			/// (all benchmarks will be executed if empty)
			std::map<std::string, bool> pickedBenchmarks {};

			/// Results of the benchmarks.
			std::map<std::string, std::vector<benchmark_result>> benchmarkResults {};

			/// Default columns to print for benchmarks.
			std::vector<std::string> benchmarkColumns = {
				"funcName", "averageRuntime", "runsPerSecond"
			};
			
		} state;


		/// Setup the benchmark environment.
		///
		/// @param moduleName Name of the module under test.
		/// @param argc The number of command line arguments
		/// @param argv A list of C-style strings containing
		/// the command line arguments.
		inline void setup(
			std::string moduleName,
			int argc = 0,
			const char** argv = nullptr) {

			// Initialize list of picked tests
			if(argc && argv)
				for (int i = 1; i < argc; ++i)
					state.pickedBenchmarks[argv[i]] = true;

			std::cout << "Starting benchmarks of the "
				<< moduleName << " module ..." << std::endl;

			state.moduleName = moduleName;
			state.failedBenchmarks = 0;

			random::setup();
			output::setup();
		}


		/// Terminate the precision testing environment.
		///
		/// @param exit Whether to exit after terminating testing.
		inline void terminate(bool exit = true) {

			output::state.quiet = state.quiet;

			if(state.outputToFile) {

				std::string filename;
				filename = state.outputFolder + state.filenamePrefix
					+ state.moduleName + state.filenameSuffix;

				output::state.outputFiles[filename] = std::ofstream(filename);

				if(!output::state.outputFiles[filename].is_open()) {
					std::cout << "Unable to open output file,"
						" results will NOT be saved!" << std::endl;
					state.outputToFile = false;
				}
			}

			std::cout << "\n";

			output::table_state benchmarkTable {};

			if(state.benchmarkResults.size())
				output::header(benchmarkTable, state.benchmarkColumns);


			for (auto it = state.benchmarkResults.begin();
				it != state.benchmarkResults.end(); ++it) {

				const auto res_list = it->second;

				for (size_t i = 0; i < res_list.size(); ++i) {

					benchmarkTable.rowIndex++;

					if(it != state.benchmarkResults.end()
					&& std::next(it) == state.benchmarkResults.end()
					&& (i == res_list.size() - 1))
						benchmarkTable.isLastRow = true;

					output::print(res_list[i], benchmarkTable, state.benchmarkColumns);
				}
			}

			std::cout << "Finished benchmarking " << state.moduleName << '\n';
			std::cout << state.totalBenchmarks << " total benchmarks, "
				<< state.failedBenchmarks << " failed (" <<
				(state.failedBenchmarks / (double) state.totalBenchmarks) * 100 << "%)"
				<< '\n';
				
			for (auto& p : output::state.outputFiles)
				std::cout << "Results have been saved in " << p.first << std::endl;

			output::terminate();
			state = benchmark_state();

			if(exit)
				std::exit(state.failedBenchmarks);
		}


		/// Run a benchmark on a generic function,
		/// with the given options.
		template<typename InputType = double, typename Function>
		inline void benchmark(
			const std::string& funcName,
			Function func,
			benchmark_options<InputType> opt) {

			// Generate input set
			std::vector<InputType> input (opt.iterations);
			for (unsigned int i = 0; i < opt.iterations; ++i)
				input[i] = opt.inputGenerator(i);

			// Sum of m runs with n iterations each
			long double totalRuntime = 0;

			// Whether the benchmark failed because of an exception
			bool failed = false;

			try {

				// Dummy variable
				__volatile__ auto c = func(input[0]);

				for (unsigned int i = 0; i < opt.runs; ++i) {

					timer t = timer();

					for (unsigned int j = 0; j < opt.iterations; ++j)
						c += func(input[j]);

					totalRuntime += t();
				}

				// Differentiate between types with operator+= or not ?
				// c = *((&c + 1) - 1);

			} catch(...) {
				failed = true;
			}

			benchmark_result res {};
			res.funcName = funcName;
			res.runs = opt.runs;
			res.iterations = opt.iterations;
			res.totalRuntime = totalRuntime;
			res.averageRuntime = totalRuntime / (opt.runs * opt.iterations);
			res.runsPerSecond = 1000.0 / res.averageRuntime;
			res.failed = failed;

			state.totalBenchmarks++;
			if(failed)
				state.failedBenchmarks++;

			state.benchmarkResults[funcName].push_back(res);
		}


		/// Run a benchmark on a generic function,
		/// with the given options.
		template<typename InputType = double, typename Function>
		inline void benchmark(
			const std::string& funcName,
			Function func,
			unsigned int runs = CHEBYSHEV_BENCHMARK_RUNS,
			unsigned int iterations = CHEBYSHEV_BENCHMARK_ITER,
			InputGenerator<InputType> inputGenerator = generator::uniform1D(0, 1)) {

			benchmark_options<InputType> opt;
			opt.runs = runs;
			opt.iterations = iterations;
			opt.inputGenerator = inputGenerator;

			benchmark(funcName, func, opt);
		}


	}

}

#endif
