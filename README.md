# Chebyshev Testing Framework
A testing and benchmarking framework specialized for mathematical functions, using error estimation.

## Structure
The framework is divided in three different components:
- **chebyshev::prec** - Error estimation of real functions
- **chebyshev::benchmark** - Function benchmarking
- **chebyshev::err** - ERRNO and exception checking

## Examples
The `examples` folder contains example code for all three parts of the framework. Here is a short example of error estimation:

```c
// Setup the precision testing environment
prec::setup("chebyshev");

	// Estimate errors on f_a on [0, 10]
	prec::estimate("approx", f_approx, f, interval(0, 10));

	// Check that two values are almost equal
	prec::equals("f_approx", f_approx(1), 1, 0.2);

	// Check multiple pairs of values
	prec::equals("f_approx", {
		{f_a(1), 1},
		{f_a(2), 4},
		{f_a(3), 3}
	});

        // Or more simply, if the function is the same:
	prec::equals("f_a", f_a, {
		{1, 1},
		{2, 4},
		{3, 3}
	});

// Stop precision testing
prec::terminate();
```

## Dependencies
The library has no dependencies. Only a compiler supporting C++11 is needed.
