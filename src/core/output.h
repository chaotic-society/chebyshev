
///
/// @file output.h The output module, with formatting capabilities.
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
#include "../err/err_structures.h"


namespace chebyshev {

	/// @namespace output Functions to manage printing results.
	namespace output {

		/// @class field_options
		/// Custom options for printing a certain field.
		struct field_options {

			using FieldFormat = std::function<void(std::ostream&, const field_options&)>;

			using FieldInterpreter = std::function<std::string(const std::string&)>;
			
			/// Width for the column of the field.
			unsigned int columnWidth = CHEBYSHEV_OUTPUT_WIDTH;

			/// A function which changes the state of the output stream
			/// before printing the field value (defaults to no change).
			FieldFormat fieldFormat = [](std::ostream& s, const field_options& o) {};

			/// A function which changes the state of the output stream
			/// before printing the field header title (defaults to no change).
			FieldFormat fieldTitleFormat = [](std::ostream& s, const field_options& o) {};

			/// A function which gets as input the value of a field as a string
			/// and returns a new string (e.g. "1" -> "FAIL" in the field "failed").
			FieldInterpreter fieldInterpreter = [](const std::string& s) { return s; };

			/// Additional custom options.
			std::map<std::string, long double> additionalFields {};
		};


		/// @class table_state
		/// A structure holding the state of an output table.
		struct table_state {

			/// Index of the current row, 0 is for headers.
			unsigned int rowIndex = 0;

			/// Whether the table data has finished printing
			/// and the current row is the last.
			bool isLastRow = false;
			
			/// Additional custom fields.
			std::map<std::string, long double> additionalFields {};
		};


		/// @class output_state
		/// Global state of printing results to standard output.
		struct output_state {

			using OutputFormat_t = std::function<
			std::string(
				const std::vector<std::string>&,
				const std::vector<std::string>&,
				const table_state&,
				const output_state&)>;

			/// Map of field name to output string
			/// (e.g. "maxErr" -> "Max Err.").
			std::map<std::string, std::string> fieldNames {};

			/// Options for the different fields.
			std::map<std::string, field_options> fieldOptions {};

			/// The output files, indexed by filename
			std::map<std::string, std::ofstream> outputFiles {};

			/// Default width for a field.
			unsigned int defaultColumnWidth = CHEBYSHEV_OUTPUT_WIDTH;

			/// The number of digits to show in scientific notation.
			unsigned int outputPrecision = 1;

			/// A function which converts the table entries of a row
			/// to a string to print (e.g. adding separators and padding)
			/// to standard output.
			OutputFormat_t outputFormat {};

			/// The default output format to use for files,
			/// when no format has been set.
			OutputFormat_t defaultFileOutputFormat {};

			/// The output format to use for a specific file, by filename.
			std::map<std::string, OutputFormat_t> fileOutputFormat {};

			/// Whether to output to standard output.
			bool quiet = false;

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
					const table_state& table,
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
							if(table.rowIndex)
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


					// Add header outline
					if(table.rowIndex == 0) {

						std::string line = s.str();
						std::string outline = " +";
						for (size_t i = 4; i < line.size(); ++i)
							outline += "-";
						outline += "+";

						return outline + "\n" + line + "\n" + outline;
					}

					// Add a newline between tables
					if(table.isLastRow) {

						std::string line = s.str();
						std::string outline = " +";
						for (size_t i = 4; i < line.size(); ++i)
							outline += "-";
						outline += "+";

						return line + "\n" + outline;
					}

					return s.str();
				};
			}


