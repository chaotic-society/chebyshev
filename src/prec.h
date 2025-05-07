
///
/// @file prec.h Precision testing module
///

#ifndef CHEBYSHEV_PREC_H
#define CHEBYSHEV_PREC_H

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <memory>

#include "./prec/prec_structures.h"
#include "./core/output.h"
#include "./core/random.h"
#include "./prec/estimator.h"
#include "./prec/fail.h"


namespace chebyshev {

/// @namespace chebyshev::prec
namespace prec {


	/// @class prec_settings
	/// Settings for the precision testing module, used in prec_context.
	struct prec_settings {
		
		/// Name of the module being tested
		std::string moduleName = "unknown";

		/// Default number of iterations for integral quadrature
		unsigned int defaultIterations = CHEBYSHEV_PREC_ITER;

		/// Default fail function
		FailFunction defaultFailFunction = fail::fail_on_max_err();

		/// Default tolerance on max absolute error
		long double defaultTolerance = CHEBYSHEV_PREC_TOLERANCE;

		/// The files to write all precision testing results to
		std::vector<std::string> outputFiles {};

		/// Default columns to print for precision estimates.
		std::vector<std::string> estimateColumns = {
			"name", "meanErr", "rmsErr", "maxErr", "failed"
		};

		/// The files to write estimate results to
		/// (if empty, all results are output to a generic file).
		std::vector<std::string> estimateOutputFiles {};

		/// Default columns to print for equations.
		std::vector<std::string> equationColumns = {
			"name", "difference", "tolerance", "failed"
		};

		/// The files to write equation results to
		/// (if empty, all results are output to a generic file).
		std::vector<std::string> equationOutputFiles {};

		/// Target tests marked for execution,
		/// can be picked by passing test case names
		/// by command line (all tests will be executed if empty).
		std::map<std::string, bool> pickedTests {};

	};


	/// @class prec_results
	/// Structure holding results of the precision testing module.
	struct prec_results {

		/// Total number of tests run
		unsigned int totalTests = 0;

		/// Number of failed tests
		unsigned int failedTests = 0;

		/// Results of error estimation
		std::map<std::string, std::vector<estimate_result>> estimateResults {};

		/// Results of equation evaluation
		std::map<std::string, std::vector<equation_result>> equationResults {};
		
	};


	/// @class prec_context
	/// Precision testing context, handling precision test cases.
	class prec_context {
	public:

		/// Settings for the precision testing context
		prec_settings settings;
		
		/// Results of precision testing
		prec_results results;

		/// Output module settings for the context, dynamically allocated
		/// and possibly shared between multiple contexts.
		std::shared_ptr<output::output_context> output;

		/// Random module settings for the context, dynamically allocated
		/// and possibly shared between multiple contexts.
		std::shared_ptr<random::random_context> random;


		/// Setup the precision testing module
		///
		/// @param moduleName Name of the module under test
		/// @param argc Number of command line arguments
		/// @param argv List of command line arguments as C strings
		inline void setup(
			const std::string& moduleName,
			int argc = 0, const char** argv = nullptr) {

			// Initialize other modules
			settings = prec_settings();
			results = prec_results();
			output = std::make_shared<output::output_context>();
			random = std::make_shared<random::random_context>();

			// Initialize list of picked tests
			if(argc && argv)
				for (int i = 1; i < argc; ++i)
					settings.pickedTests[argv[i]] = true;

			std::cout << "Starting precision testing of the ";
			std::cout << moduleName << " module ..." << std::endl;

			settings.moduleName = moduleName;
		}


