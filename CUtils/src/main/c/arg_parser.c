/*
 * arg_parser.c
 *
 *  Created on: Jul 20, 2017
 *      Author: koldar
 */

#include "arg_parser.h"
#include "regularExpression.h"
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include "binaryTree.h"
#include "macros.h"
#include <errno.h>
#include <limits.h>
#include <math.h>
#include "log.h"
#include <string.h>
#include "errors.h"

/**
 * the type of a value requested by a CLI option
 */
enum cli_arg_type {
	/**
	 * the option is an integer
	 */
	CLI_ARG_INT,
	/**
	 * the option is a long
	 */
	CLI_ARG_LONG,
	/**
	 * the option is a boolean
	 */
	CLI_ARG_BOOL,
	/**
	 * the option is a string
	 */
	CLI_ARG_STRING,
	/**
	 * the option is a float
	 */
	CLI_ARG_FLOAT,
	/**
	 * the option is a double
	 */
	CLI_ARG_DOUBLE
};

typedef union cli_option_default_values {
	//if the default value is an int
	int i;
	//if the defautl value is a long
	long l;
	//if the default value is a boolean
	bool b;
	//if the default value is a float
	float f;
	//if the defatul value is a double
	double d;
	//if the default value is a string
	const char* s;
	//if the default value is array. We use char* because in this way pointer arithemic deals with bytes
	const char* a;
} cli_option_default_values;

typedef struct cli_option {
	/**
	 * represents a structure understandable by getopt_long
	 */
	struct option getoptOption;
	/**
	 * A pointer where the data associated to the command flag is stored
	 */
	void* whereToStoreData;
	/**
	 * Represents the size of the array pointed by ::cli_option::whereToStoreData
	 *
	 * If the option can appear at most one time, this field is and ::cli_option::whereToStoreData
	 * is not pointing to an array of data, but at a single value.
	 * Otherwise ::cli_option::whereToStoreData is indeed pointing at an array of data and this
	 * field represent sthe capacity of such array.
	 * Hence this field is greater than 1, that means the option can appear multiple
	 * times within a single CLI parsing, not just one.
	 *
	 * For example `gcc -fno-strict-aliasing -ffast-math`
	 */
	int whereToStoreDataSize;
	/**
	 * True if the user has written at least once the tag of this option
	 *
	 * Basically it means that we didn't load the default value, but the user has written something
	 * expliticly
	 */
	bool specified;
	/**
	 * If ::cli_option::whereToStoreDataSize is greater than 1, represents the next cell of ::cli_option::whereToStoreData where to put data
	 *
	 * During parsing operation, you need to know what is the first index where you don't have put data yet.
	 * This field tells you exactly that. This is used only when ::cli_option::whereToStoreData is an array though
	 */
	int nextCellOfWhereToStoreDataToPopulate;
	/**
	 * If true it means that this option **has** to be explicitly specified by the user
	 *
	 * Do not confuse this with the getopt "required_argument": the former tells whether or not
	 * the user can avoid explicitly writing this down while the latter tell if the option has or not
	 * a value.
	 *
	 * Required options don't have the default VALUE by definition
	 */
	bool required;
	/**
	 * The type of the data to store in ::cli_option::whereToStoreData
	 */
	enum cli_arg_type type;
	/**
	 * If the default value is an array, the field represents the size of such array
	 *
	 * 1 if default value is not an array
	 */
	int defaultValueSize;
	/**
	 * the place where we put the default values of the option
	 */
	cli_option_default_values defaultValue;
	/**
	 * The description of the flag. Useful to automatically create the help page
	 */
	const char* description;
} cli_option;

typedef binary_tree cli_option_bst;

struct cli_parser {
	cli_option_bst* options;
	const char* briefDescription;
	const char* longDescription;
};

static int optionOrderer(const cli_option* _o1, const cli_option* _o2);
static cli_option* newCLIOption(const char* name, bool isRequired, int has_arg, int* flag, int value, enum cli_arg_type type, int defaultValueSize, const void* defaultValue, int sizeOfWhereToStoreValue, void* whereToStoreData, const char* description);
#define CU_FUNCTION_POINTER_destructor_void_destroyCLIOption_voidConstPtr_var_argsConstPtr CU_DESTRUCTOR_ID
static void destroyCLIOption(const cli_option* o, CU_NULLABLE const struct var_args* context);
static void computeShortOptionString(cli_parser* parser, char* output);
static void addOptionInCLIParser(cli_parser* p, char briefName, const char* longName, bool isRequired,  int argumentRequirement, enum cli_arg_type optionType, int defaultValueSize, const void* defaultValue, int sizeOfWhereToStoreValue, void* whereToStoreValue, const char* description);
#define CU_FUNCTION_POINTER_finder_boolean_findOptionById_voidConstPtr_var_argsConstPtr CU_FINDER_ID
static bool findOptionById(const cli_option* o, const var_args* va);
static void populateDataOfCLIOption(const cli_parser* p, const char* argument, cli_option* option);
static void setDefaultValueInStoreValue(cli_option* o);
static bool isOptionMultiOccurence(const cli_option* o);
static size_t getSizeOfArrayCell(enum cli_arg_type type);
static bool isDefaultArray(const cli_option* o);
static cli_option* getFirstMissingRequiredArgument(const cli_parser* parser);
static payload_functions _cuGetOptionPaylodFunctions();


