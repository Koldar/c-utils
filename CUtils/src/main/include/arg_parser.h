/**
 * @file
 *
 * Easy wrapper on getopt_long
 *
 * Thanks to this module, you can avoid knowing how getopt works and focus on your CLI arguments section.
 *
 * This allows you to generate simple, but wonderful CLI options
 * This is an example of creating the CLI parser:
 *
 * @code
 * int nodes;
 * int edges;
 *
 * cli_parser* parser = cuNewCliParser();
 * cuCliParserAddOptionalInt(parser, 'n', "nodes", 5, &nodes, "node");
 * cuCliParserAddOptionalInt(parser, 'e', "edges", 6, &edges, "edges");
 *
 * const char* args[1] = {"Example", "-e10"}; //no space between e and 10
 * const int argc = 2;
 * cuCliParserParseArguments(parser, argc, args);
 * //do somthing with the parsed values
 * cuDestroyCliParser(parser);
 * @endcode
 *
 *
 *  @date Jul 20, 2017
 *  @author koldar
 */

#ifndef ARG_PARSER_H_
#define ARG_PARSER_H_

#include <stdbool.h>
#include "macros.h"
#include "typedefs.h"
#include "var_args.h"

/**
 * The maximum number of options storable in the parser
 */
#ifndef CLIPARSER_MAX_OPTIONS_NUMBER
#	define CLIPARSER_MAX_OPTIONS_NUMBER 100
#endif

/**
 * The size of the buffer the optlong argument string will be positioned
 */
#ifndef CLIPARSER_MAX_OPTIONS_BUFFER_SIZE
#	define CLIPARSER_MAX_OPTIONS_BUFFER_SIZE (3* CLIPARSER_MAX_OPTIONS_NUMBER)
#endif

typedef struct cli_parser cli_parser;

/**
 * Initialize a new CLI parser
 *
 * @return the support structure allowing you to create CLI options with ease
 */
cli_parser* cuNewCliParser();

/**
 * Destroy a CLI support from the heap
 *
 * @param[in] p the support to destroy
 */
void cuDestroyCliParser(CU_NOTNULL const cli_parser* p, CU_NULLABLE const struct var_args* context);
#define CU_FUNCTION_POINTER_destructor_void_cuDestroyCliParser_voidConstPtr_var_argsConstPtr CU_DESTRUCTOR_ID

/**
 * Set the brief description of the command
 *
 * The variable is used within the help message
 *
 * @param[inout] p the parser involved
 * @param[in] brief the message
 */
void cuCliParserSetBriefDescription(CU_NOTNULL cli_parser* p, CU_NOTNULL const char* brief);

/**
 * Set the long description of the command
 *
 * The variable is used within the help message
 *
 * @param[inout] p the parser involved
 * @param[in] description the message
 */
void cuCliParserSetLongDescription(CU_NOTNULL cli_parser* p, CU_NOTNULL const char* description);

/**
 * **Main method** of the module. Perform the actual parsing
 *
 * @param[inout] p the parser involved
 * @param[in] argc the \c argc value from \c main
 * @param[in] args the \c args value from \c main
 * @return
 *  \li \c true
 */
bool cuCliParserParseArguments(CU_NOTNULL cli_parser* p, const int argc, CU_NOTNULL const char* const args[]);

/**
 * Add a CLI flag option
 *
 * For example, the following code create a verbose flag in the command line:
 *
 * @code
 * bool verbose;
 * cuCliParserAddFlag(p, 'v', "verbose", false, &verbose, "if present, we will enable verbose output");
 * @endcode
 *
 * CLI flags are by definition **optional**: they can either be present or not
 *
 * @param[inout] p the parser involved
 * @param[in] briefName a character **uniquely** identifying the option. Case sensitive. For example it might be <tt>'v'</tt>
 * @param[in] longName a string **uniquely identifying the option. Alias of \c briefName. For example it might be <tt>"verbose"</tt>
 * @param[in] defaultValue the value pointed by \c whereToStore has to store if the user doesn't specify this option
 * @param[inout] whereToStore a place where, at the end of the parsing, the value parsed (or the default one) is put
 * @param[in] description a description to show in the help
 */
