
///
/// @file benchmark.h Benchmark module.
///

#ifndef CHEBYSHEV_BENCHMARK_H
#define CHEBYSHEV_BENCHMARK_H

#include <ctime>
#include <iostream>
#include <thread>
#include <future>

#include "./core/random.h"
#include "./benchmark/timer.h"
#include "./benchmark/generator.h"
#include "./benchmark/benchmark_structures.h"


namespace chebyshev {


/// @namespace chebyshev::benchmark Benchmark module
///
/// This module provides routines for measuring the average
/// runtime of functions of any kind over a randomized or fixed
/// vector of inputs. The benchmark::benchmark implements this
/// functionality and registers the results for analysis and output.
namespace benchmark {


	/// Measure the total runtime of a function over
	/// the given input for a single run. It is generally
	/// not needed to call this function directly,
	/// as benchmarks can be run and registered using
	/// benchmark::benchmark.
	///
	/// @param func The function to measure the runtime of
	/// @param input The vector of inputs
	/// @return The total runtime of the function over the input vector.
	template<typename InputType, typename Function>
	inline long double runtime(
		Function func, const std::vector<InputType>& input) {

		if (input.size() == 0)
			return 0.0;

		// Dummy variable
		__volatile__ auto c = func(input[0]);

		timer t = timer();

		for (unsigned int j = 0; j < input.size(); ++j)
			c += func(input[j]);

		return t();
	}


	/// @class benchmark_settings
	/// Global settings of the benchmark module, used in benchmark_context.
	struct benchmark_settings {

		/// Name of the module currently being benchmarked
		std::string moduleName;

		/// Default number of iterations
		unsigned int defaultIterations = CHEBYSHEV_BENCHMARK_ITER;

		/// Default number of runs
		unsigned int defaultRuns = CHEBYSHEV_BENCHMARK_RUNS;

		/// The files to write all benchmark results to.
		std::vector<std::string> outputFiles {};

		/// Target benchmarks marked for execution
		/// (all benchmarks will be executed if empty)
		std::map<std::string, bool> pickedBenchmarks {};

		/// The files to write benchmark results to
		/// (if empty, all results are output to a generic file).
		std::vector<std::string> benchmarkOutputFiles {};

		/// Default columns to print for benchmarks.
		std::vector<std::string> benchmarkColumns = {
			"name", "averageRuntime", "stdevRuntime", "runsPerSecond"
		};
		
	};


	/// @class benchmark_context
	/// Benchmark module context, handling benchmark requests concurrently.
	class benchmark_context {

	private:

		/// Results of the benchmarks.
		std::map <std::string, std::vector<benchmark_result>> benchmarkResults {};

		/// Mutex to lock benchmark results.
		std::mutex benchmarkMutex;

		/// Threads running benchmarks.
		std::vector<std::thread> benchmarkThreads {};

		/// Whether the context was previously terminated
		/// by calling terminate() or the destructor.
		bool wasTerminated {false};

	public:

		/// Settings for the benchmark context.
		benchmark_settings settings;

		/// Output module settings for the context, dynamically allocated
		/// and possibly shared between multiple contexts.
		std::shared_ptr<output::output_context> output;

		/// Random module settings for the context, dynamically allocated
		/// and possibly shared between multiple contexts.
		std::shared_ptr<random::random_context> random;


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

			// Initialize other modules
			settings = benchmark_settings();
			output = std::make_shared<output::output_context>();
			random = std::make_shared<random::random_context>();

			// Initialize list of picked tests
			if(argc && argv)
				for (int i = 1; i < argc; ++i)
					settings.pickedBenchmarks[argv[i]] = true;

			std::cout << "Starting benchmarks of the ";
			std::cout << moduleName << " module ..." << std::endl;

			settings.moduleName = moduleName;
			benchmarkResults.clear();
			wasTerminated = false;
		}