cli_parser* cuNewCliParser() {
	cli_parser* retVal = malloc(sizeof(cli_parser));
	if (retVal == NULL) {
		ERROR_MALLOC();
	}

	retVal->options = cuBinaryTreeNew(_cuGetOptionPaylodFunctions());
	retVal->briefDescription = NULL;
	retVal->longDescription = NULL;

	cuCliParserAddFlag(retVal, 'h', "help", false, NULL, "print this program help");

	return retVal;
}

void cuCliParserSetBriefDescription(cli_parser* p, const char* brief) {
	p->briefDescription = brief;
}

void cuCliParserSetLongDescription(cli_parser* p, const char* description) {
	p->longDescription = description;
}

void cuDestroyCliParser(CU_NOTNULL const cli_parser* p, CU_NULLABLE const struct var_args* context) {
	cuBinaryTreeDestroyWithElements(p->options, context);
	free((void*)p);
}

void cuCliParserAddFlag(cli_parser* p, char briefName, const char* longName, bool defaultValue, bool* whereToStore, const char* description) {
	addOptionInCLIParser(p, briefName, longName, false, no_argument, CLI_ARG_BOOL, 1, &defaultValue, 1, whereToStore, description);
}

void cuCliParserAddBoolOption(cli_parser* p, char briefName, const char* longName, bool defaultValue, bool* whereToStore, const char* description) {
	cuCliParserAddFlag(p, briefName, longName, defaultValue, whereToStore, description);
}

void cuCliParserAddOptionalInt(cli_parser* p, char briefName, const char* longName, int defaultValue, int* whereToStore, const char* description) {
	addOptionInCLIParser(p, briefName, longName, false, required_argument, CLI_ARG_INT, 1, &defaultValue, 1, whereToStore, description);
}

void cuCliParserAddOptionalLong(cli_parser* p, char briefName, const char* longName, long defaultValue, long* whereToStore, const char* description) {
	addOptionInCLIParser(p, briefName, longName, false, required_argument, CLI_ARG_LONG, 1, &defaultValue, 1, whereToStore, description);
}

void cuCliParserAddOptionalString(cli_parser* p, char briefName, const char* longName, const char* defaultValue, char** whereToStore, const char* description) {
	addOptionInCLIParser(p, briefName, longName, false, required_argument, CLI_ARG_STRING, 1, defaultValue, 1, whereToStore, description);
}

void cuCliParserAddOptionalFloat(cli_parser* p, char briefName, const char* longName, float defaultValue, float* whereToStore, const char* description) {
	addOptionInCLIParser(p, briefName, longName, false, required_argument, CLI_ARG_FLOAT, 1, &defaultValue, 1, whereToStore, description);
}

void cuCliParserAddOptionalDouble(cli_parser* p, char briefName, const char* longName, double defaultValue, double* whereToStore, const char* description) {
	addOptionInCLIParser(p, briefName, longName, false, required_argument, CLI_ARG_DOUBLE, 1, &defaultValue, 1, whereToStore, description);
}

void cuCliParserAddOptionalListString(cli_parser* p, char brief, const char* longName, int defaultValueSize, const char* defaultValue[], int whereToStoreSize, char* whereToStore[], const char* description) {
	addOptionInCLIParser(p, brief, longName, false, required_argument, CLI_ARG_STRING, defaultValueSize, (void*)defaultValue, whereToStoreSize, whereToStore, description);
}

void cuCliParserAddRequiredInt(cli_parser* p, char briefName, const char* longName, int* whereToStore, const char* description) {
	addOptionInCLIParser(p, briefName, longName, true, required_argument, CLI_ARG_INT, 1, NULL, 1, whereToStore, description);
}

void cuCliParserAddRequiredLong(cli_parser* p, char briefName, const char* longName, long* whereToStore, const char* description) {
	addOptionInCLIParser(p, briefName, longName, true, required_argument, CLI_ARG_LONG, 1, NULL, 1, whereToStore, description);
}

