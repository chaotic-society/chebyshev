
///
/// @file fail.h Default fail functions.
///

#ifndef CHEBYSHEV_FAIL_H
#define CHEBYSHEV_FAIL_H

#include "./prec_structures.h"


namespace chebyshev {
namespace prec {


	namespace fail {


		/// Default fail function which marks the test as failed
		/// if the maximum error on the domain is bigger than the tolerance
		auto fail_on_max_err = [](estimate_result r) -> bool {
			return (r.max_err > r.tolerance) || (r.max_err != r.max_err);
		};


		/// Marks the test as failed if the mean error on the domain
		/// is bigger than the tolerance or the error is NaN.
		auto fail_on_mean_err = [](estimate_result r) -> bool {
			return (r.mean_err > r.tolerance) || (r.mean_err != r.mean_err);
		};


		/// Marks the test as failed if the RMS error on the domain
		/// is bigger than the tolerance or the error is NaN.
		auto fail_on_rms_err = [](estimate_result r) -> bool {
			return (r.rms_err > r.tolerance) || (r.rms_err != r.rms_err);
		};


		/// Marks the test as failed if the relative error on the
		/// domain is bigger than the tolerance or the error is NaN.
		auto fail_on_rel_err = [](estimate_result r) -> bool {
			return (r.rel_err > r.tolerance) || (r.rel_err != r.rel_err);
		};

	}
}}


#endif
