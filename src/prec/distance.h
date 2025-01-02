
///
/// @file distance.h Distance functions.
///

#ifndef CHEBYSHEV_DISTANCE_H
#define CHEBYSHEV_DISTANCE_H


namespace chebyshev {

namespace _internal {

	template<typename Type>
	inline Type abs(Type x) {
		return (x > 0) ? x : -x;
	}
}

namespace prec {

	/// @namespace chebyshev::prec::distance Distance functions for use in prec::equals
	namespace distance {

		/// Absolute distance between two real values.
		template<typename Type = double>
		inline Type abs_distance(Type a, Type b) {
			return _internal::abs(b - a);
		}

	}

}}

#endif
