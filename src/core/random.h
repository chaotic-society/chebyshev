
///
/// @file random.h The pseudorandom number generation and sampling module.
///

#ifndef CHEBYSHEV_RANDOM_H
#define CHEBYSHEV_RANDOM_H

#include <cstdlib>
#include "../prec/interval.h"


namespace chebyshev {

	/// @namespace chebyshev::random
	/// Pseudorandom number generation and sampling module.
	namespace random {


		struct random_state {
			
			/// The seed for random number generation
			uint64_t seed = 0;

		} state;


		/// Initialize the random module.
		inline void setup(uint64_t seed = 0) {

			if(seed == 0)
				seed = time(nullptr);

			state.seed = seed;
			srand(state.seed);
		}


		/// Generate a random natural number.
		inline uint64_t natural() {
			return rand();
		}


		/// Generate a uniformly distributed random number.
		///
		/// @param a The lower extreme of the interval
		/// @param b The upper extreme of the interval
		/// @return A pseudorandom number uniformly
		/// distributed over (a, b).
		template<typename FloatType>
		inline FloatType uniform(FloatType a, FloatType b) {
			return (rand() / (long double) RAND_MAX) * (b - a) + a;
		}


		/// Fill an already allocated vector with uniformly
		/// distributed numbers over different intervals.
		///
		/// @param x The already initialized vector to fill.
		/// @param intervals The intervals to generate over.
		/// @return A reference to the overwritten vector.
		template<typename Vector>
		inline Vector& sample_uniform(Vector& x, const std::vector<prec::interval> intervals) {

			if(x.size() != intervals.size())
				throw std::runtime_error(
					"Vector and domain size mismatch in chebyshev::sample_uniform");

			for (int i = 0; i < x.size(); ++i)
				x[i] = uniform(intervals[i].a, intervals[i].b);
		
			return x;
		}

	}
}

#endif
