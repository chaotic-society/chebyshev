
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

	/// @namespace chebyshev::prec::distance Distance functions for use in prec_context::equals()
	namespace distance {

		/// Absolute distance between two real values.
		template<typename Type = real_t>
		inline prec_t absolute(Type a, Type b) {
			return prec_t(_internal::abs(b - a));
		}


		/// Hamming distance between two strings, defined as
		/// the number of positions at which the corresponding characters are different.
		inline unsigned int hamming(const std::string& a, const std::string& b) {

			unsigned int sum = 0;
			for (size_t i = 0; i < std::min(a.size(), b.size()); i++)
				if (a[i] != b[i])
					sum++;

			if (a.size() != b.size())
				sum += std::abs(int(a.size()) - int(b.size()));

			return sum;
		}

	}

}}

#endif
