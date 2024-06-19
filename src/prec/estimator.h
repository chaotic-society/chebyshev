
///
/// @file estimator.h Default precision estimators.
///

#ifndef CHEBYSHEV_ESTIMATOR
#define CHEBYSHEV_ESTIMATOR


#include <functional>
#include "../core/common.h"
#include "./prec_structures.h"


namespace chebyshev {
namespace prec {


	/// @namespace chebyshev::prec::estimator Precision estimators.
	namespace estimator {


		/// Use the trapezoid quadrature scheme to approximate error integrals.
		template<typename FloatType = double>
		estimate_result trapezoid(
			RealFunction<FloatType> f_approx,
			RealFunction<FloatType> f_exp,
			estimate_options<FloatType, FloatType> options) {


			estimate_result res;

			/// ...

			return res;
		}


		/// Use crude Monte Carlo integration to approximate error integrals.
		// template<typename FloatType = double>
		// estimate_result montecarlo(
		// 	RealFunction<FloatType> f_approx, RealFunction<FloatType> f_exp,
		// 	estimate_options<FloatType, FloatType> options) {


		// 	estimate_result res;

		// 	/// ...

		// 	return res;
		// }


	}

}}


#endif