void cuCliParserAddRequiredString(cli_parser* p, char briefName, const char* longName, char** whereToStore, const char* description) {
	addOptionInCLIParser(p, briefName, longName, true, required_argument, CLI_ARG_STRING, 1, NULL, 1, whereToStore, description);
}

void cuCliParserAddRequiredFloat(cli_parser* p, char briefName, const char* longName, float* whereToStore, const char* description) {
	addOptionInCLIParser(p, briefName, longName, true, required_argument, CLI_ARG_FLOAT, 1, NULL, 1, whereToStore, description);
}

void cuCliParserAddRequiredDouble(cli_parser* p, char briefName, const char* longName, double* whereToStore, const char* description) {
	addOptionInCLIParser(p, briefName, longName, true, required_argument, CLI_ARG_DOUBLE, 1, NULL, 1, whereToStore, description);
}

void cuCliParserAddRequiredListString(cli_parser* p, char brief, const char* longName, int whereToStoreSize, char* whereToStore[], const char* description) {
	addOptionInCLIParser(p, brief, longName, true, required_argument, CLI_ARG_STRING, 1, NULL, whereToStoreSize, whereToStore, description);
}

int cuCliParserGetOptionsNumberParsed(const cli_parser* p, char optionIdentifier) {
	cli_option* o = NULL;

	cuInitVarArgsOnStack(va, optionIdentifier);
	o = cuBinaryTreeFindItem(p->options, CU_AS_FINDER(findOptionById), va);

	if (o == NULL) {
		return 0;
	}

	if (!isOptionMultiOccurence(o)) {
		return o->specified ? 1 : 0;
	}

	return o->specified ? o->nextCellOfWhereToStoreDataToPopulate : o->defaultValueSize;

}

bool cuCliParserParseArguments(cli_parser* p, const int argc, const char* const args[]) {
	bool retVal = true;

	//create the array
	struct option* long_options = calloc(sizeof(struct option), cuBinaryTreeGetSize(p->options) + 1);
	if (long_options == NULL) {
		ERROR_MALLOC();
	}

	int counter = 0;
	ITERATE_THROUGH_BST(p->options, o, cli_option*) {
		memcpy(&long_options[counter], &o->getoptOption, sizeof(struct option));
		counter++;
	}
	//the last value in the array needs to be all nULL
	long_options[counter] = (struct option){0, 0, 0, 0};

	//compute the short option string
	char shortOption[CLIPARSER_MAX_OPTIONS_BUFFER_SIZE];
	computeShortOptionString(p, shortOption);


	int index_option_encountered;
	int value;
	cli_option* chosenOption;

	while (true) {
		value = getopt_long(argc, (char *const *)args, shortOption, long_options, &index_option_encountered);

		//value is -1 when we reached the end of the input
		if (value == -1) {
			goto exit;
		}

		switch (value) {
		case 'h': {
			char buffer[10 * BUFFER_SIZE];
			cuCLIParseGetOptions(p, buffer);
			fprintf(stdout, "%s", buffer);
			exit(0);
		}
		case '?': {
			exit(100);
		}
		default: {
			cuInitVarArgsOnStack(va, value);
			chosenOption = cuBinaryTreeFindItem(p->options, CU_AS_FINDER(findOptionById), va);
			//the user has typed a useful choice for the program. This won't be reached by "?" and "h"
			if (chosenOption == NULL) {
				fprintf(stderr, "Unknown flag \"%d\". Use \"--help\" to check available options.\n", value);
				exit(-1);
			}
			chosenOption->specified = true;
			populateDataOfCLIOption(p, optarg, chosenOption);
		}
		}


	}

	exit: ;

	//check required options
	cli_option* missing = getFirstMissingRequiredArgument(p);
	if (missing != NULL) {
		fprintf(stderr, "Missing required option \"-%c\" [--%s]. Use \"--help\" to check available options.\n", missing->getoptOption.val, missing->getoptOption.name);
		exit(-1);
	}

	free(long_options);
	return retVal;
}

void* _cuCliParserGetWhereToStoreValueOfOption(const cli_parser* p, char optionIdentifier) {
	cuInitVarArgsOnStack(va, optionIdentifier);
	cli_option* o = cuBinaryTreeFindItem(p->options, CU_AS_FINDER(findOptionById), va);

	if (o == NULL) {
		return NULL;
	}

	return o->whereToStoreData;
}

#define PARAMETER_REGEX "\\(" CU_RE_ALPHANUMDOT "\\)=\\(" CU_RE_ALPHANUMDOT "\\)"

