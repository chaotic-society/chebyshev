
///
/// @file prec.h Precision testing module.
///

#ifndef CHEBYSHEV_PREC_H
#define CHEBYSHEV_PREC_H

#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <iostream>

#include "prec/prec_structures.h"
#include "prec/fail.h"
#include "prec/estimator.h"
#include "core/output.h"


namespace chebyshev {

	/// @namespace chebyshev::prec Precision testing module.
	namespace prec {


		/// Global state of the precision testing module.
		struct prec_state {
			
			/// Name of the module being tested
			std::string moduleName = "unknown";
			
			/// Print to standard output or not
			bool quiet = false;

			/// Write to standard output only failed/not passed estimates?
			bool estimateOnlyFailed = false;

			/// Write to standard output only failed/not passed equations?
			bool equalsOnlyFailed = false;

			/// Output to file?
			bool outputToFile = true;

			/// Output file
			std::ofstream outputFile;

			/// Relative or absolute path to output folder
			std::string outputFolder = "";

			/// Prefix to prepend to the filename, in addition
			/// to the module name.
			std::string filenamePrefix = "prec_";

			/// Total number of tests run
			uint32_t totalTests = 0;

			/// Number of failed tests
			uint32_t failedTests = 0;

			/// Default number of iterations for integral quadrature
			uint32_t defaultIterations = CHEBYSHEV_PREC_ITER;

			/// Default fail function
			FailFunction defaultFailFunction = fail::fail_on_max_err;

			/// Default tolerance on max absolute error
			long double defaultTolerance = CHEBYSHEV_PREC_TOLERANCE;

			/// Results of precision testing
			std::map<std::string, std::vector<estimate_result>> estimateResults {};
			
			/// Results of equations
			std::map<std::string, std::vector<equation_result>> equationResults {};

			/// Target tests marked for execution,
			/// can be picked by passing test case names
			/// by command line. (all tests will be executed if empty)
			std::map<std::string, bool> pickedTests {};

		} state;


		/// Setup the precision testing environment.
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
					state.pickedTests[argv[i]] = true;

			std::cout << "Starting precision testing of the "
				<< moduleName << " module ..." << std::endl;

			state.moduleName = moduleName;
			state.failedTests = 0;
			state.totalTests = 0;

			if(state.outputToFile) {

				std::string filename;
				filename = state.outputFolder + "prec_" + moduleName + ".csv";

				if(state.outputFile.is_open())
					state.outputFile.close();

				state.outputFile.open(filename);

				if(!state.outputFile.is_open()) {
					std::cout << "Unable to open output file,"
						" results will NOT be saved!" << std::endl;
					state.outputToFile = false;
				}
			}

			output::setup();
		}


		/// Terminate the precision testing environment.
		///
		/// @param exit Whether to exit after terminating testing.
		inline void terminate(bool exit = true) {


			if(state.estimateResults.size()) {

				// Print header for estimates
				if(!state.quiet) {
					std::cout << "\n";
					output::header_estimate();
				}

				// Print to file as CSV
				if(state.outputToFile)
					output::header_estimate(state.outputFile, ",", false);
			}


			// Print estimate results
			for (const auto& p : state.estimateResults) {

				const auto res_list = p.second;

				for (size_t i = 0; i < res_list.size(); ++i) {

					if(!state.quiet)
						output::print_estimate(res_list[i]);
				
					if(state.outputToFile)
						output::print_estimate(
							res_list[i], state.outputFile, ",", false);
				}
			}

			if(state.equationResults.size()) {

				if(!state.quiet) {
					std::cout << "\n";
					output::header_equation();
				}

				if(state.outputToFile)
					output::header_equation(state.outputFile, ",", false);
			}


			// Print equation results
			for (const auto& p : state.equationResults) {

				const auto res_list = p.second;

				for (size_t i = 0; i < res_list.size(); ++i) {

					if(!state.quiet)
						output::print_equation(res_list[i]);
				
					if(state.outputToFile)
						output::print_equation(
							res_list[i], state.outputFile, ",", false);
				}
			}

			std::cout << "\nFinished testing " << state.moduleName << '\n';
			std::cout << state.totalTests << " total tests, "
				<< state.failedTests << " failed (" <<
				(state.failedTests / (double) state.totalTests) * 100 << "%)"
				<< '\n';
				
			std::cout << "Results have been saved in "
				<< state.outputFolder << state.filenamePrefix
				<< state.moduleName << ".csv" << std::endl;

			if(state.outputFile.is_open())
				state.outputFile.close();

			state = prec_state();

			if(exit)
				std::exit(state.failedTests);
		}