		/// Terminate the precision testing module
		///
		/// @param exit Whether to exit after finishing execution.
		inline void terminate(bool exit = true) {


			// Ensure that an output file is specified
			if(	 output->settings.outputToFile &&
				!output->settings.outputFiles.size() &&
				!settings.estimateOutputFiles.size() &&
				!settings.equationOutputFiles.size() &&
				!settings.outputFiles.size()) {

				settings.outputFiles = { settings.moduleName + "_results" };
			}


			// Print test results
			std::vector<std::string> outputFiles;

			// Print estimate results
			outputFiles  = settings.outputFiles;
			outputFiles.insert(
				outputFiles.end(),
				settings.estimateOutputFiles.begin(),
				settings.estimateOutputFiles.end()
			);

			output->print_results(
				results.estimateResults, settings.estimateColumns, outputFiles
			);


			// Print equation results
			outputFiles  = settings.outputFiles;
			outputFiles.insert(
				outputFiles.end(),
				settings.equationOutputFiles.begin(),
				settings.equationOutputFiles.end()
			);

			output->print_results(
				results.equationResults, settings.equationColumns, outputFiles
			);


			// Print overall test results
			std::cout << "Finished testing " << settings.moduleName << '\n';
			std::cout << results.totalTests << " total tests, ";
			std::cout << results.failedTests << " failed";

			// Print proportion of failed test, avoiding division by zero
			if (results.totalTests > 0) {
				std::cout << " (" << std::setprecision(3);
				std::cout << (results.failedTests / (double) results.totalTests) * 100;
				std::cout << "%)";
			}
			std::cout << std::endl;

			// Discard previous results
			results = prec_results();

			if(exit) {
				output->terminate();
				std::exit(results.failedTests);
			}
		}


		/// Construct a precision testing context
		/// @param moduleName Name of the module under test
		/// @param argc Number of command line arguments
		/// @param argv List of command line arguments as C strings
		prec_context(const std::string& moduleName, int argc, const char** argv) {
			setup(moduleName, argc, argv);
		}


		/// Destructor for the context, automatically terminates the module.
		~prec_context() {
			terminate();
		}


		/// Estimate error integrals over a function
		/// with respect to an exact function,
		/// with the given options.
		///
		/// @param name The name of the test case
		/// @param funcApprox The approximation to test
		/// @param funcExpected The expected result
		/// @param opt The options for the estimation
		template <
			typename R, typename ...Args,
			typename Function1 = std::function<R(Args...)>,
			typename Function2 = Function1
		>
		inline void estimate(
			const std::string& name,
			Function1 funcApprox,
			Function2 funcExpected,
			estimate_options<R, Args...> opt) {

			// Skip the test case if any tests have been picked
			// and this one was not picked.
			if(settings.pickedTests.size())
				if(settings.pickedTests.find(name) == settings.pickedTests.end())
					return;

			// Use the estimator to estimate error integrals.
			auto res = opt.estimator(funcApprox, funcExpected, opt);

			res.name = name;
			res.domain = opt.domain;
			res.tolerance = opt.tolerance;
			res.quiet = opt.quiet;
			res.iterations = opt.iterations;

			// Use the fail function to determine whether the test failed.
			res.failed = opt.fail(res);

			results.totalTests++;
			if(res.failed)
				results.failedTests++;

			results.estimateResults[name].push_back(res);
		}


		/// Estimate error integrals over a function
		/// with respect to an exact function.
		///
		/// @param name The name of the test case.
		/// @param funcApprox The approximation to test.
		/// @param funcExpected The expected result.
		/// @param intervals The (potentially multidimensional)
		/// domain of estimation.
		/// @param iterations The number of function evaluations.
		/// @param fail The fail function to determine whether
		/// the test failed.
		/// @param estimator The precision estimator to use.
		/// @param quiet Whether to output the result.
		template <
			typename R, typename ...Args,
			typename Function1 = std::function<R(Args...)>,
			typename Function2 = Function1
		>
		inline void estimate(
			const std::string& name,
			Function1 funcApprox,
			Function2 funcExpected,
			std::vector<interval> domain,
			long double tolerance, unsigned int iterations,
			FailFunction fail,
			Estimator<R, Args...> estimator,
			bool quiet = false) {

			estimate_options<R, Args...> opt {};
			opt.domain = domain;
			opt.tolerance = tolerance;
			opt.iterations = iterations;
			opt.fail = fail;
			opt.estimator = estimator;
			opt.quiet = quiet;

			estimate(name, funcApprox, funcExpected, opt);
		}


