
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


		/// @class field_options
		/// Custom options for printing a certain field.
		struct field_options {

			using FieldFormat = std::function<void(std::ostream&, const field_options&)>;
			
			/// Width for the column of the field.
			unsigned int columnWidth = CHEBYSHEV_OUTPUT_WIDTH;

			/// A function which changes the state of the output stream
			/// before printing the field value (defaults to no change).
			FieldFormat fieldFormat = [](std::ostream& s, const field_options& o) {};

			/// A function which changes the state of the output stream
			/// before printing the field header title (defaults to no change).
			FieldFormat fieldTitleFormat = [](std::ostream& s, const field_options& o) {};

			/// Additional custom options.
			std::map<std::string, long double> additionalFields {};

		};


		/// @class output_state
		/// Global state of printing results to standard output.
		struct output_state {

			using OutputFormat_t = std::function<
			std::string(
				const std::vector<std::string>&,
				const std::vector<std::string>&,
				unsigned int,
				const output_state&)>;

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

			/// A function which converts the table entries of a row
			/// to a string to print (e.g. adding separators and padding).
			OutputFormat_t outputFormat {};

			/// The output format used for printing to file.
			OutputFormat_t fileOutputFormat {};

			/// Whether the output module was setup.
			bool wasSetup = false;

		} state;


		/// A function which converts the table entries of a row
		/// to a string to print (e.g. adding separators and padding).
		/// @see output_state::OutputFormat_t
		using OutputFormat = output_state::OutputFormat_t;


		/// @namespace format Output formatting functions.
		namespace format {

			/// Default output format which prints the fields
			/// separated by the separator string and padding, if enabled.
			/// The OutputFormat is returned as a lambda function.
			///
			/// @param separator The string to print between different fields.
			/// @param horizontal A character to print horizontal lines.
			/// @param adjustWidth Whether to add padding to the fields.
			inline OutputFormat simple(
				const std::string& separator = " | ",
				bool adjustWidth = true) {

				return [=](
					const std::vector<std::string>& values,
					const std::vector<std::string>& fields,
					unsigned int row_index,
					const output_state& state) -> std::string {

					if(values.size() != fields.size()) {
						throw std::runtime_error(
							"values and fields arguments must have the "
							"same size in format::simple");
					}

					std::stringstream s;
					s << separator;

					for (unsigned int i = 0; i < values.size(); ++i) {
						
						// Search for custom options for this field
						const auto opt_it = state.fieldOptions.find(fields[i]);

						if(opt_it != state.fieldOptions.end()) {
							
							// Format table value
							if(row_index)
								opt_it->second.fieldFormat(s, opt_it->second);
							// Format column title
							else
								opt_it->second.fieldTitleFormat(s, opt_it->second);
						}

						// Adjust field width with a custom value
						// if it exists, or the default otherwise.
						if(adjustWidth) {

							if(opt_it != state.fieldOptions.end())
								s << std::setw(opt_it->second.columnWidth);
							else
								s << std::setw(state.defaultColumnWidth);
						}

						s << values[i];
						s << separator;
					}

					return s.str();
				};
			}


			/// Format function for CSV format files.
			/// The OutputFormat is returned as a lambda function.
			///
			/// @param separator The string to print between
			/// different fields (defaults to ",").
			inline OutputFormat csv(
				const std::string& separator = ",") {

				return [=](
					const std::vector<std::string>& values,
					const std::vector<std::string>& fields,
					unsigned int row_index,
					const output_state& state) -> std::string {

					if(values.size() != fields.size()) {
						throw std::runtime_error(
							"values and fields arguments must have the "
							"same size in format::csv");
					}

					std::stringstream s;

					for (unsigned int i = 0; i < values.size(); ++i) {

						s << "\"" << values[i] << "\"";

						if(i != values.size() - 1)
							s << separator;
					}

					return s.str();
				};
			}


			/// Format the table as Markdown.
			/// The OutputFormat is returned as a lambda function.
			inline OutputFormat markdown() {

				return [=](
					const std::vector<std::string>& values,
					const std::vector<std::string>& fields,
					unsigned int row_index,
					const output_state& state) -> std::string {

					if(values.size() != fields.size()) {
						throw std::runtime_error(
							"values and fields arguments must have the "
							"same size in format::csv");
					}

					std::stringstream s;
					s << "|";

					for (unsigned int i = 0; i < values.size(); ++i) {

						const auto opt_it = state.fieldOptions.find(fields[i]);

						if(opt_it != state.fieldOptions.end())
								s << std::setw(opt_it->second.columnWidth);
							else
								s << std::setw(state.defaultColumnWidth);

						s << values[i] << "|";
					}


					// Print header underline
					if(row_index == 0) {

						std::string line = "|";						

						for (unsigned int i = 0; i < values.size(); ++i) {

							int width = state.defaultColumnWidth;;

							const auto opt_it = state.fieldOptions.find(fields[i]);

							if(opt_it != state.fieldOptions.end())
								width = opt_it->second.columnWidth;

							line += " ";

							for (int l = 0; l < width - 2; ++l)
								line += "-";
							
							line += " ";
							line += "|";
						}

						s << "\n" << line;
					}


					return s.str();
				};
			}

		}


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

			// Set the default formats
			state.outputFormat = format::simple();
			state.fileOutputFormat = format::csv();

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


		inline void header(
			std::ostream& outputStream,
			OutputFormat format,
			std::vector<std::string> columns) {

			// Allocate vector of titles
			std::vector<std::string> titles (columns.size());

			for (unsigned int i = 0; i < columns.size(); ++i) {

				const auto field_it = state.fieldNames.find(columns[i]);

				// Associate string to field name
				if(field_it != state.fieldNames.end())
					titles[i] = field_it->second;
				else
					titles[i] = columns[i];
			}

			// row_index = 0 is used for the header row
			outputStream << format(titles, columns, 0, state) << std::endl;
		}


		/// Print the header of a table for estimate results,
		/// with the given column names.
		inline void header_estimate(
			std::ostream& outputStream = std::cout,
			OutputFormat format = state.outputFormat,
			std::vector<std::string> columns = state.estimateColumns) {

			header(outputStream, format, columns);
		}


		/// Print the header of a table for estimate results,
		/// with the given column names.
		inline void header_estimate(
			std::ofstream& outputStream,
			OutputFormat format = state.fileOutputFormat,
			std::vector<std::string> columns = state.estimateColumns) {

			header(outputStream, format, columns);
		}


		/// Print the header of a table for equation results,
		/// with the given column names.
		inline void header_equation(
			std::ostream& outputStream = std::cout,
			OutputFormat format = state.outputFormat,
			std::vector<std::string> columns = state.equationColumns) {

			header(outputStream, format, columns);
		}


		/// Print the header of a table for equation results,
		/// with the given column names.
		inline void header_equation(
			std::ofstream& outputStream,
			OutputFormat format = state.fileOutputFormat,
			std::vector<std::string> columns = state.equationColumns) {

			header(outputStream, format, columns);
		}


		/// Print the header of a table for benchmark results,
		/// with the given column names.
		inline void header_benchmark(
			std::ostream& outputStream = std::cout,
			OutputFormat format = state.outputFormat,
			std::vector<std::string> columns = state.benchmarkColumns) {

			header(outputStream, format, columns);
		}


		/// Print the header of a table for benchmark results,
		/// with the given column names.
		inline void header_benchmark(
			std::ofstream& outputStream,
			OutputFormat format = state.fileOutputFormat,
			std::vector<std::string> columns = state.benchmarkColumns) {

			header(outputStream, format, columns);
		}


		/// Print a row of information about
		/// a result of arbitrary type.
		template<typename ResultType>
		inline void print_result(
			ResultType res,
			std::ostream& outputStream,
			OutputFormat format,
			std::vector<std::string> columns) {

			// Allocate vector of titles
			std::vector<std::string> values (columns.size());

			// Resolve fields to their values
			for (unsigned int i = 0; i < columns.size(); ++i)
				values[i] = resolve_field(columns[i], res);

			// row_index > 0 is used for table entries
			outputStream << format(values, columns, 1, state) << std::endl;
		}


		/// Print an estimate result as a table row.
		inline void print_estimate(
			prec::estimate_result res,
			std::ostream& outputStream = std::cout,
			OutputFormat format = state.outputFormat,
			std::vector<std::string> columns = state.estimateColumns) {

			print_result(res, outputStream, format, columns);
		}


		/// Print an estimate result as a table row.
		inline void print_estimate(
			prec::estimate_result res,
			std::ofstream& outputStream,
			OutputFormat format = state.fileOutputFormat,
			std::vector<std::string> columns = state.estimateColumns) {

			print_result(res, outputStream, format, columns);
		}


		/// Print an equation result as a table row.
		inline void print_equation(
			prec::equation_result res,
			std::ostream& outputStream = std::cout,
			OutputFormat format = state.outputFormat,
			std::vector<std::string> columns = state.equationColumns) {

			print_result(res, outputStream, format, columns);
		}


		/// Print an equation result as a table row.
		inline void print_equation(
			prec::equation_result res,
			std::ofstream& outputStream,
			OutputFormat format = state.fileOutputFormat,
			std::vector<std::string> columns = state.equationColumns) {

			print_result(res, outputStream, format, columns);
		}


		/// Print a benchmark result as a table row.
		inline void print_benchmark(
			benchmark::benchmark_result res,
			std::ostream& outputStream = std::cout,
			OutputFormat format = state.outputFormat,
			std::vector<std::string> columns = state.benchmarkColumns) {

			print_result(res, outputStream, format, columns);
		}


		/// Print a benchmark result as a table row.
		inline void print_benchmark(
			benchmark::benchmark_result res,
			std::ofstream& outputStream,
			OutputFormat format = state.fileOutputFormat,
			std::vector<std::string> columns = state.benchmarkColumns) {

			print_result(res, outputStream, format, columns);
		}


	}

}

#endif
