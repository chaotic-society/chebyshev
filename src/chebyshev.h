
///
/// @file chebyshev.h Main file of the testing library
///

#pragma once

#include "./precision.h"
#include "./error_checking.h"
#include "./benchmark.h"

	
/// Construct a lambda from any function
#define REAL_LAMBDA(f) [](chebyshev::Real x){ return f(x); }


namespace chebyshev {


	/// Construct a lambda from a real function
	RealFunction freal(Real(*f)(Real)) {
		return [f](Real x){ return f(x); };
	}
	

	/// Terminate all testing
	void terminate() {
		exit(prec::failed_tests + err::failed_checks);
	}

}


/// Namespace alias
#ifndef CHEBYSHEV_NO_ALIAS
namespace ch = chebyshev;
#endif