			/// Fancy output format using Unicode characters
			/// to print a continuous outline around the table.
			/// The OutputFormat is returned as a lambda function.
			inline OutputFormat fancy() {

				return [=](
					const std::vector<std::string>& values,
					const std::vector<std::string>& fields,
					const table_state& table,
					const output_state& state) -> std::string {

					if(values.size() != fields.size()) {
						throw std::runtime_error(
							"values and fields arguments must have the "
							"same size in format::fancy");
					}

					// Effective length of the string
					// (needed because Unicode is used)
					size_t eff_length = 0;
					std::stringstream s;
					
					s << " │ ";
					eff_length += 3;

					for (unsigned int i = 0; i < values.size(); ++i) {
						
						// Search for custom options for this field
						const auto opt_it = state.fieldOptions.find(fields[i]);

						if(opt_it != state.fieldOptions.end()) {
							
							// Format table value
							if(table.rowIndex)
								opt_it->second.fieldFormat(s, opt_it->second);
							// Format column title
							else
								opt_it->second.fieldTitleFormat(s, opt_it->second);
						}

						// Adjust field width with a custom value
						// if it exists, or the default otherwise.
						if(opt_it != state.fieldOptions.end()) {
							s << std::setw(opt_it->second.columnWidth);
							eff_length += opt_it->second.columnWidth;
						} else {
							s << std::setw(state.defaultColumnWidth);
							eff_length += state.defaultColumnWidth;
						}

						s << values[i];
						s << " │ ";
						eff_length += 3;
					}

					// Resulting line of formatted text
					const std::string line = s.str();

					// Add header outline
					if(table.rowIndex == 0) {

						std::string upper = " ┌";
						std::string lower = " ├";

						for (unsigned int i = 4; i < eff_length; ++i) {
							upper += "─";
							lower += "─";
						}

						upper += "┐ ";
						lower += "┤ ";

						return upper + "\n" + line + "\n" + lower;
					}

					// Add outline at the bottom of the table
					if(table.isLastRow) {

						std::string outline = " └";
						for (unsigned int i = 4; i < eff_length; ++i)
							outline += "─";
						outline += "┘ ";

						return line + "\n" + outline + "\n";
					}

					return line;
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
					const table_state& table,
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

				return [](
					const std::vector<std::string>& values,
					const std::vector<std::string>& fields,
					const table_state& table,
					const output_state& state) -> std::string {

					if(values.size() != fields.size()) {
						throw std::runtime_error(
							"values and fields arguments must have the "
							"same size in format::markdown");
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
					if(table.rowIndex == 0) {

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


			/// Format the table as a LaTeX table in the tabular environment.
			/// The OutputFormat is returned as a lambda function.
			///
			/// @warning The output of this format does not include the
			/// enclosing statement of the environment, which is "\end{tabular}"
			/// and it must be added for correct LaTeX output.
			inline OutputFormat latex() {

				return [](
					const std::vector<std::string>& values,
					const std::vector<std::string>& fields,
					const table_state& table,
					const output_state& state) -> std::string {

					if(values.size() != fields.size()) {
						throw std::runtime_error(
							"values and fields arguments must have the "
							"same size in format::latex");
					}


					std::stringstream s;

					for (unsigned int i = 0; i < values.size(); ++i) {

						// TO-DO Escape characters such as &, $ and _

						s << values[i];

						if(i != values.size() - 1)
							s << " & ";
						else
							s << " \\\\";
					}


					// Print environment setup
					if(table.rowIndex == 0) {

						std::string header = "\\begin{tabular}{";

						if(values.size())
							header += "|";

						for (unsigned int i = 0; i < values.size(); ++i)
							header += "c|";

						header += "}\n";
						header += s.str();
						header += "\n\\hline";

						return header;
					}


					// Close tabular environment on last row
					if(table.isLastRow)
						s << "\n\\end{tabular}";

					return s.str();
				};
			}

		}


		/// Setup printing to the output stream with default options.
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

			// Error checking
			state.fieldNames["correctType"] = "Correct Type";
			state.fieldNames["description"] = "Description";
			state.fieldNames["expectedFlags"] = "Exp. Flags";
			state.fieldNames["thrown"] = "Has Thrown";

			// Set wider column width for some fields
			state.fieldOptions["funcName"].columnWidth = 16;
			state.fieldOptions["averageRuntime"].columnWidth = 14;
			state.fieldOptions["runsPerSecond"].columnWidth = 14;
			state.fieldOptions["description"].columnWidth = 20;

			// Set a special field interpreter for the "failed" field
			state.fieldOptions["failed"].fieldInterpreter = [](const std::string& s) {
				if(s == "0") return "PASS";
				else if(s == "1") return "FAIL";
				else return "UNKNOWN";
			};

			// Set the default formats
			state.outputFormat = format::fancy();
			state.defaultFileOutputFormat = format::csv();

			state.wasSetup = true;
		}


		/// Terminate the output module by closing all output files
		/// and resetting its state.
		inline void terminate() {

			for (auto& p : state.outputFiles)
				if(p.second.is_open())
					p.second.close();

			state.outputFiles.clear();
			state = output_state();
		}


		/// Resolve the field of an estimate result by name,
		/// returning the value as a string.
		///
		/// @param fieldName The name of the field to resolve
		/// @param r The estimate result to read the fields of
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
		///
		/// @param fieldName The name of the field to resolve
		/// @param r The equation result to read the fields of
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


		/// Resolve the field of an assertion result by name,
		/// returning the value as a string.
		///
		/// @param fieldName The name of the field to resolve
		/// @param r The assertion result to read the fields of
		inline std::string resolve_field(
			const std::string& fieldName, err::assert_result r) {

			std::stringstream value;

			if(fieldName == "funcName") {
				value << r.funcName;
			} else if(fieldName == "evaluated") {
				value << r.evaluated;
			} else if(fieldName == "description") {
				value << r.description;
			} else if(fieldName == "failed") {
				value << r.failed;
			} else {
				return "";
			}

			return value.str();
		}


		/// Resolve the field of an errno checking result by name,
		/// returning the value as a string.
		///
		/// @param fieldName The name of the field to resolve
		/// @param r The errno checking result to read the fields of
		inline std::string resolve_field(
			const std::string& fieldName, err::errno_result r) {

			std::stringstream value;

			if(fieldName == "funcName") {
				value << r.funcName;
			} else if(fieldName == "evaluated") {
				value << r.evaluated;
			} else if(fieldName == "expectedFlags") {

				int res_flag = 0xFFFFFFFF;
				for (int flag : r.expectedFlags)
					res_flag &= flag;

				value << res_flag;

			} else if(fieldName == "failed") {
				value << r.failed;
			} else {
				return "";
			}

			return value.str();
		}


		/// Resolve the field of an exception checking result by name,
		/// returning the value as a string.
		///
		/// @param fieldName The name of the field to resolve
		/// @param r The exception checking result to read the fields of
		inline std::string resolve_field(
			const std::string& fieldName, err::exception_result r) {

			std::stringstream value;

			if(fieldName == "funcName") {
				value << r.funcName;
			} else if(fieldName == "thrown") {
				value << r.thrown;
			} else if(fieldName == "correctType") {
				value << r.correctType;
			} else if(fieldName == "failed") {
				value << r.failed;
			} else {
				return "";
			}

			return value.str();
		}


		/// Write the header of a table of results to all
		/// output streams with their corresponding format.
		///
		/// @param table The state of the result table
		/// @param columns A list of the fields to include in
		/// the table columns.
		inline void header(
			const table_state& table,
			std::vector<std::string> columns) {

			// Allocate and populate vector of titles
			// by associating display names to field names
			std::vector<std::string> titles (columns.size());

			for (unsigned int i = 0; i < columns.size(); ++i) {

				const auto field_it = state.fieldNames.find(columns[i]);

				// Associate string to field name
				if(field_it != state.fieldNames.end())
					titles[i] = field_it->second;
				else
					titles[i] = columns[i];
			}

			// Print to standard output if not quiet
			if(!state.quiet) {
				std::cout << state.outputFormat(titles, columns, table, state) << std::endl;
			}

			// Print to each file with its output format
			for (auto& p : state.outputFiles) {

				auto it = state.fileOutputFormat.find(p.first);

				if(it != state.fileOutputFormat.end())
					p.second << (it->second)(titles, columns, table, state) << std::endl;
				else
					p.second << state.defaultFileOutputFormat(titles, columns, table, state) << std::endl;

			}
		}


		/// Print a row of information about
		/// a result of arbitrary type (e.g. estimate_result).
		///
		/// @param res The result to write to output
		/// @param table The state of the table
		/// @param columns A list of the fields to include in
		/// the table columns.
		template<typename ResultType>
		inline void print(
			ResultType res,
			const table_state& table,
			std::vector<std::string> columns) {

			// Allocate vector of titles
			std::vector<std::string> values (columns.size());

			// Resolve fields to their values
			for (unsigned int i = 0; i < columns.size(); ++i) {

				auto opt_it = state.fieldOptions.find(columns[i]);

				// Apply a field interpreter if it is specified
				if(opt_it != state.fieldOptions.end()) {
					values[i] = opt_it->second.fieldInterpreter(
						resolve_field(columns[i], res));
				} else {
					values[i] = resolve_field(columns[i], res);
				}
			}

			// Print row to standard output
			if(!state.quiet) {
				std::cout << state.outputFormat(values, columns, table, state) << std::endl;
			}

			for (auto& p : state.outputFiles) {

				auto it = state.fileOutputFormat.find(p.first);

				if(it != state.fileOutputFormat.end())
					p.second << (it->second)(values, columns, table, state) << std::endl;
				else
					p.second << state.defaultFileOutputFormat(values, columns, table, state) << std::endl;
			}
		}


	}

}

#endif