void _cuCliParserParseOptionString(const char* parameter, char** optionName, char** optionValue) {
	bool valid;
	const char * const * groups;

	valid = cuRegularExpressionApply(parameter, PARAMETER_REGEX, 2, &groups);
	if (!valid) {
		ERROR_APPLICATION_FAILED("regex", PARAMETER_REGEX, "string", parameter);
	}

	*optionName = strdup(groups[1]);
	*optionValue = strdup(groups[2]);

	cuRegularExpressionDestroyGroupInfo(2, &groups);
}

void cuCLIParserGetSynopsis(CU_NOTNULL const cli_parser* p, CU_NOTNULL char* outputBuffer) {
	int i = 0;
	ITERATE_THROUGH_BST(p->options, o, cli_option*) {
		if (!o->required) {
			i += sprintf(&outputBuffer[i], "[ ");
		}

		i += sprintf(&outputBuffer[i], "-%c|--%s", o->getoptOption.val, o->getoptOption.name);

		switch (o->type) {
		case CLI_ARG_BOOL: {
			//bool has no value attached (--verbose=true it's not valid). Every other values can indeed have a value attached to it
			break;
		}
		case CLI_ARG_DOUBLE: {
			i += sprintf(&outputBuffer[i], " double");
			break;
		}
		case CLI_ARG_FLOAT: {
			i += sprintf(&outputBuffer[i], " float");
			break;
		}
		case CLI_ARG_INT: {
			i += sprintf(&outputBuffer[i], " int");
			break;
		}
		case CLI_ARG_LONG: {
			i += sprintf(&outputBuffer[i], " long");
			break;
		}
		case CLI_ARG_STRING: {
			i += sprintf(&outputBuffer[i], " string");
			break;
		}
		default: {
			ERROR_INVALID_SWITCH_CASE("option type", "%d", o->type);
		}
		}

		if (!o->required) {
			i += sprintf(&outputBuffer[i], " ]");
		} else {
			i += sprintf(&outputBuffer[i], " ");
		}
	}
}

static cli_option* getFirstMissingRequiredArgument(const cli_parser* parser) {
	critical("size of options %d", cuBinaryTreeGetSize(parser->options));
	ITERATE_THROUGH_BST(parser->options, o, cli_option*) {
		finest("o= %s required= %s specified= %s",
				o->getoptOption.name, o->required ?"yes":"no", o->specified?"yes":"no");
		if (o->required && !o->specified) {
			return o;
		}
	}
	return NULL;
}

/**
 * Set the default value inside the place where the caller program will read the parsed data
 *
 * @param[in] sizeOfWhereToStoreValue size of \c whereToStoreValue. If 1, \c whereToStoreValue is a single value, otherwise it is an array
 * @param[inout] whereToStoreValue the address where the main program will read the parsed argument. If NULL we do nothing
 * @param[in] optionType the type of the \c defaultValue to write inside \c whereToStoreValue
 * @param[in] defaultValueSize the size of \c defaultValue. Useful only if \c defaultValue is actually an array
 * @param[in] defaultValue the value to write into \c whereToStoreValue
 */
static void setDefaultValueInStoreValue(cli_option* o) {
	//int sizeOfWhereToStoreValue, void* whereToStoreValue, cli_arg_type optionType, int defaultValueSize, void* defaultValue
	if (o->whereToStoreData == NULL) {
		return;
	}

	void* whereToStoreData = NULL;
	const size_t cellSize = isDefaultArray(o) ? getSizeOfArrayCell(o->type) : 1;

	for (int i=0; i<o->defaultValueSize; i++) {

		if (i >= o->whereToStoreDataSize) {
			warning("index %d of the default value cannot be written inside whereToStoreValue because it is out of bounds. Ignoring it.", i);
			continue;
		}


		const void* whereToLoadData = (o->defaultValueSize == 1)
						//the defautl value size is 1. So this means the defatul value is not an array.
						//we load the data from defaultValue itself
						? &o->defaultValue
						//we load the data from the array sector of defaultValue
						: (const void*)(o->defaultValue.a + cellSize * i);

		if (o->whereToStoreDataSize == 1) {
			whereToStoreData = o->whereToStoreData;
		} else {
			whereToStoreData = (char*)(o->whereToStoreData) + cellSize * i;
		}

		switch (o->type) {
		case CLI_ARG_BOOL: {
			*((bool*)whereToStoreData) = *((bool*)whereToLoadData);
			break;
		}
		case CLI_ARG_DOUBLE: {
			*((double*)whereToStoreData) = *((double*)whereToLoadData);
			break;
		}
		case CLI_ARG_FLOAT: {
			*((float*)whereToStoreData) = *((float*)whereToLoadData);
			break;
		}
		case CLI_ARG_INT: {
			*((int*)whereToStoreData) = *((int*)whereToLoadData);
			break;
		}
		case CLI_ARG_LONG: {
			*((long*)whereToStoreData) = *((long*)whereToLoadData);
			break;
		}
		case CLI_ARG_STRING: {
			*((char**)whereToStoreData) = *((char**)whereToLoadData);
			break;
		}

		default: {
			ERROR_UNHANDLED_CASE("type", o->type);
		}
		}
	}
}

