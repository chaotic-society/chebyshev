
///
/// @file definitions.h General definitions
///

#pragma once

#include <functional>


namespace chebyshev {

	/// Real number type
#ifdef CHEBYSHEV_FLOAT
	using Real = float;
#elif defined(CHEBYSHEV_LONG_DOUBLE)
	using Real = long double;
#else
	using Real = double;
#endif

	/// A real function of real argument
	using RealFunction = std::function<Real(Real)>;

	/// An input generating function
	using RealInputGenerator = std::function<Real(unsigned int)>;

}
