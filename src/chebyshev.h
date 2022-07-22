
///
/// @file chebyshev.h Main file of the testing library
///

#pragma once

#include "./precision.h"
#include "./error_checking.h"


#define REAL_LAMBDA(f) [](Real x){ return f(x); }


namespace chebyshev {

	/// Terminate all testing
	void terminate() {
		exit(prec::failed_tests + err::failed_checks);
	}

}


/// Namespace alias
#ifndef CHEBYSHEV_NO_ALIAS
namespace ch = chebyshev;
#endif
