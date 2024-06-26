
///
/// @file error_handling.h Error handling checks
///

#ifndef CHEBYSHEV_ERR_H
#define CHEBYSHEV_ERR_H

#include <vector>
#include <cstdlib>

#include "./core/common.h"
#include "./core/random.h"
#include "./err/err_structures.h"


namespace chebyshev {

	/// @namespace chebyshev::err Error testing
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

			/// Relative or absolute path to output folder
			std::string outputFolder = "";

			/// Prefix to prepend to the filename, in addition
			/// to the module name.
			std::string filenamePrefix = "err_";

			/// Suffix to append to the filename, in addition
			/// to the module name.
			std::string filenameSuffix = ".csv";

			/// Whether to print to an output file.
			bool outputToFile = true;

			/// Results of checking assertions
			std::map<std::string, std::vector<assert_result>> assertResults {};

			/// Default columns to print for assertions.
			std::vector<std::string> assertColumns = {
				"funcName", "evaluated", "failed", "description"
			};

			/// Results of checking errno
			std::map<std::string, std::vector<errno_result>> errnoResults {};

			/// Default columns to print for errno checks.
			std::vector<std::string> errnoColumns = {
				"funcName", "evaluated", "expectedFlags", "failed"
			};

			/// Results of exception testing
			std::map<std::string, std::vector<exception_result>> exceptionResults {};

			/// Default columns to print for exception checks.
			std::vector<std::string> exceptionColumns = {
				"funcName", "thrown", "correctType", "failed"
			};

			/// Target checks marked for execution,
			/// can be picked by passing test case names
			/// by command line. (all tests will be executed if empty)
			std::map<std::string, bool> pickedChecks {};

