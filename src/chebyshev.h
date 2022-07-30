
///
/// @file chebyshev.h Main include file of the testing library
///

#include "prec.h"
#include "benchmark.h"
#include "err.h"


/// Construct a RealFunction from any function
#define REAL_LAMBDA(f) [](chebyshev::Real x){ return f(x); }


namespace chebyshev {}


/// Namespace alias
#ifndef CHEBYSHEV_NO_ALIAS
namespace ch = chebyshev;
#endif
