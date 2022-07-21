
///
/// @file precision.h Precision estimation of real functions
///

#pragma once

#include "./interval.h"
#include "./definitions.h"

#include <iostream>
#include <vector>
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

			prec_estimates() = default;
		};


		void print_estimate(prec_estimates p) {

			std::cout << p.f_name << "\t\t" << p.k.a << "\t\t" << p.k.b << "\t\t" <<
				p.mean_err << "\t\t" << p.rms_err << "\t\t" << p.max_err << "\t\t" << p.rel_err << std::endl;

			output_file << p.f_name << ", " << p.k.a << ", " << p.k.b << ", " <<
				p.mean_err << ", " << p.rms_err << ", " << p.max_err << ", " << p.rel_err << std::endl;
		}


		/// Setup the testing environment
		void setup(std::string module = "unknown") {

			module_name = module;

			if(output_to_file) {
				std::string filename = "prec_";
				filename += module_name;
				filename += ".csv";
				output_file.open(filename);
			}

		}


		/// Print precision testing header
		void header() {
			std::cout << "Starting precision testing of " << module_name << "\n" << std::endl;

			std::cout << "Function\tInt. Min.\tInt. Max.\t" <<
						 "Mean Err.\tRMS Err.\tMax Err.\tRel. Err.\n" << std::endl;

			output_file << "Function, Int. Min., Int. Max., Mean Err., RMS Err., Max Err., Rel. Err." << std::endl;
		}


		/// Terminate precision testing
		void terminate() {

			std::cout << "\nFinished testing " << module_name << std::endl;
			std::cout << total_tests << " total tests, " << failed_tests << " failed (" <<
				(failed_tests / (double) total_tests) * 100 << "%)" << std::endl;
			std::cout << "Results have been saved in prec_" << module_name << ".csv" << std::endl;
			output_file.close();
		}


		/// Estimate the precision of a real function on a single interval
		prec_estimates estimate(
			std::string name,
			RealFunction f_approx,
			RealFunction f_exp,
			interval k,
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

			print_estimate(result);
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
			Real tolerance = CHEBYSHEV_TOLERANCE,
			unsigned int n = CHEBYSHEV_INTEGRAL_ITER) {

			std::vector<prec_estimates> results;
			results.resize(requested_intervals.size());

			for (const auto& i : requested_intervals) {
				
				// Estimate error on interval
				results.push_back(estimate(name, f_approx, f_exp, i, tolerance));
			}

			return results;
		}

	}
	
}
