
///
/// @file distance.h Distance functions.
///

#ifndef CHEBYSHEV_DISTANCE_H
#define CHEBYSHEV_DISTANCE_H
#include <cmath>


namespace chebyshev {
namespace prec {

	/// @namespace chebyshev::prec::distance Distance functions for use in prec_context::equals()
	namespace distance {


		/// Absolute distance between two values
		/// which have an ordering with respect to zero.
		template<typename Type = real_t>
		inline prec_t absolute(Type a, Type b) {
			const Type diff = b - a;
			return prec_t(diff > Type(0.0) ? diff : -diff);
		}


		/// Euclidean distance between vectors (any type with a size() method and [] operator).
		/// If the size of the vectors is different, NaN is returned.
		template<typename Vector>
		inline prec_t euclidean(const Vector& v1, const Vector& v2) {

			if (v1.size() != v2.size() || !v1.size())
				return get_nan();

			auto sum = (v1[0] - v2[0]) * (v1[0] - v2[0]);
			for (size_t i = 1; i < v1.size(); i++) {
				sum += (v1[i] - v2[i]) * (v1[i] - v2[i]);
			}

			return std::sqrt(sum);
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
