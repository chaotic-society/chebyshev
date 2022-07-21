
///
/// @file precision.h Precision estimation of real functions
///

#pragma once

#include "./interval.h"
#include "./definitions.h"

#include <iostream>
#include <vector>
#include <array>
#include <fstream>

#include <cmath>
#include <ctime>


#ifndef CHEBYSHEV_INTEGRAL_ITER
#define CHEBYSHEV_INTEGRAL_ITER 1000
#endif


#ifndef CHEBYSHEV_TOLERANCE
#define CHEBYSHEV_TOLERANCE 0.00000001
#endif


namespace chebyshev {

	/// @namespace chebyshev::prec Precision testing functions
	namespace prec {


		/// Write results to standard output?
		bool quiet = false;

		/// Write results to file?
		bool output_to_file = true;

		/// Write only failed tests?
		bool write_only_failed = false;

		/// Module name
		std::string module_name = "unknown";

		/// Output file stream
		std::ofstream output_file;

		/// Total number of test runs
		unsigned int total_tests = 0;

		/// Number of failed tests
		unsigned int failed_tests = 0;

		/// @class prec_estimates
		/// A collection of the estimates of
		/// an approximation's error
		struct prec_estimates {

			// Name of the tested function
			std::string f_name;

			// Interval on which the function was tested
			interval k;

			// Times the function was evaluated
			unsigned int times_evaluated;

			// Estimated mean error on interval
			long double mean_err;

			// Estimated RMS error on interval
			long double rms_err;

			// Estimated maximum error on interval
			long double max_err;

			// Estimated absolute error on interval
			long double abs_err;

			// Estimated relative error on interval
			long double rel_err;

			// Did the test fail?
			bool failed;

			// Write the estimate to standard output?
			bool quiet;

			prec_estimates() = default;
		};


		/// @class equality_check
		/// The results and information of an equality check
		/// with a given tolerance
		struct equality_check {

			// Name of the tested function
			std::string f_name;

			// Expected value
			Real expected;

			// Evaluated value
			Real evaluated;

			// Absolute difference
			Real diff;

			// Tolerance
			Real tolerance;

			// Did the test fail?
			bool failed;

			// Write the check to standard output?
			bool quiet;

			equality_check() = default;
		};


		/// Estimates of the whole module
		std::vector<prec_estimates> module_estimates;

		/// Equality checks of the whole module
		std::vector<equality_check> module_eq_checks;


		void print_estimate(prec_estimates p) {

			if(write_only_failed && !p.failed)
				return;

			if(!quiet) {
				std::cout << p.f_name << "\t\t" << p.k.a << "\t\t" << p.k.b << "\t\t" <<
					p.mean_err << "\t\t" << p.rms_err << "\t\t" << p.max_err << "\t\t" << p.rel_err << std::endl;
			}

			if(output_to_file) {
				output_file << p.f_name << ", " << p.k.a << ", " << p.k.b << ", " <<
					p.mean_err << ", " << p.rms_err << ", " << p.max_err << ", " << p.rel_err << std::endl;
			}
		}


		void print_check(equality_check ec) {

			if(write_only_failed && !ec.failed)
				return;

			if(!quiet) {
				std::cout << ec.f_name << "\t\t" << ec.evaluated << "\t\t" << ec.expected
				<< "\t\t" << ec.diff << "\t\t" << ec.tolerance << std::endl;
			}

			if(output_to_file) {
				output_file << ec.f_name << ", " << ec.evaluated << ", " << ec.expected
				<< ", " << ec.diff << ", " << ec.tolerance << std::endl;
			}

		}


		/// Setup the testing environment
		void setup(std::string module = "unknown") {

			module_name = module;

			if(output_to_file) {
				std::string filename = "prec_";
				filename += module_name;
				filename += ".csv";
				output_file.open(filename);

				if(!output_file.is_open()) {
					std::cout << "Can't open output file, results won't be saved." << std::endl;
					output_to_file = false;
				}

			}

			std::cout << "Starting precision testing of " << module_name << " ...\n" << std::endl;
		}


		/// Terminate precision testing
		void terminate() {

			if(module_estimates.size()) {

				if(!quiet)
					std::cout << "Function\tInt. Min.\tInt. Max.\t" <<
							 "Mean Err.\tRMS Err.\tMax Err.\tRel. Err.\n" << std::endl;
				if(output_to_file)
					output_file << "Function, Int. Min., Int. Max., Mean Err., RMS Err., Max Err., Rel. Err." << std::endl;

				for (const auto& p : module_estimates) {
					if(!p.quiet)
						print_estimate(p);
				}

				std::cout << "\n\n";
				output_file << "\n\n";

			}

			if(module_eq_checks.size()) {
				if(!quiet)
				std::cout << "Function\tEval. Value\tExp. Value\tDiff.\t\tTol.\n" << std::endl;
				if(output_to_file)
					output_file << "Function, Eval. Value, Exp. Value, Diff., Tol." << std::endl;

				for (const auto& ec : module_eq_checks) {
					if(!ec.quiet)
						print_check(ec);
				}

				std::cout << std::endl;
			}

			std::cout << "\nFinished testing " << module_name << std::endl;
			std::cout << total_tests << " total tests, " << failed_tests << " failed (" <<
				(failed_tests / (double) total_tests) * 100 << "%)" << std::endl;
			std::cout << "Results have been saved in prec_" << module_name << ".csv" << std::endl;
			output_file.close();

			exit(failed_tests);
		}


