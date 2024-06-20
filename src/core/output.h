
///
/// @file printing.h
///

#ifndef CHEBYSHEV_OUTPUT_H
#define CHEBYSHEV_OUTPUT_H

#include <vector>
#include <string>
#include <map>
#include <sstream>
#include <iomanip>

#include "../prec/prec_structures.h"
#include "../benchmark/benchmark_structures.h"


namespace chebyshev {

	/// @namespace output Functions to manage printing results.
	namespace output {


		struct field_options {
			
			// Width for the column of the field
			unsigned int columnWidth = CHEBYSHEV_OUTPUT_WIDTH;

			// ...
		};


		/// Global state of printing results to standard output.
		struct output_state {

			/// Map of field name to output string
			/// (e.g. "maxErr" -> "Max Err.").
			std::map<std::string, std::string> fieldNames {};

			/// Default columns to print for precision estimates.
			std::vector<std::string> estimateColumns = {
				"funcName", "meanErr", "rmsErr", "maxErr", "failed"
			};

			/// Default columns to print for equations.
			std::vector<std::string> equationColumns = {
				"funcName", "difference", "tolerance", "failed"
			};

			/// Default columns to print for benchmarks.
			std::vector<std::string> benchmarkColumns = {
				"funcName", "averageRuntime", "runsPerSecond"
			};

			/// Options for the different fields.
			std::map<std::string, field_options> fieldOptions {};

			/// Default width for a field.
			unsigned int defaultColumnWidth = CHEBYSHEV_OUTPUT_WIDTH;

			/// The number of digits to show in scientific notation.
			unsigned int outputPrecision = 1;

			/// Whether the output module was setup.
			bool wasSetup = false;

		} state;


		/// Setup printing to the output stream.
		inline void setup() {

			// Estimate fields
			state.fieldNames["funcName"] = "Function";
			state.fieldNames["maxErr"] = "Max Err.";
			state.fieldNames["meanErr"] = "Mean Err.";
			state.fieldNames["rmsErr"] = "RMS Err.";
			state.fieldNames["relErr"] = "Rel. Err.";
			state.fieldNames["absErr"] = "Abs. Err.";
			state.fieldNames["tolerance"] = "Tolerance";
			state.fieldNames["failed"] = "Failed";
			state.fieldNames["iterations"] = "Iterations";

			// Equation fields
			state.fieldNames["difference"] = "Difference";
			state.fieldNames["evaluated"] = "Evaluated";
			state.fieldNames["expected"] = "Expected";

			// Benchmark fields
			state.fieldNames["totalRuntime"] = "Tot. Time (ms)";
			state.fieldNames["averageRuntime"] = "Avg. Time (ms)";
			state.fieldNames["runsPerSecond"] = "Runs per Sec.";
			state.fieldNames["runs"] = "Runs";

			// Set wider column width for some fields
			state.fieldOptions["funcName"].columnWidth = 16;
			state.fieldOptions["averageRuntime"].columnWidth = 14;

			state.wasSetup = true;
		}


		/// Resolve the field of an estimate result by name,
		/// returning the value as a string.
		inline std::string resolve_field(
			const std::string& fieldName, prec::estimate_result r) {

			std::stringstream value;

			if(fieldName == "funcName") {
				value << r.funcName;
			} else if(fieldName == "maxErr") {
				value << std::scientific
					<< std::setprecision(state.outputPrecision)
					<< r.maxErr;
			} else if(fieldName == "meanErr") {
				value << std::scientific
					<< std::setprecision(state.outputPrecision)
					<< r.meanErr;
			} else if(fieldName == "rmsErr") {
				value << std::scientific
					<< std::setprecision(state.outputPrecision)
					<< r.rmsErr;
			} else if(fieldName == "relErr") {
				value << std::scientific
					<< std::setprecision(state.outputPrecision)
					<< r.relErr;
			} else if(fieldName == "absErr") {
				value << std::scientific
					<< std::setprecision(state.outputPrecision)
					<< r.absErr;
			} else if(fieldName == "tolerance") {
				value << std::scientific
					<< std::setprecision(state.outputPrecision)
					<< r.tolerance;
			} else if(fieldName == "failed") {
				value << r.failed;
			} else {
				
				if(r.additionalFields.find(fieldName) == r.additionalFields.end())
					return "";

				value << r.additionalFields[fieldName];
			}

			return value.str();
		}


		/// Resolve the field of an equation result by name,
		/// returning the value as a string.
		inline std::string resolve_field(
			const std::string& fieldName, prec::equation_result r) {

			std::stringstream value;

			if(fieldName == "funcName") {
				value << r.funcName;
			} else if(fieldName == "evaluated") {
				value << r.evaluated;
			} else if(fieldName == "expected") {
				value << r.expected;
			} else if(fieldName == "difference") {
				value << std::scientific
					<< std::setprecision(state.outputPrecision)
					<< r.difference;
			} else if(fieldName == "tolerance") {
				value << std::scientific
					<< std::setprecision(state.outputPrecision)
					<< r.tolerance;
			} else if(fieldName == "failed") {
				value << r.failed;
			} else {
				
				if(r.additionalFields.find(fieldName) == r.additionalFields.end())
					return "";

				value << r.additionalFields[fieldName];
			}

			return value.str();
		}


