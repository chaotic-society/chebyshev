
///
/// @file generator.h Input generators for benchmarks.
///

#ifndef CHEYBYSHEV_GENERATOR_H
#define CHEYBYSHEV_GENERATOR_H

#include <functional>
#include "../core/common.h"
#include "../core/random.h"


namespace chebyshev {
namespace benchmark {

	/// @namespace chebyshev::benchmark::generator Input generators for benchmarks
	namespace generator {


		/// Uniform generator over a domain.
		///
		/// @param a The lower extreme of the domain
		/// @param b The upper extreme of the domain
		/// @return The input generator
		inline auto uniform1D(real_t a, real_t b) {

			return [a, b](random::random_source& rnd) -> real_t {
				return rnd.uniform(a, b);
			};
		}


		/// Discrete uniform generator over a domain.
		///
		/// @param a The lower extreme of the domain
		/// @param b The upper extreme of the domain
		/// @return The input generator
		inline auto discrete1D(long int a, long int b) {

			long long int length = a < b ? (b - a) : (a - b);

			return [a, length](random::random_source& rnd) {
				return a + (rnd.natural() % uint64_t(length));
			};
		}

	}

}}

#endif