			/// Whether to print to standard output
			bool quiet = false;

		} state;


		/// Setup error checking module.
		void setup(
			const std::string& moduleName, int argc = 0, const char** argv = nullptr) {

			if(argc && argv)
				for (int i = 1; i < argc; ++i)
					state.pickedChecks[argv[i]] = true;

			std::cout << "Starting error checking on "
				<< moduleName << " ...\n" << std::endl;

			state.moduleName = moduleName;
			state.failedChecks = 0;
			state.totalChecks = 0;

			random::setup();
			output::setup();
		}


		/// Terminate error checking
		void terminate(bool exit = true) {

			output::state.quiet = state.quiet;

			if(state.outputToFile) {

				std::string filename;
				filename = state.outputFolder + state.filenamePrefix
					+ state.moduleName + state.filenameSuffix;

				output::state.outputFiles[filename] = std::ofstream(filename);

				if(!output::state.outputFiles[filename].is_open()) {
					std::cout << "Unable to open output file,"
						" results will NOT be saved!" << std::endl;
					state.outputToFile = false;
				}
			}

			std::cout << "\n";

			output::table_state assertTable {};
			output::table_state errnoTable {};
			output::table_state exceptionTable {};


			// Print results of assertions
			if(state.assertResults.size())
				output::header(assertTable, state.assertColumns);


			for (auto it = state.assertResults.begin();
				it != state.assertResults.end(); ++it) {

				const auto res_list = it->second;

				for (size_t i = 0; i < res_list.size(); ++i) {

					assertTable.rowIndex++;

					if(it != state.assertResults.end()
					&& std::next(it) == state.assertResults.end()
					&& (i == res_list.size() - 1))
						assertTable.isLastRow = true;

					output::print(res_list[i], assertTable, state.assertColumns);
				}
			}


			// Print results of errno checking
			if(state.errnoResults.size())
				output::header(errnoTable, state.errnoColumns);

			for (auto it = state.errnoResults.begin();
				it != state.errnoResults.end(); ++it) {

				const auto res_list = it->second;

				for (size_t i = 0; i < res_list.size(); ++i) {

					errnoTable.rowIndex++;

					if(it != state.errnoResults.end()
					&& std::next(it) == state.errnoResults.end()
					&& (i == res_list.size() - 1))
						errnoTable.isLastRow = true;

					output::print(res_list[i], errnoTable, state.errnoColumns);
				}
			}


			// Print results of exception checking
			if(state.exceptionResults.size())
				output::header(exceptionTable, state.exceptionColumns);


			for (auto it = state.exceptionResults.begin();
				it != state.exceptionResults.end(); ++it) {

				const auto res_list = it->second;

				for (size_t i = 0; i < res_list.size(); ++i) {

					exceptionTable.rowIndex++;

					if(it != state.exceptionResults.end()
					&& std::next(it) == state.exceptionResults.end()
					&& (i == res_list.size() - 1))
						exceptionTable.isLastRow = true;

					output::print(res_list[i], exceptionTable, state.exceptionColumns);
				}
			}


			std::cout << "Ending error checking on "
				<< state.moduleName
				<< " ..." << std::endl;

			std::cout << state.totalChecks
				<< " total checks, "
				<< state.failedChecks << " failed ("
				<< (state.failedChecks / (double) state.totalChecks * 100.0)
				<< "%)" << std::endl;

			for (auto& p : output::state.outputFiles)
				std::cout << "Results have been saved in " << p.first << std::endl;

			output::terminate();
			state = err_state();

			if(exit)
				std::exit(state.failedChecks);
		}


		/// Assert that an expression is true
		///
		/// @param name Name of the check (function name or test case name).
		/// @param exp Expression to test for truth.
		/// @param description Description of the assertion.
		void assert(const std::string& name, bool exp, std::string description = "") {

			assert_result res {};

			res.funcName = name;
			res.evaluated = exp;
			res.failed = !exp;
			res.description = description;

			state.totalChecks++;

			if(!exp)
				state.failedChecks++;

			state.assertResults[name].push_back(res);
		}


		/// Check errno value after function call
		template<typename Function, typename InputType>
		void check_errno(
			const std::string& name, Function f,
			InputType x, int expected_errno) {

			errno_result res {};
			errno = 0;

			try {
				volatile auto r = f(x);
				r = *(&r);
			} catch(...) {}

			res.funcName = name;
			res.evaluated = errno;
			res.expectedFlags = { expected_errno };
			res.failed = (errno != expected_errno);


			state.totalChecks++;

			if(res.failed)
				state.failedChecks++;

			state.errnoResults[name].push_back(res);
		}


		/// Check errno value after function call
		template<typename Function, typename InputType>
		void check_errno(
			const std::string& name, Function f,
			std::function<InputType(unsigned int)> generator,
			int expected_errno) {

			check_errno(name, f, generator(rand()), expected_errno);
		}


		/// Check errno value after function call
		template<typename Function, typename InputType>
		void check_errno(
			const std::string& name, Function f,
			InputType x, std::vector<int>& expected_flags) {


			errno_result res {};
			errno = 0;

			try {
				volatile auto r = f(x);
				r = *(&r);
			} catch(...) {}

			res.funcName = name;
			res.evaluated = errno;
			res.expectedFlags = expected_flags;
			
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
		template<typename Function, typename InputType>
		void check_errno(
			const std::string& name, Function f,
			std::function<InputType(unsigned int)> generator,
			std::vector<int>& expected_flags) {

			check_errno(name, f, generator(rand()), expected_flags);
		}


		/// Check that an exception is thrown during a function call
		template<typename Function, typename InputType>
		void check_exception(const std::string& name, Function f, InputType x) {

			exception_result res {};
			bool thrown = false;

			try {
				volatile auto r = f(x);
				r = *(&r);
			} catch(...) {
				thrown = true;
			}

			res.funcName = name;
			res.thrown = thrown;
			res.failed = !thrown;
			res.correctType = true;

			state.totalChecks++;
			if(!thrown)
				state.failedChecks++;

			state.exceptionResults[name].push_back(res);
		}


		/// Check that an exception is thrown during a function call
		template<typename Function, typename InputType>
		void check_exception(
			const std::string& name, Function f,
			std::function<InputType(unsigned int)> generator) {

			check_exception(name, f, generator(rand()));
		}


		template<typename ExceptionType, typename Function, typename InputType>
		void check_exception(const std::string& name, Function f, InputType x) {

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

			res.funcName = name;
			res.thrown = thrown;
			res.failed = !(thrown && correctType);
			res.correctType = correctType;

			state.totalChecks++;
			if(!thrown)
				state.failedChecks++;

			state.exceptionResults[name].push_back(res);
		}


		template<typename ExceptionType, typename Function, typename InputType>
		void check_exception(
			const std::string& name, Function f,
			std::function<InputType(unsigned int)> generator) {

			check_exception(name, f, generator(rand()));
		}

	}

}

#endif