void cuCliParserAddFlag(CU_NOTNULL cli_parser* p, char briefName, CU_NOTNULL const char* longName, bool defaultValue, CU_NOTNULL bool* whereToStore, CU_NOTNULL const char* description);

/**
 * An alias for ::cuCliParserAddFlag
 *
 * @param[inout] p the parser involved
 * @param[in] briefName a character **uniquely** identifying the option. Case sensitive. For example it might be <tt>'v'</tt>
 * @param[in] longName a string **uniquely identifying the option. Alias of \c briefName. For example it might be <tt>"verbose"</tt>
 * @param[in] defaultValue the value pointed by \c whereToStore has to store if the user doesn't specify this option
 * @param[inout] whereToStore a place where, at the end of the parsing, the value parsed (or the default one) is put
 * @param[in] description a description to show in the help
 */
void cuCliParserAddBoolOption(CU_NOTNULL cli_parser* p, char briefName, CU_NOTNULL const char* longName, bool defaultValue, CU_NOTNULL bool* whereToStore, CU_NOTNULL const char* description);

/**
 * Add a CLI option whose value is an int. The option may appear in the command line or not
 *
 * For example, the following code create a verbose flag in the command line:
 *
 * @code
 * int size;
 * cuCliParserAddOptionalInt(p, 'n', "size", 10, &size, "the size of the problem to test");
 * @endcode
 *
 * CLI flags are by definition **optional**: they can either be present or not
 *
 * @param[inout] p the parser involved
 * @param[in] briefName a character **uniquely** identifying the option. Case sensitive. For example it might be <tt>'v'</tt>
 * @param[in] longName a string **uniquely identifying the option. Alias of \c briefName. For example it might be <tt>"verbose"</tt>
 * @param[in] defaultValue the value pointed by \c whereToStore has to store if the user doesn't specify this option
 * @param[inout] whereToStore a place where, at the end of the parsing, the value parsed (or the default one) is put
 * @param[in] description a description to show in the help
 */
void cuCliParserAddOptionalInt(CU_NOTNULL cli_parser* p, char briefName, CU_NOTNULL const char* longName, int defaultValue, CU_NOTNULL int* whereToStore, CU_NOTNULL const char* description);

/**
 * Add a CLI option whose value is a long. The option may appear in the command line or not
 *
 * For example, the following code create a verbose flag in the command line:
 *
 * @code
 * long size;
 * cuCliParserAddOptionalLong(p, 'n', "size", 10, &size, "the size of the problem to test");
 * @endcode
 *
 * CLI flags are by definition **optional**: they can either be present or not
 *
 * @param[inout] p the parser involved
 * @param[in] briefName a character **uniquely** identifying the option. Case sensitive. For example it might be <tt>'v'</tt>
 * @param[in] longName a string **uniquely identifying the option. Alias of \c briefName. For example it might be <tt>"verbose"</tt>
 * @param[in] defaultValue the value pointed by \c whereToStore has to store if the user doesn't specify this option
 * @param[inout] whereToStore a place where, at the end of the parsing, the value parsed (or the default one) is put
 * @param[in] description a description to show in the help
 */
void cuCliParserAddOptionalLong(CU_NOTNULL cli_parser* p, char briefName, CU_NOTNULL const char* longName, long defaultValue, CU_NOTNULL long* whereToStore, CU_NOTNULL const char* description);

/**
 * Add a CLI option whose value is a <tt>char\*</tt>. The option may appear in the command line or not
 *
 * For example, the following code create a verbose flag in the command line:
 *
 * @code
 * char* impl;
 * cuCliParserAddOptionalString(p, 'n', "implementation", "default", &impl, "the implementation of the module creating the graph you want to use");
 * @endcode
 *
 * CLI flags are by definition **optional**: they can either be present or not
 *
 * @param[inout] p the parser involved
 * @param[in] briefName a character **uniquely** identifying the option. Case sensitive. For example it might be <tt>'v'</tt>
 * @param[in] longName a string **uniquely identifying the option. Alias of \c briefName. For example it might be <tt>"verbose"</tt>
 * @param[in] defaultValue the value pointed by \c whereToStore has to store if the user doesn't specify this option
 * @param[inout] whereToStore a place where, at the end of the parsing, the value parsed (or the default one) is put
 * @param[in] description a description to show in the help
 */
