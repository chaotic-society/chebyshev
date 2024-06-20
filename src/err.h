
///
/// @file error_handling.h Error handling checks
///

#ifndef CHEBYSHEV_ERR_H
#define CHEBYSHEV_ERR_H

#include "core/common.h"
#include <vector>
#include <cstdlib>


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

			/// Whether to print to standard output
			bool quiet = false;

		} state;


		/// Setup error checking module.
		void setup(
			const std::string& module, int argc = 0, const char** argv = nullptr) {

			state.moduleName = module;
			srand(time(nullptr));

			std::cout << "Starting error checking on "
				<< state.moduleName << " ...\n" << std::endl;
		}


		/// Terminate error checking
		void terminate(bool exit = true) {

			std::cout << "\nEnding error checking on "
				<< state.moduleName
				<< " ..." << std::endl;

			std::cout << state.totalChecks
				<< " total checks, "
				<< state.failedChecks << " failed ("
				<< (state.failedChecks / (double) state.totalChecks * 100.0)
				<< "%)" << std::endl;

			if(exit)
				std::exit(state.failedChecks);
		}


		/// Assert that an expression is true
		void assert(bool exp, std::string desc = "") {

			state.totalChecks++;

			if(!exp) {
				state.failedChecks++;

				if(!state.quiet) {
					std::cout << "\tFailed assert (n. " << state.totalChecks << ")";

					if(desc != "")
						std::cout << ":\n\t\tDescription: " << desc << std::endl;
					else
						std::cout << " (no description provided)" << std::endl;

					std::cout << std::endl;
				}
				
			} else if(!state.quiet) {
				std::cout << "\tSuccessful assert (n. " << state.totalChecks << ")";
				if(desc != "")
					std::cout << ":\n\t\tDescription: " << desc << std::endl;
				else
					std::cout << " (no description provided)" << std::endl;

				std::cout << std::endl;
			}
		}


		/// Check errno value after function call
		template<typename Function, typename InputType>
		void check_errno(Function f, InputType x, int exp_errno) {

			state.totalChecks++;

			try {
				volatile auto r = f(x);
				r = *(&r);
			} catch(...) {}

			if(errno != exp_errno) {

				state.failedChecks++;

				if(!state.quiet) {
					std::cout << "\tFailed assert (n. " << state.totalChecks << ")\n";
					std::cout << "\t\tExpected ERRNO: " << exp_errno << std::endl;
					std::cout << "\t\tEvaluated ERRNO: " << errno << std::endl;
					std::cout << "\t\tInput: " << x << std::endl;
					std::cout << std::endl;
				}
				
			} else {

				if(!state.quiet) {
					std::cout << "\tSuccessful assert (n. "
					<< state.totalChecks
						<< "): ERRNO was set correctly" << std::endl;
				}

			}

		}


		/// Check errno value after function call
		template<typename Function, typename InputType>
		void check_errno(
			Function f,
			std::function<InputType(unsigned int)> generator,
			int exp_errno) {

			state.totalChecks++;

			auto x = generator(rand());

			try {
				volatile auto r = f(x);
				r = *(&r);
			} catch(...) {}

			if(errno != exp_errno) {

				state.failedChecks++;

				if(!state.quiet) {
					std::cout << "\tFailed assert (n. " << state.totalChecks << ")\n";
					std::cout << "\t\tExpected ERRNO: " << exp_errno << std::endl;
					std::cout << "\t\tEvaluated ERRNO: " << errno << std::endl;
					std::cout << std::endl;
				}
				
			} else {

				if(!state.quiet) {
					std::cout << "\tSuccessful assert (n. " << state.totalChecks
						<< "): ERRNO was set correctly" << std::endl;
				}

			}

		}


		/// Check errno value after function call
		template<typename Function, typename InputType>
		void check_errno(
			Function f,
			std::function<InputType(unsigned int)> generator,
			const std::vector<int>& exp_flags) {


			state.totalChecks++;

			auto x = generator(rand());
			bool all_set = true;

			try {
				volatile auto r = f(x);
				r = *(&r);
			} catch(...) {}

			for (unsigned int i = 0; i < exp_flags.size(); ++i) {
				if(!(errno & exp_flags[i]))
					all_set = false;
			}

			if(!all_set) {

				state.failedChecks++;

				if(!state.quiet) {
					std::cout << "\tFailed assert (n. " << state.totalChecks
						<< "): ERRNO was NOT set correctly" << std::endl;
					std::cout << std::endl;
				}
				
			} else {

				if(!state.quiet) {
					std::cout << "\tSuccessful assert (n. " << state.totalChecks
						<< "): ERRNO was set correctly" << std::endl;
				}

			}

		}


		/// Check that an exception is thrown during a function call
		template<typename Function, typename InputType>
		void check_exception(Function f, InputType x) {

			state.totalChecks++;

			bool thrown = false;

			try {
				volatile auto r = f(x);
				r = *(&r);
			} catch(...) {
				thrown = true;
			}

			if(!thrown) {

				state.failedChecks++;

				if(!state.quiet) {
					std::cout << "\tFailed assert (n. " << state.totalChecks << ")\n";
					std::cout << "\tNo exception was thrown" << std::endl;
					std::cout << std::endl;
				}
				
			} else if(!state.quiet) {
				std::cout << "\tSuccessful assert (n. " << state.totalChecks
					<< "): Exception was thrown correctly" << std::endl;
			}

		}


		/// Check that an exception is thrown during a function call
		template<typename Function, typename InputType>
		void check_exception(
			Function f, std::function<InputType(unsigned int)> generator) {

			state.totalChecks++;

			bool thrown = false;
			auto x = generator(rand());

			try {
				volatile auto r = f(x);
				r = *(&r);
			} catch(...) {
				thrown = true;
			}

			if(!thrown) {

				state.failedChecks++;

				if(!state.quiet) {
					std::cout << "\tFailed assert (n. " << state.totalChecks << "):\n";
					std::cout << "\tNo exception was thrown" << std::endl;
					std::cout << std::endl;
				}
				
			} else if(!state.quiet) {
				std::cout << "\tSuccessful assert (n. " << state.totalChecks
					<< "): Exception was thrown correctly" << std::endl;
			}

		}

	}

}

#endif