		/// Terminate the benchmarking environment.
		/// If benchmarks have been run, their results will be printed.
		///
		/// @param exit Whether to exit after terminating the module.
		inline void terminate(bool exit = false) {

			// Ensure that all benchmarks have been run
			this->wait_results();

			std::lock_guard<std::mutex> lock(benchmarkMutex);

			unsigned int totalBenchmarks = 0;
			unsigned int failedBenchmarks = 0;

			for (const auto& pair : benchmarkResults) {
				for (const auto& testCase : pair.second) {
					totalBenchmarks++;
					failedBenchmarks += testCase.failed ? 1 : 0;
				}
			}


			// Ensure that an output file is specified
			if(	 output->settings.outputToFile &&
				!output->settings.outputFiles.size() &&
				!settings.benchmarkOutputFiles.size() &&
				!settings.outputFiles.size()) {
				
				settings.outputFiles = { settings.moduleName + "_results" };
			}


			// Print benchmark results
			std::vector<std::string> outputFiles;
			outputFiles  = settings.outputFiles;
			outputFiles.insert(
				outputFiles.end(),
				settings.benchmarkOutputFiles.begin(),
				settings.benchmarkOutputFiles.end()
			);

			output->print_results(
				benchmarkResults,
				settings.benchmarkColumns,
				outputFiles
			);


			// Print overall test results
			std::cout << "Finished testing " << settings.moduleName << '\n';
			std::cout << totalBenchmarks << " total tests, ";
			std::cout << failedBenchmarks << " failed";

			// Print proportion of failed test, avoiding division by zero
			if (totalBenchmarks > 0) {
				std::cout << " (" << std::setprecision(3);
				std::cout << (failedBenchmarks / (double) totalBenchmarks) * 100;
				std::cout << "%)";
			}
			std::cout << std::endl;

			if(exit) {
				output->terminate();
				std::exit(failedBenchmarks);
			}

			wasTerminated = true;
		}


		/// Default constructor setting up the context.
		benchmark_context(
			std::string moduleName, int argc = 0,
			const char** argv = nullptr) {

			setup(moduleName, argc, argv);
		}


		/// Terminate the benchmark module.
		~benchmark_context() {

			if (!wasTerminated)
				terminate();
		}


		/// Custom copy constructor to avoid copying std::mutex.
		benchmark_context(const benchmark_context& other) {

			std::lock_guard<std::mutex> lock(benchmarkMutex);
			benchmarkResults = other.benchmarkResults;
			settings = other.settings;
			output = other.output;
			random = other.random;
		}


		/// Custom assignment operator to avoid copying std::mutex.
		inline benchmark_context& operator=(const benchmark_context& other) {

			std::lock_guard<std::mutex> lock(benchmarkMutex);
			benchmarkResults = other.benchmarkResults;
			settings = other.settings;
			output = other.output;
			random = other.random;

			return *this;
		}