void cuCliParserAddOptionalString(CU_NOTNULL cli_parser* p, char briefName, CU_NOTNULL const char* longName, CU_NOTNULL const char* defaultValue, CU_NOTNULL char** whereToStore, CU_NOTNULL const char* description);

/**
 * Add a CLI option whose value is a float. The option may appear in the command line or not
 *
 * For example, the following code create a verbose flag in the command line:
 *
 * @code
 * float density;
 * cuCliParserAddOptionalFloat(p, 'd', "density", 1.0, &density, "the density of the graph to build");
 * @endcode
 *
 * CLI flags are by definition **optional**: they can either be present or not
 *
 * @param[inout] p the parser involved
 * @param[in] briefName a character **uniquely** identifying the option. Case sensitive. For example it might be <tt>'v'</tt>
 * @param[in] longName a string **uniquely identifying the option. Alias of \c briefName. For example it might be <tt>"verbose"</tt>
 * @param[in] defaultValue the value pointed by \c whereToStore has to store if the user doesn't specify this option
 * @param[inout] whereToStore a place where, at the end of the parsing, the value parsed (or the default one) is put
 * @param[in] description a description to show in the help
 */
void cuCliParserAddOptionalFloat(CU_NOTNULL cli_parser* p, char briefName, CU_NOTNULL const char* longName, float defaultValue, CU_NOTNULL float* whereToStore, CU_NOTNULL const char* description);

/**
 * Add a CLI option whose value is a double. The option may appear in the command line or not
 *
 * For example, the following code create a verbose flag in the command line:
 *
 * @code
 * double density;
 * cuCliParserAddOptionalDouble(p, 'd', "density", 1.0, &density, "the density of the graph to build");
 * @endcode
 *
 * CLI flags are by definition **optional**: they can either be present or not
 *
 * @param[inout] p the parser involved
 * @param[in] briefName a character **uniquely** identifying the option. Case sensitive. For example it might be <tt>'v'</tt>
 * @param[in] longName a string **uniquely identifying the option. Alias of \c briefName. For example it might be <tt>"verbose"</tt>
 * @param[in] defaultValue the value pointed by \c whereToStore has to store if the user doesn't specify this option
 * @param[inout] whereToStore a place where, at the end of the parsing, the value parsed (or the default one) is put
 * @param[in] description a description to show in the help
 */
void cuCliParserAddOptionalDouble(CU_NOTNULL cli_parser* p, char briefName, CU_NOTNULL const char* longName, double defaultValue, CU_NOTNULL double* whereToStore, CU_NOTNULL const char* description);

/**
 * Add a CLI option that may appear multiple times, not just at most once
 *
 * This function is usually terribly useful for parameters, which further tweaks a particular program funciton. For example:
 *
 * @code
 * #define PARAMETER_SIZE 100
 * char* parameters[PARAMETER_SIZE];
 * cuCliParserAddOptionalListString(
 * 		p, 'p', "parameter",
 * 		2, (String[]){"algorithm_version=3", "path_consistency_version=1"},
 * 		PARAMETER_SIZE, &parameters,
 * 		"values to tweak algorithm. Comes in the form of strings \"KEY=VALUE\""
 * );
 * @endocde
 *
 * the option is optional, in the sense that the user can avoid typing it on the terminal
 *
 * @param[inout] p the parser involved
 * @param[in] briefName a character **uniquely** identifying the option. Case sensitive. For example it might be <tt>'v'</tt>
 * @param[in] longName a string **uniquely identifying the option. Alias of \c briefName. For example it might be <tt>"verbose"</tt>
 * @param[in] defaultValueSize the size of the array of strings \c defaultValue
 * @param[in] defaultValue an array of strings representing the default value
 * @param[in] whereToStoreSize the size of the array \c whereToStore
 * @param[inout] whereToStore a place where, at the end of the parsing, the value parsed (or the default one) is put
 * @param[in] description a description to show in the help
 */
