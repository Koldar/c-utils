/**
 * @file
 *
 * Represents a module allowing you to produce csv files
 *
 * Using this module is pretty easy:
 *
 * @code
 * //create a csv with 3 columns, namely number (integer), value (integet) and description (string)
 * //we open the file in overwrite mode
 * csv_helper* csvHelper = cuCSVHelperNew("test.csv", ',', '\n', "%d %d %s", {"number", "value", "description"}, "w");
 * //adds 2 rows in the csv "test.csv"
 * cuCSVHelperprintDataRow(csvHelper, 0, 4, "first data");
 * cuCSVHelperprintDataRow(csvHelper, 1, 5, "second data");
 * //close the csv
 * cuCSVHelperDestroy(csvHelper);
 * @endcode
 *
 * @date Dec 26, 2016
 * @author koldar
 */

#ifndef CSVPRODUCER_H_
#define CSVPRODUCER_H_

#include <stdbool.h>
#include <stdio.h>
#include "macros.h"
#include "var_args.h"

/**
 * The main structure used to fully use the functions inside the module
 */
typedef struct csv_helper {
	/**
	 * The file path to the csv we want to write on
	 */
	const char* filePath;
	/**
	 * a reference to the file pointer ::csv_helper::filePath
	 */
	FILE* csvFile;
	/**
	 * a reference to an array of strings representing the headers to be put into the CSV
	 */
	const char** headerNames;
	/**
	 * A string containing the templates
	 *
	 * like template in the constructor, but instead of spaces the format specifiers are divided with ::csv_helper::delimiter
	 */
	char* actualTemplate;
	/**
	 * the number of string representing the header
	 */
	int headerSize;
	/**
	 * The character used to separate different columns
	 */
	char delimiter;
	/**
	 * the character used to separate different rows
	 */
	char carriageReturn;
	/**
	 * true if the software already printed the header anad its options, false otherwise
	 */
	bool alreadyPrintedHeader;
	/**
	 * When using ::cuCSVHelperPrintSingleDataInRow, determine which is the column that we need to print
	 */
	int nextColumnIndex;
} csv_helper;


/**
 * Creates a new helper
 *
 * @param[in] filePath a file to the csv that the software will create. Avoid using the *.csv extension inside this argument
 * @param[in] delimiter the character to put between one column and another one
 * @param[in] carriageReturn the character to put at the end of each row
 * @param[in] template a stirng containing how you want to format the data for each header. The string should contain only \c printf formatters.
 * 				For example if you want to format a csv with 5 fields where the first is an int, the second is a string and the others are float, you
 * 				have to write <tt>%d %s %f %3.2f %5f</tt>. Note that you can use every printf specifier (like <tt>%3.2f</tt> in the example)
 * @param[in] header a sequence of strings (<tt>char*</tt>) representing the header. You should use a constant array, like <tt>{"foo", "bar", "hello"}</tt>.
 * @param[in] openFormat how you would like to open the file. Use "a" for append mode or "w" for write mode. Other values will reult in unknown behaviour
 * @return the helper structure
 */
csv_helper* cuCSVHelperNew(const char* filePath, char delimiter, char carriageReturn, const char* template, const char* header[], const char* openFormat);

/**
 * Free the memory from the helper
 *
 * @param[in] csvHelper the structure to remove from the memory
 */
void cuCSVHelperDestroy(CU_NOTNULL const csv_helper* csvHelper, CU_NULLABLE const struct var_args* context);
#define CU_FUNCTION_POINTER_destructor_void_cuCSVHelperDestroy_voidConstPtr_var_argsConstPtr CU_DESTRUCTOR_ID

/**
 * Prints a data row inside the csv
 *
 * @param[in] csvHelper the structure that allows you to easily print data on the csv
 * @param[in] ... the data to print.. It is of the same size of the header
 */
void cuCSVHelperprintDataRow(csv_helper* csvHelper, ...);

/**
 * Print the CSV header inside ::csv_helper::csvFile if not already done
 *
 * @private
 * \note
 * 	\li if the header has been already printed into the file, nothing is performed
 *
 * \post
 * 	\li :csv_helper::csvFile contains options and header
 *
 * @param[in] csvHelper the helper referencing a CSV file
 */
void _cuCSVHelperTryPrintHeader(csv_helper* csvHelper);

/**
 * get the n-th format specifier inside \c template
 *
 * @param[in] helper the helper involved
 * @param[in] n the index of the format specfier we wantt o fetch. Indexes start from 0
 * @param[out] buffer a buffer that will contain the nth format specifier needed
 */
void cuCSVHelperComputeNColumnSpecifier(const csv_helper* helper, int n, char* buffer);

/**
 * Print a row inside the csv
 *
 * \note
 * 	if the header has not been printed yet, it also prints the header
 *
 *
 * \pre
 * 	\li \c csvHelper of type ::csv_helper;
 * 	\li the number of data in the \a vararg is the same of the size of the header;
 *
 * \attention
 * 	Be particularly aware of the preconditions. Since this function is a <b>macro function</b>, the compiler won't be able to help
 * 	you out
 *
 * @param[in] csvHelper the support function
 * @param[in] ... a list of data to print in the csv file
 */
#define cuCSVHelperprintDataRow(csvHelper, ...) \
		_cuCSVHelperTryPrintHeader(((csv_helper*)csvHelper)); \
		fprintf(((csv_helper*)csvHelper)->csvFile, ((csv_helper*)csvHelper)->actualTemplate,##__VA_ARGS__); \
		fprintf(((csv_helper*)csvHelper)->csvFile, "%c", ((csv_helper*)csvHelper)->carriageReturn)


/**
 * Like ::cuCSVHelperprintDataRow but instead of printing a whole row it prints the next element in the row.
 *
 * For example, if the template is <tt>%d %s %.1f%</tt> and the delimiter is <tt>:</tt>
 * you can print a whole row (thereby creating the same output of a ingle call of ::cuCSVHelperprintDataRow)
 * via:
 *
 * @code
 * 	cuCSVHelperPrintSingleDataInRow(csvHelper, 5); //the function prints "5:" in the csv
 * 	cuCSVHelperPrintSingleDataInRow(csvHelper, "hello"); //the function prints "hello:" in the csv
 * 	cuCSVHelperPrintSingleDataInRow(csvHelper, 3.4); //the function prints "3.4\n" in the csv
 * @endcode
 *
 * @param[in] csvHelper a support structure to help you write csvs
 * @param[in] data the data to print. Ensure you're writing data compliant with the template!
 */
#define cuCSVHelperPrintSingleDataInRow(csvHelper, data) { \
		char _buffer[10]; \
		csv_helper* _csvHelper = (csvHelper); \
		_cuCSVHelperTryPrintHeader(_csvHelper); \
		cuCSVHelperComputeNColumnSpecifier(_csvHelper, _csvHelper->nextColumnIndex, _buffer); \
		debug("buffer: \"%s\"", _buffer); \
		fprintf(_csvHelper->csvFile, _buffer, data); \
		_csvHelper->nextColumnIndex++; \
		if (_csvHelper->nextColumnIndex < _csvHelper->headerSize) { \
			fprintf(_csvHelper->csvFile, "%c", _csvHelper->delimiter); \
		} else { \
			fprintf(_csvHelper->csvFile, "%c", _csvHelper->carriageReturn); \
			_csvHelper->nextColumnIndex = 0; \
		} \
		}


#endif /* CSVPRODUCER_H_ */
