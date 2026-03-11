
///
/// @file log.h Logging system for the output module
///

#ifndef CHEBYSHEV_LOG_H
#define CHEBYSHEV_LOG_H


namespace chebyshev {

	/// @namespace chebyshev::output Functions to manage printing results.
	namespace output {

		/// Log levels for the output module
		enum class LogLevel {
			DEBUG,
			INFO,
			WARNING,
			ERROR
		};


        /// Convert a log level to a string for printing.
        std::string loglevel_to_string(LogLevel level) {
            switch (level) {
                case LogLevel::DEBUG: return "DEBUG";
                case LogLevel::INFO: return "INFO";
                case LogLevel::WARNING: return "WARNING";
                case LogLevel::ERROR: return "ERROR";
                default: return "UNKNOWN";
            }
        }
	}
}

#endif
