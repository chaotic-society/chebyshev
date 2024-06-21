# Chebyshev Testing Framework
A C++ testing and benchmark header-only framework specialized in precision testing for scientific software.

## Framework Structure
The framework is divided in three different components:

- ### Precision testing (chebyshev::prec)
	Functions of real variable are tested and their precision, with respect to an exact function, is estimated by computing the **trapezoid quadrature** of **absolute and RMS error integrals**:
	$$\epsilon_{abs} = \int_\Omega |f_{exact}(x) - f_{approx}(x)| dx$$
	$$\epsilon_{mean} = \frac{1}{\mu(\Omega)} \int_\Omega |f_{exact}(x) - f_{approx}(x)| dx$$
	$$\epsilon_{rms} = \frac{1}{\mu(\Omega)} \sqrt{\int_\Omega |f_{exact}(x) - f_{approx}(x)|^2 dx}$$
	$$\epsilon_{max} = \max_{\Omega} |f_{exact}(x) - f_{approx}(x)|$$
	$$\epsilon_{rel} = \frac{\int_\Omega |f_{exact}(x) - f_{approx}(x)| dx}{\int_\Omega |f_{exact}(x)|dx}$$
	
	The function `chebyshev::prec::estimate()` is used to estimate the error integrals for a generic function. The results of precision testing are displayed once `chebyshev::prec::terminate()` is called.

- ### Benchmarking (chebyshev::benchmark)
	Functions' performance is tested by running the functions multiple times with randomized input in their domain and averaging the elapsed time. The function `chebyshev::benchmark::benchmark()` is used to benchmark a generic function with a given random input generator. The results of the benchmarks are displayed once `chebyshev::benchmark::terminate()` is called.

- ### Error checking (chebyshev::err)
	Check for correct error reporting through `errno` and exceptions. The function `chebyshev::err::assert` can be used to assert expressions, while `chebyshev::err::check_errno` and `chebyshev::err::check_exception()` can be used to test the behaviour of error reporting.

## Setup and usage
The library is standalone and needs no dependencies or installation, so you only need to include the header files inside your code. You can automatically include all header files of the library by including `chebyshev.h`. All three modules are initialized and terminated using the `<module>::setup()` and `<module>::terminate()` functions in their respective namespaces.


## Examples
The `examples` folder contains example code for all three parts of the framework. Here is a short example of error estimation:

```c
// Setup the precision testing environment
prec::setup("chebyshev", argc, argv);

	// Estimate errors on f_a on [0, 10]
	prec::estimate("f_approx", f_approx, f, interval(0, 10));

	// Check that two values are almost equal,
	// up to a tolerance
	prec::equals("f_approx", f_approx(1), 1, 0.2);

	// Check multiple pairs of values
	prec::equals("f_approx", {
		{f_approx(1), 1},
		{f_approx(2), 4},
		{f_approx(3), 3}
	});

        // Or more simply, if the function is the same:
	prec::equals("f_approx", f_approx, {
		{1, 1},
		{2, 4},
		{3, 3}
	});

// Stop precision testing
prec::terminate();
```

## Dependencies
The library has no dependencies. Only a compiler supporting C++14 is needed.