		/// Run a benchmark on a generic function, with the given input vector.
		/// The result is registered inside benchmarkResults.
		///
		/// @param name The name of the test case
		/// @param func The function to benchmark
		/// @param input The vector of input values
		/// (InputType must correspond to the argument of func, but may be any POD
		/// or aggregate data type, such as std::tuple).
		/// @param runs The number of runs with the same input
		/// (defaults to settings.defaultRuns).
		template<typename InputType = double, typename Function>
		inline void benchmark(
			const std::string& name,
			Function func,
			const std::vector<InputType>& input,
			unsigned int runs = 0,
			bool quiet = false) {

			if (runs == 0)
				runs = settings.defaultRuns;

			// Package task for multi-threaded execution
			benchmarkThreads.emplace_back([this, name, func, input, runs, quiet]() {

				// Whether the benchmark failed because of an exception
				bool failed = false;

				// Running average
				long double averageRuntime;

				// Running total sum of squares
				long double sumSquares;

				// Total runtime
				long double totalRuntime;

				try {

					// Use Welford's algorithm to compute
					// the average and the variance
					totalRuntime = runtime(func, input);
					averageRuntime = totalRuntime / input.size();
					sumSquares = 0.0;

					for (unsigned int i = 1; i < runs; ++i) {
						
						// Compute the runtime for a single run
						// and update the running estimates
						const long double currentRun = runtime(func, input);
						const long double currentAverage = currentRun / input.size();
						totalRuntime += currentRun;

						const long double tmp = averageRuntime;
						averageRuntime = tmp + (currentAverage - tmp) / (i + 1);
						sumSquares += (currentAverage - tmp)
							* (currentAverage - averageRuntime);
					}

				} catch(...) {

					// Catch any exception and mark the benchmark as failed
					failed = true;
				}

				benchmark_result res {};
				res.name = name;
				res.runs = runs;
				res.iterations = input.size();
				res.totalRuntime = totalRuntime;
				res.averageRuntime = averageRuntime;
				res.runsPerSecond = 1000.0 / res.averageRuntime;
				res.failed = failed;
				res.quiet = quiet;

				if (runs > 1)
					res.stdevRuntime = std::sqrt(sumSquares / (runs - 1));

				std::lock_guard<std::mutex> lock(benchmarkMutex);
				benchmarkResults[name].push_back(res);
			});
		}


		/// Run a benchmark on a generic function, with the given options.
		/// The result is registered inside benchmarkResults.
		///
		/// @param name The name of the test case
		/// @param func The function to benchmark
		/// @param opt The benchmark options
		template <typename InputType = double, typename Function>
		inline void benchmark(
			const std::string& name,
			Function func,
			const benchmark_options<InputType>& opt) {


			// Generate input set
			random::random_source rnd = random->get_rnd();
			std::vector<InputType> input (opt.iterations);

			for (unsigned int i = 0; i < opt.iterations; ++i)
				input[i] = opt.inputGenerator(rnd);

			// Benchmark over input set
			benchmark(name, func, input, opt.runs, opt.quiet);
		}


		/// Run a benchmark on a generic function, with the given argument options.
		/// The result is registered inside benchmarkResults.
		///
		/// @param name The name of the test case
		/// @param func The function to benchmark
		/// @param run The number of runs with the same input
		/// @param iterations The number of iterations of the function
		/// @param inputGenerator The input generator to use
		template<typename InputType = double, typename Function>
		inline void benchmark(
			const std::string& name,
			Function func,
			InputGenerator<InputType> inputGenerator,
			unsigned int runs = 0,
			unsigned int iterations = 0,
			bool quiet = false) {

			if (runs == 0)
				runs = settings.defaultRuns;

			if (iterations == 0)
				iterations = settings.defaultIterations;

			benchmark_options<InputType> opt;
			opt.runs = runs;
			opt.iterations = iterations;
			opt.inputGenerator = inputGenerator;
			opt.quiet = quiet;

			benchmark(name, func, opt);
		}


		/// Wait for all concurrent benchmarks to finish execution.
		inline void wait_results() {

			for (auto& t : benchmarkThreads)
				if (t.joinable())
					t.join();

			benchmarkThreads.clear();
		}


		/// Get a list of benchmarks results associated
		/// to the given name or label.
		inline std::vector<benchmark_result> get_benchmark(const std::string& name) {

			this->wait_results();
			return benchmarkResults[name];
		}


		/// Get a benchmark result associated to the given
		/// name or label and index.
		inline benchmark_result get_benchmark(const std::string& name, unsigned int i) {
			
			this->wait_results();
			return benchmarkResults[name].at(i);
		}

	};


	/// Construct a benchmarking context with the given parameters.
	///
	/// @param moduleName Name of the module under test.
	/// @param argc The number of command line arguments.
	/// @param argv An array of command line arguments as C-like strings.
	benchmark_context make_context(const std::string& moduleName,
			int argc = 0, const char** argv = nullptr) {

		return benchmark_context(moduleName, argc, argv);
	}

}}

#endif