void cuCliParserAddOptionalListString(CU_NOTNULL cli_parser* p, char briefName, CU_NOTNULL const char* longName, int defaultValueSize, CU_NOTNULL const char* defaultValue[], int whereToStoreSize, CU_NOTNULL char* whereToStore[], CU_NOTNULL const char* description);

/**
 * Add a CLI option whose value is an int. The option has to appear in the command line
 *
 * For example, the following code create a verbose flag in the command line:
 *
 * @code
 * int size;
 * cuCliParserAddRequiredInt(p, 'n', "size", &size, "the size of the problem to test");
 * @endcode
 *
 * @param[inout] p the parser involved
 * @param[in] briefName a character **uniquely** identifying the option. Case sensitive. For example it might be <tt>'v'</tt>
 * @param[in] longName a string **uniquely identifying the option. Alias of \c briefName. For example it might be <tt>"verbose"</tt>
 * @param[inout] whereToStore a place where, at the end of the parsing, the value parsed (or the default one) is put
 * @param[in] description a description to show in the help
 */
void cuCliParserAddRequiredInt(CU_NOTNULL cli_parser* p, char briefName, CU_NOTNULL const char* longName, CU_NOTNULL int* whereToStore, CU_NOTNULL const char* description);

/**
 * Add a CLI option whose value is a long. The option has to appear in the command line
 *
 * For example, the following code create a verbose flag in the command line:
 *
 * @code
 * long size;
 * cuCliParserAddRequiredLong(p, 'n', "size", &size, "the size of the problem to test");
 * @endcode
 *
 * CLI flags are by definition **optional**: they can either be present or not
 *
 * @param[inout] p the parser involved
 * @param[in] briefName a character **uniquely** identifying the option. Case sensitive. For example it might be <tt>'v'</tt>
 * @param[in] longName a string **uniquely identifying the option. Alias of \c briefName. For example it might be <tt>"verbose"</tt>
 * @param[inout] whereToStore a place where, at the end of the parsing, the value parsed (or the default one) is put
 * @param[in] description a description to show in the help
 */
void cuCliParserAddRequiredLong(CU_NOTNULL cli_parser* p, char briefName, CU_NOTNULL const char* longName, CU_NOTNULL long* whereToStore,CU_NOTNULL  const char* description);

/**
 * Add a CLI option whose value is a <tt>char\*</tt>. The option has to appear in the command line
 *
 * For example, the following code create a verbose flag in the command line:
 *
 * @code
 * char* impl;
 * cuCliParserAddRequiredString(p, 'n', "implementation", &impl, "the implementation of the module creating the graph you want to use");
 * @endcode
 *
 * CLI flags are by definition **optional**: they can either be present or not
 *
 * @param[inout] p the parser involved
 * @param[in] briefName a character **uniquely** identifying the option. Case sensitive. For example it might be <tt>'v'</tt>
 * @param[in] longName a string **uniquely identifying the option. Alias of \c briefName. For example it might be <tt>"verbose"</tt>
 * @param[inout] whereToStore a place where, at the end of the parsing, the value parsed (or the default one) is put
 * @param[in] description a description to show in the help
 */
void cuCliParserAddRequiredString(CU_NOTNULL cli_parser* p, char briefName, CU_NOTNULL const char* longName, CU_NOTNULL char** whereToStore, CU_NOTNULL const char* description);

void cuCliParserAddRequiredFloat(CU_NOTNULL cli_parser* p, char briefName, CU_NOTNULL const char* longName, CU_NOTNULL float* whereToStore, CU_NOTNULL const char* description);

void cuCliParserAddRequiredDouble(CU_NOTNULL cli_parser* p, char briefName, CU_NOTNULL const char* longName, CU_NOTNULL double* whereToStore, CU_NOTNULL const char* description);

