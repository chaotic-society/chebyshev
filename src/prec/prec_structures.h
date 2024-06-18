
///
/// @file prec_structures.h Structures for precision testing.
///

#ifndef CHEBYSHEV_PREC_STRUCTURES_H
#define CHEBYSHEV_PREC_STRUCTURES_H

#include <string>
#include <vector>
#include <functional>

#include "../core/common.h"
#include "./interval.h"
#include "./distance.h"


namespace chebyshev {

	namespace prec {


		/// @class estimate_result
		/// A structure holding the result of precision estimation.
		struct estimate_result {
			
			/// Uniquely identifying name of the function.
			std::string funcName = "unknown";

			/// Interval of estimation.
			std::vector<interval> domain {};

			/// Tolerance on the max absolute error.
			long double tolerance = 0;

			/// Estimated maximum absolute error on interval.
			long double max_err = get_nan<long double>();

			/// Estimated mean error on interval.
			long double mean_err = get_nan<long double>();

			/// Estimated RMS error on interval.
			long double rms_err = get_nan<long double>();

			/// Estimated relative error on interval.
			long double rel_err = get_nan<long double>();

			/// Estimated absolute error on interval.
			long double abs_err = get_nan<long double>();

			/// Whether the test failed.
			bool failed = false;

			/// Print to standard output or not.
			bool quiet = false;

			/// Total number of iterations for integral quadrature.
			unsigned int iterations;
		};


		/// A function which determines whether an estimation failed
		using FailFunction = std::function<bool(estimate_result)>;


		/// Distance function between two elements.
		template<typename Type>
		using DistanceFunction = std::function<long double(Type, Type)>;


		/// Generic estimator function signature
		/// (Param should generally be equal to estimate_options).
		template<typename Param, typename R, typename ...Args>
		using Estimator = std::function<
		estimate_result(std::function<R(Args...)>, std::function<R(Args...)>, Param)>;


		/// @class estimate_options
		/// A structure holding the options for precision estimation.
		template<typename R, typename ...Args>
		struct estimate_options {

			/// The domain of estimation.
			std::vector<interval> domain {};

			/// The tolerance to use to determine whether the test failed.
			long double tolerance = get_nan();

			/// Number of function evaluations to use.
			unsigned int iterations = 0;

			/// The function to determine whether the test failed.
			FailFunction fail;

			/// The precision estimator to use.
			Estimator<estimate_options, R, Args...> estimator;

			/// Whether to show the test result or not.
			bool quiet = false;
			
		};


		/// @class equation_result
		/// A structure holding the result of an evaluation.
		struct equation_result {

			/// Uniquely identifying function name.
			std::string funcName = "unknown";

			/// Evaluated value.
			long double evaluated = get_nan<long double>();

			/// Expected value.
			long double expected = get_nan<long double>();
			
			/// Evaluated difference between expected and evaluated values.
			long double difference = get_nan<long double>();

			/// Tolerance on the absolute difference.
			long double tolerance = 0;

			/// Whether the test failed.
			bool failed = true;

			/// Print to standard output or not.
			bool quiet = false;
		};


		/// @class equation_options
		/// Structure holding options for equivalence evaluation.
		template<typename T>
		struct equation_options {
			
			/// Distance function to measure the distance
			/// between the expected and evaluated value.
			DistanceFunction<T> distance;

			/// Tolerance on the absolute difference.
			long double tolerance = 0;

			/// Print to standard output or not.
			bool quiet = false;

		};

	}
}

#endif