		/// Estimate the precision of a real function on a single interval
		prec_estimates estimate(
			std::string name,
			RealFunction f_approx,
			RealFunction f_exp,
			interval k,
			bool quiet = false,
			Real tolerance = CHEBYSHEV_TOLERANCE,
			unsigned int n = CHEBYSHEV_INTEGRAL_ITER) {

			prec_estimates result;

			Real sum = 0;
			Real sum_sqr = 0;
			Real sum_abs = 0;
			Real max = 0;

			Real measure = std::abs(k.b - k.a);
			Real dx = measure / n;
			Real x, coeff;

			Real diff = std::abs(f_approx(k.a) - f_exp(k.a));

			sum += diff;
			sum_sqr += diff * diff;
			sum_abs += std::abs(f_exp(k.a));
			max = diff;

			for (unsigned int i = 1; i < n; ++i) {

				x = k.a + i * dx;
				diff = std::abs(f_approx(x) - f_exp(x));

				if(diff > max)
					max = diff;

				if(i % 2 == 0)
					coeff = 2;
				else
					coeff = 4;

				sum += coeff * diff;
				sum_sqr += coeff * diff * diff;
				sum_abs += coeff * f_exp(x);
			}

			diff = std::abs(f_approx(k.b) - f_exp(k.b));

			sum += diff;
			sum_sqr += diff * diff;
			sum_abs += std::abs(f_exp(k.b));
			
			if(diff > max)
				max = diff;

			result.f_name = name;
			result.quiet = quiet;
			result.k = k;
			result.times_evaluated = n;
			result.abs_err = sum;
			result.max_err = max;
			result.mean_err = (sum * dx / 3.0) / measure;
			result.rms_err = std::sqrt((sum_sqr * dx / 3.0) / measure);
			result.rel_err = (sum * dx / 3.0) / (sum_abs * dx / 3.0);

			if(result.max_err >= tolerance) {
				failed_tests++;
				result.failed = true;
			} else {
				result.failed = false;
			}

			module_estimates.push_back(result);
			total_tests++;

			return result;
		}


		/// Estimate the precision of a real function
		/// on the given intervals
		std::vector<prec_estimates> estimate(
			std::string name,
			RealFunction f_approx,
			RealFunction f_exp,
			std::vector<interval> requested_intervals,
			bool quiet = false,
			Real tolerance = CHEBYSHEV_TOLERANCE,
			unsigned int n = CHEBYSHEV_INTEGRAL_ITER) {

			std::vector<prec_estimates> results;
			results.resize(requested_intervals.size());

			for (const auto& i : requested_intervals) {
				
				// Estimate error on interval
				results.push_back(estimate(name, f_approx, f_exp, i, quiet, tolerance));
			}

			return results;
		}


		/// Test whether two integer values are equal
		template<typename IntegerType>
		inline void equals_int(
			const std::string& name,
			IntegerType evaluated, IntegerType expected, bool quiet = false) {

			equality_check ec;
			Real diff = std::abs(expected - evaluated);

			if(diff != 0) {
				failed_tests++;
				ec.failed = true;
			} else {
				ec.failed = false;
			}

			total_tests++;
			
			ec.f_name = name;
			ec.diff = diff;
			ec.expected = expected;
			ec.evaluated = evaluated;
			ec.tolerance = 0;
			ec.quiet = quiet;

			module_eq_checks.push_back(ec);
		}


		/// Test whether two real values are almost equal, to the given tolerance
		inline void equals(
			const std::string& name,
			Real evaluated, Real expected,
			Real tolerance = CHEBYSHEV_TOLERANCE, bool quiet = false) {

			equality_check ec;
			Real diff = std::abs(expected - evaluated);

			if(diff > tolerance) {
				failed_tests++;
				ec.failed = true;
			} else {
			 	ec.failed = false;
			}

			total_tests++;

			ec.f_name = name;
			ec.diff = diff;
			ec.expected = expected;
			ec.evaluated = evaluated;
			ec.tolerance = tolerance;
			ec.quiet = quiet;

			module_eq_checks.push_back(ec);
		}


		/// Test whether two real values are almost equal, to the given tolerance
		inline void equals(
			const std::string& name,
			std::vector<std::array<Real, 2>> values,
			Real tolerance = CHEBYSHEV_TOLERANCE) {

			for (const auto& v : values)
				equals(name, v[0], v[1], tolerance);
		}


		/// Test whether two real values are almost equal, to the given tolerance,
		/// by evaluating a function at the given values
		inline void equals(
			const std::string& name,
			RealFunction f,
			std::vector<std::array<Real, 2>> values,
			Real tolerance = CHEBYSHEV_TOLERANCE) {

			for (const auto& v : values)
				equals(name, f(v[0]), v[1], tolerance);
		}

	}
	
}