		/// Resolve the field of a benchmark result by name,
		/// returning the value as a string.
		inline std::string resolve_field(
			const std::string& fieldName, benchmark::benchmark_result r) {

			std::stringstream value;

			if(fieldName == "funcName") {
				value << r.funcName;
			} else if(fieldName == "runs") {
				value << r.runs;
			} else if(fieldName == "iterations") {
				value << r.iterations;
			} else if(fieldName == "totalRuntime") {
				value << r.totalRuntime;
			} else if(fieldName == "averageRuntime") {
				value << r.averageRuntime;
			} else if(fieldName == "runsPerSecond") {
				value << r.runsPerSecond;
			} else if(fieldName == "failed") {
				value << r.failed;
			} else {
				
				if(r.additionalFields.find(fieldName) == r.additionalFields.end())
					return "";

				value << r.additionalFields[fieldName];
			}

			return value.str();
		}


		/// Print the header of a table for estimate results,
		/// with the given column names.
		inline void header_estimate(
			std::ostream& outputStream = std::cout,
			const std::string& separator = " | ",
			bool adjustWidth = true,
			std::vector<std::string> columns = state.estimateColumns) {

			// Print the chosen columns by using state.fieldNames
			for (size_t i = 0; i < columns.size(); ++i) {

				if(adjustWidth) {
					if(state.fieldOptions.find(columns[i]) != state.fieldOptions.end())
						outputStream << std::setw(state.fieldOptions[columns[i]].columnWidth);
					else
						outputStream << std::setw(state.defaultColumnWidth);
				}

				if(state.fieldNames.find(columns[i]) == state.fieldNames.end())
					outputStream << columns[i];
				else
					outputStream << state.fieldNames[columns[i]];

				if(i != columns.size() - 1)
					outputStream << separator;
			}

			outputStream << std::endl;
		}


		/// Print the header of a table for equation results,
		/// with the given column names.
		inline void header_equation(
			std::ostream& outputStream = std::cout,
			const std::string& separator = " | ",
			bool adjustWidth = true,
			std::vector<std::string> columns = state.equationColumns) {

			// Print the chosen columns by using state.fieldNames
			for (size_t i = 0; i < columns.size(); ++i) {

				if(adjustWidth) {
					if(state.fieldOptions.find(columns[i]) != state.fieldOptions.end())
						outputStream << std::setw(state.fieldOptions[columns[i]].columnWidth);
					else
						outputStream << std::setw(state.defaultColumnWidth);
				}

				if(state.fieldNames.find(columns[i]) == state.fieldNames.end())
					outputStream << columns[i];
				else
					outputStream << state.fieldNames[columns[i]];

				if(i != columns.size() - 1)
					outputStream << separator;
			}

			outputStream << std::endl;
		}


		/// Print the header of a table for benchmark results,
		/// with the given column names.
		inline void header_benchmark(
			std::ostream& outputStream = std::cout,
			const std::string& separator = " | ",
			bool adjustWidth = true,
			std::vector<std::string> columns = state.benchmarkColumns) {

			// Print the chosen columns by using state.fieldNames
			for (size_t i = 0; i < columns.size(); ++i) {

				if(adjustWidth) {
					if(state.fieldOptions.find(columns[i]) != state.fieldOptions.end())
						outputStream << std::setw(state.fieldOptions[columns[i]].columnWidth);
					else
						outputStream << std::setw(state.defaultColumnWidth);
				}

				if(state.fieldNames.find(columns[i]) == state.fieldNames.end())
					outputStream << columns[i];
				else
					outputStream << state.fieldNames[columns[i]];

				if(i != columns.size() - 1)
					outputStream << separator;
			}

			outputStream << std::endl;
		}


		/// Print an estimate result as a table row.
		inline void print_estimate(
			prec::estimate_result res,
			std::ostream& outputStream = std::cout,
			const std::string& separator = " | ",
			bool adjustWidth = true,
			std::vector<std::string> columns = state.estimateColumns) {

			// Print the chosen columns
			for (size_t i = 0; i < columns.size(); ++i) {

				if(adjustWidth) {
					if(state.fieldOptions.find(columns[i]) != state.fieldOptions.end())
						outputStream << std::setw(state.fieldOptions[columns[i]].columnWidth);
					else
						outputStream << std::setw(state.defaultColumnWidth);
				}

				outputStream << resolve_field(columns[i], res);

				if(i != columns.size() - 1)
					outputStream << separator;
			}

			outputStream << std::endl;

		}


		/// Print an equation result as a table row.
		inline void print_equation(
			prec::equation_result res,
			std::ostream& outputStream = std::cout,
			const std::string& separator = " | ",
			bool adjustWidth = true,
			std::vector<std::string> columns = state.equationColumns) {

			// Print the chosen columns
			for (size_t i = 0; i < columns.size(); ++i) {


				if(adjustWidth) {
					if(state.fieldOptions.find(columns[i]) != state.fieldOptions.end())
						outputStream << std::setw(state.fieldOptions[columns[i]].columnWidth);
					else
						outputStream << std::setw(state.defaultColumnWidth);
				}

				outputStream << resolve_field(columns[i], res);

				if(i != columns.size() - 1)
					outputStream << separator;
			}

			outputStream << std::endl;
		}


		/// Print a benchmark result as a table row.
		inline void print_benchmark(
			benchmark::benchmark_result res,
			std::ostream& outputStream = std::cout,
			const std::string& separator = " | ",
			bool adjustWidth = true,
			std::vector<std::string> columns = state.benchmarkColumns) {

			// Print the chosen columns
			for (size_t i = 0; i < columns.size(); ++i) {


				if(adjustWidth) {
					if(state.fieldOptions.find(columns[i]) != state.fieldOptions.end())
						outputStream << std::setw(state.fieldOptions[columns[i]].columnWidth);
					else
						outputStream << std::setw(state.defaultColumnWidth);
				}

				outputStream << resolve_field(columns[i], res);

				if(i != columns.size() - 1)
					outputStream << separator;
			}

			outputStream << std::endl;
		}


	}

}

#endif
