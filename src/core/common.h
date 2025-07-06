
///
/// @file common.h Common definitions for the framework.
///

#ifndef CHEBYSHEV_COMMON_H
#define CHEBYSHEV_COMMON_H

#ifndef CHEBYSHEV_PREC_ITER
/// Default number of function evaluations
/// in precision testing.
#define CHEBYSHEV_PREC_ITER 1000
#endif

#ifndef CHEBYSHEV_PREC_TOLERANCE
/// Default tolerance in precision testing.
#define CHEBYSHEV_PREC_TOLERANCE 1E-08
#endif

#ifndef CHEBYSHEV_BENCHMARK_ITER
/// Default number of benchmark iterations.
#define CHEBYSHEV_BENCHMARK_ITER 1000
#endif

#ifndef CHEBYSHEV_BENCHMARK_RUNS
/// Default number of benchmark runs.
#define CHEBYSHEV_BENCHMARK_RUNS 10
#endif

#ifndef CHEBYSHEV_OUTPUT_WIDTH
/// Default width of output columns
#define CHEBYSHEV_OUTPUT_WIDTH 12
#endif


/// Floating-point type, used as default for function arguments.
using real_t = double;


/// Floating-point type of higher precision, used in computations,
/// such as error estimation.
using prec_t = long double;


#include <limits>
#include <vector>
#include "../prec/interval.h"


#define CAST_LAMBDA(func, type) [=](type x){ return func(static_cast<type>(x)); }


namespace chebyshev {


	/// An endofunction is a function which has the same type
	/// of input and output, such as a real function of real variable
	/// \f$f: \mathbb{R} \rightarrow \mathbb{R}\f$.
	template<typename Type = real_t>
	using EndoFunction = std::function<Type(Type)>;


	/// Get a quiet NaN of the specified floating point type.
	template<typename FloatType = real_t>
	inline constexpr FloatType get_nan() {
		return std::numeric_limits<FloatType>::quiet_NaN();
	}


	/// The Pi mathematical constant
	const prec_t PI_CONST = 3.141592653589793238462643L;
	
}


/// Namespace alias
#ifndef CHEBYSHEV_NO_ALIAS
namespace ch = chebyshev;
#endif

#endif