		/// Estimate error integrals over a real function
		/// of real variable, with respect to an exact function.
		///
		/// @param name The name of the test case.
		/// @param funcApprox The approximation to test.
		/// @param funcExpected The expected result.
		/// @param intervals The (potentially multidimensional)
		/// domain of estimation.
		/// @param iterations The number of function evaluations.
		/// @param fail The fail function to determine whether
		/// the test failed (defaults to fail_on_max_err).
		/// @param estimator The precision estimator to use
		/// (defaults to the trapezoid<double> estimator).
		/// @param quiet Whether to output the result.
		inline void estimate(
			const std::string& name,
			EndoFunction<double> funcApprox,
			EndoFunction<double> funcExpected,
			interval domain,
			long double tolerance = get_nan(),
			unsigned int iterations = 0,
			FailFunction fail = fail::fail_on_max_err(),
			Estimator<double, double> estimator = estimator::quadrature1D<double>(),
			bool quiet = false) {

			if (tolerance != tolerance)
				tolerance = settings.defaultTolerance;

			if (iterations == 0)
				iterations = settings.defaultIterations;

			estimate_options<double, double> opt {};
			opt.domain = { domain };
			opt.tolerance = tolerance;
			opt.iterations = iterations;
			opt.fail = fail;
			opt.estimator = estimator;
			opt.quiet = quiet;

			estimate(name, funcApprox, funcExpected, opt);
		}


		/// Precision testing of an endofunction which is
		/// equivalent to the identity.
		///
		/// @param name The name of the test case.
		/// @param id The identity function to test.
		/// @param opt The options for estimation.
		template <
			typename Type, typename Identity = EndoFunction<Type>
		>
		inline void identity(
			const std::string& name,
			Identity id,
			const estimate_options<Type, Type>& opt) {

			// Apply the identity function
			EndoFunction<Type> funcApprox = [&](Type x) -> Type {
				return id(x);
			};

			// And compare it to the identity
			EndoFunction<Type> funcExpected = [](Type x) -> Type {
				return x;
			};

			estimate(name, funcApprox, funcExpected, opt);
		}


		/// Precision testing of an endofunction which is
		/// an involution. The function is applied two times
		/// to input values and it is checked against the identity.
		///
		/// @param name The name of the test case.
		/// @param involution The involution to test.
		/// @param opt The options for estimation.
		template <
			typename Type, typename Involution = EndoFunction<Type>
		>
		inline void involution(
			const std::string& name,
			Involution invol,
			const estimate_options<Type, Type>& opt) {

			// Apply the involution two times
			EndoFunction<Type> funcApprox = [&](Type x) -> Type {
				return invol(invol(x));
			};

			// And compare it to the identity
			EndoFunction<Type> funcExpected = [](Type x) -> Type {
				return x;
			};

			estimate(name, funcApprox, funcExpected, opt);
		}


		/// Precision testing of an endofunction which is
		/// idempotent. The function is applied two times
		/// to input values and it is checked against itself.
		///
		/// @param name The name of the test case.
		/// @param idem The idempotent function to test.
		/// @param opt The options for estimation.
		template <
			typename Type, typename Involution = EndoFunction<Type>
		>
		inline void idempotence(
			const std::string& name,
			Involution idem,
			const estimate_options<Type, Type>& opt) {

			// Apply the idem two times
			EndoFunction<Type> funcApprox = [&](Type x) -> Type {
				return idem(idem(x));
			};

			// And compare it to the identity
			EndoFunction<Type> funcExpected = [&](Type x) -> Type {
				return idem(x);
			};

			estimate(name, funcApprox, funcExpected, opt);
		}


		/// Precision testing of an function which is
		/// homogeneous over the domain. The function is applied
		/// to input values and it is checked against zero.
		/// The zero value is constructed as OutputType(0.0), but may
		/// be specified as an additional argument.
		///
		/// @param name The name of the test case.
		/// @param hom The homogeneous function to test.
		/// @param opt The options for estimation.
		/// @param zero_element The zero element of type OutputType
		/// (defaults to OutputType(0.0)).
		template <
			typename InputType, typename OutputType = InputType,
			typename Homogeneous = std::function<OutputType(InputType)>
		>
		inline void homogeneous(
			const std::string& name,
			Homogeneous hom,
			const estimate_options<InputType, OutputType>& opt,
			OutputType zero_element = OutputType(0.0)) {

			// Apply the homogeneous function
			std::function<OutputType(InputType)> funcApprox =
				[&](InputType x) -> OutputType {
					return hom(x);
				};

			// And compare it to the zero element
			std::function<OutputType(InputType)> funcExpected =
				[&](InputType x) -> OutputType {
					return zero_element;
				};

			estimate(name, funcApprox, funcExpected, opt);
		}


