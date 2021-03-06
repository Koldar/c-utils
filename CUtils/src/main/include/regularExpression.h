/*
 * @file
 *
 * Contains useful method to handle regular expression in easy way with C
 *
 * Here's a normal use case for this library:
 *
 * @includedoc regularExpressionExample.dox
 *
 * In the following some rules for writing regular expressions in C:
 *
 * @li the capturing groups are encoded with <tt>\(</tt> and <tt>\)</tt>: hence in your string, since the backslash is used as escaping,
 *  you need to double the backslash. Hence <tt>\\(</tt> and <tt>\\)<tt>;
 * @li There aren't available shortcuts like <tt>\\d<tt> or <tt>\\w</tt>: use classes. For further information see <a href="https://www.regular-expressions.info/posixbrackets.html">here</a>
 * @li There isn't the operator "+", so you need to manually create it with "*" (e.g., "a+" can be encoded with "aa*")
 *
 * @date May 18, 2017
 * @author koldar
 */

#ifndef REGULAREXPRESSION_H_
#define REGULAREXPRESSION_H_

#include <regex.h>
#include <stdbool.h>
#include "macros.h"
#include "var_args.h"

/**
 * Represents a digit
 */
#define CU_RE_DIGIT "[[:digit:]]"
/**
 * Convenience macro that can be used within regex representing a unsigned integer number
 */
#define CU_RE_NUMBER "[[:digit:]][[:digit:]]*"
/**
 * Convenience macro that can be used within regex representing a space
 */
#define CU_RE_SPACE "[[:blank:]][[:blank:]]*"
/**
 * Convenience macro user to represents zero or more spaces
 */
#define CU_RE_SPACEQ "[[:blank:]]*"
/**
 * Convenience macro that can be used within regex representing a alphabetic word (case unsensitive)
 */
#define CU_RE_WORD "[[:alpha:]][[:alpha:]]*"
/**
 * zero or more alphabetic words (case unsensitive)
 */
#define CU_RE_WORDQ "[[:alpha:]]*"

/**
 * Convenience macro that can be used within regex representing an alphanumeric word (case unsensitive)
 */
#define CU_RE_ALPHAWORD "[[:alnum:]][[:alnum:]]*"
/**
 * Conveniente macro for a string composed by either with a letter, a digit, "." or "+"
 */
#define CU_RE_ALPHANUMDOT "[\\.\\+a-zA-Z0-9][\\.\\+a-zA-Z0-9]*"
/**
 * Capturing group
 *
 * @code
 * cuRegularExpressionApply("ciao !Mondo!", "!" CU_RE_CG(CU_RE_WORD) "!", 0, groups);
 * @endcode
 *
 * @param[in] x the regular pattern you want to capture
 */
#define CU_RE_CG(x) "\\(" x "\\)"


#define _CU_RE_SEP(NAME, I, REC, RES) REC "\\|" RES
#define _CU_RE_OP(NAME, X, I) X
/**
 * Alternatives pattern
 *
 * This will put lots of "|" between the choices you have put.
 * It directly translates into:
 * @code
 * (?:5|d|r|\+)
 * @endcode
 *
 * @param[in] ... the choices you want to have.
 */
#define CU_RE_PIPE(...) P99_FOR(_, P99_NARG(__VA_ARGS__), _CU_RE_SEP, _CU_RE_OP, __VA_ARGS__)
/**
 * Quack encoding of the operator "+"
 *
 * For example
 * @code
 * CU_RE_1N("[[:digit]]");
 * @endcode
 *
 * means one or more digits.
 *
 * @param[in] x what you want to repeat one or more times
 */
#define CU_RE_1N(x) x x "*"

/**
 * Execute a regular expression
 *
 * After performing the function call, you can retrieve all the capturing groups via:
 *
 * @includedoc regularExpressionExample.dox
 *
 * TODO groupSize can be removed! check https://stackoverflow.com/a/4604945/1887602
 *
 * @param[in] string the string to apply the regex on
 * @param[in] regex the regular expression. See the link <a href="http://www.regular-expressions.info/posix.html">here</a>
 * @param[in] groupSize the number of capturing groups inside the regular expression. A capturing group is one like <tt>\(ciao\)</tt>.
 * For exampel if in your regular expression there are 4 capturing groups, put here 4
 * @param[out] groups a double point of char that will be populated with all the capturing groups. The value is ignored if groupSize is 0
 * @return
 * 	\li true if string maatches the regex;
 * 	\li false otherwise. There can be several cause on this scenario: malformed regex, string doesn't match regex and so on
 */
bool cuRegularExpressionApply(CU_NOTNULL const char* restrict string, CU_NOTNULL const char* restrict regex, int groupSize, CU_NULLABLE char const *const * * groups);

/**
 * Check if an expression satisfies a regular expression
 *
 * If you want to use capturing groups, consider using ::cuRegularExpressionApply
 *
 * @param[in] string the string to check
 * @param[in] regex the regular expression
 * @return
 *  @li true if @c string is compliant with @c regex;
 *  @li false otherwise
 */
bool cuRegularExpressionIsSatisfying(CU_NOTNULL const char* restrict string, CU_NOTNULL const char* restrict regex);

/**
 * Release from the memory the data inside groups ( see ::cuRegularExpressionApply) generated by ::cuRegularExpressionApply function
 *
 * @param[in] groupSize the number of capturing group inside the regex. It is the same value you have put in ::cuRegularExpressionApply
 * @param[in] groups the array of string matched
 */
void cuRegularExpressionDestroyGroupInfo(int groupSize, CU_NOTNULL char const *const *const * groups);

#endif /* REGULAREXPRESSION_H_ */