/**
 * Adds a new option inside the CLI parser.
 *
 * This is a main function for this module
 *
 * @param[inout] p the parser to update
 * @param[in] briefName the one character representing this option. For exampel "r", "s" or "t";
 * @param[in] longName the name representing the long option. For example "recursive" or "listall";
 * @param[in] isRequired if true the user must explicitly write the option in the command line
 * @param[in] argumentRequirement either optional_argument, required_argument or no_argument
 * @param[in] optionType the type of the option value to store
 * @param[int] defaultValueSize the \c defaultValue size. Useful only when \c defaultValue is an array
 * @param[in] defaultValue a non NULL pointer representing the default value this option has if it's not present inside the terminal.
 * 				you can ignore it if \c argumentRequirement is no_argument. Default value is copy by "value"
 * @param[in] sizeOfWhereToStoreValue if 1 \c whereToStoreValue is a pointer to a value and the option can appear at most once. Otherwise \c whereToStoreValue
 * 				points to an array of cells and the option ca appear mutliple times.
 * @param[in] whereToStoreValue a pointer that will receive the value already parsed from the command line. Put it NULL if you don't care about
 * 				what was written in the command line (for flags if you don't care if the flag was present or not). This scenario is pretty much
 * 				rare.
 * @param[in] description a small description of the command.
 */
static void addOptionInCLIParser(cli_parser* p, char briefName, const char* longName, bool isRequired,  int argumentRequirement, enum cli_arg_type optionType, int defaultValueSize, const void* defaultValue, int sizeOfWhereToStoreValue, void* whereToStoreValue, const char* description) {
	cuInitVarArgsOnStack(va, briefName);
	if (cuBinaryTreeFindItem(p->options, CU_AS_FINDER(findOptionById), va) != NULL) {
		ERROR_IS_ALREADY_PRESENT(briefName, "options", "%c");
	}

	cli_option* o = newCLIOption(longName, isRequired, argumentRequirement, 0, briefName, optionType, defaultValueSize, defaultValue, sizeOfWhereToStoreValue, whereToStoreValue, description);
	cuBinaryTreeAddItem(p->options, o);
	//set the default value before parsing. In this way if we don't encounter a flag, the default value is already applied
	if (!isRequired && (argumentRequirement == optional_argument || argumentRequirement == required_argument)) {
		setDefaultValueInStoreValue(o);
	}
}

static bool findOptionById(const cli_option* o, const var_args* va) {
	return o->getoptOption.val == cuVarArgsGetItem(va, 0, char);
}

/**
 * Check if, after executing a \c strtoX function, the conversion is successful
 */
#define CHECK_STRTOX_ERROR(variable_to_set, min, max, flagName, flagValue) \
		if (((errno == ERANGE) && (variable_to_set == min || variable_to_set == max)) || (errno != 0 && variable_to_set == 0)) { \
			fprintf(stderr, "Can't decode the value of the flag %s: %s\n", flagName, flagValue); \
			exit(100); \
		} \

/**
 * Check if the string \c x is case insensistive equal to the string \c constant_string. If so, return \c value
 */
#define CHECK_BOOL_VALUE(x, constant_string, value) \
		if (strcasecmp(x, constant_string) == 0) { \
			return value; \
		} \

/**
 * Stores a value inside ::cli_option::whereToStoreData
 *
 * @param[in] _option a ::cli_option pointer containing the option whose value we want to store in ::cli_option::whereToStoreData;
 * @param[in] type_val the type of the \c option
 * @param[in] _val the value to store
 * @param[in] argument the string where _val has been parsed from
 */
