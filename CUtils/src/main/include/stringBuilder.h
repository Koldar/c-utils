/**
 * @file
 *
 * A naive porting of java's string_builder class
 *
 * Standard usage can be the following one:
 *
 * ```
 * string_builder* sb = cuStringBulilderNew();
 * cuStringBuilderAppendTemplate(sb, "hello %s", "world");
 * //generates a string:
 * const char* str = cuStringBuilderGetString(sb);
 * //after having used str destroy the string builder
 * cuStringBulilderDestroy(sb);
 * ```
 *
 * @date Feb 27, 2017
 * @author koldar
 */

#ifndef STRINGBUILDER_H_
#define STRINGBUILDER_H_

#include <stdbool.h>
#include "macros.h"
#include "var_args.h"

typedef struct string_builder string_builder;

/**
 * Initialize a new string builder
 *
 * @param[in] startString the string to initialize with the string builder. NULL if you don't want to intialize it with anything. Defaults to ""
 * @param[in] resizeFactor if the buffer is filled, we enlarge it by such quantity. Positive non zero. Defaults to 100
 * @return the ::string_builder just initialized
 */
string_builder* cuStringBulilderNew(CU_NULLABLE const char* startString, int resizeFactor);
CU_DECLARE_FUNCTION_WITH_DEFAULTS(string_builder*, cuStringBulilderNew, const char*, int);
#define cuStringBulilderNew(...) CU_CALL_FUNCTION_WITH_DEFAULTS(cuStringBulilderNew, 2, __VA_ARGS__)
CU_DECLARE_DEFAULT_VALUES(cuStringBulilderNew,
        NULL,
        100
);

/**
 * Appends a string into the builder
 *
 * The string \c string is copied by \b value inside the string buider;
 *
 * @param[inout] sb the string builder to alter
 * @param[in] string the string to append to the ::string_builder
 */
void cuStringBuilderAppendString(CU_NOTNULL string_builder* sb, CU_NOTNULL const char* string);

/**
 * like ::cuStringBuilderAppendString but it accepts a \c printf like var arguments
 *
 * @param[inout] sb the string builder to alter
 * @param[in] template the string to append to the ::string_builder
 * @param[in] ... the variable argument to add inside the \c template
 */
void cuStringBuilderAppendTemplate(CU_NOTNULL string_builder* sb, CU_NOTNULL const char* template, ...);

/**
 * Appends a character into the builder
 *
 * @param[inout] sb the string builder to alter
 * @param[in] ch the data to put inside the builder
 */
void cuStringBuilderAppendChar(CU_NOTNULL string_builder* sb, char ch);

/**
 * Appends an integer into the builder
 *
 * @param[inout] sb the string builder to alter
 * @param[in] i the data to put inside the builder
 */
void cuStringBuilderAppendInt(CU_NOTNULL string_builder* sb, int i);

/**
 * Appends a float into the builder
 *
 * @param[inout] sb the string builder to alter
 * @param[in] f the data to put inside the builder
 */
void cuStringBuilderAppendFloat(CU_NOTNULL string_builder* sb, float f);

/**
 * Appends a double into the builder
 *
 * @param[inout] sb the string builder to alter
 * @param[in] d the data to put inside the builder
 */
void cuStringBuilderAppendDouble(CU_NOTNULL string_builder* sb, double d);

/**
 * Appends a long into the builder
 *
 * @param[inout] sb the string builder to alter
 * @param[in] l the data to put inside the builder
 */
void cuStringBuilderAppendLong(CU_NOTNULL string_builder* sb, long l);

/**
 * Get the string built so far by this structure
 *
 * @note
 * The return value is passed **by reference**, so any changes to the return value will change the builder as well
 *
 * @param[in] sb the string builder
 * @return the string built so far
 */
char* cuStringBuilderGetString(CU_NOTNULL const string_builder* sb);

/**
 * Clear the content of the string buiilder
 *
 * @param[inout] sb the builder to clear
 */
void cuStringBuilderClear(CU_NOTNULL string_builder* sb);

/**
 * Removes from memory the string_builder
 *
 * @param[in] sb the string builder to remove from memory
 */
void cuStringBulilderDestroy(CU_NOTNULL const string_builder* sb, CU_NULLABLE const struct var_args* context);
#define CU_FUNCTION_POINTER_destructor_void_cuStringBulilderDestroy_voidConstPtr_var_argsConstPtr CU_DESTRUCTOR_ID

#endif /* STRINGBUILDER_H_ */