		/// Estimate error integrals over a function
		/// with respect to an exact function,
		/// with the given options.
		///
		/// @param name The name of the test case
		/// @param f_approx The approximation to test
		/// @param f_expected The expected result
		/// @param opt The options for the estimation
		template<typename R, typename ...Args>
		inline void estimate(
			const std::string& name,
			std::function<R(Args...)> f_approx,
			std::function<R(Args...)> f_expected,
			estimate_options<R, Args...> opt) {

			// Skip the test case if any tests have been picked
			// and this one was not picked.
			if(state.pickedTests.size())
				if(state.pickedTests.find(name) == state.pickedTests.end())
					return;

			// Use the estimator to estimate error integrals.
			auto res = opt.estimator(f_approx, f_expected, opt);

			res.funcName = name;
			res.domain = opt.domain;
			res.tolerance = opt.tolerance;
			res.quiet = opt.quiet;

			// Use the fail function to determine whether the test failed.
			res.failed = opt.fail(res);

			state.totalTests++;
			if(res.failed)
				state.failedTests++;

			state.estimateResults[name].push_back(res);
		}


		/// Estimate error integrals over a function
		/// with respect to an exact function.
		///
		/// @param name The name of the test case.
		/// @param f_approx The approximation to test.
		/// @param f_expected The expected result.
		/// @param intervals The (potentially multidimensional)
		/// domain of estimation.
		/// @param iterations The number of function evaluations.
		/// @param fail The fail function to determine whether
		/// the test failed.
		/// @param estimator The precision estimator to use.
		/// @param quiet Whether to output the result.
		template<typename R, typename ...Args>
		inline void estimate(
			const std::string& name,
			std::function<R(Args...)> f_approx,
			std::function<R(Args...)> f_expected,
			std::vector<interval> domain,
			long double tolerance, unsigned int iterations,
			FailFunction fail,
			Estimator<estimate_options<R, Args...>, R, Args...> estimator,
			bool quiet = false) {

			// Skip the test case if any tests have been picked
			// and this one was not picked.
			if(state.pickedTests.size())
				if(state.pickedTests.find(name) == state.pickedTests.end())
					return;

			estimate_options<R, Args...> opt {};
			opt.domain = domain;
			opt.tolerance = tolerance;
			opt.iterations = iterations;
			opt.estimator = estimator;
			opt.fail = fail;
			opt.quiet = quiet;

			// Use the estimator to estimate error integrals.
			auto res = opt.estimator(f_approx, f_expected, opt);

			res.funcName = name;
			res.domain = domain;
			res.tolerance = tolerance;
			res.iterations = iterations;

			// Use the fail function to determine whether the test failed.
			res.failed = opt.fail(res);
			res.quiet = quiet;

			state.totalTests++;
			if(res.failed)
				state.failedTests++;

			state.estimateResults[name].push_back(res);
		}


		/// Estimate error integrals over a function
		/// with respect to an exact function.
		///
		/// @param name The name of the test case.
		/// @param f_approx The approximation to test.
		/// @param f_expected The expected result.
		/// @param intervals The (potentially multidimensional)
		/// domain of estimation.
		/// @param iterations The number of function evaluations.
		/// @param fail The fail function to determine whether
		/// the test failed (defaults to fail_on_max_err).
		/// @param estimator The precision estimator to use
		/// (defaults to the trapezoid<double> estimator).
		/// @param quiet Whether to output the result.
		inline void estimate(
			const std::string& name,
			RealFunction<double> f_approx,
			RealFunction<double> f_expected,
			interval domain,
			long double tolerance = CHEBYSHEV_PREC_TOLERANCE,
			unsigned int iterations = CHEBYSHEV_PREC_ITER,
			FailFunction fail = fail::fail_on_max_err,
			Estimator<estimate_options<double, double>, double, double>
			estimator = estimator::trapezoid<double>,
			bool quiet = false) {

			estimate_options<double, double> opt {};
			opt.quiet = quiet;
			opt.estimator = estimator;
			opt.fail = fail;
			opt.domain.push_back(domain);
			opt.iterations = iterations;
			opt.tolerance = tolerance;

			estimate_result res = estimator(f_approx, f_expected, opt);

			// Use the fail function to determine whether the test failed.
			res.failed = fail(res);

			res.funcName = name;
			res.quiet = quiet;
			res.domain = { domain };
			res.tolerance = tolerance;
			res.iterations = iterations;

			state.totalTests++;
			if(res.failed)
				state.failedTests++;

			state.estimateResults[name].push_back(res);
		}


