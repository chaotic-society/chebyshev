
///
/// @file common.h Common definitions for the framework.
///

#ifndef CHEBYSHEV_COMMON_H
#define CHEBYSHEV_COMMON_H

#ifndef CHEBYSHEV_PREC_ITER
/// Default number of function evaluations
/// in precision testing.
#define CHEBYSHEV_PREC_ITER 1E+06
#endif

#ifndef CHEBYSHEV_PREC_TOLERANCE
/// Default tolerance in precision testing.
#define CHEBYSHEV_PREC_TOLERANCE 1E-08
#endif

#ifndef CHEBYSHEV_BENCHMARK_ITER
/// Default number of benchmark iterations.
#define CHEBYSHEV_BENCHMARK_ITER 100
#endif

#ifndef CHEBYSHEV_BENCHMARK_RUNS
/// Default number of benchmark runs.
#define CHEBYSHEV_BENCHMARK_RUNS 10
#endif


#include <limits>


namespace chebyshev {


	/// A real function of real variable.
	template<typename FloatType = double>
	using RealFunction = std::function<FloatType(FloatType)>;


	/// Get a quiet NaN of the specified floating point type.
	template<typename FloatType = long double>
	inline constexpr FloatType get_nan() {
		return std::numeric_limits<FloatType>::quiet_NaN();
	}
	
}

#endif
