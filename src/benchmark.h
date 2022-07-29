
///
/// @file benchmark.h Function benchmarking
///

#pragma once

/// Default number of iterations
#ifndef CHEBYSHEV_ITER
#define CHEBYSHEV_ITER 1000000
#endif

/// Default number of runs for benchmarks
#ifndef CHEBYSHEV_RUNS
#define CHEBYSHEV_RUNS 10
#endif

#include "timer.h"
#include <iostream>
#include <fstream>


namespace chebyshev {

	/// @namespace chebyshev::benchmark Function benchmarking procedures
	namespace benchmark {


		/// Benchmark run request, used to store
		/// information about requested benchmarks
		/// for later execution
		struct benchmark_request {
			std::string funcName {"unknown"};
			RealFunction func {nullptr};
			RealInputGenerator gen {nullptr};
			unsigned int iter {CHEBYSHEV_ITER};
			unsigned int runs {CHEBYSHEV_RUNS};
		};


		/// Benchmark result, used to store
		/// information about a benchmark run
		struct benchmark_result {
			std::string funcName {"unknown"};
			unsigned int iter {CHEBYSHEV_ITER};
			unsigned int runs {CHEBYSHEV_RUNS};
			Real total_time {0};
			Real avg_time {0};
			Real runs_per_sec {0};
		};


		/// List of requested benchmark runs.
		/// Benchmarks are run in random order
		/// when the benchmark::run() function is called
		std::vector<benchmark_request> requests;

		/// Print to standard output?
		bool quiet = false;

		/// Name of the module currently being benchmarked
		std::string module_name;

		/// Default number of iterations
		unsigned int N = CHEBYSHEV_ITER;

		/// Default number of runs
		unsigned int M = CHEBYSHEV_RUNS;

		/// Output file for the current module
		std::ofstream output_file;

		/// Benchmark results
		std::vector<benchmark_result> results;

		/// Number of failed benchmarks
		unsigned int failed_benchmarks = 0;


		/// Setup a module's benchmark
		void setup(const std::string& module = "unknown",
			unsigned int iter = CHEBYSHEV_ITER,
			unsigned int runs = CHEBYSHEV_RUNS) {

			module_name = module;
			N = iter;
			M = runs;
			srand(time(nullptr));

			if(output_file.is_open())
				output_file.close();

			output_file.open(std::string("benchmark_") + module_name + ".csv");

			if(!output_file.is_open()) {
				std::cout << "Can't open output file" << std::endl;
				exit(1);
			}

			std::cout.precision(8);

			if(!quiet) {
				std::cout << "Starting benchmark of " << module_name << std::endl;
				std::cout << "Parameters: N = " << N << ", M = " << M << "\n" << std::endl;

				std::cout << std::left << "Function\t\tTime (ms)\tRuns/sec\n" << std::endl;
				output_file << "Function, Time(ms), Runs/sec" << std::endl;
			}

		}


		/// Register a function to be benchmarked
		void register_function(const std::string& f_name,
			RealFunction f, RealInputGenerator g,
			unsigned int n = N, unsigned int m = M) {

			benchmark_request r;
			r.funcName = f_name;
			r.func = f;
			r.gen = g;
			r.iter = n;
			r.runs = m;

			requests.push_back(r);
		}


		/// Benchmark a function
		benchmark_result benchmark(const std::string& f_name, RealFunction f, RealInputGenerator g,
			unsigned int n = N, unsigned int m = M) {

			// Dummy variable
			__volatile__ Real c = 0;

			std::vector<Real> input;
			input.resize(n);

			for (unsigned int i = 0; i < n; ++i)
				input[i] = g(i);

			// Sum of M runs with N iterations each
			long double sum = 0;

			for (unsigned int i = 0; i < m; ++i) {

				timer t = timer();

				for (unsigned int j = 0; j < n; ++j)
					c += f(input[j]);

				long double elapsed = t();
				sum += elapsed / (long double) n;
			}

			benchmark_result br;
			br.funcName = f_name;
			br.iter = n;
			br.runs = m;
			br.total_time = sum * n;
			br.avg_time = sum / (long double) m;
			br.runs_per_sec = 1.0 / (sum * 0.001 / (long double) m);

			return br;
		}


		/// Benchmark a function
		benchmark_result benchmark(
			const std::string& f_name, RealFunction f,
			const std::vector<Real> input,
			unsigned int n = N, unsigned int m = M) {

			if(input.size() < n) {
				std::cout << "Wrong input size in benchmark, skipping request ..." << std::endl;
				failed_benchmarks++;
				return benchmark_result();
			}

			// Dummy variable
			__volatile__ Real c = 0;

			// Sum of M runs with N iterations each
			long double sum = 0;

			for (unsigned int i = 0; i < m; ++i) {

				timer t = timer();

				for (unsigned int j = 0; j < n; ++j)
					c += f(input[j]);

				long double elapsed = t();
				sum += elapsed / (long double) n;
			}

			benchmark_result br;
			br.funcName = f_name;
			br.iter = n;
			br.runs = m;
			br.total_time = sum * n;
			br.avg_time = sum / (long double) m;
			br.runs_per_sec = 1.0 / (sum * 0.001 / (long double) m);

			return br;
		}


		/// Benchmark a function
		benchmark_result benchmark(benchmark_request r) {
			return benchmark(r.funcName, r.func, r.gen, r.iter, r.runs);
		}


		/// Print the result of a benchmark
		void print_benchmark_result(benchmark_result br) {

			std::cout << br.funcName << "\t\t" << br.avg_time << "\t" << br.runs_per_sec << std::endl;

			output_file << br.funcName << ", "
						<< br.avg_time << ", "
						<< br.runs_per_sec << std::endl;
		}


		/// Run all registered benchmarks
		void run() {
			for (const auto& r : requests) {
				benchmark_result br = benchmark(r);
				results.push_back(br);
				print_benchmark_result(br);
			}
		}


		/// End benchmarking of the current module
		void terminate(bool exit = true) {

			std::cout << "\nFinished benchmark of " << module_name << std::endl;
			if(failed_benchmarks)
				std::cout << failed_benchmarks << " benchmarks failed!" << std::endl;

			std::string filename = "benchmark_";
			filename += module_name;
			filename += ".csv";
			std::cout << "Results have been saved in " << filename << std::endl;

			module_name = "unknown";
			output_file.close();

			if(exit)
				std::exit(failed_benchmarks);
		}

	}

}
