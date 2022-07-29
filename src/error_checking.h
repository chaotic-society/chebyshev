
///
/// @file error_handling.h Error handling checks
///

#pragma once

#include "definitions.h"
#include <vector>
#include <cstdlib>


#define AUTOASSERT(exp) chebyshev::err::assert(exp, #exp);


namespace chebyshev {

	/// @namespace chebyshev::err Error testing
	namespace err {

		///
		std::string module_name = "unknown";

		/// Print to standard output?
		bool quiet = false;

		// Total number of checks
		unsigned int total_checks = 0;

		/// Number of failed checks
		unsigned int failed_checks = 0;


		/// Setup error checking
		void setup(const std::string& module) {

			module_name = module;
			srand(time(nullptr));

			std::cout << "Starting error checking on " << module_name << " ...\n" << std::endl;
		}


		/// Terminate error checking
		void terminate(bool exit = true) {

			std::cout << "\nEnding error checking on " << module_name << " ..." << std::endl;
			std::cout << total_checks << " total checks, " << failed_checks << " failed ("
				<< (failed_checks / Real(total_checks) * 100) << "%)" << std::endl;

			if(exit)
				std::exit(failed_checks);
		}


		/// Assert that an expression is true
		void assert(bool exp, std::string desc = "") {

			total_checks++;

			if(!exp) {
				failed_checks++;

				if(!quiet) {
					std::cout << "\tFailed assert (n. " << total_checks << ")";

					if(desc != "")
						std::cout << ":\n\t\tDescription: " << desc << std::endl;
					else
						std::cout << " (no description provided)" << std::endl;

					std::cout << std::endl;
				}
				
			} else if(!quiet) {
				std::cout << "\tSuccessful assert (n. " << total_checks << ")";
				if(desc != "")
					std::cout << ":\n\t\tDescription: " << desc << std::endl;
				else
					std::cout << " (no description provided)" << std::endl;

				std::cout << std::endl;
			}
		}


		/// Check errno value after function call
		void check_errno(RealFunction f, Real x, int exp_errno) {

			total_checks++;

			try {
				volatile Real r = f(x);
				r += 1;
			} catch(...) {}

			if(errno != exp_errno) {

				failed_checks++;

				if(!quiet) {
					std::cout << "\tFailed assert (n. " << total_checks << ")" << std::endl;
					std::cout << "\t\tExpected ERRNO: " << exp_errno << std::endl;
					std::cout << "\t\tEvaluated ERRNO: " << errno << std::endl;
					std::cout << "\t\tInput: " << x << std::endl;
					std::cout << std::endl;
				}
				
			} else {

				if(!quiet) {
					std::cout << "\tSuccessful assert (n. " << total_checks
						<< "): ERRNO was set correctly" << std::endl;
				}

			}

		}


		/// Check errno value after function call
		void check_errno(RealFunction f, RealInputGenerator g, int exp_errno) {

			total_checks++;

			Real x = g(rand());

			try {
				volatile Real r = f(x);
				r += 1;
			} catch(...) {}

			if(errno != exp_errno) {

				failed_checks++;

				if(!quiet) {
					std::cout << "\tFailed assert (n. " << total_checks << ")" << std::endl;
					std::cout << "\t\tExpected ERRNO: " << exp_errno << std::endl;
					std::cout << "\t\tEvaluated ERRNO: " << errno << std::endl;
					std::cout << std::endl;
				}
				
			} else {

				if(!quiet) {
					std::cout << "\tSuccessful assert (n. " << total_checks
						<< "): ERRNO was set correctly" << std::endl;
				}

			}

		}


		/// Check errno value after function call
		void check_errno(RealFunction f, RealInputGenerator g, const std::vector<int>& exp_flags) {


			total_checks++;

			Real x = g(rand());
			bool all_set = true;

			try {
				volatile Real r = f(x);
				r += 1;
			} catch(...) {}

			for (unsigned int i = 0; i < exp_flags.size(); ++i) {
				if(!(errno & exp_flags[i]))
					all_set = false;
			}

			if(!all_set) {

				failed_checks++;

				if(!quiet) {
					std::cout << "\tFailed assert (n. " << total_checks
						<< "): ERRNO was NOT set correctly" << std::endl;
					std::cout << std::endl;
				}
				
			} else {

				if(!quiet) {
					std::cout << "\tSuccessful assert (n. " << total_checks
						<< "): ERRNO was set correctly" << std::endl;
				}

			}

		}


		/// Check that an exception is thrown during a function call
		void check_exception(RealFunction f, Real x) {

			total_checks++;

			bool thrown = false;

			try {
				volatile Real r = f(x);
				r += 1;
			} catch(...) {
				thrown = true;
			}

			if(!thrown) {

				failed_checks++;

				if(!quiet) {
					std::cout << "\tFailed assert (n. " << total_checks << ")" << std::endl;
					std::cout << "\tNo exception was thrown" << std::endl;
					std::cout << std::endl;
				}
				
			} else if(!quiet) {
				std::cout << "\tSuccessful assert (n. " << total_checks
					<< "): Exception was thrown correctly" << std::endl;
			}

		}


		/// Check that an exception is thrown during a function call
		void check_exception(RealFunction f, RealInputGenerator g) {

			total_checks++;

			bool thrown = false;
			Real x = g(rand());

			try {
				volatile Real r = f(x);
				r += 1;
			} catch(...) {
				thrown = true;
			}

			if(!thrown) {

				failed_checks++;

				if(!quiet) {
					std::cout << "\tFailed assert (n. " << total_checks << "):" << std::endl;
					std::cout << "\tNo exception was thrown" << std::endl;
					std::cout << std::endl;
				}
				
			} else if(!quiet) {
				std::cout << "\tSuccessful assert (n. " << total_checks
					<< "): Exception was thrown correctly" << std::endl;
			}

		}

	}

}
