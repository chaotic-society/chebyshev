
///
/// @file err.h Error checking module
///

#ifndef CHEBYSHEV_ERR_H
#define CHEBYSHEV_ERR_H

#include <vector>
#include <cstdlib>
#include <iostream>

#include "./core/common.h"
#include "./core/random.h"
#include "./err/err_structures.h"


namespace chebyshev {


	// To err is human; to forgive, divine.


	/// @namespace chebyshev::err Error checking module
	///
	/// This module provides functions to test error reporting
	/// with different methods. Assertions are checked with
	/// err::assert, while the value of errno after a function
	/// call can be checked using err::check_errno and the
	/// throwing of exceptions can be checked using check_exception.
	namespace err {


		/// @class err_state
		/// Global state of the error testing module.
		struct err_state {
				
			/// Name of the module being tested
			std::string moduleName = "unknown";

			// Total number of checks
			unsigned int totalChecks = 0;

			/// Number of failed checks
			unsigned int failedChecks = 0;

			/// Whether to print to an output file.
			bool outputToFile = true;

			/// The files to write all error checking results to.
			std::vector<std::string> outputFiles {};

			/// Results of checking assertions
			std::map<std::string, std::vector<assert_result>> assertResults {};

			/// The files to write assertion results results to
			/// (if empty, all results are output to a generic file).
			std::vector<std::string> assertOutputFiles {};

			/// Default columns to print for assertions.
			std::vector<std::string> assertColumns = {
				"name", "evaluated", "failed", "description"
			};

			/// Results of checking errno
			std::map<std::string, std::vector<errno_result>> errnoResults {};

			/// The files to write errno checking results to
			/// (if empty, all results are output to a generic file).
			std::vector<std::string> errnoOutputFiles {};

			/// Default columns to print for errno checks.
			std::vector<std::string> errnoColumns = {
				"name", "evaluated", "expectedFlags", "failed"
			};

			/// Results of exception testing
			std::map<std::string, std::vector<exception_result>> exceptionResults {};

			/// The files to write exception results results to
			/// (if empty, all results are output to a generic file).
			std::vector<std::string> exceptionOutputFiles {};

			/// Default columns to print for exception checks.
			std::vector<std::string> exceptionColumns = {
				"name", "thrown", "correctType", "failed"
			};

			/// Target checks marked for execution,
			/// can be picked by passing test case names
			/// by command line. (all tests will be executed if empty)
			std::map<std::string, bool> pickedChecks {};

			/// Whether to print to standard output
			bool quiet = false;

		} state;


		/// Setup error checking module.
		///
		/// @param moduleName Name of the module under test.
		/// @param argc The number of command line arguments
		/// @param argv A list of C-style strings containing
		/// the command line arguments.
		inline void setup(
			const std::string& moduleName, int argc = 0, const char** argv = nullptr) {

			if(argc && argv)
				for (int i = 1; i < argc; ++i)
					state.pickedChecks[argv[i]] = true;

			std::cout << "Starting error checking on "
				<< moduleName << " ..." << std::endl;

			state.moduleName = moduleName;
			state.failedChecks = 0;
			state.totalChecks = 0;

			random::setup();
			output::setup();
		}


		/// Terminate the error testing environment.
		/// If test cases have been run, their results will be printed.
		///
		/// @param exit Whether to exit after terminating the module.
		inline void terminate(bool exit = true) {

			output::state.quiet = state.quiet;

			// Output to file is true but no specific files are specified, add default output file.
			if(	 state.outputToFile &&
				!output::state.outputFiles.size() &&
				!state.assertOutputFiles.size() &&
				!state.errnoOutputFiles.size() &&
				!state.exceptionOutputFiles.size() &&
				!state.outputFiles.size()) {
				
				state.outputFiles = { state.moduleName + "_results" };
			}

			std::vector<std::string> outputFiles;

			// Print assert results
			outputFiles  = state.outputFiles;
			outputFiles.insert(outputFiles.end(), state.assertOutputFiles.begin(), state.assertOutputFiles.end());


			output::print_results(state.assertResults, state.assertColumns, outputFiles);

			// Print errno checking results
			outputFiles  = state.outputFiles;
			outputFiles.insert(outputFiles.end(), state.errnoOutputFiles.begin(), state.errnoOutputFiles.end());


			output::print_results(state.errnoResults, state.errnoColumns, outputFiles);

			// Print exception checking results
			outputFiles  = state.outputFiles;
			outputFiles.insert(outputFiles.end(), state.exceptionOutputFiles.begin(), state.exceptionOutputFiles.end());


			output::print_results(state.exceptionResults, state.exceptionColumns, outputFiles);

			std::cout << "Finished error checking " << state.moduleName << " ...\n";
			std::cout << state.totalChecks
				<< " total checks, "
				<< state.failedChecks << " failed ("  << std::setprecision(3)
				<< (state.failedChecks / (double) state.totalChecks * 100.0)
				<< "%)" << std::endl;

			// Reset module information
			state = err_state();

			if(exit) {
				output::terminate();
				std::exit(state.failedChecks);
			}
		}


		/// Assert that an expression is true.
		///
		/// @param name Name of the check (function name or test case name).
		/// @param exp Expression to test for truth.
		/// @param description Description of the assertion.
		inline void assert(
			const std::string& name,
			bool exp,
			std::string description = "",
			bool quiet = false) {

			assert_result res {};

			res.name = name;
			res.evaluated = exp;
			res.failed = !exp;
			res.description = description;
			res.quiet = quiet;

			state.totalChecks++;

			if(!exp)
				state.failedChecks++;

			state.assertResults[name].push_back(res);
		}