#define STORE_IN_DATA(_option, type_val, _val, argument) \
		cli_option* UV(option) = _option; \
		type_val UV(val) = _val ; \
		size_t UV(cellSize) = getSizeOfArrayCell(option->type); \
		if (isOptionMultiOccurence(UV(option))) { \
			if (option->nextCellOfWhereToStoreDataToPopulate < option->whereToStoreDataSize) { \
				type_val* UV(whereToStore) = (type_val*)(((char*)UV(option)->whereToStoreData) + UV(cellSize) * UV(option)->nextCellOfWhereToStoreDataToPopulate); \
				*UV(whereToStore) = UV(val); \
				UV(option)->nextCellOfWhereToStoreDataToPopulate++; \
			} else { \
				warning("Can't store another option \"%s\" inside the store variable. Increase whereToStoreSize. \"%s\" will be ignored", UV(option)->getoptOption.name, argument); \
			} \
		} else { \
			*((type_val*)UV(option)->whereToStoreData) = (type_val) UV(val); \
		}

static bool handleBoolOption(const char* argument, cli_option* option) {
	CHECK_BOOL_VALUE(argument, "on", true);
	CHECK_BOOL_VALUE(argument, "off", false);

	CHECK_BOOL_VALUE(argument, "yes", true);
	CHECK_BOOL_VALUE(argument, "no", false);

	CHECK_BOOL_VALUE(argument, "true", true);
	CHECK_BOOL_VALUE(argument, "false", false);

	CHECK_BOOL_VALUE(argument, "ok", true);
	CHECK_BOOL_VALUE(argument, "ko", false);

	fprintf(stderr, "Can't decode boolean value \"%s\" of flag \"%s\"", argument, option->getoptOption.name);
	exit(100);
}

static void populateDataOfCLIOption(const cli_parser* p, const char* argument, cli_option* option) {
	char* firstInvalidCharacter;
	errno = 0; //reset errors

	if (option->whereToStoreData == NULL) {
		return;
	}

	switch (option->type) {
	case CLI_ARG_BOOL: {
		if (option->getoptOption.has_arg == no_argument) {
			*((bool*)option->whereToStoreData) = true;
		} else {
			*((bool*)option->whereToStoreData) = handleBoolOption(argument, option);
		}
		break;
	}
	case CLI_ARG_DOUBLE: {
		double val = strtof(argument, &firstInvalidCharacter);
		CHECK_STRTOX_ERROR(val, HUGE_VALF, HUGE_VALL, option->getoptOption.name, argument);

		STORE_IN_DATA(option, double, val, argument);
		break;
	}
	case CLI_ARG_FLOAT: {
		float val = strtof(argument, &firstInvalidCharacter);
		CHECK_STRTOX_ERROR(val, HUGE_VALF, HUGE_VALL, option->getoptOption.name, argument);

		STORE_IN_DATA(option, float, val, argument);
		break;
	}
	case CLI_ARG_INT: {
		long val = strtol(argument, &firstInvalidCharacter, 10);
		CHECK_STRTOX_ERROR(val, LONG_MIN, LONG_MAX, option->getoptOption.name, argument);

		STORE_IN_DATA(option, int, val, argument);
		break;
	}
	case CLI_ARG_LONG: {
		long val = strtol(argument, &firstInvalidCharacter, 10);
		CHECK_STRTOX_ERROR(val, LONG_MIN, LONG_MAX, option->getoptOption.name, argument);

		STORE_IN_DATA(option, long, val, argument);
		break;
	}
	case CLI_ARG_STRING: {
		//we clone the string
		if (isOptionMultiOccurence(option)) {
			size_t cellSize = getSizeOfArrayCell(option->type);
			if (option->nextCellOfWhereToStoreDataToPopulate < option->whereToStoreDataSize) { \
				char** whereToStore = (char**)(((char*)option->whereToStoreData) + cellSize * option->nextCellOfWhereToStoreDataToPopulate);
			*whereToStore = strdup(argument);
			option->nextCellOfWhereToStoreDataToPopulate++;
			} else { \
				warning("Can't store another option \"%s\" inside the store variable. Increase whereToStoreSize. \"%s\" will be ignored", option->getoptOption.name, argument); \
			} \

		} else {
			*(char**)option->whereToStoreData = strdup(argument);
		}

		break;
	}
	default: {
		ERROR_UNHANDLED_CASE("type", option->type);
	}
	}
}

/**
 * @param[in] o the option to handle
 * @return
 * 	\li true if the option can happen multiple times;
 * 	\li false otherwise
 *
 */
static bool isOptionMultiOccurence(const cli_option* o) {
	return o->whereToStoreDataSize > 1;
}