void cuCliParserAddRequiredListString(CU_NOTNULL cli_parser* p, char brief, CU_NOTNULL const char* longName, int whereToStoreSize, CU_NOTNULL char* whereToStore[], CU_NOTNULL const char* description);

/**
 * Check the number of times the given option has been displayed in the command line
 *
 * This function is useful for options that may be used multiple times: if you want to cycle through the array where those options
 * are stored, you need to know how many of those elements are actually used. This function can help you in that
 *
 * Use this function **after** you have called ::cuCliParserParseArguments
 *
 * @param[in] p the parse to analyze
 * @param[in] optionIdentifier the character uniquely identifying the option you want
 * @return
 * 	\li 0 if the option has not been registered at all;
 * 	\li 1 if the option can be used at most 1 time;
 * 	\li the number of times the option \c optionIdentifier has been used inside the CLI
 */
int cuCliParserGetOptionsNumberParsed(CU_NOTNULL const cli_parser* p, char optionIdentifier);

void* _cuCliParserGetWhereToStoreValueOfOption(CU_NOTNULL const cli_parser* p, char optionIdentifier);

/**
 * Parse a parameter string from command line
 *
 * @private
 *
 * An example fo command line parameter may be:
 *
 * <tt>OHSAT.sigma1Algorithm=IPASAT</tt>
 *
 * @param[in] parameter the parameter value
 * @param[out] optionName the place where to store the option name section of the parameter value. In the above example it weould be "OHSAT.sigma1Algorithm"
 * @param[out] optionValue the place where to store the option value section of the parameter value. In the above example it weould be "IPASAT"
 */
void _cuCliParserParseOptionString(CU_NOTNULL const char* parameter, CU_NOTNULL char** optionName, CU_NOTNULL char** optionValue);

/**
 * Generate a string which is a synopsys of the command
 *
 * For example it generates the following string:
 *
 * @code
 * [-v|--verbose] [-c|--count int] -i|--input_file string
 * @endcode
 *
 * @param[in] p the parser to manage
 * @param[inout] outputBuffer a place where the synopsys will be put
 *
 */
void cuCLIParserGetSynopsis(CU_NOTNULL const cli_parser* p, CU_NOTNULL char* outputBuffer);

/**
 * Generate a string containing the options of the software
 *
 * @param[in] p the parser involved
 * @param[inout] output the buffer that will contain the options
 */
void cuCLIParseGetOptions(const cli_parser* p, char* output);

/**
 * Loop over all the options declared by the flag \c option
 *
 * For example, if you have:
 *
 * <p><tt>--parameter="OHSAT.sigma1Algorithm=IPASAT" --parameter="OHSAT.improvement=true"</tt></p>
 *
 * this macro will allow you to cycle through all the parameters entries and fetch the values.
 *
 * \note
 * This macro works only with options list of strings.
 *
 * @param[in] p ::cli_parser to use
 * @param[in] value the flag identifiying the option. For exampe 'v' or 'm'
 * @param[in] optionName a variable name that will be used to allow you to get the left member of the parameter
 * @param[in] optionValue a variable name that will be used to allow you to get the right member of the parameter
 */
#define CU_ITERATE_OVER__STRING_LIST_OF_CLI_PARSER(p, value, optionName, optionValue) \
		cli_parser* UV(parser) = p; \
		char UV(option) = value; \
		int UV(optionSize) = cuCliParserGetOptionsNumberParsed(UV(parser), UV(option)); \
		char** UV(whereToStore) = (char**)_cuCliParserGetWhereToStoreValueOfOption(UV(parser), UV(option)); \
		for (int UV(i)=0, UV(access)=true; (UV(access)=true, UV(i)<UV(optionSize)); UV(i)++) \
			for (char* optionName, *optionValue; UV(access); UV(access)=false) \
				for (_cuCliParserParseOptionString(UV(whereToStore)[UV(i)], &optionName, &optionValue); UV(access); UV(access)=false)



#endif /* ARG_PARSER_H_ */