		/// Test an equivalence up to a tolerance,
		/// with the given options.
		///
		/// @param name The name of the test case
		/// @param evaluate The evaluated value
		/// @param expected The expected value
		/// @param opt The options for the evaluation
		template<typename T = double>
		inline void equals(
			const std::string& name,
			const T& evaluated, const T& expected,
			equation_options<T> opt) {

			// Skip the test case if any tests have been picked
			// and this one was not picked.
			if(state.pickedTests.size())
				if(state.pickedTests.find(name) == state.pickedTests.end())
					return;

			equation_result res {};

			long double diff = opt.distance(evaluated, expected);

			// Mark the test as failed if the
			// distance between the two values
			// is bigger than the tolerance.
			res.failed = (diff > opt.tolerance);

			res.funcName = name;
			res.difference = diff;
			res.tolerance = opt.tolerance;
			res.quiet = opt.quiet;

			state.totalTests++;
			if(res.failed)
				state.failedTests++;

			// Register the result of the equation by name
			state.equationResults[name].push_back(res);
		}


		/// Test an equivalence up to a tolerance,
		/// with the given options.
		///
		/// @param name The name of the test case
		/// @param evaluate The evaluated value
		/// @param expected The expected value
		/// @param distance The distance function to use
		/// @param tolerance The tolerance for the evaluation
		/// @param quiet Whether to output the result
		template<typename T = double>
		inline void equals(
			const std::string& name,
			const T& evaluated, const T& expected,
			long double tolerance,
			DistanceFunction<T> distance,
			bool quiet = false) {

			// Skip the test case if any tests have been picked
			// and this one was not picked.
			if(state.pickedTests.size())
				if(state.pickedTests.find(name) == state.pickedTests.end())
					return;

			equation_result res {};

			long double diff = distance(evaluated, expected);

			// Mark the test as failed if the
			// distance between the two values
			// is bigger than the tolerance.
			res.failed = (diff > tolerance);

			res.funcName = name;
			res.difference = diff;
			res.tolerance = tolerance;
			res.quiet = quiet;

			state.totalTests++;
			if(res.failed)
				state.failedTests++;

			// Register the result of the equation by name
			state.equationResults[name].push_back(res);
		}


		/// Test an equivalence up to a tolerance,
		/// with the given options.
		///
		/// @param name The name of the test case
		/// @param evaluate The evaluated value
		/// @param expected The expected value
		/// @param tolerance The tolerance for the evaluation
		/// @param quiet Whether to output the result
		inline void equals(
			const std::string& name,
			long double evaluated, long double expected,
			long double tolerance = state.defaultTolerance,
			bool quiet = false) {

			// Skip the test case if any tests have been picked
			// and this one was not picked.
			if(state.pickedTests.size())
				if(state.pickedTests.find(name) == state.pickedTests.end())
					return;

			equation_result res {};

			long double diff = distance::abs_distance(evaluated, expected);

			// Mark the test as failed if the
			// distance between the two values
			// is bigger than the tolerance.
			res.failed = (diff > tolerance);

			res.funcName = name;
			res.difference = diff;
			res.tolerance = tolerance;
			res.quiet = quiet;

			res.evaluated = evaluated;
			res.expected = expected;

			state.totalTests++;
			if(res.failed)
				state.failedTests++;

			// Register the result of the equation by name
			state.equationResults[name].push_back(res);
		}


		/// Evaluate multiple pairs of values for equivalence
		/// up to the given tolerance.
		inline void equals(
			const std::string& name,
			std::vector<std::array<long double, 2>> values,
			long double tolerance = state.defaultTolerance,
			bool quiet = false) {

			// Skip the test case if any tests have been picked
			// and this one was not picked.
			if(state.pickedTests.size())
				if(state.pickedTests.find(name) == state.pickedTests.end())
					return;

			for (const auto& v : values)
				equals(name, v[0], v[1], tolerance, quiet);
		}


	}
}

#endif