void cuCLIParseGetOptions(const cli_parser* p, char* output) {
	//TODO put the name of the application
	int i = 0;

	if (p->briefDescription != NULL) {
		i += sprintf(&output[i], "%s\n", p->briefDescription);
		i += sprintf(&output[i], "\n");
	}
	if (p->longDescription != NULL) {
		i += sprintf(&output[i], "%s\n", p->longDescription);
		i += sprintf(&output[i], "\n");
	}

	ITERATE_THROUGH_BST(p->options, o, cli_option*) {
		// ********************* FLAGS ****************************
		i += sprintf(&output[i], "\t-%c, --%s: ", o->getoptOption.val, o->getoptOption.name);

		// ******************** TYPE *********************************
		switch (o->type) {
		case CLI_ARG_BOOL: { i += sprintf(&output[i], "BOOL"); break; }
		case CLI_ARG_INT: { i += sprintf(&output[i], "INT"); break; }
		case CLI_ARG_LONG: { i += sprintf(&output[i], "LONG"); break; }
		case CLI_ARG_FLOAT: { i += sprintf(&output[i], "FLOAT"); break; }
		case CLI_ARG_DOUBLE: { i += sprintf(&output[i], "DOUBLE"); break; }
		case CLI_ARG_STRING: { i += sprintf(&output[i], "STRING"); break; }
		default: ERROR_UNHANDLED_CASE("type", o->type);
		}
		i += sprintf(&output[i], " ");
		// ************************* REQUIRED *********************************
		if (o->required) {
			i += sprintf(&output[i], "[REQUIRED]");
		}
		// ************************* DEFAULT VALUE ****************************
		if (!o->required) {
			i += sprintf(&output[i], "[DEFAULT = ");
			if (isOptionMultiOccurence(o)) {
				i += sprintf(&output[i], "{");
			}

			size_t cellSize;
			if (isDefaultArray(o)) {
				cellSize = getSizeOfArrayCell(o->type);
			}

			for (int j=0; j<o->defaultValueSize; j++) {
				const void* whereToLoad = isDefaultArray(o)
						? (const void*)(o->defaultValue.a + cellSize * j)
						: &o->defaultValue;

				critical("where to load : %p, i=%d", whereToLoad, i);
				switch (o->type) {
					case CLI_ARG_BOOL: {
						i += sprintf(&output[i], "%s", *((bool*)whereToLoad) ? "true" : "false");
						break;
					}
					case CLI_ARG_INT: {
						i += sprintf(&output[i], "%d", *((int*)whereToLoad));
						break;
					}
					case CLI_ARG_LONG: {
						i += sprintf(&output[i], "%ld", *((long*)whereToLoad));
						break;
					}
					case CLI_ARG_FLOAT: {
						i += sprintf(&output[i], "%2.3f", *((float*)whereToLoad));
						break;
					}
					case CLI_ARG_DOUBLE: {
						i += sprintf(&output[i], "%2.3f", *((double*)whereToLoad));
						break;
					}
					case CLI_ARG_STRING: {
						i += sprintf(&output[i], "\"%s\"", *((char**)whereToLoad));
						break;
					}
					default: {
						ERROR_UNHANDLED_CASE("type", o->type);
					}
				}

				if ((j+1) < o->defaultValueSize) {
					i += sprintf(&output[i], ", ");
				}
			}

			if (isOptionMultiOccurence(o)) {
				i += sprintf(&output[i], "}");
			}
			i += sprintf(&output[i], "]");
		}
		i += sprintf(&output[i], "\n");
		//added in order to print the indentation of the description correctly
		bool hasToAddTabs = true;
		// ********************** TIMES YOU MAY USE IT ***********************
		if (isOptionMultiOccurence(o)) {
			i += sprintf(&output[i], "\t\t[CAN BE USED MULTIPLE TIMES] ");
			hasToAddTabs = false;
		}
		// *********************** DESCRIPTION OF COMMAND ************************

		const char* printFrom = o->description;
		char* carriageIndex;
		do {
			carriageIndex = strchr(printFrom, '\n'); //return NULL if it can't find it
			i += sprintf(&output[i], "%s%.*s\n",
					hasToAddTabs ? "\t\t" : "",
							(int)(carriageIndex - printFrom), printFrom
			);
			printFrom = carriageIndex + 1;
			hasToAddTabs = true;
		} while(carriageIndex != NULL);
		i += sprintf(&output[i], "\n");
	}
}

static bool isDefaultArray(const cli_option* o) {
	return o->defaultValueSize > 1;
}

/**
 * Computes the string "options required by getopt
 *
 * see <a href="https://www.gnu.org/software/libc/manual/html_node/Using-Getopt.html#Using-Getopt">here</a> for further information
 *
 * @param[in] parser the parser to use
 * @param[out] a initialized buffer where the string will be inserted
 */