		/// Test an equivalence up to a tolerance,
		/// with the given options (e.g. for residual testing).
		///
		/// @param name The name of the test case
		/// @param evaluate The evaluated value
		/// @param expected The expected value
		/// @param opt The options for the evaluation
		template<typename Type = double>
		inline void equals(
			const std::string& name,
			const Type& evaluated, const Type& expected,
			equation_options<Type> opt = equation_options<Type>()) {

			// Skip the test case if any tests have been picked
			// and this one was not picked.
			if(settings.pickedTests.size())
				if(settings.pickedTests.find(name) == settings.pickedTests.end())
					return;

			equation_result res {};

			long double diff = opt.distance(evaluated, expected);

			// Mark the test as failed if the
			// distance between the two values
			// is bigger than the tolerance.
			res.failed = (diff > opt.tolerance);

			res.name = name;
			res.difference = diff;
			res.tolerance = opt.tolerance;
			res.quiet = opt.quiet;

			results.totalTests++;
			if(res.failed)
				results.failedTests++;

			// Register the result of the equation by name
			results.equationResults[name].push_back(res);
		}


		/// Test an equivalence up to a tolerance,
		/// with the given options (e.g. for residual testing).
		///
		/// @param name The name of the test case
		/// @param evaluate The evaluated value
		/// @param expected The expected value
		/// @param distance The distance function to use
		/// @param tolerance The tolerance for the evaluation
		/// @param quiet Whether to output the result
		template<typename Type = double>
		inline void equals(
			const std::string& name,
			const Type& evaluated, const Type& expected,
			long double tolerance,
			DistanceFunction<Type> distance,
			bool quiet = false) {

			equation_options<Type> opt {};
			opt.tolerance = tolerance;
			opt.distance = distance;
			opt.quiet = quiet;

			equals(name, evaluated, expected, opt);
		}


		/// Test an equivalence up to a tolerance,
		/// with the given options (e.g. for residual testing).
		///
		/// @param name The name of the test case
		/// @param evaluate The evaluated value
		/// @param expected The expected value
		/// @param tolerance The tolerance for the evaluation
		/// @param quiet Whether to output the result
		inline void equals(
			const std::string& name,
			long double evaluated, long double expected,
			long double tolerance = get_nan(),
			bool quiet = false) {

			if (tolerance != tolerance)
				tolerance = settings.defaultTolerance;

			// Skip the test case if any tests have been picked
			// and this one was not picked.
			if(settings.pickedTests.size())
				if(settings.pickedTests.find(name) == settings.pickedTests.end())
					return;

			equation_result res {};

			long double diff = distance::abs_distance(evaluated, expected);

			// Mark the test as failed if the
			// distance between the two values
			// is bigger than the tolerance.
			res.failed = (diff > tolerance);

			res.name = name;
			res.difference = diff;
			res.tolerance = tolerance;
			res.quiet = quiet;

			res.evaluated = evaluated;
			res.expected = expected;

			results.totalTests++;
			if(res.failed)
				results.failedTests++;

			// Register the result of the equation by name
			results.equationResults[name].push_back(res);
		}


		/// Evaluate multiple pairs of values for equivalence
		/// up to the given tolerance (e.g. for residual testing).
		///
		/// @param name The name of the function or test case
		/// @param values A list of values to equate
		/// @param tolerance The tolerance for the evaluation
		/// @param quiet Whether to output the result
		template<typename Type>
		inline void equals(
			const std::string& name,
			std::vector<std::array<Type, 2>> values,
			long double tolerance = get_nan(),
			bool quiet = false) {

			if (tolerance != tolerance)
				tolerance = settings.defaultTolerance;

			// Skip the test case if any tests have been picked
			// and this one was not picked.
			if(settings.pickedTests.size())
				if(settings.pickedTests.find(name) == settings.pickedTests.end())
					return;

			for (const auto& v : values)
				equals(name, v[0], v[1], tolerance, quiet);
		}
	};


	/// Construct a precision testing context with the given parameters.
	///
	/// @param moduleName Name of the module under test.
	/// @param argc The number of command line arguments.
	/// @param argv An array of command line arguments as C-like strings.
	prec_context make_context(const std::string& moduleName,
			int argc = 0, const char** argv = nullptr) {
		
		return prec_context(moduleName, argc, argv);
	}


}}

#endif