		/// Check errno value after function call
		///
		/// @param name The name of the function or test case
		/// @param f The function to test
		/// @param x The input value to evaluate the function at
		/// @param expected_errno The expected value of errno
		template<typename Function, typename InputType>
		inline void check_errno(
			const std::string& name,
			Function f,
			InputType x,
			int expected_errno,
			bool quiet = false) {

			errno_result res {};
			errno = 0;

			try {
				volatile auto r = f(x);
				r = *(&r);
			} catch(...) {}

			res.name = name;
			res.evaluated = errno;
			res.expectedFlags = { expected_errno };
			res.failed = (errno != expected_errno);
			res.quiet = quiet;

			state.totalChecks++;

			if(res.failed)
				state.failedChecks++;

			state.errnoResults[name].push_back(res);
		}


		/// Check errno value after function call
		///
		/// @param name The name of the function or test case
		/// @param f The function to test
		/// @param generator A function which takes in an index
		/// and returns a (potentially random) input value
		/// @param expected_errno The expected value of errno
		template<typename Function, typename InputType>
		inline void check_errno(
			const std::string& name, Function f,
			std::function<InputType()> generator,
			int expected_errno,
			bool quiet = false) {

			check_errno(name, f, generator(), expected_errno, quiet);
		}


		/// Check errno value after function call
		///
		/// @param name The name of the function or test case
		/// @param f The function to test
		/// @param x The input value to evaluate the function at
		/// @param expected_flags A list of the expected errno flags
		template<typename Function, typename InputType>
		inline void check_errno(
			const std::string& name,
			Function f,
			InputType x,
			std::vector<int>& expected_flags,
			bool quiet = false) {


			errno_result res {};
			errno = 0;

			try {
				volatile auto r = f(x);
				r = *(&r);
			} catch(...) {}

			res.name = name;
			res.evaluated = errno;
			res.expectedFlags = expected_flags;
			res.quiet = quiet;

			res.failed = false;
			for (int flag : expected_flags)
				if(!(errno & flag))
					res.failed = true;

			state.totalChecks++;

			if(res.failed)
				state.failedChecks++;

			state.errnoResults[name].push_back(res);
		}


		/// Check errno value after function call
		///
		/// @param name The name of the function or test case
		/// @param f The function to test
		/// @param generator A function which takes in an index
		/// and returns a (potentially random) input value
		/// @param expected_flags A list of the expected errno flags
		template<typename Function, typename InputType>
		void check_errno(
			const std::string& name, Function f,
			std::function<InputType()> generator,
			std::vector<int>& expected_flags,
			bool quiet = false) {

			check_errno(name, f, generator(), expected_flags, quiet);
		}


		/// Check that an exception is thrown during a function call
		///
		/// @param name The name of the function or test case
		/// @param f The function to test
		/// @param x The input value to use
		template<typename Function, typename InputType>
		inline void check_exception(
			const std::string& name,
			Function f,
			InputType x,
			bool quiet = false) {

			exception_result res {};
			bool thrown = false;

			try {
				volatile auto r = f(x);
				r = *(&r);
			} catch(...) {
				thrown = true;
			}

			res.name = name;
			res.thrown = thrown;
			res.failed = !thrown;
			res.correctType = true;
			res.quiet = quiet;

			state.totalChecks++;
			if(!thrown)
				state.failedChecks++;

			state.exceptionResults[name].push_back(res);
		}


		/// Check that an exception is thrown during a function call
		///
		/// @param name The name of the function or test case
		/// @param f The function to test
		/// @param generator A function which takes in an index
		/// and returns a (potentially random) input value
		template<typename Function, typename InputType>
		inline void check_exception(
			const std::string& name, Function f,
			std::function<InputType()> generator,
			bool quiet = false) {

			check_exception(name, f, generator(), quiet);
		}


		/// Check that an exception is thrown during a function call
		/// and that the type of the exception is correct.
		///
		/// @param name The name of the function or test case
		/// @param f The function to test
		/// @param x The input value to use
		template<typename ExceptionType, typename Function, typename InputType>
		inline void check_exception(
			const std::string& name,
			Function f,
			InputType x,
			bool quiet = false) {

			exception_result res {};
			bool thrown = false;
			bool correctType = false;

			try {
				volatile auto r = f(x);
				r = *(&r);
			} catch(ExceptionType& exc) {

				correctType = true;
				thrown = true;

			} catch(...) {
				thrown = true;
			}

			res.name = name;
			res.thrown = thrown;
			res.failed = !(thrown && correctType);
			res.correctType = correctType;
			res.quiet = quiet;

			state.totalChecks++;
			if(!thrown)
				state.failedChecks++;

			state.exceptionResults[name].push_back(res);
		}


		/// Check that an exception is thrown during a function call
		/// and that the type of the exception is correct.
		///
		/// @param name The name of the function or test case
		/// @param f The function to test
		/// @param generator A function which takes in an index
		/// and returns a (potentially random) input value
		template<typename ExceptionType, typename Function, typename InputType>
		inline void check_exception(
			const std::string& name, Function f,
			std::function<InputType()> generator,
			bool quiet = false) {

			check_exception(name, f, generator(), quiet);
		}
	}
}

#endif