static void computeShortOptionString(cli_parser* parser, char* output) {
	int i = 0;

	ITERATE_THROUGH_BST(parser->options, o, cli_option*) {
		i += sprintf(&output[i], "%c", o->getoptOption.val);

		switch (o->getoptOption.has_arg) {
		case no_argument: {
			break;
		}
		case required_argument: {
			i += sprintf(&output[i], ":");
			break;
		}
		case optional_argument: {
			i += sprintf(&output[i], "::");
			break;
		}
		default: {
			ERROR_UNHANDLED_CASE("getopt_long has arg", o->getoptOption.has_arg);
		}
		}
	}
}

static size_t getSizeOfArrayCell(enum cli_arg_type type) {
	switch (type) {
	case CLI_ARG_BOOL: return sizeof(bool);
	case CLI_ARG_DOUBLE: return sizeof(double);
	case CLI_ARG_FLOAT: return sizeof(float);
	case CLI_ARG_INT: return sizeof(int);
	case CLI_ARG_LONG: return sizeof(long);
	case CLI_ARG_STRING: return sizeof(char*);
	default:
		ERROR_UNHANDLED_CASE("type", type);
	}
	return CLI_ARG_INT;
}

/**
 * Set the field ::cli_option::defaultValue within the given \c option
 *
 * The default value is copied by **value**.
 * No strings are duplicated though (they are default value, so they are probably in the stack of the application)
 *
 * @param[inout] option the option to alter
 * @param[in] type the type of the defautl value
 * @param[in] a pointer pointing the default value to copy.
 */
static void setDefaultValue(cli_option* option, enum cli_arg_type type, const void* defaultValue) {

	size_t cellSize = 0;
	if (isDefaultArray(option)) {
		//the default value is an array. We need to malloc the area where the array is
		cellSize = getSizeOfArrayCell(type);
		option->defaultValue.a = defaultValue;
		return;
	}

	void* whereToStoreDefault = NULL;
	const void* whereToLoadDefault = NULL;

	//if the defaultValueSize is 1, then the default value is not an array.
	//we can store the value directly inside the defaultValue field, without
	//going through the array at all
	whereToStoreDefault = &option->defaultValue;
	whereToLoadDefault = defaultValue;

	//TODO make a macro (void* to, void* from, type)
	switch (type) {
	case CLI_ARG_BOOL: {
		*(bool*)whereToStoreDefault = *((bool*)whereToLoadDefault);
		break;
	}
	case CLI_ARG_DOUBLE: {
		*(double*)whereToStoreDefault = *((double*)whereToLoadDefault);
		break;
	}
	case CLI_ARG_FLOAT: {
		*(float*)whereToStoreDefault = *((float*)whereToLoadDefault);
		break;
	}
	case CLI_ARG_INT: {
		*(int*)whereToStoreDefault = *((int*)whereToLoadDefault);
		break;
	}
	case CLI_ARG_LONG: {
		*(long*)whereToStoreDefault = *((long*)whereToLoadDefault);
		break;
	}
	case CLI_ARG_STRING: {
		*(char**)whereToStoreDefault = ((char*)whereToLoadDefault);
		break;
	}
	default: {
		ERROR_UNHANDLED_CASE("type", type);
	}
	}
}

static cli_option* newCLIOption(const char* name, bool isRequired, int has_arg, int* flag, int value, enum cli_arg_type type, int defaultValueSize, const void* defaultValue, int sizeOfWhereToStoreValue, void* whereToStoreData, const char* description) {
	cli_option* retVal = malloc(sizeof(cli_option));
	if (retVal == NULL) {
		ERROR_MALLOC();
	}

	retVal->description = description;
	retVal->type = type;
	retVal->specified = false;
	retVal->nextCellOfWhereToStoreDataToPopulate = 0;
	retVal->whereToStoreDataSize = sizeOfWhereToStoreValue;
	retVal->whereToStoreData = whereToStoreData;
	retVal->required = isRequired;

	retVal->defaultValueSize = defaultValueSize;
	if (!isRequired && has_arg == required_argument) {
		setDefaultValue(retVal, type, defaultValue);
	}

	retVal->getoptOption.name = name;
	retVal->getoptOption.has_arg = has_arg;
	retVal->getoptOption.flag = flag;
	retVal->getoptOption.val = value;

	return retVal;
}


static void destroyCLIOption(const cli_option* o, CU_NULLABLE const struct var_args* context) {
	free((void*)o);
}

static int optionOrderer(const cli_option* o1, const cli_option* o2) {
	return o1->getoptOption.val - o2->getoptOption.val;
}

static payload_functions _cuGetOptionPaylodFunctions() {
	payload_functions result;
	result.destroy = CU_AS_DESTRUCTOR(destroyCLIOption);
	result.order = CU_AS_ORDERER(optionOrderer);

	return result;
}
