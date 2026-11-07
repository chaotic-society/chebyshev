# Chebyshev Test
[![Build Status](https://github.com/chaotic-society/chebyshev/actions/workflows/build.yml/badge.svg)](https://github.com/chaotic-society/chebyshev/actions/workflows/build.yml)
[![Documentation](https://img.shields.io/badge/Doxygen-docs-blue?style=flat&cacheSeconds=https%3A%2F%2Fchaotic-society.github.io%2Ftheoretica%2F&link=https%3A%2F%2Fchaotic-society.github.io%2Fchebyshev%2F)](https://chaotic-society.github.io/chebyshev)

> A C++ testing framework designed for scientific software

Chebyshev is a header-only C++ testing framework specifically designed for scientific computing and numerical software. The framework is made up of different modules which handle specific types of testing: the `prec` module estimates the precision of numerical methods, `benchmark` module benchmarks code performance, the `err` module controls proper error handling. In addition, the `output` module controls output of test results in a fully customizable way, and `random` provides simple, thread-safe random numbers and distribution sampling.

## Features

- **Standalone:** Chebyshev is header-only and works out of the box, with no dependencies or installation required.
- **Modular:** The framework is lightweight and modular, with high customization capabilities.
- **Error estimation:** Chebyshev is built to estimate numerical errors with deterministic or stochastic methods out of the box.
- **Flexibility:** Since numerical methods take many forms, Chebyshev provides a unified interface to tackle many different problems.
- **Many output formats**: The `output` module supports different output formats (including Latex and Markdown for visualization) and custom format support.

## Usage
The three modules `prec`, `benchmark` and `err` are used through _contexts_, which are simply objects that handle requests and settings:

```c
auto ctx = ch::prec::make_context("myTestUnit");
ctx.settings.multithreading = true;
```

Tests are run through the context and are automatically executed, either sequentially or in parallel, according to the set policy (`ctx.settings.multithreading`).
Once a call to `ctx.terminate()` is reached or the context is destroyed, it waits for test results and outputs the results according to settings (`ctx.settings` and `ctx.output->settings`).

 ### Precision testing
 The `prec` module uses deterministic and stochastic methods to estimate the numerical errors of the functions you want to test. This is done by calling `ctx.equals()` to check a simple equivalence (e.g. `sqrt(4) == 2`) or by calling `ctx.estimate()` to use an `estimator` to get a more detailed error estimation over a discrete or continuous domain (e.g. checking that `sqrt(x)` works correctly over the positive real numbers):

 ```c
// Check an equivalence
ctx.equals("mySqrt(4)", mySqrt(4.0), 2.0);

// Estimate error on a continuous domain
ctx.estimate("mySqrt", mySqrt, std::sqrt, prec::interval(0, 1E+06));
```

This `ctx.estimate()` call will estimate relevant error integrals such as the mean and max error:

$$
\varepsilon_{\text{avg}} = \frac{1}{\mu(\Omega)} \int_\Omega |\tilde f(x) - f(x)| dx, \ \ \ \ \ \ \ \ \ \ \ \varepsilon_{\text{max}} = \max_\Omega |\tilde f(x) - f(x)|
$$

These results are stored and written to file, according to the settings. You can also implement fully customized _estimators_ for your own use cases. Other than comparing to a more precise implementation, you can check for properties of your method (e.g. homogeneity, involution, idempotence).

### Benchmarks
The `benchmark` module provides the `ctx.benchmark()` method which allows to estimate the average runtime and other statistics of any given function. The function under benchmark is run multiple times over many _runs_ and _iterations_, where different runs use the same set of inputs and iterations run over different inputs. The input sets can be fully customized, allowing for deterministic or randomized samples over a function's domain, using the `random` module (e.g. what is the average runtime of my `sqrt(x)` implementation on the real numbers?).

### Getting started
To use Chebyshev, simply download the code and include the relevant header file for the module you need (e.g. `prec.h`) and start writing tests!
You can also include `chebyshev.h` to include all modules and functionalities. The framework is designed to be **easy to use**, with a minimal learning curve and many **customization options**.

Here is a simple snippet:

```c
auto ctx = prec::make_context("example", argc, argv);

// Estimate errors on f() on [0, 10] comparing to an "exact" g()
ctx.estimate("f", f, g, prec::interval(0, 10));

// Check that two values are equal up to a tolerance
ctx.equals("f", f(1), 1, 0.2);
```

The output of this code is something like this:

```
Starting precision testing of the example module ...

 ┌──────────────────────────────────────────────────────────────────────────────┐
 │         Function │    Mean Err. │     RMS Err. │     Max Err. │       Result │
 ├──────────────────────────────────────────────────────────────────────────────┤
 │                f │      3.3e-12 │      3.5e-12 │      5.1e-12 │         PASS │
 └──────────────────────────────────────────────────────────────────────────────┘

Results have been saved in: example_results

 ┌───────────────────────────────────────────────────────────────┐
 │         Function │   Difference │    Tolerance │       Result │
 ├───────────────────────────────────────────────────────────────┤
 │                f │      0.0e+00 │      2.0e-01 │         PASS │
 └───────────────────────────────────────────────────────────────┘

Results have been saved in: example_results
Finished testing example
2 total tests, 0 failed (0%)
```

### Dependencies
Chebyshev has no dependencies, and only a C++14 compiler is needed to use it. Simply include the module headers in your code and start writing tests.
