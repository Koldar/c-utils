/**
 * @file
 *
 * Contains several useful macros used throughout all the projects
 *
 * \note
 * This file patch some macro definitions within P99 project.
 *
 * <h1>Default arguments in fuctions</h1>
 *
 * This API is directly derived from P99 project.
 * Here's a brief example on how to use it
 *
 * @code
 * //a function with a lot of parameters; I want c and d to have default values
 * int defaultFunction(int a, int b, int c, int d) {r
 * 	return a+b+c+d;
 * }
 *
 * //let P99 know the prototype of the full function you want to have default values
 * CU_DECLARE_FUNCTION_WITH_DEFAULTS(int, defaultFunction, int, int, int, int);
 * //overwrite defaultFunction function with a macro: you need to give the name of the underlying function and the number of maximum parameters it accepts
 * #define defaultFunction(...) CU_CALL_FUNCTION_WITH_DEFAULTS(defaultFunction, 4, __VA_ARGS__)
 * //let P99 know the default arguments of each parameter. Empty parameter means no default value (hence it is required)
 * CU_DECLARE_DEFAULT_VALUES(defaultFunction,
 * 		, //no default value for A: it is required
 * 		, //no default value for B: it is required
 * 		5, //default value for C
 * 		10 //default vlaue for d
 * );
 *
 * //testing the default value
 * void testDefaultArguments() {
 *  assert(defaultFunction(1, 2, 3, 4) == 10);
 *  ssert(defaultFunction(1, 2, 3) == 16); //d=10
 *  assert(defaultFunction(1, 2) == 18); //c=5, d=10
 *  //assert(defaultFunction(1) == 17); //error
 * }
 * @endcode
 *
 *
 * @date May 17, 2017
 * @author koldar
 */

#ifndef MACROS_H_
#define MACROS_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <P99/p99_if.h>
#include <P99/p99.h>
#include <P99/p99_paste.h>
#include <P99/p99_generated.h>
#include <P99/p99_defarg.h>
#include "cutilsConfig.h"

#define VALUE(x) x
#define XSTR(x) #x
#define STR(x) XSTR(x)
#define CONCAT(x,y) x##y

#define TOKENPASTE(x, y) x ## y
#define TOKENPASTE2(x, y) TOKENPASTE(x, y)

/** @addtogroup defaultArgs
 * @{
 * @brief implements default argument for functions
 *
 * These macros are useful to declare functions with default parameters. The implementation is C99 compliant and exploit P99 module.
 * Here's an example:
 *
 * @includedoc defaultArguments.dox
 *
 */

/**
 * Function allowing you to decalre that a function indeed uses defauolt arguments
 *
 * @includedoc defaultArguments.dox
 *
 * @param[in] returnValue the type of the return value of the function where you want to use default values.
 * 	Put `void` if the function does not have a return value
 * @param[in] functionName the name of the function using default parameters
 * @param[in] ... list of ordered types each representing the type of a function parameter. Do not put anything if the the function has no parameters
 */
#define CU_DECLARE_FUNCTION_WITH_DEFAULTS P99_PROTOTYPE

/**
 * Call a function which uses default parameters
 *
 * @includedoc defaultArguments.dox
 *
 * This macro actually executes \c functionName
 *
 * @param[in] functionName the name of the function using default parameters
 * @param[in] paramsNumber number of the paramters the function has
 * @param[in] ... the actual value of the parameters to inject in the function
 */
#define CU_CALL_FUNCTION_WITH_DEFAULTS(functionName, paramsNumber, ...) \
	P99_CALL_DEFARG(functionName, paramsNumber, __VA_ARGS__)

/**
 * Declare what are the defaul value of each parameter
 *
 * This function simple provides declarations of the the default values, but it does not actually creates them.
 * ::CU_DEFINE_DEFAULT_VALUES perform such task. This macro is handy when the function requiring the default values is declared
 * in an header file and you don't want to pollute the header with ::CU_DEFINE_DEFAULT_VALUES (since that macro defines new functions).
 * This function is used in the default argument machinery only when you wnat to have an header and a source file separated.
 * Ignore it if you have only the source code.
 *
 * @includedoc defaultArguments.dox
 *
 * @param[in] functionName the name of the function using default paramteres
 * @param[in] ... a set of values to inject to each parameter of \c functionName. Do not put anything if the parameter is required;
 * 		put a value otherwise
 * @see CU_DEFINE_DEFAULT_VALUES
 */
#define CU_DECLARE_DEFAULT_VALUES P99_DEFINE_DEFARG

/**
 * Defines what are the default value of each parameter
 *
 * @includedoc defaultArguments.dox
 *
 * This function actually defines functions for the default value, hence it should always be put in <tt>*.c</tt> files
 *
 * @param[in] functionName the name of the function using default paramteres
 * @param[in] ... a set of values to inject to each parameter of \c functionName. Do not put anything if the parameter is required;
 * 		put a value otherwise
 * @see CU_DECLARE_DEFAULT_VALUES
 */
#define CU_DEFINE_DEFAULT_VALUES P99_DECLARE_DEFARG

///@}

///@addtogroup p99Patches
///@{

/**
 * Patch for declaration of default argument machinery
 *
 * Usually the declaration of functions uses the "inline" reserved word. I don't like it. Let the compiler handles it
 */
#undef p00_instantiate
#define p00_instantiate extern

/**
 * Patch for default argument machinery
 *
 *
 * On my computer the inline functions don't generate code and thereby cannot be called anywhere.
 * What I had done is to remove the "inline" keywords at the beginning of the default functions definition
 */
#undef P00_EXPR_FUNCTION
#define P00_EXPR_FUNCTION(NAME, X, N)                                   \
P99_IF_EMPTY(X)                                                         \
()                                                                      \
(                                                                       \
 P99_PASTE3(NAME, _prototype_, N) P99_PASTE3(NAME, _defarg_, N)(void) { \
   P99_PASTE3(NAME, _prototype_, N) p00_ret = (X);                      \
   return p00_ret;                                                      \
 }                                                                      \
)


///@}

/**@addtogroup documentationMacro
 * @{
 *
 * @brief macros to help documenting the code
 *
 */

/**
 * Macro to document the type of a pointer
 *
 * Function pointers are useful, but if you want to inject a <tt>void\*</tt> they are a bit too uninformative.
 * Suppose to have the following
 *
 * @code
 * typedef void (*destructor)(const void*);
 * @endcode
 *
 * It represents a simple destructor which deallocates memory from the heap.
 * But if you want to create a function which destroy a custom struct (like a point2D), you usually declared it like:
 *
 * @code
 * void destroyPoint2D(const point2d* p, CU_NULLABLE const struct var_args* context);
 * @endcode
 *
 * Sadly if you try to do something like:
 *
 * @code
 * destructor d = destroyPoint2D;
 * @endcode
 *
 * The compiler will raise a warning. Using this macro prevents the warning:
 *
 * @code
 * void destroyPoint2D(const CU_PDOC(point2d*, CU_NULLABLE const struct var_args* context) p);
 * destructor d = destroyPoint2D;
 * @endcode
 */
#define CU_PDOC(...) void*



/**
 * Macro telling you a variable can be NULL
 *
 * @code
 * void handleObject(CU_NULLABLE void* obj, int priority);
 * @endcode
 *
 * This macro, although it does nothing, is correctly print in doxygen output.
 *
 */
#define CU_NULLABLE /*can be null*/

/**
 * Macro telling you a variable can't be NULL
 *
 * @code
 * void handleObject(CU_NOTNULL void* obj, int priority);
 * @endcode
 *
 * This macro, although it does nothing, is correctly print in doxygen output.
 *
 */
#define CU_NOTNULL /*Can't be null*/

///@}

/**@addtogroup warningHelper
 * @{
 *
 * @brief macros to help removing warnings
 *
 */

/**
 * Cast an integer to a pointer
 *
 * \note
 * This is normally not a completely safe operation since <tt>size(int)</tt> might be different than <tt>sizeof(void*)</tt>.
 *
 * You can use this macro to cast an integer to a pointer without raising a warning. For example:
 *
 * @code
 * int a = 5;
 * void* p = (void*) a; //this raise a warning
 * void* q = CU_CAST_INT2PTR(a); //this doesn't raise a warning
 * @endcode
 *
 * @param[in] x the integer to cast to a `void*`
 */
#define CU_CAST_INT2PTR(x) ((void*)((intptr_t)(x)))

/**
 * Cast a pointer into an integer
 *
 * @code
 * 	int* p = listCell->payload; //stores an integer as pointer
 * 	int q = CU_CAST_PTR2INT(p);
 * @endcode
 *
 * @param[in] x the pointer to cast to int
 */
#define CU_CAST_PTR2INT(x) ((intptr_t)(x))

/**
 * Cast a boolean to a pointer
 *
 * \note
 * This is normally not a completely safe operation since <tt>size(bool)</tt> might be different than <tt>sizeof(void*)</tt>.
 *
 * You can use this macro to cast a boolean to a pointer without raising a warning. For example:
 *
 * @code
 * #include <stdbool.h>
 * bool a = true;
 * void* p = (void*) a; //this raise a warning
 * void* q = CU_CAST_INT2PTR(a); //this doesn't raise a warning
 * @endcode
 *
 * @param[in] x the value to cast
 */
#define CU_CAST_BOOL2PTR(x) CU_CAST_INT2PTR(x)

/**
 * Cast a unsigned long into a pointer
 *
 * @attention
 * this is not completely safe since <tt>sizeof(unsigned long)>sizeof(void*)</tt>
 *
 * @code
 * unsigned long x = 5;
 * CU_CAST_UL2PTR(x);
 * @endcode
 *
 * @param[in] x the value to cast
 */
#define CU_CAST_UL2PTR(x) ((void*)x)

/**
 * This macro implements an assignment but in a way to avoid warning due to casting.
 *
 * Suppose you have a data structure wrapping a stack:
 *
 * @code
 * struct stack {
 * 	int size;
 * 	int top;
 * 	void* s;
 * }
 *
 * void addToArray(struct stack* stack, const void* payload) {
 * 	stack->s[top] = payload;
 * 	top++;
 * }
 * @endcode
 *
 * Clearly \c addToArray doesn't modify the payload, hence its const-ness. However in general the data structure
 * has "s" defined as <tt>void\*</tt> since other functions may alter the stack itself. In the assignment:
 *
 * @code
 * stack->s[top] = payload;
 * @endcode
 *
 * a normal compiler arises the warning "assignment discards ‘const’ qualifier from pointer target type". If you're compiling with "-Werror" this might
 * be a problem.
 * For scenarios like this ::CU_SAFE_ASSIGN is perfect. Just replace the assignment with this macro call:
 *
 * @code
 * stack->s[top] = payload; //to be replaced
 * CU_SAFE_ASSIGN(stack->s[top], payload);
 * @endcode
 *
 */
#define CU_SAFE_ASSIGN(lvalue, rvalue) memcpy(&(lvalue), &(rvalue), sizeof(rvalue))

///@}

/**@addtogroup improvingCLanguage
 * @{
 *
 * @brief macros to improve the C programming without affecting performances too much
 *
 */


/**
 * generates a compile error with the given string error
 *
 * Use this macro when you need to generate a compile error within a macro
 *
 * @code
 * CU_COMPILE_ERROR();
 * @endcode
 *
 */
#define CU_COMPILE_ERROR() _Pragma("GCC error \"custom compile error!\"")

/**
 * generates a compile warning with the given string warning
 *
 * Use this macro when you need to generate a compile warning within a macro
 *
 * @code
 * CU_COMPILE_ERROR();
 * @endcode
 *
 */
#define CU_COMPILE_WARNING() _Pragma("GCC warning \"custom compile warning\"")

//TODO remove
//#ifdef __GNUC__
/**
 * Allows you to write a macro definition inside a macro argument
 *
 * @attention
 * This macro is available only on GCC compilers! It's heaviuly unportable!
 *
 * @code
 * CU_CREATE_NEW_MACRO( //new line is mandatory!
 * #define foo "bar"
 * )
 *
 * int main() {
 * 	printf(foo);
 * }
 * @endcode
 *
 * @param[in] macroDefinition the whole macro definition argument. It includes the `#define`
 * 	preprocessor. In order to work the parameter needs to be in a new line!
 */
//#define CU_CREATE_NEW_MACRO(macroDefinition) macroDefinition
//#else
//	#define _CU_CREATE_NEW_MACRO(...) CU_COMPILE_ERROR()
//	#define CU_CREATE_NEW_MACRO(macroName, macroValue) CU_COMPILE_ERROR()
//#endif

#define _CU_DEFINE_FUNCTION_POINTER_OP(CONTEXT, INDEX, CURRENT, NEXT) P99_PASTE(CURRENT, NEXT)
#define _CU_DEFINE_FUNCTION_POINTER_FUNC(CONTEXT, INDEX, CURRENT) P99_PASTE(_, CURRENT)


#define _CU_GET_FUNCTION_POINTER_MACRO(functionPointerType, returnValue, functionName, ...) \
	P99_PASTE(CU_FUNCTION_POINTER, _, functionPointerType, _, returnValue, _, functionName, CU_FOR_EACH(, _CU_DEFINE_FUNCTION_POINTER_OP, _CU_DEFINE_FUNCTION_POINTER_FUNC, ## __VA_ARGS__))

/**
 * Create a pointer of `x` for ::CU_ENSURE_FUNCTION_POINTER
 *
 * @code
 * 	CU_FT_PTR(int) //intPtr
 * @endcode
 *
 * @param[in] x type which needs to be on pointer
 */
#define CU_FT_PTR(x) P99_PASTE(x, Ptr)

/**
 * Creates an array of `x` for ::CU_ENSURE_FUNCTION_POINTER
 *
 * @code
 * 	CU_FT_ARRAY(int) //intArray
 * @endcode
 *
 * @param[in] x type which needs to be on array
 */
#define CU_FT_ARRAY(x) P99_PASTE(x, Array)

/**
 * Creates the const of `x` for ::CU_ENSURE_FUNCTION_POINTER
 *
 * @code
 * 	CU_FT_CONST(int) //intConst
 * @endcode
 *
 * @param[in] x type which needs to be on const
 */
#define CU_FT_CONST(x) P99_PASTE(x, Const)

/**
 * Creates a volatile of `x` for ::CU_ENSURE_FUNCTION_POINTER
 *
 * @code
 * 	CU_FT_VOLATILE(int) //intVolatile
 * @endcode
 *
 * @param[in] x type which needs to be on volatile
 */
#define CU_FT_VOLATILE(x) P99_PASTE(x, Volatile)

/**
 * Creates a restrict of `x` for ::CU_ENSURE_FUNCTION_POINTER
 *
 * @code
 * 	CU_FT_RESTRICT(int) //intRestrict
 * @endcode
 *
 * @param[in] x type which needs to be on restrict
 */
#define CU_FT_RESTRICT(x) P99_PASTE(x, Restrict)

/**
 * Creates an const pointer of `x` for ::CU_ENSURE_FUNCTION_POINTER
 *
 * @code
 * 	CU_FT_CONST_PTR(int) //intConstPtr
 * @endcode
 *
 * @param[in] x type which needs to be on const pointer
 */
#define CU_FT_CONST_PTR(x) CU_FT_PTR(CU_FT_CONST(x))

#define _CU_FT_OP(CONTEXT, INDEX, CURRENT, NEXT) CURRENT(NEXT)
#define _CU_FT_FUNC(CONTEXT, INDEX, CURRENT) P99_IF_EQ_1(INDEX)\
	(P99_PASTE(CU_FT_, CURRENT)(CONTEXT)) \
	(P99_PASTE(CU_FT_, CURRENT))

/**
 * Creates a type with the modifiers specified in `...`
 *
 * @code
 *  CU_FT_(int, CONST, PTR, PTR, RESTRICT, ARRAY) //intConstPtrPtrRestrictArray
 * @endcode
 *
 * @param[in] x type which needs to be output using the modifiers in `...`
 * @param[in] ... list of modifiers. Accepted values are all macros starting with CU_FT. For example if you want to generate a const int call `CU_FT(int, CONST)`.
 * 	This will work because macro CU_FT_CONST exists and is defined.
 */
#define CU_FT(x, ...) CU_FOR_EACH(x, _CU_FT_OP, _CU_FT_FUNC, ## __VA_ARGS__)

/**
 * Ensures that a given function has the expected signature.
 *
 * If the function has a different signature, a compile-time error will be thrown.
 * Otherwise the function pointer will be casted to the function pointer type `castTo`.
 *
 * In order to work, when you declared `functionPointer` you mandatory need to define the
 * following macro (and set it to 1):
 *
 * @code
 *  typedef X (*<function_type>)(...);
 *
 *  #define CU_FUNCTION_POINTER_<functionPointerType>_<return_value>_<functionName>_<type_param1>_<type_param2>_<type_param_i>_<type_param_N> 1
 *  //function declaration <functionName>
 *
 *  #define ENSURE_FUNCTION_TYPE(functionName) CU_ENSURE_FUNCTION_POINTER(function_type, X, functionName, type_param1, type_param2, ..., type_param_N)
 *
 *  function_type localVariable = CU_ENSURE_FUNCTION_TYPE(functionName);
 * @endcode
 *
 * For example assume you have a function type which destroy an object:
 *
 * @code
 * //define typedef and ENSURE_DESTRUCTOR MACRO
 * typedef void (*destructor)(const void*, int context);
 * #define ENSURE_DESTRUCTOR(functionName) CU_ENSURE_FUNCTION_POINTER(1, destructor, void, functionName, voidPtr, int);
 *
 * //define a function which is compliant with destructor
 * #define CU_FUNCTION_POINTER_destructor_void_destroyIntPointer_voidPtr_int 1
 * void destroyIntPointer(const int* p, int context) {
 * 	free((void*)p);
 * 	if (context == 3) {
 * 		printf("hello\n");
 * 	}
 * }
 *
 * //define a function which is NOT compliant with destructor
 * void foo(int a, int b) {
 * 	return a + b;
 * }
 *
 * //example of usage
 * int main() {
 * 	destructor d = CU_ENSURE_DESTRUCTOR(destroyIntPointer); //this will generate a destroyIntPointer which is compliant with destructor type
 * 	destructor d2 = CU_ENSURE_DESTRUCTOR(foo); //this will generate a compile error
 * }
 *
 * @endcode
 *
 * Actually, very weak conditions needs to be ensure to let the whole machinery work. First of all the CU_ENSURE_FUNCTION_POINTER macro call:
 * the return value and the parameters type are actually generic strings. You should use a convention over all your codebase. For example a convention maybe
 * pointer have `Ptr` at end, arrays have `Array` at end and so on. You then need to write those string **as is** in the macro definition above
 * your function definition (in the exmaple `CU_FUNCTION_POINTER_void_destroyIntPointer_voidPtr_int`): the macro name always start with
 * `CU_FUNCTION_POINTER` followed by the string representing the return value, its name and the same strings you used in `CU_FUNCTION_POINTER`. This means
 * the example would have even if you have set the names without any association with the actual function signature:
 *
 * @code
 * //define typedef and ENSURE_DESTRUCTOR MACRO
 * typedef void (*destructor)(const void*, int context);
 * #define ENSURE_DESTRUCTOR(functionName) CU_ENSURE_FUNCTION_POINTER(1, destructor, hello, functionName, world);
 *
 * //define a function which is compliant with destructor
 * #define CU_FUNCTION_POINTER_destructor_hello_destroyIntPointer_world 1
 * void destroyIntPointer(const int* p, int context) {
 * 	free((void*)p);
 * 	if (context == 3) {
 * 		printf("hello\n");
 * 	}
 * }
 *
 * //define a function which is NOT compliant with destructor
 * void foo(int a, int b) {
 * 	return a + b;
 * }
 *
 * //example of usage
 * int main() {
 * 	destructor d = CU_ENSURE_DESTRUCTOR(destroyIntPointer); //this will generate a destroyIntPointer which is compliant with destructor type
 * 	destructor d2 = CU_ENSURE_DESTRUCTOR(foo); //this will generate a compile error
 * }
 *
 * @endcode
 *
 * If you need this machinery for multiple typedefs, you can use differnet values of the first parameter of ::CU_ENSURE_FUNCTION_POINTER
 *
 * @note
 * If you want to use a convention, you may as well use the macro starting with CU_FT_
 *
 * @param[in] valueToCheck an integer value that the macro of the function should have
 * @param[in] castTo the type you want `functionPointer` to be casted to
 * @param[in] returnValue the type that the expected signature of `functionPointer` should be return
 * @param[in] ... type of parameters `functionPointer` should have
 */
#define CU_ENSURE_FUNCTION_POINTER(valueToCheck, castTo, returnValue, functionPointer, ...) \
		P99_IF_EQ(valueToCheck, _CU_GET_FUNCTION_POINTER_MACRO(castTo, returnValue, functionPointer, ## __VA_ARGS__)) \
			((castTo)(functionPointer)) \
			(CU_COMPILE_ERROR())
//CU_FT_TYPE(int, PTR, PTR, ARRAY)


/**
 * evaluates to true/false depending if a macro has been defined or not in compile time
 *
 * @note
 * This macro will not be evaluated during runtime!
 *
 * @code
 *  #define FOO
 *  CU_IS_MACRO_DEFINED(FOO) //evaluates to true
 *  CU_IS_MACRO_DEFINED(BAR) //evaluates to false
 * @endcode
 *
 * @param[in] ... name of the macro to check
 */
#define CU_IS_MACRO_DEFINED(...) \
	P99_IF_EMPTY(__VA_ARGS__)(true)(false)

/**
 * Perform different operation depending if a particular macro has been defined or not
 *
 * The macro is useful when you cannot use `#ifdef` directive, namely inside another macro
 *
 * @code
 * CU_IF_MACRO_DEFINED(FOO)(<action to take when the macro is defined>)(<action to take when the macro is not defined>)
 * @endcode
 *
 * Follows a concrete example:
 *
 * @code
 * #define FOO
 * int a = CU_IF_MACRO_DEFINED(FOO)(5)(7); //a will be 5
 * int b = CU_IF_MACRO_DEFINED(BAR)(5)(7); //b will be 7
 * @endcode
 *
 * @param[in] ... name of the macro to check
 */
#define CU_IF_MACRO_DEFINED(...) \
	P99_IF_EQ(P99_IS_EMPTY(__VA_ARGS__), 1)

#define _CU_MULTILINE_SEP(NAME, I, REC, RES) REC RES
#define _CU_MULTILINE_OP(NAME, X, I) X
/**
 * Creates a multiline string
 *
 * The substring will be concatenated automatically without inserting carriage returns.
 *
 * For example
 *
 * ```
 * CU_MULTILINE(
 * 	"Hello ",
 * 	"World!"
 * )
 * ```
 * Will result in:
 * ```
 * Hello World!
 * ```
 *
 * @param[in] ... a set of C string which will be concatenated together
 */
#define CU_MULTILINE(...) P99_FOR(_, P99_NARG(__VA_ARGS__), _CU_MULTILINE_SEP, _CU_MULTILINE_OP, __VA_ARGS__)

#define _CU_MULTILINE_N_SEP(NAME, I, REC, RES) REC "\n" RES
#define _CU_MULTILINE_N_OP(NAME, X, I) X
/**
 * Creates a multiline string
 *
 * The substring will be concatenated by automatically inserting carriage returns.
 *
 * For example
 *
 * ```
 * CU_MULTILINE_N(
 * 	"Hello",
 * 	"World!"
 * )
 * ```
 * Will result in:
 * ```
 * Hello
 * World!
 * ```
 *
 * @param[in] ... a set of C string which will be concatenated together
 */
#define CU_MULTILINE_N(...) P99_FOR(_, P99_NARG(__VA_ARGS__), _CU_MULTILINE_N_SEP, _CU_MULTILINE_N_OP, __VA_ARGS__)


#ifdef DEBUG

#define CU_REQUIRE(x) { \
		if (!(x)) { \
			ERROR_IMPOSSIBLE_SCENARIO("assertion \"" #x "\" failed"); \
		} \
	}

/**
 * check that a certain pointer is not null.
 *
 * @note
 * In production, this check is automatically ignored
 *
 * If it is raise an error
 *
 * @param[in] x the pointer to check
 */
#define CU_REQUIRE_NOTNULL(x) { \
		if ((x) == NULL) { \
			ERROR_NULL(x); \
		} \
	}

/**
 * check that a certain pointer is null.
 *
 * @note
 * In production, this check is automatically ignored
 *
 * If the pointer is not NULL the macro raise an error
 *
 * @param[in] x the pointer to check
 */
#define CU_REQUIRE_NULL(x) { \
		if ((x) == NULL) { \
			ERROR_NULL(x); \
		} \
	}

/**
 * check that a value is less than another value
 *
 * @note
 * An error is raises if the requirment is not satisfied
 *
 * The check if performed only if macro DEBUG is defined
 *
 * @param[in] x first value
 * @param[in] y second value
 */
#define CU_REQUIRE_LT(x, y) { \
	if ((x) >= (y)) { \
		ERROR_IMPOSSIBLE_SCENARIO("assert \"" #x " < " #y "\" failed"); \
	} \
}

/**
 * check that a value is less or equal than another value
 *
 * @note
 * An error is raises if the requirment is not satisfied
 *
 * The check if performed only if macro DEBUG is defined
 *
 * @param[in] x first value
 * @param[in] y second value
 */
#define CU_REQUIRE_LEQ(x, y) { \
	if ((x) >= (y)) { \
		ERROR_IMPOSSIBLE_SCENARIO("assert \"" #x " <= " #y "\" failed"); \
	} \
}

/**
 * check that a value is greater than another value
 *
 * @note
 * An error is raises if the requirment is not satisfied
 *
 * The check if performed only if macro DEBUG is defined
 *
 * @param[in] x first value
 * @param[in] y second value
 */
#define CU_REQUIRE_GT(x, y) { \
	if ((x) <= (y)) { \
		ERROR_IMPOSSIBLE_SCENARIO("assert \"" #x " > " #y "\" failed"); \
	} \
}

/**
 * check that a value is greater or equal than another value
 *
 * @note
 * An error is raises if the requirment is not satisfied
 *
 * The check if performed only if macro DEBUG is defined
 *
 * @param[in] x first value
 * @param[in] y second value
 */
#define CU_REQUIRE_GEQ(x, y) { \
	if ((x) < (y)) { \
		ERROR_IMPOSSIBLE_SCENARIO("assert \"" #x " >= " #y "\" failed"); \
	} \
}

/**
 * check that a certain expression is true.
 *
 * @note
 * In production, this check is automatically ignored
 *
 *
 * @param[in] x the expression to check
 */
#define CU_REQUIRE_TRUE(x) { \
	if (!(x)) { \
		ERROR_IMPOSSIBLE_SCENARIO("assertion \"" #x "\" is false"); \
	} \
	}

/**
 * check that a certain expression is false.
 *
 * @note
 * In production, this check is automatically ignored
 *
 *
 * @param[in] x the expression to check
 */
#define CU_REQUIRE_FALSE(x) { \
	if ((x)) { \
		ERROR_IMPOSSIBLE_SCENARIO("assertion \"" #x "\" is true"); \
	} \
	}

/**
 * check that a certain expression is false.
 *
 * @note
 * In production, this check is automatically ignored
 *
 *
 * @param[in] x the expression to check
 */
#define CU_REQUIRE_FALSE(x) { \
	if ((x)) { \
		ERROR_IMPOSSIBLE_SCENARIO("assertion \"" #x "\" is true"); \
	} \
	}

/**
 * Check that 2 values are actually the same.
 *
 * Comparison is performed via `x == y`
 *
 * @note
 * In production this check is automatically ignored
 *
 * @param[in] x first value to compare
 * @param[in] y second value to compare
 */
#define CU_REQUIRE_IS(x, y) { \
	if ((x) != (y)) { \
		ERROR_IMPOSSIBLE_SCENARIO("assertion \"" #x " == " #y "\" failed. x=" STR(x) " y=" STR(y) ); \
	} \
}

#define CU_REQUIRE_ISNOT(x, y) { \
	if ((x) == (y)) { \
		ERROR_IMPOSSIBLE_SCENARIO("assertion \"" #x " != " #y "\" failed. x=" STR(x) " y=" STR(y) ); \
	} \
}

#else
#define CU_REQUIRE(x) ;
#define CU_REQUIRE_NOTNULL(x) ;
#define CU_REQUIRE_NULL(x) ;
#define CU_REQUIRE_LT(x, y) ;
#define CU_REQUIRE_LEQ(x, y) ;
#define CU_REQUIRE_GT(x, y) ;
#define CU_REQUIRE_GEQ(x, y) ;
#define CU_REQUIRE_TRUE(x) ;
#define CU_REQUIRE_FALSE(x) ;
#define CU_REQUIRE_IS(x, y) ;
#define CU_REQUIRE_ISNOT(x, y) ;
#endif




#define CU_ENABLE_BIT(x, nbit) ((x) | 1UL << (nbit))
#define CU_DISABLE_BIT(x, nbit) ((x) & ~(1UL << (nbit)))
#define CU_SET_BIT(x, nbit, value) (value == 0 ? (CU_DISABLE_BIT(x, nbit)) : (CU_ENABLE_BIT(x, nbit)))
#define CU_TOGGLE_BIT(x, nbit) ((x) ^ (1UL << nbit))
#define CU_GET_BIT(x, nbit) ((x >> nbit) & 1U)

#define CU_GENERATE_RANGE_OP(CONTEXT, INDEX, CURRENT, NEXT) (CURRENT + NEXT)
#define CU_GENERATE_RANGE_FUNC(CONTEXT, INDEX, A) (0xFF << (INDEX * 8))

#define _CU_GENERATE_RANGE(x, ubExcluded) \
	P99_IF_EQ(P99_ADD(x, 1), ubExcluded)(x)(x, _CU_GENERATE_RANGE(P99_ADD(x, 1), ubExcluded))

#define CU_GENERATE_RANGE(lbIncluded, ubExcluded) _CU_GENERATE_RANGE(lbIncluded, ubExcluded)

#define CU_GENERATE_ALLONE_OP(CONTEXT, INDEX, CURRENT, NEXT) (CURRENT + NEXT)
#define CU_GENERATE_ALLONE_FUNC(CONTEXT, INDEX, A) (0xFF << (INDEX * 8))


#define CU_GENERATE_ALLONE(numberType) CU_FOR_EACH(, CU_GENERATE_ALLONE_OP, CU_GENERATE_ALLONE_FUNC, CU_GENERATE_RANGE(0, sizeof(numberType)))



/**
 * Avoid errors in preprocessor macros
 *
 * Assume you have a macro defined as follows:
 *
 * @code
 * #define FOO(x,y) x; y
 * @endcode
 *
 * Consider the following use case:
 *
 * @code
 * FOO(int a,b , char b='c') //int a,b is the first argument while char b='c' is the second one
 * @endcode
 *
 * The macro doesn't work for such input since the preprocess detects 3 arguments, not 2 (there are 2 commas).
 * One can define a macro which simpluy translate the comma in order to get the desired effect:
 *
 * @code
 * #define CU_COMMA ,
 * FOO(int a CU_COMMA b , char b='c') //int a,b is the first argument while char b='c' is the second one
 * @endcode
 *
 * For further information see <a href="https://stackoverflow.com/a/19841470/1887602">this SO answer</a>
 */
#define CU_COMMA ,

/**
 * First piece of for loop for CU_WITH
 *
 * @private
 *
 * @param[in] ... the code to execute before entering the for loop
 */
#define _CU_WITH_1(...) \
	for ( \
		bool UV(with_variable) = true; \
		UV(with_variable); \
	) \
		for ( \
				__VA_ARGS__; \
				UV(with_variable); \
				_CU_WITH_2 /*_CU_WITH_1 starts a for loop up until the third statement of the loop. _CU_WITH_2 completes such loop*/

/**
 * Second piece of for loop for CU_WITH
 *
 * @private
 *
 * @param[in] ... code to execute after exiting the for loop
 */
#define _CU_WITH_2(...) \
	__VA_ARGS__, \
		UV(with_variable) = false \
	)

/**
 * A structure mimicking `with` python syntactical structure
 *
 * An typical example is to automatically handle the destructor of a variable created in the heap
 * @code
 *  CU_WITH(int* a = malloc( sizeof(int) ) ) (free(a)) {
 *		*a = 0;
 *		*a += 2;
 *		assert(*a == 2);
 *	}
 *
 * @endcode
 *
 * generally, the macro can be used as follows:
 * @code
 * CU_WITH(setupCode)(endCode) {
 * 	//code
 * }
 * @endocde
 *
 * You can also use `break` statement to immedi
 *
 * @attention
 * This macro does **not** support signal detections, gotos or `return` statements within the wrapped block!
 *
 */
#define CU_WITH _CU_WITH_1

/**
 * Implements a switch case operating on strings
 *
 * In C99 switch cases can't work with string. This macro helps you in that
 *
 * @code
 * 	int a = 0;
 * 	char str[300] = "ciao"
 *	CU_STRING_SWITCH(str) {
 *		CU_CASE("ciao") {
 *			a = 1;
 *			break;
 *		}
 *		CU_CASE("world") {
 *			a = 2;
 *			break;
 *		}
 *		CU_DEFAULT() {
 *			a = 3;
 *			break;
 *		}
 *	}
 * @endcode
 *
 * @param[in] actualString the string to check
 * @see CU_CASE
 * @see CU_DEFAULT
 */
#define CU_STRING_SWITCH(actualString) \
	char* _cu_switchVar= (char*)(actualString); \
	for (bool UV(count)=true; UV(count); UV(count)=false) \
		if (false) { \
			0; \
		} else

/**
 * case for a particular value of a string
 *
 * @param[in] expectedString the string we expected
 * @see CU_STRING_SWITCH
 */
#define CU_CASE(expectedString) \
		if (isStrEqual(_cu_switchVar, expectedString)) \

/**
 * Default case for a particular value of a string
 *
 * @see CU_STRING_SWITCH
 */
#define CU_DEFAULT() \
		else

/**
 * A constant macro to hide a useless constant.
 *
 * Main usage is in ::CU_WRAP_CODE
 */
#define CU_NOCODE true

/**
 * Abstract view of a if
 *
 * The API allows you to execute a "before code" and a "after code" between an arbitrary code.
 * For example
 *
 *
 * @code
 * 	CU_WRAP_CODE(printf("before\n"), printf("after\n")) {
 * 		printf("between\n");
 * 	}
 *
 *  // output is:
 *  // before
 *  // between
 *  // after
 * @endcode
 *
 * If you don't want to put any code before or after the main code, just put ::CU_NOCODE.
 *
 * @code
 * 	CU_WRAP_CODE(printf("before"), CU_NOCODE) {
 * 		printf("between");
 * 	}
 * @endcode
 *
 * Ths macro can be safely nested together. The only condition is that they **cannot appear in the same line** of the file.
 *
 * @param[in] beforeStatement the C statement to execute before executing the main code
 * @param[in] afterStatement the C statement to execute after executing the main code
 */
#define CU_WRAP_CODE(beforeStatement, afterStatement) \
	for (bool UV(_cu_wrap_code_flag)=true; UV(_cu_wrap_code_flag); ) \
		for (beforeStatement; UV(_cu_wrap_code_flag); UV(_cu_wrap_code_flag)=false, afterStatement) \

/**
 * An implementation slightly faster than ::CU_WRAP_CODE but more intelligence consuming for the developer
 *
 * This api shines if you have multiple ::CU_WRAP_CODE nested. The drawback is that all the nested mult wrap **has to be on the same line**
 *
 * @note
 * Multi wrap codes cannot be nested. Hence you cannot do
 * 	@code
 * 		CU_MULTI_WRAP_CODE_START(, )
 * 			CU_MULTI_WRAP_CODE_START(, )
 * 				CU_MULTI_WRAP_CODE_END(, )
 * 					CU_MULTI_WRAP_CODE(, )
 * 						CU_MULTI_WRAP_CODE_END(, )
 * 	@endcode
 *
 * Thisis the correct way to use this api
 * @code
 *  //assume CU_MULTI_WRAP_CODE_START, CU_MULTI_WRAP_CODE and CU_MULTI_WRAP_CODE_END are on the same line
 * 	CU_MULTI_WRAP_CODE_START(int a = 4,) //the first wrap code needs to be CU_MULTI_WRAP_CODE_START
 * 		CU_MULTI_WRAP_CODE(char b = 5, ) //every intermediate wrap codes(possibly none) need to be CU_MULTI_WRAP_CODE
 * 			CU_MULTI_WRAP_CODE_END(char f[300]="ciao",) { //the last wrap code needs to be CU_MULTI_WRAP_CODE_END
 * 				printf("a is %d, b is %c and f is %s\n", a, b, f);
 * 	}
 * @endcode
 *
 * @param[in] beforeStatement an expression (possibly an assignment) to execute before the block
 * @param[in] afterStatement an expression to execute after the block
 *
 * @see CU_MULTI_WRAP_CODE
 * @see CU_MULTI_WRAP_CODE_END
 */
#define CU_MULTI_WRAP_CODE_START(beforeStatement, afterStatement) \
	for (bool UV(_cu_wrap_code_flag)=true; UV(_cu_wrap_code_flag); ) \
		CU_MULTI_WRAP_CODE(beforeStatement, afterStatement) \

/**
 * wraps code after ::CU_MULTI_WRAP_CODE_START and before ::CU_MULTI_WRAP_CODE_END
 *
 * @param[in] beforeStatement an expression (possibly an assignment) to execute before the block
 * @param[in] afterStatement an expression to execute after the block
 */
#define CU_MULTI_WRAP_CODE(beforeStatement, afterStatement) \
	for (beforeStatement; UV(_cu_wrap_code_flag); afterStatement) \

/**
 * last wrap code in a  wrap code sequence started by ::CU_MULTI_WRAP_CODE_START
 *
 * @param[in] beforeStatement an expression (possibly an assignment) to execute before the block
 * @param[in] afterStatement an expression to execute after the block
 */
#define CU_MULTI_WRAP_CODE_END(beforeStatement, afterStatement) \
	for (beforeStatement; UV(_cu_wrap_code_flag); UV(_cu_wrap_code_flag)=false, afterStatement) \

/**
 * Internal used in the implementation of  ::CU_NEWSCOPE_WITH_VARIABLES
 *
 * Check P99 project and the P99_FOR macro
 *
 * @private
 *
 * @param[in] countNumber context of P99_FOR. This represents the number of elements to cycle over
 * @param[in] i nuber between 0 and \c countNumber representing the i-th iteration
 * @param[in] rec building for content
 * @param[in] res new content to append to \c rec
 */
#define _CU_NEWSCOPE_WITH_VARIABLES_SEPARATOR(countNumber, i, rec, res) rec res
/**
 * Internal used in the implementation of  ::CU_NEWSCOPE_WITH_VARIABLES
 *
 * Check P99 project and the P99_FOR macro
 *
 * @private
 *
 * @param[in] countNumber context of P99_FOR. This represents the number of elements to cycle over
 * @param[in] x i-th value in the for macro
 * @param[in] i nuber between 0 and \c countNumber representing the i-th iteration
 */
#define _CU_NEWSCOPE_WITH_VARIABLES_OPERATION(countNumber, x, i) \
	P99_IF_EQ(i, 0)\
		(CU_MULTI_WRAP_CODE_START(x, CU_NOCODE)\
	)(\
		P99_IF_LT(i, countNumber-1)(\
			CU_MULTI_WRAP_CODE(x, CU_NO_CODE) \
		)(\
			CU_MULTI_WRAP_CODE_END(x, CU_NOCODE)\
		)\
	)

/**
 * Declare a new C scope and define some variables.
 *
 * The variable automatically stop exisiting after exiting from the scope
 *
 * CU_NEWSCOPE_WITH_VARIABLES(int a, char b = 'f') {
 * 	//do something with a and b
 * }
 *
 * @param[in] ... the definition or assignment of all the variables you wish
 */
#define CU_NEWSCOPE_WITH_VARIABLES(...) \
	P99_IF_EQ(CU_COUNT_ARGS(__VA_ARGS__), 0)(\
		\
	)(\
		P99_IF_EQ(CU_COUNT_ARGS(__VA_ARGS__),1)(\
			CU_WRAP_CODE(__VA_ARGS__, CU_NOCODE) \
		)(\
			P99_FOR(CU_COUNT_ARGS(__VA_ARGS__), CU_COUNT_ARGS(__VA_ARGS__), _CU_NEWSCOPE_WITH_VARIABLES_SEPARATOR, _CU_NEWSCOPE_WITH_VARIABLES_OPERATION, __VA_ARGS__) \
		)\
	)\

/**
 * Swap the contents of 2 variables
 *
 * An example of usage:
 *
 * @code
 * 	int* a = 0x555555;
 * 	int* b = 0x666666;
 * 	SWAP(a, b, int*);
 * @endcode
 *
 * @param[in] x the first variable
 * @param[in] y the second variable
 * @param[in] type the type of the variable \c x and \c y
 */
#define SWAP(x, y, type) { \
	type UV(tmp) = (x); \
	(x) = (y); \
	(y) = UV(tmp); \
}

/**
 * A shortcut for a well called malloc.
 *
 * for example:
 * @code
 * MALLOC(struct pippo)
 * @endcode
 * is automatically expanded into:
 * @code
 * (struct pippo*)malloc(sizeof(struct pippo))
 * @endcode
 *
 * @note
 * This macro won't automatically check the return value fo the malloc!
 *
 * @param[in] type the type of variable you want to malloc
 */
#define CU_MALLOC(type) ((type*)malloc(sizeof(type)))

/**
 * A safe call to \c free
 *
 * As exploained by linux <a href="http://yarchive.net/comp/const.html">here</a>, free should have its parameter "const".
 *
 * I tend to call free with parameters which hare const. However, since \c free does not accept the "const" itself, it generates
 * a warning. This macro solves it.
 *
 * @code
 *  void destroyInt(const int* i, CU_NULLABLE const struct var_args* context) {
 *  	free(i); //it generates a warning!
 *  	CU_FREE(i); //this wont' since it is translated into free((void*)i);
 *  }
 *
 *  int main() {
 * 	 int* a = malloc(sizeof(int));
 * 	 destroyInt(a);
 * 	}
 *
 * @endcode
 *
 * @param[in] call the object to call free upon
 */
#define CU_FREE(call) free((void*)call)

/**
 * @includedoc repeatUntil.dox
 *
 */
#define CU_REPEAT do

/**
 * @includedoc repeatUntil.dox
 *
 * @includedoc untilDo.dox
 *
 * @param[in] condition when the condition is true, you will be able to exit from the loop.
 */
#define CU_UNTIL(condition) while (!(condition))

/**
 * Realize the `until ...do` cycle
 *
 * @includedoc untilDo.dox
 */
#define CU_DO /*nothing*/

///@}

/**
 * Unique Variable
 *
 * Generate a unique name of a particular varaible. Useful in macro definitions
 *
 * See <a="http://stackoverflow.com/a/1597129/1887602">here</a> for further information
 */
#define UV(v) TOKENPASTE2(v, __LINE__)

/**
 * Convenience macro to put values within a string. This is especially useful when generating paths in the application
 *
 * @code
 * 	CU_SAVE_STRING_IN_BUFFER(300, filename, "%s/%s.dat2", path, "ciao");
 * 	//we can now use the variable filename. for instance:
 * 	strdup(filename);
 * @endcode
 *
 * @param[in] buffersize the sizeof the buffer to generate
 * @param[in] buffer the name of an identifier representing an array of char to use as a buffer
 * @param[in] template the format of the string to put inside the buffer. It's a `printf` like string
 * @param[in] ... values to put inside the @c template
 *
 */
#define CU_SAVE_STRING_IN_BUFFER(buffersize, buffer, template, ...) \
		char buffer[buffersize]; \
		cuSnprintf(buffer, buffersize, template, ## __VA_ARGS__);

/**
 * Convenience macro to check if this is the last iteration of a for loop
 *
 * In order to work, the for loop you're handlng **has** to be in the for
 *
 * @code
 * for (int i=0; i<check; i++) {
 *  //stuff...
 * }
 * @endcode
 *
 * You can use the macro as in the following:
 *
 * @code
 *  char buffer[300];
 *  char* names[] = {"hello", "world", "!"};
 *  int j = 0;
 * 	for (int i=0; i<3; i++) {
 * 		j += sprintf(&buffer[j], "%s%s", names[i], CU_LASTLOOP(i, 5) ? "" : ", ");
 * 	}
 * 	//this generates "hello, world, !"
 * @endcode
 *
 * @param[in] i the variable used to loop over
 * @param[in] check the maximum limit the variable needs to achieve
 */
#define CU_LASTLOOP(i, check) (((i)+1) >= (check))

/**
 * Convenience macro to check if this is **not** the last iteration of a for loop
 *
 * In order to work, the for loop you're handlng **has** to be in the for
 *
 * @code
 * for (int i=0; i<check; i++) {
 *  //stuff...
 * }
 * @endcode
 *
 *
 * @param[in] i the variable used to loop over
 * @param[in] check the maximum limit the variable needs to achieve
 */
#define CU_NOTLASTLOOP(i, check) (((i)+1) < (check))

/**
 * Convenience macro to check if this is the first iteration of a for loop
 *
 * In order to work, the for loop you're handlng **has** to be in the for
 *
 * @code
 * for (int i=start; i<5; i++) {
 *  //stuff...
 * }
 * @endcode
 *
 *
 * @param[in] i the variable used to loop over
 * @param[in] start the minumum limit the variable starts to have
 */
#define CU_FIRSTLOOP(i, start) ((i) == (start))

/**
 * Convenience macro to check if this is **not** the first iteration of a for loop
 *
 * In order to work, the for loop you're handlng **has** to be in the for
 *
 * @code
 * for (int i=start; i<5; i++) {
 *  //stuff...
 * }
 * @endcode
 *
 *
 * @param[in] i the variable used to loop over
 * @param[in] start the minumum limit the variable starts to have
 */
#define CU_NOTFIRSTLOOP(i, start) ((i) > (start))

/**
 * Convenience macro to check if this is **not** either the first nor the last iteration of a for loop
 *
 * In order to work, the for loop you're handlng **has** to be in the for
 *
 * @code
 * for (int i=start; i<end; i++) {
 *  //stuff...
 * }
 * @endcode
 *
 *
 * @param[in] i the variable used to loop over
 * @param[in] start the minumum limit the variable starts to have
 * @param[in] end the maximum limit the variable needs to achieve
 */
#define CU_MIDDLELOOP(i, start, end) (((i) > (start)) && (((i)+1) < (end)))

/**
 * \def STATICR
 *
 * This macro is replaced with "static" reserved word during release compilation.
 * If DEBUG macro is present, the macro is not replaced with anything.
 *
 * This macro is useful during debugging: you might want to test some internal component of your project.
 * Use this macro instead of the simple "static" reserved word: in this way during debugging you will be able to see it whilst
 * in production the function tested will be hidden
 */
#ifndef DEBUG
#	define STATICR static
#else
#	define STATICR
#endif

/**
 * \def EXPORT_SYMBOL
 *
 * This macro will allow you to add specific code to alter the linker exporting policy during building.
 * Annotate every function or variable you want to export in your shared library with EXPORT_SYMBOL.
 * Everything not annotated will, by default, not be exported at all. In this way you can avoid publishing stuff you don't want to export at all.
 *
 * During the compilation of your library please set the definition "BUILDING_LIBRARY" (ie via "-DBUILDING_LIBRARY").
 * When using the library, do your usual stuff.
 */
#if BUILDING_LIBRARY
#define EXPORT_SYMBOL __attribute__((__visibility__("default")))
#else
#define EXPORT_SYMBOL
#endif

#if BUILDING_LIBRARY
#define DONT_EXPORT_SYMBOL
#else
#define DONT_EXPORT_SYMBOL
#endif

/**
 * Retrieve the number of arguments within the parenthesis.
 *
 * @code
 * COUNTARGS(pippo, "5", 1E+3) //generate "3"
 * COUNTARGS() //generates "0"
 * @endcode
 *
 * @param[in] ... all the arguments you want to have
 */
#define CU_COUNT_ARGS(...) P99_NARG(__VA_ARGS__)

/**
 * Perform the actions only if the variadic field of the macro is not empty
 *
 * @code
 * #define TEST(...) \
 * 	CU_IF_HAS_VARIADIC_DO(__VA_ARGS__)( \
 * 		foo() \
 * 	) \
 * @endcode
 *
 * * @param[in] ... the variadic you need to test
 */
#define CU_IF_HAS_VARIADIC_DO(...) P99_IF_EQ_0(CU_COUNT_ARGS(__VA_ARGS__))(0)
/**
 * Perform the actions only if the variadic field of the macro is empty
 *
 * @code
 * #define TEST(...) \
 * 	CU_IF_HAS_NOT_VARIADIC_DO(__VA_ARGS__)( \
 * 		foo() \
 * 	) \
 * @endcode
 *
 * @param[in] ... the variadic you need to test
 */
#define CU_IF_HAS_NOT_VARIADIC_DO(...) P99_IF_GT(CU_COUNT_ARGS(__VA_ARGS__), 0)(0)

/**
 * Perform an action depending if the variadic argument exist or not
 *
 * The usage is the following one:
 *
 * @code
 * #define EPRINTF(template, ...) CU_HANDLE_VARIADIC(__VA_ARGS__)(fprintf(stderr, template, __VA_ARGS__))(fprintf(stderr, template))
 * @endcode
 *
 * @param[in] ... the variadic you need to test
 */
#define CU_HANDLE_VARIADIC(...) P99_IF_GT(CU_COUNT_ARGS(__VA_ARGS__), 0)

/**
 * Check if the variadic argument exist
 *
 * @code
 * #define MACRO(...) __VA_ARGS__
 *
 * CU_HAS_VARIADIC() //0
 * CU_HAS_VARIADIC(hello) //1
 * CU_HAS_VARIADIC(hello, world) //2
 * @endcode
 *
 * @param[in] ... the variadic argument
 * @return
 * 	\li 1 if the variadic argument ahs at least one element
 * 	\li 0 otherwise
 */
#define CU_HAS_VARIADIC(...) P99_IF_EQ_0(CU_COUNT_ARGS(__VA_ARGS__))(0)(1)


#define CU_NLAST_VARIADIC(N, ...) P99_PASTE2(_CU_NLAST_VARIADIC_, N)(__VA_ARGS__)
#define _CU_NLAST_VARIADIC_1(...) P99_LAST(__VA_ARGS__)
#define _CU_NLAST_VARIADIC_2(...) _CU_NLAST_VARIADIC_1(P99_ALLBUTLAST(__VA_ARGS__))
#define _CU_NLAST_VARIADIC_3(...) _CU_NLAST_VARIADIC_2(P99_ALLBUTLAST(__VA_ARGS__))
#define _CU_NLAST_VARIADIC_4(...) _CU_NLAST_VARIADIC_3(P99_ALLBUTLAST(__VA_ARGS__))
#define _CU_NLAST_VARIADIC_5(...) _CU_NLAST_VARIADIC_4(P99_ALLBUTLAST(__VA_ARGS__))
#define _CU_NLAST_VARIADIC_6(...) _CU_NLAST_VARIADIC_5(P99_ALLBUTLAST(__VA_ARGS__))
#define _CU_NLAST_VARIADIC_7(...) _CU_NLAST_VARIADIC_6(P99_ALLBUTLAST(__VA_ARGS__))
#define _CU_NLAST_VARIADIC_8(...) _CU_NLAST_VARIADIC_7(P99_ALLBUTLAST(__VA_ARGS__))
#define _CU_NLAST_VARIADIC_9(...) _CU_NLAST_VARIADIC_8(P99_ALLBUTLAST(__VA_ARGS__))
#define _CU_NLAST_VARIADIC_10(...) _CU_NLAST_VARIADIC_9(P99_ALLBUTLAST(__VA_ARGS__))
#define _CU_NLAST_VARIADIC_11(...) _CU_NLAST_VARIADIC_10(P99_ALLBUTLAST(__VA_ARGS__))
#define _CU_NLAST_VARIADIC_12(...) _CU_NLAST_VARIADIC_11(P99_ALLBUTLAST(__VA_ARGS__))
#define _CU_NLAST_VARIADIC_13(...) _CU_NLAST_VARIADIC_12(P99_ALLBUTLAST(__VA_ARGS__))
#define _CU_NLAST_VARIADIC_14(...) _CU_NLAST_VARIADIC_13(P99_ALLBUTLAST(__VA_ARGS__))
#define _CU_NLAST_VARIADIC_15(...) _CU_NLAST_VARIADIC_14(P99_ALLBUTLAST(__VA_ARGS__))
#define _CU_NLAST_VARIADIC_16(...) _CU_NLAST_VARIADIC_15(P99_ALLBUTLAST(__VA_ARGS__))
#define _CU_NLAST_VARIADIC_17(...) _CU_NLAST_VARIADIC_16(P99_ALLBUTLAST(__VA_ARGS__))
#define _CU_NLAST_VARIADIC_18(...) _CU_NLAST_VARIADIC_17(P99_ALLBUTLAST(__VA_ARGS__))
#define _CU_NLAST_VARIADIC_19(...) _CU_NLAST_VARIADIC_18(P99_ALLBUTLAST(__VA_ARGS__))
#define _CU_NLAST_VARIADIC_20(...) _CU_NLAST_VARIADIC_19(P99_ALLBUTLAST(__VA_ARGS__))
#define _CU_NLAST_VARIADIC_21(...) _CU_NLAST_VARIADIC_20(P99_ALLBUTLAST(__VA_ARGS__))
#define _CU_NLAST_VARIADIC_22(...) _CU_NLAST_VARIADIC_21(P99_ALLBUTLAST(__VA_ARGS__))
#define _CU_NLAST_VARIADIC_23(...) _CU_NLAST_VARIADIC_22(P99_ALLBUTLAST(__VA_ARGS__))
#define _CU_NLAST_VARIADIC_24(...) _CU_NLAST_VARIADIC_23(P99_ALLBUTLAST(__VA_ARGS__))
#define _CU_NLAST_VARIADIC_25(...) _CU_NLAST_VARIADIC_24(P99_ALLBUTLAST(__VA_ARGS__))
#define _CU_NLAST_VARIADIC_26(...) _CU_NLAST_VARIADIC_25(P99_ALLBUTLAST(__VA_ARGS__))
#define _CU_NLAST_VARIADIC_27(...) _CU_NLAST_VARIADIC_26(P99_ALLBUTLAST(__VA_ARGS__))
#define _CU_NLAST_VARIADIC_28(...) _CU_NLAST_VARIADIC_27(P99_ALLBUTLAST(__VA_ARGS__))
#define _CU_NLAST_VARIADIC_29(...) _CU_NLAST_VARIADIC_28(P99_ALLBUTLAST(__VA_ARGS__))
#define _CU_NLAST_VARIADIC_30(...) _CU_NLAST_VARIADIC_29(P99_ALLBUTLAST(__VA_ARGS__))

/**
 * Fetch the entire variadic list except the last n arguments
 *
 * @code
 * CU_SUBSETNLAST_VARIADIC(0, 'a', 'b', 'c') //return 'a', 'b', 'c'
 * CU_SUBSETNLAST_VARIADIC(1, 'a', 'b', 'c') //return 'a', 'b'
 * CU_SUBSETNLAST_VARIADIC(2, 'a', 'b', 'c') //return 'a'
 * @endcode
 *
 * @pre
 *  @li something needs to remain in the variadic argument
 *  @li variadic argument not empty
 *
 * @param[in] N the number of last parameters you don't want to return
 * @return the variadic list expected the last @c n arguments
 */
#define CU_SUBSETNLAST_VARIADIC(N, ...) P99_PASTE2(_CU_SUBSETNLAST_VARIADIC_, N)(__VA_ARGS__)
#define _CU_SUBSETNLAST_VARIADIC_0(...) __VA_ARGS__
#define _CU_SUBSETNLAST_VARIADIC_1(...) P99_ALLBUTLAST(__VA_ARGS__)
#define _CU_SUBSETNLAST_VARIADIC_2(...) _CU_SUBSETNLAST_VARIADIC_1(P99_ALLBUTLAST(__VA_ARGS__))
#define _CU_SUBSETNLAST_VARIADIC_3(...) _CU_SUBSETNLAST_VARIADIC_2(P99_ALLBUTLAST(__VA_ARGS__))
#define _CU_SUBSETNLAST_VARIADIC_4(...) _CU_SUBSETNLAST_VARIADIC_3(P99_ALLBUTLAST(__VA_ARGS__))
#define _CU_SUBSETNLAST_VARIADIC_5(...) _CU_SUBSETNLAST_VARIADIC_4(P99_ALLBUTLAST(__VA_ARGS__))
#define _CU_SUBSETNLAST_VARIADIC_6(...) _CU_SUBSETNLAST_VARIADIC_5(P99_ALLBUTLAST(__VA_ARGS__))
#define _CU_SUBSETNLAST_VARIADIC_7(...) _CU_SUBSETNLAST_VARIADIC_6(P99_ALLBUTLAST(__VA_ARGS__))
#define _CU_SUBSETNLAST_VARIADIC_8(...) _CU_SUBSETNLAST_VARIADIC_7(P99_ALLBUTLAST(__VA_ARGS__))
#define _CU_SUBSETNLAST_VARIADIC_9(...) _CU_SUBSETNLAST_VARIADIC_8(P99_ALLBUTLAST(__VA_ARGS__))
#define _CU_SUBSETNLAST_VARIADIC_10(...) _CU_SUBSETNLAST_VARIADIC_9(P99_ALLBUTLAST(__VA_ARGS__))
#define _CU_SUBSETNLAST_VARIADIC_11(...) _CU_SUBSETNLAST_VARIADIC_10(P99_ALLBUTLAST(__VA_ARGS__))
#define _CU_SUBSETNLAST_VARIADIC_12(...) _CU_SUBSETNLAST_VARIADIC_11(P99_ALLBUTLAST(__VA_ARGS__))
#define _CU_SUBSETNLAST_VARIADIC_13(...) _CU_SUBSETNLAST_VARIADIC_12(P99_ALLBUTLAST(__VA_ARGS__))
#define _CU_SUBSETNLAST_VARIADIC_14(...) _CU_SUBSETNLAST_VARIADIC_13(P99_ALLBUTLAST(__VA_ARGS__))
#define _CU_SUBSETNLAST_VARIADIC_15(...) _CU_SUBSETNLAST_VARIADIC_14(P99_ALLBUTLAST(__VA_ARGS__))
#define _CU_SUBSETNLAST_VARIADIC_16(...) _CU_SUBSETNLAST_VARIADIC_15(P99_ALLBUTLAST(__VA_ARGS__))
#define _CU_SUBSETNLAST_VARIADIC_17(...) _CU_SUBSETNLAST_VARIADIC_16(P99_ALLBUTLAST(__VA_ARGS__))
#define _CU_SUBSETNLAST_VARIADIC_18(...) _CU_SUBSETNLAST_VARIADIC_17(P99_ALLBUTLAST(__VA_ARGS__))
#define _CU_SUBSETNLAST_VARIADIC_19(...) _CU_SUBSETNLAST_VARIADIC_18(P99_ALLBUTLAST(__VA_ARGS__))
#define _CU_SUBSETNLAST_VARIADIC_20(...) _CU_SUBSETNLAST_VARIADIC_19(P99_ALLBUTLAST(__VA_ARGS__))
#define _CU_SUBSETNLAST_VARIADIC_21(...) _CU_SUBSETNLAST_VARIADIC_20(P99_ALLBUTLAST(__VA_ARGS__))
#define _CU_SUBSETNLAST_VARIADIC_22(...) _CU_SUBSETNLAST_VARIADIC_21(P99_ALLBUTLAST(__VA_ARGS__))
#define _CU_SUBSETNLAST_VARIADIC_23(...) _CU_SUBSETNLAST_VARIADIC_22(P99_ALLBUTLAST(__VA_ARGS__))
#define _CU_SUBSETNLAST_VARIADIC_24(...) _CU_SUBSETNLAST_VARIADIC_23(P99_ALLBUTLAST(__VA_ARGS__))
#define _CU_SUBSETNLAST_VARIADIC_25(...) _CU_SUBSETNLAST_VARIADIC_24(P99_ALLBUTLAST(__VA_ARGS__))
#define _CU_SUBSETNLAST_VARIADIC_26(...) _CU_SUBSETNLAST_VARIADIC_25(P99_ALLBUTLAST(__VA_ARGS__))
#define _CU_SUBSETNLAST_VARIADIC_27(...) _CU_SUBSETNLAST_VARIADIC_26(P99_ALLBUTLAST(__VA_ARGS__))
#define _CU_SUBSETNLAST_VARIADIC_28(...) _CU_SUBSETNLAST_VARIADIC_27(P99_ALLBUTLAST(__VA_ARGS__))
#define _CU_SUBSETNLAST_VARIADIC_29(...) _CU_SUBSETNLAST_VARIADIC_28(P99_ALLBUTLAST(__VA_ARGS__))
#define _CU_SUBSETNLAST_VARIADIC_30(...) _CU_SUBSETNLAST_VARIADIC_29(P99_ALLBUTLAST(__VA_ARGS__))
#define _CU_SUBSETNLAST_VARIADIC_31(...) _CU_SUBSETNLAST_VARIADIC_30(P99_ALLBUTLAST(__VA_ARGS__))
#define _CU_SUBSETNLAST_VARIADIC_32(...) _CU_SUBSETNLAST_VARIADIC_31(P99_ALLBUTLAST(__VA_ARGS__))
#define _CU_SUBSETNLAST_VARIADIC_33(...) _CU_SUBSETNLAST_VARIADIC_32(P99_ALLBUTLAST(__VA_ARGS__))
#define _CU_SUBSETNLAST_VARIADIC_34(...) _CU_SUBSETNLAST_VARIADIC_33(P99_ALLBUTLAST(__VA_ARGS__))
#define _CU_SUBSETNLAST_VARIADIC_35(...) _CU_SUBSETNLAST_VARIADIC_34(P99_ALLBUTLAST(__VA_ARGS__))
#define _CU_SUBSETNLAST_VARIADIC_36(...) _CU_SUBSETNLAST_VARIADIC_35(P99_ALLBUTLAST(__VA_ARGS__))
#define _CU_SUBSETNLAST_VARIADIC_37(...) _CU_SUBSETNLAST_VARIADIC_36(P99_ALLBUTLAST(__VA_ARGS__))
#define _CU_SUBSETNLAST_VARIADIC_38(...) _CU_SUBSETNLAST_VARIADIC_37(P99_ALLBUTLAST(__VA_ARGS__))
#define _CU_SUBSETNLAST_VARIADIC_39(...) _CU_SUBSETNLAST_VARIADIC_38(P99_ALLBUTLAST(__VA_ARGS__))
#define _CU_SUBSETNLAST_VARIADIC_40(...) _CU_SUBSETNLAST_VARIADIC_39(P99_ALLBUTLAST(__VA_ARGS__))
#define _CU_SUBSETNLAST_VARIADIC_41(...) _CU_SUBSETNLAST_VARIADIC_40(P99_ALLBUTLAST(__VA_ARGS__))
#define _CU_SUBSETNLAST_VARIADIC_42(...) _CU_SUBSETNLAST_VARIADIC_41(P99_ALLBUTLAST(__VA_ARGS__))
#define _CU_SUBSETNLAST_VARIADIC_43(...) _CU_SUBSETNLAST_VARIADIC_42(P99_ALLBUTLAST(__VA_ARGS__))
#define _CU_SUBSETNLAST_VARIADIC_44(...) _CU_SUBSETNLAST_VARIADIC_43(P99_ALLBUTLAST(__VA_ARGS__))
#define _CU_SUBSETNLAST_VARIADIC_45(...) _CU_SUBSETNLAST_VARIADIC_44(P99_ALLBUTLAST(__VA_ARGS__))
#define _CU_SUBSETNLAST_VARIADIC_46(...) _CU_SUBSETNLAST_VARIADIC_45(P99_ALLBUTLAST(__VA_ARGS__))
#define _CU_SUBSETNLAST_VARIADIC_47(...) _CU_SUBSETNLAST_VARIADIC_46(P99_ALLBUTLAST(__VA_ARGS__))
#define _CU_SUBSETNLAST_VARIADIC_48(...) _CU_SUBSETNLAST_VARIADIC_47(P99_ALLBUTLAST(__VA_ARGS__))
#define _CU_SUBSETNLAST_VARIADIC_49(...) _CU_SUBSETNLAST_VARIADIC_48(P99_ALLBUTLAST(__VA_ARGS__))
#define _CU_SUBSETNLAST_VARIADIC_50(...) _CU_SUBSETNLAST_VARIADIC_49(P99_ALLBUTLAST(__VA_ARGS__))
#define _CU_SUBSETNLAST_VARIADIC_51(...) _CU_SUBSETNLAST_VARIADIC_50(P99_ALLBUTLAST(__VA_ARGS__))
#define _CU_SUBSETNLAST_VARIADIC_52(...) _CU_SUBSETNLAST_VARIADIC_51(P99_ALLBUTLAST(__VA_ARGS__))
#define _CU_SUBSETNLAST_VARIADIC_53(...) _CU_SUBSETNLAST_VARIADIC_52(P99_ALLBUTLAST(__VA_ARGS__))
#define _CU_SUBSETNLAST_VARIADIC_54(...) _CU_SUBSETNLAST_VARIADIC_53(P99_ALLBUTLAST(__VA_ARGS__))
#define _CU_SUBSETNLAST_VARIADIC_55(...) _CU_SUBSETNLAST_VARIADIC_54(P99_ALLBUTLAST(__VA_ARGS__))
#define _CU_SUBSETNLAST_VARIADIC_56(...) _CU_SUBSETNLAST_VARIADIC_55(P99_ALLBUTLAST(__VA_ARGS__))
#define _CU_SUBSETNLAST_VARIADIC_57(...) _CU_SUBSETNLAST_VARIADIC_56(P99_ALLBUTLAST(__VA_ARGS__))
#define _CU_SUBSETNLAST_VARIADIC_58(...) _CU_SUBSETNLAST_VARIADIC_57(P99_ALLBUTLAST(__VA_ARGS__))
#define _CU_SUBSETNLAST_VARIADIC_59(...) _CU_SUBSETNLAST_VARIADIC_58(P99_ALLBUTLAST(__VA_ARGS__))

/**
 * generate a sequence of number starting from 0
 *
 * @code
 * CU_SEQUENCE(1); //0
 * CU_SEQUENCE(2); //0, 1
 * CU_SEQUENCE(3); //0, 1, 2
 * @endcode
 *
 * @param[in] ubExcluded upperbound of the sequence generated (the value will be excluded by the sequence)
 */
#define CU_SEQUENCE(ubExcluded) P99_PASTE(_CU_SEQUENCE_, 0)(ubExcluded)
#define _CU_SEQUENCE_0(ubExcluded) P99_IF_EQ(1, ubExcluded)(0)(0, _CU_SEQUENCE_1(ubExcluded))
#define _CU_SEQUENCE_1(ubExcluded) P99_IF_EQ(2, ubExcluded)(1)(1, _CU_SEQUENCE_2(ubExcluded))
#define _CU_SEQUENCE_2(ubExcluded) P99_IF_EQ(3, ubExcluded)(2)(2, _CU_SEQUENCE_3(ubExcluded))
#define _CU_SEQUENCE_3(ubExcluded) P99_IF_EQ(4, ubExcluded)(3)(3, _CU_SEQUENCE_4(ubExcluded))
#define _CU_SEQUENCE_4(ubExcluded) P99_IF_EQ(5, ubExcluded)(4)(4, _CU_SEQUENCE_5(ubExcluded))
#define _CU_SEQUENCE_5(ubExcluded) P99_IF_EQ(6, ubExcluded)(5)(5, _CU_SEQUENCE_6(ubExcluded))
#define _CU_SEQUENCE_6(ubExcluded) P99_IF_EQ(7, ubExcluded)(6)(6, _CU_SEQUENCE_7(ubExcluded))
#define _CU_SEQUENCE_7(ubExcluded) P99_IF_EQ(8, ubExcluded)(7)(7, _CU_SEQUENCE_8(ubExcluded))
#define _CU_SEQUENCE_8(ubExcluded) P99_IF_EQ(9, ubExcluded)(8)(8, _CU_SEQUENCE_9(ubExcluded))
#define _CU_SEQUENCE_9(ubExcluded) P99_IF_EQ(10, ubExcluded)(9)(9, _CU_SEQUENCE_10(ubExcluded))
#define _CU_SEQUENCE_10(ubExcluded) P99_IF_EQ(11, ubExcluded)(10)(10, _CU_SEQUENCE_11(ubExcluded))
#define _CU_SEQUENCE_11(ubExcluded) P99_IF_EQ(12, ubExcluded)(11)(11, _CU_SEQUENCE_12(ubExcluded))
#define _CU_SEQUENCE_12(ubExcluded) P99_IF_EQ(13, ubExcluded)(12)(12, _CU_SEQUENCE_13(ubExcluded))
#define _CU_SEQUENCE_13(ubExcluded) P99_IF_EQ(14, ubExcluded)(13)(13, _CU_SEQUENCE_14(ubExcluded))
#define _CU_SEQUENCE_14(ubExcluded) P99_IF_EQ(15, ubExcluded)(14)(14, _CU_SEQUENCE_15(ubExcluded))
#define _CU_SEQUENCE_15(ubExcluded) P99_IF_EQ(16, ubExcluded)(15)(15, _CU_SEQUENCE_16(ubExcluded))
#define _CU_SEQUENCE_16(ubExcluded) P99_IF_EQ(17, ubExcluded)(16)(16, _CU_SEQUENCE_17(ubExcluded))
#define _CU_SEQUENCE_17(ubExcluded) P99_IF_EQ(18, ubExcluded)(17)(17, _CU_SEQUENCE_18(ubExcluded))
#define _CU_SEQUENCE_18(ubExcluded) P99_IF_EQ(19, ubExcluded)(18)(18, _CU_SEQUENCE_19(ubExcluded))
#define _CU_SEQUENCE_19(ubExcluded) P99_IF_EQ(20, ubExcluded)(19)(19, _CU_SEQUENCE_20(ubExcluded))
#define _CU_SEQUENCE_20(ubExcluded) P99_IF_EQ(21, ubExcluded)(20)(20, _CU_SEQUENCE_21(ubExcluded))
#define _CU_SEQUENCE_21(ubExcluded) P99_IF_EQ(22, ubExcluded)(21)(21, _CU_SEQUENCE_22(ubExcluded))
#define _CU_SEQUENCE_22(ubExcluded) P99_IF_EQ(23, ubExcluded)(22)(22, _CU_SEQUENCE_23(ubExcluded))
#define _CU_SEQUENCE_23(ubExcluded) P99_IF_EQ(24, ubExcluded)(23)(23, _CU_SEQUENCE_24(ubExcluded))
#define _CU_SEQUENCE_24(ubExcluded) P99_IF_EQ(25, ubExcluded)(24)(24, _CU_SEQUENCE_25(ubExcluded))
#define _CU_SEQUENCE_25(ubExcluded) P99_IF_EQ(26, ubExcluded)(25)(25, _CU_SEQUENCE_26(ubExcluded))
#define _CU_SEQUENCE_26(ubExcluded) P99_IF_EQ(27, ubExcluded)(26)(26, _CU_SEQUENCE_27(ubExcluded))
#define _CU_SEQUENCE_27(ubExcluded) P99_IF_EQ(28, ubExcluded)(27)(27, _CU_SEQUENCE_28(ubExcluded))
#define _CU_SEQUENCE_28(ubExcluded) P99_IF_EQ(29, ubExcluded)(28)(28, _CU_SEQUENCE_29(ubExcluded))
#define _CU_SEQUENCE_29(ubExcluded) P99_IF_EQ(30, ubExcluded)(29)(29, _CU_SEQUENCE_30(ubExcluded))
#define _CU_SEQUENCE_30(ubExcluded) P99_IF_EQ(31, ubExcluded)(30)(30, _CU_SEQUENCE_31(ubExcluded))
#define _CU_SEQUENCE_31(ubExcluded) P99_IF_EQ(32, ubExcluded)(31)(31, _CU_SEQUENCE_32(ubExcluded))
#define _CU_SEQUENCE_32(ubExcluded) P99_IF_EQ(33, ubExcluded)(32)(32, _CU_SEQUENCE_33(ubExcluded))
#define _CU_SEQUENCE_33(ubExcluded) P99_IF_EQ(34, ubExcluded)(33)(33, _CU_SEQUENCE_34(ubExcluded))
#define _CU_SEQUENCE_34(ubExcluded) P99_IF_EQ(35, ubExcluded)(34)(34, _CU_SEQUENCE_35(ubExcluded))
#define _CU_SEQUENCE_35(ubExcluded) P99_IF_EQ(36, ubExcluded)(35)(35, _CU_SEQUENCE_36(ubExcluded))
#define _CU_SEQUENCE_36(ubExcluded) P99_IF_EQ(37, ubExcluded)(36)(36, _CU_SEQUENCE_37(ubExcluded))
#define _CU_SEQUENCE_37(ubExcluded) P99_IF_EQ(38, ubExcluded)(37)(37, _CU_SEQUENCE_38(ubExcluded))
#define _CU_SEQUENCE_38(ubExcluded) P99_IF_EQ(39, ubExcluded)(38)(38, _CU_SEQUENCE_39(ubExcluded))
#define _CU_SEQUENCE_39(ubExcluded) P99_IF_EQ(40, ubExcluded)(39)(39, _CU_SEQUENCE_40(ubExcluded))
#define _CU_SEQUENCE_40(ubExcluded) P99_IF_EQ(41, ubExcluded)(40)(40, _CU_SEQUENCE_41(ubExcluded))
#define _CU_SEQUENCE_41(ubExcluded) P99_IF_EQ(42, ubExcluded)(41)(41, _CU_SEQUENCE_42(ubExcluded))
#define _CU_SEQUENCE_42(ubExcluded) P99_IF_EQ(43, ubExcluded)(42)(42, _CU_SEQUENCE_43(ubExcluded))
#define _CU_SEQUENCE_43(ubExcluded) P99_IF_EQ(44, ubExcluded)(43)(43, _CU_SEQUENCE_44(ubExcluded))
#define _CU_SEQUENCE_44(ubExcluded) P99_IF_EQ(45, ubExcluded)(44)(44, _CU_SEQUENCE_45(ubExcluded))
#define _CU_SEQUENCE_45(ubExcluded) P99_IF_EQ(46, ubExcluded)(45)(45, _CU_SEQUENCE_46(ubExcluded))
#define _CU_SEQUENCE_46(ubExcluded) P99_IF_EQ(47, ubExcluded)(46)(46, _CU_SEQUENCE_47(ubExcluded))
#define _CU_SEQUENCE_47(ubExcluded) P99_IF_EQ(48, ubExcluded)(47)(47, _CU_SEQUENCE_48(ubExcluded))
#define _CU_SEQUENCE_48(ubExcluded) P99_IF_EQ(49, ubExcluded)(48)(48, _CU_SEQUENCE_49(ubExcluded))
#define _CU_SEQUENCE_49(ubExcluded) P99_IF_EQ(50, ubExcluded)(49)(49, _CU_SEQUENCE_50(ubExcluded))

#define _CU_RANGE_OP(CONTEXT, INDEX, CURRENT, NEXT) CURRENT, NEXT
#define _CU_RANGE_FUNC(CONTEXT, INDEX, A) P99_ADD(CONTEXT, A)

/**
 * generate a sequence of number starting from @c lbIncluded
 *
 * @code
 * CU_SEQUENCE(0, 3); //0, 1, 2
 * CU_SEQUENCE(2, 7); //2, 3, 4, 5, 6
 * @endcode
 *
 * @param[in] lbIncluded lowerbound of the sequence generated (the value will be included by the sequence)
 * @param[in] ubExcluded upperbound of the sequence generated (the value will be excluded by the sequence)
 */
#define CU_RANGE(lbIncluded, ubExcluded) \
	CU_FOR_EACH(lbIncluded, _CU_RANGE_OP, _CU_RANGE_FUNC, CU_SEQUENCE(P99_MINUS(ubExcluded, lbIncluded)))

/**
 * Perform actions for every value in the variadic
 *
 * @code
 * #define OP(CONTEXT, INDEX, CURRENT, NEXT) (CURRENT + NEXT)
 * #define FUNC(CONTEXT, INDEX, A) (A)
 *
 * assert(CU_FOR_EACH(, OP, FUNC, 1, 2, 3, 4, 5, 6) == (1+2+3+4+5+6));
 * @endcode
 *
 * @param[in] CONTEXT something that will be passed to every @c OP and @c FUNC macro
 * @param[in] OP a macro allowing you to concatenate the value generated by a @c FUNC call and the remainder part of the foreach
 *  @li CONTEXT: the @c CONTEXT stuff;
 * 	@li INDEX: a value which identifies the nth @c FUNC call. Starts with 0
 * 	@li CURRENT: the value returned by ther n-th @c FUNC call
 * 	@li NEXT: the stuff that will be generated by the \f$n+1\f$-th @c FUNC call
 * @param[in] FUNC a macro computing a single pair. it requires you to define a macro:
 * 	@li CONTEXT: the @c CONTEXT stuff;
 * 	@li INDEX: a value which identifies the nth @c FUNC call. . Starts with 0
 * 	@li A: the value
 */
#define CU_FOR_EACH(CONTEXT, OP, FUNC, ...) P99_PASTE2(_CU_BASE_FOR_EACH_, CU_COUNT_ARGS(__VA_ARGS__))(CONTEXT, OP, FUNC, __VA_ARGS__)

#define _CU_BASE_FOR_EACH_1(CONTEXT, OP, FUNC, value) FUNC(CONTEXT, 1, value)
#define _CU_BASE_FOR_EACH_2(CONTEXT, OP, FUNC, value, ...) OP(CONTEXT, 2, FUNC(CONTEXT, 2, value), _CU_BASE_FOR_EACH_1(CONTEXT, OP, FUNC, __VA_ARGS__))
#define _CU_BASE_FOR_EACH_3(CONTEXT, OP, FUNC, value, ...) OP(CONTEXT, 3, FUNC(CONTEXT, 3, value), _CU_BASE_FOR_EACH_2(CONTEXT, OP, FUNC, __VA_ARGS__))
#define _CU_BASE_FOR_EACH_4(CONTEXT, OP, FUNC, value, ...) OP(CONTEXT, 4, FUNC(CONTEXT, 4, value), _CU_BASE_FOR_EACH_3(CONTEXT, OP, FUNC, __VA_ARGS__))
#define _CU_BASE_FOR_EACH_5(CONTEXT, OP, FUNC, value, ...) OP(CONTEXT, 5, FUNC(CONTEXT, 5, value), _CU_BASE_FOR_EACH_4(CONTEXT, OP, FUNC, __VA_ARGS__))
#define _CU_BASE_FOR_EACH_6(CONTEXT, OP, FUNC, value, ...) OP(CONTEXT, 6, FUNC(CONTEXT, 6, value), _CU_BASE_FOR_EACH_5(CONTEXT, OP, FUNC, __VA_ARGS__))
#define _CU_BASE_FOR_EACH_7(CONTEXT, OP, FUNC, value, ...) OP(CONTEXT, 7, FUNC(CONTEXT, 7, value), _CU_BASE_FOR_EACH_6(CONTEXT, OP, FUNC, __VA_ARGS__))
#define _CU_BASE_FOR_EACH_8(CONTEXT, OP, FUNC, value, ...) OP(CONTEXT, 8, FUNC(CONTEXT, 8, value), _CU_BASE_FOR_EACH_7(CONTEXT, OP, FUNC, __VA_ARGS__))
#define _CU_BASE_FOR_EACH_9(CONTEXT, OP, FUNC, value, ...) OP(CONTEXT, 9, FUNC(CONTEXT, 9, value), _CU_BASE_FOR_EACH_8(CONTEXT, OP, FUNC, __VA_ARGS__))
#define _CU_BASE_FOR_EACH_10(CONTEXT, OP, FUNC, value, ...) OP(CONTEXT, 10, FUNC(CONTEXT, 10, value), _CU_BASE_FOR_EACH_9(CONTEXT, OP, FUNC, __VA_ARGS__))
#define _CU_BASE_FOR_EACH_11(CONTEXT, OP, FUNC, value, ...) OP(CONTEXT, 11, FUNC(CONTEXT, 11, value), _CU_BASE_FOR_EACH_10(CONTEXT, OP, FUNC, __VA_ARGS__))
#define _CU_BASE_FOR_EACH_12(CONTEXT, OP, FUNC, value, ...) OP(CONTEXT, 12, FUNC(CONTEXT, 12, value), _CU_BASE_FOR_EACH_11(CONTEXT, OP, FUNC, __VA_ARGS__))
#define _CU_BASE_FOR_EACH_13(CONTEXT, OP, FUNC, value, ...) OP(CONTEXT, 13, FUNC(CONTEXT, 13, value), _CU_BASE_FOR_EACH_12(CONTEXT, OP, FUNC, __VA_ARGS__))
#define _CU_BASE_FOR_EACH_14(CONTEXT, OP, FUNC, value, ...) OP(CONTEXT, 14, FUNC(CONTEXT, 14, value), _CU_BASE_FOR_EACH_13(CONTEXT, OP, FUNC, __VA_ARGS__))
#define _CU_BASE_FOR_EACH_15(CONTEXT, OP, FUNC, value, ...) OP(CONTEXT, 15, FUNC(CONTEXT, 15, value), _CU_BASE_FOR_EACH_14(CONTEXT, OP, FUNC, __VA_ARGS__))
#define _CU_BASE_FOR_EACH_16(CONTEXT, OP, FUNC, value, ...) OP(CONTEXT, 16, FUNC(CONTEXT, 16, value), _CU_BASE_FOR_EACH_15(CONTEXT, OP, FUNC, __VA_ARGS__))
#define _CU_BASE_FOR_EACH_17(CONTEXT, OP, FUNC, value, ...) OP(CONTEXT, 17, FUNC(CONTEXT, 17, value), _CU_BASE_FOR_EACH_16(CONTEXT, OP, FUNC, __VA_ARGS__))
#define _CU_BASE_FOR_EACH_18(CONTEXT, OP, FUNC, value, ...) OP(CONTEXT, 18, FUNC(CONTEXT, 18, value), _CU_BASE_FOR_EACH_17(CONTEXT, OP, FUNC, __VA_ARGS__))
#define _CU_BASE_FOR_EACH_19(CONTEXT, OP, FUNC, value, ...) OP(CONTEXT, 19, FUNC(CONTEXT, 19, value), _CU_BASE_FOR_EACH_18(CONTEXT, OP, FUNC, __VA_ARGS__))
#define _CU_BASE_FOR_EACH_20(CONTEXT, OP, FUNC, value, ...) OP(CONTEXT, 20, FUNC(CONTEXT, 20, value), _CU_BASE_FOR_EACH_19(CONTEXT, OP, FUNC, __VA_ARGS__))
#define _CU_BASE_FOR_EACH_21(CONTEXT, OP, FUNC, value, ...) OP(CONTEXT, 21, FUNC(CONTEXT, 21, value), _CU_BASE_FOR_EACH_20(CONTEXT, OP, FUNC, __VA_ARGS__))
#define _CU_BASE_FOR_EACH_22(CONTEXT, OP, FUNC, value, ...) OP(CONTEXT, 22, FUNC(CONTEXT, 22, value), _CU_BASE_FOR_EACH_21(CONTEXT, OP, FUNC, __VA_ARGS__))
#define _CU_BASE_FOR_EACH_23(CONTEXT, OP, FUNC, value, ...) OP(CONTEXT, 23, FUNC(CONTEXT, 23, value), _CU_BASE_FOR_EACH_22(CONTEXT, OP, FUNC, __VA_ARGS__))
#define _CU_BASE_FOR_EACH_24(CONTEXT, OP, FUNC, value, ...) OP(CONTEXT, 24, FUNC(CONTEXT, 24, value), _CU_BASE_FOR_EACH_23(CONTEXT, OP, FUNC, __VA_ARGS__))
#define _CU_BASE_FOR_EACH_25(CONTEXT, OP, FUNC, value, ...) OP(CONTEXT, 25, FUNC(CONTEXT, 25, value), _CU_BASE_FOR_EACH_24(CONTEXT, OP, FUNC, __VA_ARGS__))
#define _CU_BASE_FOR_EACH_26(CONTEXT, OP, FUNC, value, ...) OP(CONTEXT, 26, FUNC(CONTEXT, 26, value), _CU_BASE_FOR_EACH_25(CONTEXT, OP, FUNC, __VA_ARGS__))
#define _CU_BASE_FOR_EACH_27(CONTEXT, OP, FUNC, value, ...) OP(CONTEXT, 27, FUNC(CONTEXT, 27, value), _CU_BASE_FOR_EACH_26(CONTEXT, OP, FUNC, __VA_ARGS__))
#define _CU_BASE_FOR_EACH_28(CONTEXT, OP, FUNC, value, ...) OP(CONTEXT, 28, FUNC(CONTEXT, 28, value), _CU_BASE_FOR_EACH_27(CONTEXT, OP, FUNC, __VA_ARGS__))
#define _CU_BASE_FOR_EACH_29(CONTEXT, OP, FUNC, value, ...) OP(CONTEXT, 29, FUNC(CONTEXT, 29, value), _CU_BASE_FOR_EACH_28(CONTEXT, OP, FUNC, __VA_ARGS__))
#define _CU_BASE_FOR_EACH_30(CONTEXT, OP, FUNC, value, ...) OP(CONTEXT, 30, FUNC(CONTEXT, 30, value), _CU_BASE_FOR_EACH_29(CONTEXT, OP, FUNC, __VA_ARGS__))
#define _CU_BASE_FOR_EACH_31(CONTEXT, OP, FUNC, value, ...) OP(CONTEXT, 31, FUNC(CONTEXT, 31, value), _CU_BASE_FOR_EACH_30(CONTEXT, OP, FUNC, __VA_ARGS__))
#define _CU_BASE_FOR_EACH_32(CONTEXT, OP, FUNC, value, ...) OP(CONTEXT, 32, FUNC(CONTEXT, 32, value), _CU_BASE_FOR_EACH_31(CONTEXT, OP, FUNC, __VA_ARGS__))
#define _CU_BASE_FOR_EACH_33(CONTEXT, OP, FUNC, value, ...) OP(CONTEXT, 33, FUNC(CONTEXT, 33, value), _CU_BASE_FOR_EACH_32(CONTEXT, OP, FUNC, __VA_ARGS__))
#define _CU_BASE_FOR_EACH_34(CONTEXT, OP, FUNC, value, ...) OP(CONTEXT, 34, FUNC(CONTEXT, 34, value), _CU_BASE_FOR_EACH_33(CONTEXT, OP, FUNC, __VA_ARGS__))
#define _CU_BASE_FOR_EACH_35(CONTEXT, OP, FUNC, value, ...) OP(CONTEXT, 35, FUNC(CONTEXT, 35, value), _CU_BASE_FOR_EACH_34(CONTEXT, OP, FUNC, __VA_ARGS__))
#define _CU_BASE_FOR_EACH_36(CONTEXT, OP, FUNC, value, ...) OP(CONTEXT, 36, FUNC(CONTEXT, 36, value), _CU_BASE_FOR_EACH_35(CONTEXT, OP, FUNC, __VA_ARGS__))
#define _CU_BASE_FOR_EACH_37(CONTEXT, OP, FUNC, value, ...) OP(CONTEXT, 37, FUNC(CONTEXT, 37, value), _CU_BASE_FOR_EACH_36(CONTEXT, OP, FUNC, __VA_ARGS__))
#define _CU_BASE_FOR_EACH_38(CONTEXT, OP, FUNC, value, ...) OP(CONTEXT, 38, FUNC(CONTEXT, 38, value), _CU_BASE_FOR_EACH_37(CONTEXT, OP, FUNC, __VA_ARGS__))
#define _CU_BASE_FOR_EACH_39(CONTEXT, OP, FUNC, value, ...) OP(CONTEXT, 39, FUNC(CONTEXT, 39, value), _CU_BASE_FOR_EACH_38(CONTEXT, OP, FUNC, __VA_ARGS__))
#define _CU_BASE_FOR_EACH_40(CONTEXT, OP, FUNC, value, ...) OP(CONTEXT, 40, FUNC(CONTEXT, 40, value), _CU_BASE_FOR_EACH_39(CONTEXT, OP, FUNC, __VA_ARGS__))
#define _CU_BASE_FOR_EACH_41(CONTEXT, OP, FUNC, value, ...) OP(CONTEXT, 41, FUNC(CONTEXT, 41, value), _CU_BASE_FOR_EACH_40(CONTEXT, OP, FUNC, __VA_ARGS__))
#define _CU_BASE_FOR_EACH_42(CONTEXT, OP, FUNC, value, ...) OP(CONTEXT, 42, FUNC(CONTEXT, 42, value), _CU_BASE_FOR_EACH_41(CONTEXT, OP, FUNC, __VA_ARGS__))
#define _CU_BASE_FOR_EACH_43(CONTEXT, OP, FUNC, value, ...) OP(CONTEXT, 43, FUNC(CONTEXT, 43, value), _CU_BASE_FOR_EACH_42(CONTEXT, OP, FUNC, __VA_ARGS__))
#define _CU_BASE_FOR_EACH_44(CONTEXT, OP, FUNC, value, ...) OP(CONTEXT, 44, FUNC(CONTEXT, 44, value), _CU_BASE_FOR_EACH_43(CONTEXT, OP, FUNC, __VA_ARGS__))
#define _CU_BASE_FOR_EACH_45(CONTEXT, OP, FUNC, value, ...) OP(CONTEXT, 45, FUNC(CONTEXT, 45, value), _CU_BASE_FOR_EACH_44(CONTEXT, OP, FUNC, __VA_ARGS__))
#define _CU_BASE_FOR_EACH_46(CONTEXT, OP, FUNC, value, ...) OP(CONTEXT, 46, FUNC(CONTEXT, 46, value), _CU_BASE_FOR_EACH_45(CONTEXT, OP, FUNC, __VA_ARGS__))
#define _CU_BASE_FOR_EACH_47(CONTEXT, OP, FUNC, value, ...) OP(CONTEXT, 47, FUNC(CONTEXT, 47, value), _CU_BASE_FOR_EACH_46(CONTEXT, OP, FUNC, __VA_ARGS__))
#define _CU_BASE_FOR_EACH_48(CONTEXT, OP, FUNC, value, ...) OP(CONTEXT, 48, FUNC(CONTEXT, 48, value), _CU_BASE_FOR_EACH_47(CONTEXT, OP, FUNC, __VA_ARGS__))
#define _CU_BASE_FOR_EACH_49(CONTEXT, OP, FUNC, value, ...) OP(CONTEXT, 49, FUNC(CONTEXT, 49, value), _CU_BASE_FOR_EACH_48(CONTEXT, OP, FUNC, __VA_ARGS__))
#define _CU_BASE_FOR_EACH_50(CONTEXT, OP, FUNC, value, ...) OP(CONTEXT, 50, FUNC(CONTEXT, 50, value), _CU_BASE_FOR_EACH_49(CONTEXT, OP, FUNC, __VA_ARGS__))
#define _CU_BASE_FOR_EACH_51(CONTEXT, OP, FUNC, value, ...) OP(CONTEXT, 51, FUNC(CONTEXT, 51, value), _CU_BASE_FOR_EACH_50(CONTEXT, OP, FUNC, __VA_ARGS__))
#define _CU_BASE_FOR_EACH_52(CONTEXT, OP, FUNC, value, ...) OP(CONTEXT, 52, FUNC(CONTEXT, 52, value), _CU_BASE_FOR_EACH_51(CONTEXT, OP, FUNC, __VA_ARGS__))
#define _CU_BASE_FOR_EACH_53(CONTEXT, OP, FUNC, value, ...) OP(CONTEXT, 53, FUNC(CONTEXT, 53, value), _CU_BASE_FOR_EACH_52(CONTEXT, OP, FUNC, __VA_ARGS__))
#define _CU_BASE_FOR_EACH_54(CONTEXT, OP, FUNC, value, ...) OP(CONTEXT, 54, FUNC(CONTEXT, 54, value), _CU_BASE_FOR_EACH_53(CONTEXT, OP, FUNC, __VA_ARGS__))
#define _CU_BASE_FOR_EACH_55(CONTEXT, OP, FUNC, value, ...) OP(CONTEXT, 55, FUNC(CONTEXT, 55, value), _CU_BASE_FOR_EACH_54(CONTEXT, OP, FUNC, __VA_ARGS__))
#define _CU_BASE_FOR_EACH_56(CONTEXT, OP, FUNC, value, ...) OP(CONTEXT, 56, FUNC(CONTEXT, 56, value), _CU_BASE_FOR_EACH_55(CONTEXT, OP, FUNC, __VA_ARGS__))
#define _CU_BASE_FOR_EACH_57(CONTEXT, OP, FUNC, value, ...) OP(CONTEXT, 57, FUNC(CONTEXT, 57, value), _CU_BASE_FOR_EACH_56(CONTEXT, OP, FUNC, __VA_ARGS__))
#define _CU_BASE_FOR_EACH_58(CONTEXT, OP, FUNC, value, ...) OP(CONTEXT, 58, FUNC(CONTEXT, 58, value), _CU_BASE_FOR_EACH_57(CONTEXT, OP, FUNC, __VA_ARGS__))
#define _CU_BASE_FOR_EACH_59(CONTEXT, OP, FUNC, value, ...) OP(CONTEXT, 59, FUNC(CONTEXT, 59, value), _CU_BASE_FOR_EACH_58(CONTEXT, OP, FUNC, __VA_ARGS__))
#define _CU_BASE_FOR_EACH_60(CONTEXT, OP, FUNC, value, ...) OP(CONTEXT, 60, FUNC(CONTEXT, 60, value), _CU_BASE_FOR_EACH_59(CONTEXT, OP, FUNC, __VA_ARGS__))




/**
 * Perform actions every sequential pair of values in the variadic
 *
 * @code
 * #define OP(CONTEXT, INDEX, CURRENT, NEXT) (CURRENT + NEXT)
 * #define FUNC(CONTEXT, INDEX, A, B) (A * B)
 *
 * assert(CU_FOR_PAIR(, OP, FUNC, 1, 2, 3, 4, 5, 6) ==  (1*2+3*4+5*6));
 * @endcode
 *
 * @param[in] CONTEXT something that will be passed to every @c OP and @c FUNC macro
 * @param[in] OP a macro allowing you to concatenate the value generated by a @c FUNC call and the remainder part of the foreach
 *  @li CONTEXT: the @c CONTEXT stuff;
 * 	@li INDEX: a value which identifies the nth @c FUNC call. Starts with 0
 * 	@li CURRENT: the value returned by ther n-th @c FUNC call
 * 	@li NEXT: the stuff that will be generated by the \f$n+1\f$-th @c FUNC call
 * @param[in] FUNC a macro computing a single pair. it requires you to define a macro:
 * 	@li CONTEXT: the @c CONTEXT stuff;
 * 	@li INDEX: a value which identifies the nth @c FUNC call. Starts with 0
 * 	@li A: the first value of the pair
 * 	@li B: the second value of the pair
 */
#define CU_FOR_PAIR(CONTEXT, OP, FUNC, ...) _CU_FOR_PAIR(CONTEXT, CU_COUNT_ARGS(__VA_ARGS__), OP, FUNC, __VA_ARGS__)

#define _CU_FOR_PAIR(CONTEXT, SIZE, OP, FUNC, ...) P99_PASTE2(_CU_BASE_FOR_PAIR_, SIZE)(CONTEXT, SIZE, OP, FUNC, __VA_ARGS__)
#define _CU_BASE_FOR_PAIR_0(CONTEXT, SIZE, OP, FUNC, ...)
#define _CU_BASE_FOR_PAIR_2(CONTEXT, SIZE, OP, FUNC, ...) FUNC(CONTEXT, 0, CU_NLAST_VARIADIC(2, __VA_ARGS__), CU_NLAST_VARIADIC(1, __VA_ARGS__))
#define _CU_BASE_FOR_PAIR_4(CONTEXT, SIZE, OP, FUNC, ...) OP( \
	CONTEXT, \
	1, \
	FUNC(CONTEXT, 1, CU_NLAST_VARIADIC(2, __VA_ARGS__), CU_NLAST_VARIADIC(1, __VA_ARGS__)), \
	_CU_BASE_FOR_PAIR_2(CONTEXT, 1, OP, FUNC, CU_SUBSETNLAST_VARIADIC(2, __VA_ARGS__)) \
)
#define _CU_BASE_FOR_PAIR_6(CONTEXT, SIZE, OP, FUNC, ...) OP( \
	CONTEXT, \
	2, \
	FUNC(CONTEXT, 2, CU_NLAST_VARIADIC(2, __VA_ARGS__), CU_NLAST_VARIADIC(1, __VA_ARGS__)), \
	_CU_BASE_FOR_PAIR_4(CONTEXT, 2, OP, FUNC, CU_SUBSETNLAST_VARIADIC(2, __VA_ARGS__)) \
)
#define _CU_BASE_FOR_PAIR_8(CONTEXT, SIZE, OP, FUNC, ...) OP( \
	CONTEXT, \
	3, \
	FUNC(CONTEXT, 3, CU_NLAST_VARIADIC(2, __VA_ARGS__), CU_NLAST_VARIADIC(1, __VA_ARGS__)), \
	_CU_BASE_FOR_PAIR_6(CONTEXT, 3, OP, FUNC, CU_SUBSETNLAST_VARIADIC(2, __VA_ARGS__)) \
)
#define _CU_BASE_FOR_PAIR_10(CONTEXT, SIZE, OP, FUNC, ...) OP( \
	CONTEXT, \
	4, \
	FUNC(CONTEXT, 4, CU_NLAST_VARIADIC(2, __VA_ARGS__), CU_NLAST_VARIADIC(1, __VA_ARGS__)), \
	_CU_BASE_FOR_PAIR_8(CONTEXT, 4, OP, FUNC, CU_SUBSETNLAST_VARIADIC(2, __VA_ARGS__)) \
)
#define _CU_BASE_FOR_PAIR_12(CONTEXT, SIZE, OP, FUNC, ...) OP( \
	CONTEXT, \
	5, \
	FUNC(CONTEXT, 5, CU_NLAST_VARIADIC(2, __VA_ARGS__), CU_NLAST_VARIADIC(1, __VA_ARGS__)), \
	_CU_BASE_FOR_PAIR_10(CONTEXT, 5, OP, FUNC, CU_SUBSETNLAST_VARIADIC(2, __VA_ARGS__)) \
)
#define _CU_BASE_FOR_PAIR_14(CONTEXT, SIZE, OP, FUNC, ...) OP( \
	CONTEXT, \
	6, \
	FUNC(CONTEXT, 6, CU_NLAST_VARIADIC(2, __VA_ARGS__), CU_NLAST_VARIADIC(1, __VA_ARGS__)), \
	_CU_BASE_FOR_PAIR_12(CONTEXT, 6, OP, FUNC, CU_SUBSETNLAST_VARIADIC(2, __VA_ARGS__)) \
)
#define _CU_BASE_FOR_PAIR_16(CONTEXT, SIZE, OP, FUNC, ...) OP( \
	CONTEXT, \
	7, \
	FUNC(CONTEXT, 7, CU_NLAST_VARIADIC(2, __VA_ARGS__), CU_NLAST_VARIADIC(1, __VA_ARGS__)), \
	_CU_BASE_FOR_PAIR_14(CONTEXT, 7, OP, FUNC, CU_SUBSETNLAST_VARIADIC(2, __VA_ARGS__)) \
)
#define _CU_BASE_FOR_PAIR_18(CONTEXT, SIZE, OP, FUNC, ...) OP( \
	CONTEXT, \
	8, \
	FUNC(CONTEXT, 8, CU_NLAST_VARIADIC(2, __VA_ARGS__), CU_NLAST_VARIADIC(1, __VA_ARGS__)), \
	_CU_BASE_FOR_PAIR_16(CONTEXT, 8, OP, FUNC, CU_SUBSETNLAST_VARIADIC(2, __VA_ARGS__)) \
)
#define _CU_BASE_FOR_PAIR_20(CONTEXT, SIZE, OP, FUNC, ...) OP( \
	CONTEXT, \
	9, \
	FUNC(CONTEXT, 9, CU_NLAST_VARIADIC(2, __VA_ARGS__), CU_NLAST_VARIADIC(1, __VA_ARGS__)), \
	_CU_BASE_FOR_PAIR_18(CONTEXT, 9, OP, FUNC, CU_SUBSETNLAST_VARIADIC(2, __VA_ARGS__)) \
)
#define _CU_BASE_FOR_PAIR_22(CONTEXT, SIZE, OP, FUNC, ...) OP( \
	CONTEXT, \
	10, \
	FUNC(CONTEXT, 10, CU_NLAST_VARIADIC(2, __VA_ARGS__), CU_NLAST_VARIADIC(1, __VA_ARGS__)), \
	_CU_BASE_FOR_PAIR_20(CONTEXT, 10, OP, FUNC, CU_SUBSETNLAST_VARIADIC(2, __VA_ARGS__)) \
)
#define _CU_BASE_FOR_PAIR_24(CONTEXT, SIZE, OP, FUNC, ...) OP( \
	CONTEXT, \
	11, \
	FUNC(CONTEXT, 11, CU_NLAST_VARIADIC(2, __VA_ARGS__), CU_NLAST_VARIADIC(1, __VA_ARGS__)), \
	_CU_BASE_FOR_PAIR_22(CONTEXT, 11, OP, FUNC, CU_SUBSETNLAST_VARIADIC(2, __VA_ARGS__)) \
)
#define _CU_BASE_FOR_PAIR_26(CONTEXT, SIZE, OP, FUNC, ...) OP( \
	CONTEXT, \
	12, \
	FUNC(CONTEXT, 12, CU_NLAST_VARIADIC(2, __VA_ARGS__), CU_NLAST_VARIADIC(1, __VA_ARGS__)), \
	_CU_BASE_FOR_PAIR_24(CONTEXT, 12, OP, FUNC, CU_SUBSETNLAST_VARIADIC(2, __VA_ARGS__)) \
)
#define _CU_BASE_FOR_PAIR_28(CONTEXT, SIZE, OP, FUNC, ...) OP( \
	CONTEXT, \
	13, \
	FUNC(CONTEXT, 13, CU_NLAST_VARIADIC(2, __VA_ARGS__), CU_NLAST_VARIADIC(1, __VA_ARGS__)), \
	_CU_BASE_FOR_PAIR_26(CONTEXT, 13, OP, FUNC, CU_SUBSETNLAST_VARIADIC(2, __VA_ARGS__)) \
)
#define _CU_BASE_FOR_PAIR_30(CONTEXT, SIZE, OP, FUNC, ...) OP( \
	CONTEXT, \
	14, \
	FUNC(CONTEXT, 14, CU_NLAST_VARIADIC(2, __VA_ARGS__), CU_NLAST_VARIADIC(1, __VA_ARGS__)), \
	_CU_BASE_FOR_PAIR_28(CONTEXT, 14, OP, FUNC, CU_SUBSETNLAST_VARIADIC(2, __VA_ARGS__)) \
)
#define _CU_BASE_FOR_PAIR_32(CONTEXT, SIZE, OP, FUNC, ...) OP( \
	CONTEXT, \
	15, \
	FUNC(CONTEXT, 15, CU_NLAST_VARIADIC(2, __VA_ARGS__), CU_NLAST_VARIADIC(1, __VA_ARGS__)), \
	_CU_BASE_FOR_PAIR_30(CONTEXT, 15, OP, FUNC, CU_SUBSETNLAST_VARIADIC(2, __VA_ARGS__)) \
)
#define _CU_BASE_FOR_PAIR_34(CONTEXT, SIZE, OP, FUNC, ...) OP( \
	CONTEXT, \
	16, \
	FUNC(CONTEXT, 16, CU_NLAST_VARIADIC(2, __VA_ARGS__), CU_NLAST_VARIADIC(1, __VA_ARGS__)), \
	_CU_BASE_FOR_PAIR_32(CONTEXT, 16, OP, FUNC, CU_SUBSETNLAST_VARIADIC(2, __VA_ARGS__)) \
)
#define _CU_BASE_FOR_PAIR_36(CONTEXT, SIZE, OP, FUNC, ...) OP( \
	CONTEXT, \
	17, \
	FUNC(CONTEXT, 17, CU_NLAST_VARIADIC(2, __VA_ARGS__), CU_NLAST_VARIADIC(1, __VA_ARGS__)), \
	_CU_BASE_FOR_PAIR_34(CONTEXT, 17, OP, FUNC, CU_SUBSETNLAST_VARIADIC(2, __VA_ARGS__)) \
)
#define _CU_BASE_FOR_PAIR_38(CONTEXT, SIZE, OP, FUNC, ...) OP( \
	CONTEXT, \
	18, \
	FUNC(CONTEXT, 18, CU_NLAST_VARIADIC(2, __VA_ARGS__), CU_NLAST_VARIADIC(1, __VA_ARGS__)), \
	_CU_BASE_FOR_PAIR_36(CONTEXT, 18, OP, FUNC, CU_SUBSETNLAST_VARIADIC(2, __VA_ARGS__)) \
)
#define _CU_BASE_FOR_PAIR_40(CONTEXT, SIZE, OP, FUNC, ...) OP( \
	CONTEXT, \
	19, \
	FUNC(CONTEXT, 19, CU_NLAST_VARIADIC(2, __VA_ARGS__), CU_NLAST_VARIADIC(1, __VA_ARGS__)), \
	_CU_BASE_FOR_PAIR_38(CONTEXT, 19, OP, FUNC, CU_SUBSETNLAST_VARIADIC(2, __VA_ARGS__)) \
)
#define _CU_BASE_FOR_PAIR_42(CONTEXT, SIZE, OP, FUNC, ...) OP( \
	CONTEXT, \
	20, \
	FUNC(CONTEXT, 20, CU_NLAST_VARIADIC(2, __VA_ARGS__), CU_NLAST_VARIADIC(1, __VA_ARGS__)), \
	_CU_BASE_FOR_PAIR_40(CONTEXT, 20, OP, FUNC, CU_SUBSETNLAST_VARIADIC(2, __VA_ARGS__)) \
)
#define _CU_BASE_FOR_PAIR_44(CONTEXT, SIZE, OP, FUNC, ...) OP( \
	CONTEXT, \
	21, \
	FUNC(CONTEXT, 21, CU_NLAST_VARIADIC(2, __VA_ARGS__), CU_NLAST_VARIADIC(1, __VA_ARGS__)), \
	_CU_BASE_FOR_PAIR_42(CONTEXT, 21, OP, FUNC, CU_SUBSETNLAST_VARIADIC(2, __VA_ARGS__)) \
)
#define _CU_BASE_FOR_PAIR_46(CONTEXT, SIZE, OP, FUNC, ...) OP( \
	CONTEXT, \
	22, \
	FUNC(CONTEXT, 22, CU_NLAST_VARIADIC(2, __VA_ARGS__), CU_NLAST_VARIADIC(1, __VA_ARGS__)), \
	_CU_BASE_FOR_PAIR_44(CONTEXT, 22, OP, FUNC, CU_SUBSETNLAST_VARIADIC(2, __VA_ARGS__)) \
)
#define _CU_BASE_FOR_PAIR_48(CONTEXT, SIZE, OP, FUNC, ...) OP( \
	CONTEXT, \
	23, \
	FUNC(CONTEXT, 23, CU_NLAST_VARIADIC(2, __VA_ARGS__), CU_NLAST_VARIADIC(1, __VA_ARGS__)), \
	_CU_BASE_FOR_PAIR_46(CONTEXT, 23, OP, FUNC, CU_SUBSETNLAST_VARIADIC(2, __VA_ARGS__)) \
)
#define _CU_BASE_FOR_PAIR_50(CONTEXT, SIZE, OP, FUNC, ...) OP( \
	CONTEXT, \
	24, \
	FUNC(CONTEXT, 24, CU_NLAST_VARIADIC(2, __VA_ARGS__), CU_NLAST_VARIADIC(1, __VA_ARGS__)), \
	_CU_BASE_FOR_PAIR_48(CONTEXT, 24, OP, FUNC, CU_SUBSETNLAST_VARIADIC(2, __VA_ARGS__)) \
)
#define _CU_BASE_FOR_PAIR_52(CONTEXT, SIZE, OP, FUNC, ...) OP( \
	CONTEXT, \
	25, \
	FUNC(CONTEXT, 25, CU_NLAST_VARIADIC(2, __VA_ARGS__), CU_NLAST_VARIADIC(1, __VA_ARGS__)), \
	_CU_BASE_FOR_PAIR_50(CONTEXT, 25, OP, FUNC, CU_SUBSETNLAST_VARIADIC(2, __VA_ARGS__)) \
)
#define _CU_BASE_FOR_PAIR_54(CONTEXT, SIZE, OP, FUNC, ...) OP( \
	CONTEXT, \
	26, \
	FUNC(CONTEXT, 26, CU_NLAST_VARIADIC(2, __VA_ARGS__), CU_NLAST_VARIADIC(1, __VA_ARGS__)), \
	_CU_BASE_FOR_PAIR_52(CONTEXT, 26, OP, FUNC, CU_SUBSETNLAST_VARIADIC(2, __VA_ARGS__)) \
)
#define _CU_BASE_FOR_PAIR_56(CONTEXT, SIZE, OP, FUNC, ...) OP( \
	CONTEXT, \
	27, \
	FUNC(CONTEXT, 27, CU_NLAST_VARIADIC(2, __VA_ARGS__), CU_NLAST_VARIADIC(1, __VA_ARGS__)), \
	_CU_BASE_FOR_PAIR_54(CONTEXT, 27, OP, FUNC, CU_SUBSETNLAST_VARIADIC(2, __VA_ARGS__)) \
)
#define _CU_BASE_FOR_PAIR_58(CONTEXT, SIZE, OP, FUNC, ...) OP( \
	CONTEXT, \
	28, \
	FUNC(CONTEXT, 28, CU_NLAST_VARIADIC(2, __VA_ARGS__), CU_NLAST_VARIADIC(1, __VA_ARGS__)), \
	_CU_BASE_FOR_PAIR_56(CONTEXT, 28, OP, FUNC, CU_SUBSETNLAST_VARIADIC(2, __VA_ARGS__)) \
)
#define _CU_BASE_FOR_PAIR_60(CONTEXT, SIZE, OP, FUNC, ...) OP( \
	CONTEXT, \
	29, \
	FUNC(CONTEXT, 29, CU_NLAST_VARIADIC(2, __VA_ARGS__), CU_NLAST_VARIADIC(1, __VA_ARGS__)), \
	_CU_BASE_FOR_PAIR_58(CONTEXT, 29, OP, FUNC, CU_SUBSETNLAST_VARIADIC(2, __VA_ARGS__)) \
)


/**
 * Perform actions every sequential triple of values in the variadic
 *
 * @code
 * #define OP(CONTEXT, INDEX, CURRENT, NEXT) (CURRENT + NEXT)
 * #define FUNC(CONTEXT, INDEX, A, B, C) (A * B * C)
 *
 * assert(CU_FOR_TRPLE(, OP, FUNC, 1, 2, 3, 4, 5, 6, 7, 8, 9) ==  (1*2*3+4*5*6+7*8*9));
 * @endcode
 *
 * @param[in] CONTEXT something that will be passed to every @c OP and @c FUNC macro
 * @param[in] OP a macro allowing you to concatenate the value generated by a @c FUNC call and the remainder part of the foreach
 *  @li CONTEXT: the @c CONTEXT stuff;
 * 	@li INDEX: a value which identifies the nth @c FUNC call. Starts with 0
 * 	@li CURRENT: the value returned by ther n-th @c FUNC call
 * 	@li NEXT: the stuff that will be generated by the \f$n+1\f$-th @c FUNC call
 * @param[in] FUNC a macro computing a single pair. it requires you to define a macro:
 * 	@li CONTEXT: the @c CONTEXT stuff;
 * 	@li INDEX: a value which identifies the nth @c FUNC call. Starts with 0
 * 	@li A: the first value of the triple
 * 	@li B: the second value of the triple
 * 	@li C: the third value of the triple
 */
#define CU_FOR_TRIPLE(CONTEXT, OP, FUNC, ...) _CU_FOR_TRIPLE(CONTEXT, CU_COUNT_ARGS(__VA_ARGS__), OP, FUNC, __VA_ARGS__)

#define _CU_FOR_TRIPLE(CONTEXT, SIZE, OP, FUNC, ...) P99_PASTE2(_CU_BASE_FOR_TRIPLE_, SIZE)(CONTEXT, SIZE, OP, FUNC, __VA_ARGS__)
#define _CU_BASE_FOR_TRIPLE_0(CONTEXT, SIZE, OP, FUNC, ...)
#define _CU_BASE_FOR_TRIPLE_3(CONTEXT, SIZE, OP, FUNC, ...) FUNC(CONTEXT, 1, CU_NLAST_VARIADIC(3, __VA_ARGS__), CU_NLAST_VARIADIC(2, __VA_ARGS__), CU_NLAST_VARIADIC(1, __VA_ARGS__))
#define _CU_BASE_FOR_TRIPLE_6(CONTEXT, SIZE, OP, FUNC, ...) OP(CONTEXT, 2, \
	FUNC(CONTEXT, 2, CU_NLAST_VARIADIC(3, __VA_ARGS__), CU_NLAST_VARIADIC(2, __VA_ARGS__), CU_NLAST_VARIADIC(1, __VA_ARGS__)), \
	_CU_BASE_FOR_TRIPLE_3(CONTEXT, 2, OP, FUNC, CU_SUBSETNLAST_VARIADIC(3, __VA_ARGS__)) \
)
#define _CU_BASE_FOR_TRIPLE_9(CONTEXT, SIZE, OP, FUNC, ...) OP(CONTEXT, 3, \
	FUNC(CONTEXT, 3, CU_NLAST_VARIADIC(3, __VA_ARGS__), CU_NLAST_VARIADIC(2, __VA_ARGS__), CU_NLAST_VARIADIC(1, __VA_ARGS__)), \
	_CU_BASE_FOR_TRIPLE_6(CONTEXT, 3, OP, FUNC, CU_SUBSETNLAST_VARIADIC(3, __VA_ARGS__)) \
)
#define _CU_BASE_FOR_TRIPLE_12(CONTEXT, SIZE, OP, FUNC, ...) OP(CONTEXT, 4, \
	FUNC(CONTEXT, 4, CU_NLAST_VARIADIC(3, __VA_ARGS__), CU_NLAST_VARIADIC(2, __VA_ARGS__), CU_NLAST_VARIADIC(1, __VA_ARGS__)), \
	_CU_BASE_FOR_TRIPLE_6(CONTEXT, 4, OP, FUNC, CU_SUBSETNLAST_VARIADIC(3, __VA_ARGS__)) \
)
#define _CU_BASE_FOR_TRIPLE_15(CONTEXT, SIZE, OP, FUNC, ...) OP(CONTEXT, 5, \
	FUNC(CONTEXT, 5, CU_NLAST_VARIADIC(3, __VA_ARGS__), CU_NLAST_VARIADIC(2, __VA_ARGS__), CU_NLAST_VARIADIC(1, __VA_ARGS__)), \
	_CU_BASE_FOR_TRIPLE_6(CONTEXT, 5, OP, FUNC, CU_SUBSETNLAST_VARIADIC(3, __VA_ARGS__)) \
)
#define _CU_BASE_FOR_TRIPLE_18(CONTEXT, SIZE, OP, FUNC, ...) OP(CONTEXT, 6, \
	FUNC(CONTEXT, 6, CU_NLAST_VARIADIC(3, __VA_ARGS__), CU_NLAST_VARIADIC(2, __VA_ARGS__), CU_NLAST_VARIADIC(1, __VA_ARGS__)), \
	_CU_BASE_FOR_TRIPLE_6(CONTEXT, 6, OP, FUNC, CU_SUBSETNLAST_VARIADIC(3, __VA_ARGS__)) \
)
#define _CU_BASE_FOR_TRIPLE_21(CONTEXT, SIZE, OP, FUNC, ...) OP(CONTEXT, 7, \
	FUNC(CONTEXT, 7, CU_NLAST_VARIADIC(3, __VA_ARGS__), CU_NLAST_VARIADIC(2, __VA_ARGS__), CU_NLAST_VARIADIC(1, __VA_ARGS__)), \
	_CU_BASE_FOR_TRIPLE_6(CONTEXT, 7, OP, FUNC, CU_SUBSETNLAST_VARIADIC(3, __VA_ARGS__)) \
)



#define _CUTILS_E1_0
#define _CUTILS_E1_1(WHAT, SEP, X) WHAT(X)
#define _CUTILS_E1_2(WHAT, SEP, X, ...) WHAT(X); _CUTILS_E1_1(WHAT, SEP, __VA_ARGS__)
#define _CUTILS_E1_3(WHAT, SEP, X, ...) WHAT(X); _CUTILS_E1_2(WHAT, SEP, __VA_ARGS__)
#define _CUTILS_E1_4(WHAT, SEP, X, ...) WHAT(X); _CUTILS_E1_3(WHAT, SEP, __VA_ARGS__)
#define _CUTILS_E1_5(WHAT, SEP, X, ...) WHAT(X); _CUTILS_E1_4(WHAT, SEP, __VA_ARGS__)
#define _CUTILS_E1_6(WHAT, SEP, X, ...) WHAT(X); _CUTILS_E1_5(WHAT, SEP, __VA_ARGS__)
#define _CUTILS_E1_7(WHAT, SEP, X, ...) WHAT(X); _CUTILS_E1_6(WHAT, SEP, __VA_ARGS__)
#define _CUTILS_E1_8(WHAT, SEP, X, ...) WHAT(X); _CUTILS_E1_7(WHAT, SEP, __VA_ARGS__)
#define _CUTILS_E1_9(WHAT, SEP, X, ...) WHAT(X); _CUTILS_E1_8(WHAT, SEP, __VA_ARGS__)
#define _CUTILS_E1_10(WHAT, SEP, X, ...) WHAT(X); _CUTILS_E1_9(WHAT, SEP, __VA_ARGS__)
#define _CUTILS_E1_11(WHAT, SEP, X, ...) WHAT(X); _CUTILS_E1_10(WHAT, SEP, __VA_ARGS__)
#define _CUTILS_E1_12(WHAT, SEP, X, ...) WHAT(X); _CUTILS_E1_11(WHAT, SEP, __VA_ARGS__)
#define _CUTILS_E1_13(WHAT, SEP, X, ...) WHAT(X); _CUTILS_E1_12(WHAT, SEP, __VA_ARGS__)
#define _CUTILS_E1_14(WHAT, SEP, X, ...) WHAT(X); _CUTILS_E1_13(WHAT, SEP, __VA_ARGS__)
#define _CUTILS_E1_15(WHAT, SEP, X, ...) WHAT(X); _CUTILS_E1_14(WHAT, SEP, __VA_ARGS__)
#define _CUTILS_E1_16(WHAT, SEP, X, ...) WHAT(X); _CUTILS_E1_15(WHAT, SEP, __VA_ARGS__)
#define _CUTILS_E1_17(WHAT, SEP, X, ...) WHAT(X); _CUTILS_E1_16(WHAT, SEP, __VA_ARGS__)
#define _CUTILS_E1_18(WHAT, SEP, X, ...) WHAT(X); _CUTILS_E1_17(WHAT, SEP, __VA_ARGS__)
#define _CUTILS_E1_19(WHAT, SEP, X, ...) WHAT(X); _CUTILS_E1_18(WHAT, SEP, __VA_ARGS__)
#define _CUTILS_E1_20(WHAT, SEP, X, ...) WHAT(X); _CUTILS_E1_19(WHAT, SEP, __VA_ARGS__)
#define _CUTILS_E1_21(WHAT, SEP, X, ...) WHAT(X); _CUTILS_E1_20(WHAT, SEP, __VA_ARGS__)
#define _CUTILS_E1_22(WHAT, SEP, X, ...) WHAT(X); _CUTILS_E1_21(WHAT, SEP, __VA_ARGS__)
#define _CUTILS_E1_23(WHAT, SEP, X, ...) WHAT(X); _CUTILS_E1_22(WHAT, SEP, __VA_ARGS__)
#define _CUTILS_E1_24(WHAT, SEP, X, ...) WHAT(X); _CUTILS_E1_23(WHAT, SEP, __VA_ARGS__)
#define _CUTILS_E1_25(WHAT, SEP, X, ...) WHAT(X); _CUTILS_E1_24(WHAT, SEP, __VA_ARGS__)
#define _CUTILS_E1_26(WHAT, SEP, X, ...) WHAT(X); _CUTILS_E1_25(WHAT, SEP, __VA_ARGS__)
#define _CUTILS_E1_27(WHAT, SEP, X, ...) WHAT(X); _CUTILS_E1_26(WHAT, SEP, __VA_ARGS__)
#define _CUTILS_E1_28(WHAT, SEP, X, ...) WHAT(X); _CUTILS_E1_27(WHAT, SEP, __VA_ARGS__)
#define _CUTILS_E1_29(WHAT, SEP, X, ...) WHAT(X); _CUTILS_E1_28(WHAT, SEP, __VA_ARGS__)
#define _CUTILS_E1_30(WHAT, SEP, X, ...) WHAT(X); _CUTILS_E1_29(WHAT, SEP, __VA_ARGS__)
#define _CUTILS_E1_31(WHAT, SEP, X, ...) WHAT(X); _CUTILS_E1_30(WHAT, SEP, __VA_ARGS__)
#define _CUTILS_E1_32(WHAT, SEP, X, ...) WHAT(X); _CUTILS_E1_31(WHAT, SEP, __VA_ARGS__)
#define _CUTILS_E1_33(WHAT, SEP, X, ...) WHAT(X); _CUTILS_E1_32(WHAT, SEP, __VA_ARGS__)
#define _CUTILS_E1_34(WHAT, SEP, X, ...) WHAT(X); _CUTILS_E1_33(WHAT, SEP, __VA_ARGS__)
#define _CUTILS_E1_35(WHAT, SEP, X, ...) WHAT(X); _CUTILS_E1_34(WHAT, SEP, __VA_ARGS__)
#define _CUTILS_E1_36(WHAT, SEP, X, ...) WHAT(X); _CUTILS_E1_35(WHAT, SEP, __VA_ARGS__)
#define _CUTILS_E1_37(WHAT, SEP, X, ...) WHAT(X); _CUTILS_E1_36(WHAT, SEP, __VA_ARGS__)
#define _CUTILS_E1_38(WHAT, SEP, X, ...) WHAT(X); _CUTILS_E1_37(WHAT, SEP, __VA_ARGS__)
#define _CUTILS_E1_39(WHAT, SEP, X, ...) WHAT(X); _CUTILS_E1_38(WHAT, SEP, __VA_ARGS__)
#define _CUTILS_E1_40(WHAT, SEP, X, ...) WHAT(X); _CUTILS_E1_39(WHAT, SEP, __VA_ARGS__)
#define _CUTILS_E1_41(WHAT, SEP, X, ...) WHAT(X); _CUTILS_E1_40(WHAT, SEP, __VA_ARGS__)
#define _CUTILS_E1_42(WHAT, SEP, X, ...) WHAT(X); _CUTILS_E1_41(WHAT, SEP, __VA_ARGS__)
#define _CUTILS_E1_43(WHAT, SEP, X, ...) WHAT(X); _CUTILS_E1_42(WHAT, SEP, __VA_ARGS__)
#define _CUTILS_E1_44(WHAT, SEP, X, ...) WHAT(X); _CUTILS_E1_43(WHAT, SEP, __VA_ARGS__)
#define _CUTILS_E1_45(WHAT, SEP, X, ...) WHAT(X); _CUTILS_E1_44(WHAT, SEP, __VA_ARGS__)
#define _CUTILS_E1_46(WHAT, SEP, X, ...) WHAT(X); _CUTILS_E1_45(WHAT, SEP, __VA_ARGS__)
#define _CUTILS_E1_47(WHAT, SEP, X, ...) WHAT(X); _CUTILS_E1_46(WHAT, SEP, __VA_ARGS__)
#define _CUTILS_E1_48(WHAT, SEP, X, ...) WHAT(X); _CUTILS_E1_47(WHAT, SEP, __VA_ARGS__)
#define _CUTILS_E1_49(WHAT, SEP, X, ...) WHAT(X); _CUTILS_E1_48(WHAT, SEP, __VA_ARGS__)
#define _CUTILS_E1_50(WHAT, SEP, X, ...) WHAT(X); _CUTILS_E1_49(WHAT, SEP, __VA_ARGS__)
#define _CUTILS_E1_51(WHAT, SEP, X, ...) WHAT(X); _CUTILS_E1_50(WHAT, SEP, __VA_ARGS__)
#define _CUTILS_E1_52(WHAT, SEP, X, ...) WHAT(X); _CUTILS_E1_51(WHAT, SEP, __VA_ARGS__)
#define _CUTILS_E1_53(WHAT, SEP, X, ...) WHAT(X); _CUTILS_E1_52(WHAT, SEP, __VA_ARGS__)
#define _CUTILS_E1_54(WHAT, SEP, X, ...) WHAT(X); _CUTILS_E1_53(WHAT, SEP, __VA_ARGS__)
#define _CUTILS_E1_55(WHAT, SEP, X, ...) WHAT(X); _CUTILS_E1_54(WHAT, SEP, __VA_ARGS__)
#define _CUTILS_E1_56(WHAT, SEP, X, ...) WHAT(X); _CUTILS_E1_55(WHAT, SEP, __VA_ARGS__)
#define _CUTILS_E1_57(WHAT, SEP, X, ...) WHAT(X); _CUTILS_E1_56(WHAT, SEP, __VA_ARGS__)
#define _CUTILS_E1_58(WHAT, SEP, X, ...) WHAT(X); _CUTILS_E1_57(WHAT, SEP, __VA_ARGS__)
#define _CUTILS_E1_59(WHAT, SEP, X, ...) WHAT(X); _CUTILS_E1_58(WHAT, SEP, __VA_ARGS__)
#define _CUTILS_E1_60(WHAT, SEP, X, ...) WHAT(X); _CUTILS_E1_59(WHAT, SEP, __VA_ARGS__)
#define _CUTILS_E1_61(WHAT, SEP, X, ...) WHAT(X); _CUTILS_E1_60(WHAT, SEP, __VA_ARGS__)
#define _CUTILS_E1_62(WHAT, SEP, X, ...) WHAT(X); _CUTILS_E1_61(WHAT, SEP, __VA_ARGS__)
#define _CUTILS_E1_63(WHAT, SEP, X, ...) WHAT(X); _CUTILS_E1_62(WHAT, SEP, __VA_ARGS__)
#define _CUTILS_E1_64(WHAT, SEP, X, ...) WHAT(X); _CUTILS_E1_63(WHAT, SEP, __VA_ARGS__)

/**
 * Macro used to implement macro variadic arguments
 */
#define _CUTILS_FETCH_NAME( 										\
	  _0, _1, _2, _3, _4, _5, _6, _7, _8, _9,_10, 		\
	 _11,_12,_13,_14,_15,_16,_17,_18,_19,_20, 			\
	 _21,_22,_23,_24,_25,_26,_27,_28,_29,_30, 			\
	 _31,_32,_33,_34,_35,_36,_37,_38,_39,_40, 			\
	 _41,_42,_43,_44,_45,_46,_47,_48,_49,_50, 			\
	 _51, _52, _53, _54, _55, _56, _57, _58, _59, 		\
	 _60, _61, _62, _63, name, ...) name

#define CUTILS_EXECUTE_EXPR1(name, sep, ...) _CUTILS_FETCH_NAME(__VA_ARGS__, \
	_CUTILS_E1_64, _CUTILS_E1_63, _CUTILS_E1_62, _CUTILS_E1_61, _CUTILS_E1_60, \
	_CUTILS_E1_59, _CUTILS_E1_58, _CUTILS_E1_57, _CUTILS_E1_56, _CUTILS_E1_55, _CUTILS_E1_54, _CUTILS_E1_53, _CUTILS_E1_52, _CUTILS_E1_51,_CUTILS_E1_50, 			\
    _CUTILS_E1_49, _CUTILS_E1_48, _CUTILS_E1_47,_CUTILS_E1_46,_CUTILS_E1_45,_CUTILS_E1_44,_CUTILS_E1_43,_CUTILS_E1_42,_CUTILS_E1_41,_CUTILS_E1_40, 								\
    _CUTILS_E1_39,_CUTILS_E1_38,_CUTILS_E1_37,_CUTILS_E1_36,_CUTILS_E1_35,_CUTILS_E1_34,_CUTILS_E1_33,_CUTILS_E1_32,_CUTILS_E1_31,_CUTILS_E1_30, 								\
    _CUTILS_E1_29,_CUTILS_E1_28,_CUTILS_E1_27,_CUTILS_E1_26,_CUTILS_E1_25,_CUTILS_E1_24,_CUTILS_E1_23,_CUTILS_E1_22,_CUTILS_E1_21,_CUTILS_E1_20, 								\
    _CUTILS_E1_19,_CUTILS_E1_18,_CUTILS_E1_17,_CUTILS_E1_16,_CUTILS_E1_15,_CUTILS_E1_14,_CUTILS_E1_13,_CUTILS_E1_12,_CUTILS_E1_11,_CUTILS_E1_10, 								\
    _CUTILS_E1_9,_CUTILS_E1_8,_CUTILS_E1_7,_CUTILS_E1_6,_CUTILS_E1_5,_CUTILS_E1_4,_CUTILS_E1_3,_CUTILS_E1_2,_CUTILS_E1_1, _CUTILS_E1_0)(name, sep, __VA_ARGS__)

#define _CUTILS_E2_00(WHAT1, WHAT2)
#define _CUTILS_E2_01(WHAT1, WHAT2, X)
#define _CUTILS_E2_02(WHAT1, WHAT2, X, Y)      WHAT1(X), WHAT2(Y); _CUTILS_E2_00(WHAT1, WHAT2)
#define _CUTILS_E2_03(WHAT1, WHAT2, X, Y, ...) WHAT1(X), WHAT2(Y); _CUTILS_E2_01(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_E2_04(WHAT1, WHAT2, X, Y, ...) WHAT1(X), WHAT2(Y); _CUTILS_E2_02(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_E2_05(WHAT1, WHAT2, X, Y, ...) WHAT1(X), WHAT2(Y); _CUTILS_E2_03(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_E2_06(WHAT1, WHAT2, X, Y, ...) WHAT1(X), WHAT2(Y); _CUTILS_E2_04(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_E2_07(WHAT1, WHAT2, X, Y, ...) WHAT1(X), WHAT2(Y); _CUTILS_E2_05(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_E2_08(WHAT1, WHAT2, X, Y, ...) WHAT1(X), WHAT2(Y); _CUTILS_E2_06(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_E2_09(WHAT1, WHAT2, X, Y, ...) WHAT1(X), WHAT2(Y); _CUTILS_E2_07(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_E2_10(WHAT1, WHAT2, X, Y, ...) WHAT1(X), WHAT2(Y); _CUTILS_E2_08(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_E2_11(WHAT1, WHAT2, X, Y, ...) WHAT1(X), WHAT2(Y); _CUTILS_E2_09(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_E2_12(WHAT1, WHAT2, X, Y, ...) WHAT1(X), WHAT2(Y); _CUTILS_E2_10(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_E2_13(WHAT1, WHAT2, X, Y, ...) WHAT1(X), WHAT2(Y); _CUTILS_E2_11(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_E2_14(WHAT1, WHAT2, X, Y, ...) WHAT1(X), WHAT2(Y); _CUTILS_E2_12(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_E2_15(WHAT1, WHAT2, X, Y, ...) WHAT1(X), WHAT2(Y); _CUTILS_E2_13(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_E2_16(WHAT1, WHAT2, X, Y, ...) WHAT1(X), WHAT2(Y); _CUTILS_E2_14(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_E2_17(WHAT1, WHAT2, X, Y, ...) WHAT1(X), WHAT2(Y); _CUTILS_E2_15(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_E2_18(WHAT1, WHAT2, X, Y, ...) WHAT1(X), WHAT2(Y); _CUTILS_E2_16(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_E2_19(WHAT1, WHAT2, X, Y, ...) WHAT1(X), WHAT2(Y); _CUTILS_E2_17(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_E2_20(WHAT1, WHAT2, X, Y, ...) WHAT1(X), WHAT2(Y); _CUTILS_E2_18(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_E2_21(WHAT1, WHAT2, X, Y, ...) WHAT1(X), WHAT2(Y); _CUTILS_E2_19(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_E2_22(WHAT1, WHAT2, X, Y, ...) WHAT1(X), WHAT2(Y); _CUTILS_E2_20(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_E2_23(WHAT1, WHAT2, X, Y, ...) WHAT1(X), WHAT2(Y); _CUTILS_E2_21(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_E2_24(WHAT1, WHAT2, X, Y, ...) WHAT1(X), WHAT2(Y); _CUTILS_E2_22(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_E2_25(WHAT1, WHAT2, X, Y, ...) WHAT1(X), WHAT2(Y); _CUTILS_E2_23(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_E2_26(WHAT1, WHAT2, X, Y, ...) WHAT1(X), WHAT2(Y); _CUTILS_E2_24(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_E2_27(WHAT1, WHAT2, X, Y, ...) WHAT1(X), WHAT2(Y); _CUTILS_E2_25(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_E2_28(WHAT1, WHAT2, X, Y, ...) WHAT1(X), WHAT2(Y); _CUTILS_E2_26(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_E2_29(WHAT1, WHAT2, X, Y, ...) WHAT1(X), WHAT2(Y); _CUTILS_E2_27(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_E2_30(WHAT1, WHAT2, X, Y, ...) WHAT1(X), WHAT2(Y); _CUTILS_E2_28(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_E2_31(WHAT1, WHAT2, X, Y, ...) WHAT1(X), WHAT2(Y); _CUTILS_E2_29(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_E2_32(WHAT1, WHAT2, X, Y, ...) WHAT1(X), WHAT2(Y); _CUTILS_E2_30(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_E2_33(WHAT1, WHAT2, X, Y, ...) WHAT1(X), WHAT2(Y); _CUTILS_E2_31(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_E2_34(WHAT1, WHAT2, X, Y, ...) WHAT1(X), WHAT2(Y); _CUTILS_E2_32(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_E2_35(WHAT1, WHAT2, X, Y, ...) WHAT1(X), WHAT2(Y); _CUTILS_E2_33(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_E2_36(WHAT1, WHAT2, X, Y, ...) WHAT1(X), WHAT2(Y); _CUTILS_E2_34(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_E2_37(WHAT1, WHAT2, X, Y, ...) WHAT1(X), WHAT2(Y); _CUTILS_E2_35(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_E2_38(WHAT1, WHAT2, X, Y, ...) WHAT1(X), WHAT2(Y); _CUTILS_E2_36(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_E2_39(WHAT1, WHAT2, X, Y, ...) WHAT1(X), WHAT2(Y); _CUTILS_E2_37(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_E2_40(WHAT1, WHAT2, X, Y, ...) WHAT1(X), WHAT2(Y); _CUTILS_E2_38(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_E2_41(WHAT1, WHAT2, X, Y, ...) WHAT1(X), WHAT2(Y); _CUTILS_E2_49(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_E2_42(WHAT1, WHAT2, X, Y, ...) WHAT1(X), WHAT2(Y); _CUTILS_E2_40(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_E2_43(WHAT1, WHAT2, X, Y, ...) WHAT1(X), WHAT2(Y); _CUTILS_E2_41(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_E2_44(WHAT1, WHAT2, X, Y, ...) WHAT1(X), WHAT2(Y); _CUTILS_E2_42(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_E2_45(WHAT1, WHAT2, X, Y, ...) WHAT1(X), WHAT2(Y); _CUTILS_E2_43(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_E2_46(WHAT1, WHAT2, X, Y, ...) WHAT1(X), WHAT2(Y); _CUTILS_E2_44(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_E2_47(WHAT1, WHAT2, X, Y, ...) WHAT1(X), WHAT2(Y); _CUTILS_E2_45(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_E2_48(WHAT1, WHAT2, X, Y, ...) WHAT1(X), WHAT2(Y); _CUTILS_E2_46(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_E2_49(WHAT1, WHAT2, X, Y, ...) WHAT1(X), WHAT2(Y); _CUTILS_E2_47(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_E2_50(WHAT1, WHAT2, X, Y, ...) WHAT1(X), WHAT2(Y); _CUTILS_E2_48(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_E2_51(WHAT1, WHAT2, X, Y, ...) WHAT1(X), WHAT2(Y); _CUTILS_E2_49(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_E2_52(WHAT1, WHAT2, X, Y, ...) WHAT1(X), WHAT2(Y); _CUTILS_E2_50(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_E2_53(WHAT1, WHAT2, X, Y, ...) WHAT1(X), WHAT2(Y); _CUTILS_E2_51(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_E2_54(WHAT1, WHAT2, X, Y, ...) WHAT1(X), WHAT2(Y); _CUTILS_E2_52(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_E2_55(WHAT1, WHAT2, X, Y, ...) WHAT1(X), WHAT2(Y); _CUTILS_E2_53(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_E2_56(WHAT1, WHAT2, X, Y, ...) WHAT1(X), WHAT2(Y); _CUTILS_E2_54(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_E2_57(WHAT1, WHAT2, X, Y, ...) WHAT1(X), WHAT2(Y); _CUTILS_E2_55(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_E2_58(WHAT1, WHAT2, X, Y, ...) WHAT1(X), WHAT2(Y); _CUTILS_E2_56(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_E2_59(WHAT1, WHAT2, X, Y, ...) WHAT1(X), WHAT2(Y); _CUTILS_E2_57(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_E2_60(WHAT1, WHAT2, X, Y, ...) WHAT1(X), WHAT2(Y); _CUTILS_E2_58(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_E2_61(WHAT1, WHAT2, X, Y, ...) WHAT1(X), WHAT2(Y); _CUTILS_E2_59(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_E2_62(WHAT1, WHAT2, X, Y, ...) WHAT1(X), WHAT2(Y); _CUTILS_E2_60(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_E2_63(WHAT1, WHAT2, X, Y, ...) WHAT1(X), WHAT2(Y); _CUTILS_E2_61(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_E2_64(WHAT1, WHAT2, X, Y, ...) WHAT1(X), WHAT2(Y); _CUTILS_E2_62(WHAT1, WHAT2, __VA_ARGS__)

#define CUTILS_EXECUTE_EXPR2(name1, name2, ...) _CUTILS_FETCH_NAME(__VA_ARGS__, \
	_CUTILS_E2_64, _CUTILS_E2_63, _CUTILS_E2_62, _CUTILS_E2_61, _CUTILS_E2_60, \
	_CUTILS_E2_59, _CUTILS_E2_58, _CUTILS_E2_57, _CUTILS_E2_56, _CUTILS_E2_55, _CUTILS_E2_54, _CUTILS_E2_53, _CUTILS_E2_52, _CUTILS_E2_51,_CUTILS_E2_50, 			\
    _CUTILS_E2_49, _CUTILS_E2_48, _CUTILS_E2_47,_CUTILS_E2_46,_CUTILS_E2_45,_CUTILS_E2_44,_CUTILS_E2_43,_CUTILS_E2_42,_CUTILS_E2_41,_CUTILS_E2_40, 								\
    _CUTILS_E2_39,_CUTILS_E2_38,_CUTILS_E2_37,_CUTILS_E2_36,_CUTILS_E2_35,_CUTILS_E2_34,_CUTILS_E2_33,_CUTILS_E2_32,_CUTILS_E2_31,_CUTILS_E2_30, 								\
    _CUTILS_E2_29,_CUTILS_E2_28,_CUTILS_E2_27,_CUTILS_E2_26,_CUTILS_E2_25,_CUTILS_E2_24,_CUTILS_E2_23,_CUTILS_E2_22,_CUTILS_E2_21,_CUTILS_E2_20, 								\
    _CUTILS_E2_19,_CUTILS_E2_18,_CUTILS_E2_17,_CUTILS_E2_16,_CUTILS_E2_15,_CUTILS_E2_14,_CUTILS_E2_13,_CUTILS_E2_12,_CUTILS_E2_11,_CUTILS_E2_10, 								\
    _CUTILS_E2_09,_CUTILS_E2_08,_CUTILS_E2_07,_CUTILS_E2_06,_CUTILS_E2_05,_CUTILS_E2_04,_CUTILS_E2_03,_CUTILS_E2_02,_CUTILS_E2_01, _CUTILS_E2_00)(name1, name2, __VA_ARGS__)

#define _CUTILS_D1_00()
#define _CUTILS_D1_01(WHAT1, WHAT2, X)      WHAT1(X), WHAT2(X)
#define _CUTILS_D1_02(WHAT1, WHAT2, X, ...) WHAT1(X), WHAT2(X), _CUTILS_D1_01(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_D1_03(WHAT1, WHAT2, X, ...) WHAT1(X), WHAT2(X), _CUTILS_D1_02(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_D1_04(WHAT1, WHAT2, X, ...) WHAT1(X), WHAT2(X), _CUTILS_D1_03(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_D1_05(WHAT1, WHAT2, X, ...) WHAT1(X), WHAT2(X), _CUTILS_D1_04(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_D1_06(WHAT1, WHAT2, X, ...) WHAT1(X), WHAT2(X), _CUTILS_D1_05(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_D1_07(WHAT1, WHAT2, X, ...) WHAT1(X), WHAT2(X), _CUTILS_D1_06(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_D1_08(WHAT1, WHAT2, X, ...) WHAT1(X), WHAT2(X), _CUTILS_D1_07(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_D1_09(WHAT1, WHAT2, X, ...) WHAT1(X), WHAT2(X), _CUTILS_D1_08(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_D1_10(WHAT1, WHAT2, X, ...) WHAT1(X), WHAT2(X), _CUTILS_D1_09(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_D1_11(WHAT1, WHAT2, X, ...) WHAT1(X), WHAT2(X), _CUTILS_D1_10(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_D1_12(WHAT1, WHAT2, X, ...) WHAT1(X), WHAT2(X), _CUTILS_D1_11(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_D1_13(WHAT1, WHAT2, X, ...) WHAT1(X), WHAT2(X), _CUTILS_D1_12(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_D1_14(WHAT1, WHAT2, X, ...) WHAT1(X), WHAT2(X), _CUTILS_D1_13(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_D1_15(WHAT1, WHAT2, X, ...) WHAT1(X), WHAT2(X), _CUTILS_D1_14(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_D1_16(WHAT1, WHAT2, X, ...) WHAT1(X), WHAT2(X), _CUTILS_D1_15(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_D1_17(WHAT1, WHAT2, X, ...) WHAT1(X), WHAT2(X), _CUTILS_D1_16(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_D1_18(WHAT1, WHAT2, X, ...) WHAT1(X), WHAT2(X), _CUTILS_D1_17(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_D1_19(WHAT1, WHAT2, X, ...) WHAT1(X), WHAT2(X), _CUTILS_D1_18(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_D1_20(WHAT1, WHAT2, X, ...) WHAT1(X), WHAT2(X), _CUTILS_D1_19(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_D1_21(WHAT1, WHAT2, X, ...) WHAT1(X), WHAT2(X), _CUTILS_D1_20(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_D1_22(WHAT1, WHAT2, X, ...) WHAT1(X), WHAT2(X), _CUTILS_D1_21(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_D1_23(WHAT1, WHAT2, X, ...) WHAT1(X), WHAT2(X), _CUTILS_D1_22(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_D1_24(WHAT1, WHAT2, X, ...) WHAT1(X), WHAT2(X), _CUTILS_D1_23(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_D1_25(WHAT1, WHAT2, X, ...) WHAT1(X), WHAT2(X), _CUTILS_D1_24(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_D1_26(WHAT1, WHAT2, X, ...) WHAT1(X), WHAT2(X), _CUTILS_D1_25(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_D1_27(WHAT1, WHAT2, X, ...) WHAT1(X), WHAT2(X), _CUTILS_D1_26(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_D1_28(WHAT1, WHAT2, X, ...) WHAT1(X), WHAT2(X), _CUTILS_D1_27(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_D1_29(WHAT1, WHAT2, X, ...) WHAT1(X), WHAT2(X), _CUTILS_D1_28(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_D1_30(WHAT1, WHAT2, X, ...) WHAT1(X), WHAT2(X), _CUTILS_D1_29(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_D1_31(WHAT1, WHAT2, X, ...) WHAT1(X), WHAT2(X), _CUTILS_D1_30(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_D1_32(WHAT1, WHAT2, X, ...) WHAT1(X), WHAT2(X), _CUTILS_D1_31(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_D1_33(WHAT1, WHAT2, X, ...) WHAT1(X), WHAT2(X), _CUTILS_D1_32(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_D1_34(WHAT1, WHAT2, X, ...) WHAT1(X), WHAT2(X), _CUTILS_D1_33(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_D1_35(WHAT1, WHAT2, X, ...) WHAT1(X), WHAT2(X), _CUTILS_D1_34(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_D1_36(WHAT1, WHAT2, X, ...) WHAT1(X), WHAT2(X), _CUTILS_D1_35(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_D1_37(WHAT1, WHAT2, X, ...) WHAT1(X), WHAT2(X), _CUTILS_D1_36(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_D1_38(WHAT1, WHAT2, X, ...) WHAT1(X), WHAT2(X), _CUTILS_D1_37(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_D1_39(WHAT1, WHAT2, X, ...) WHAT1(X), WHAT2(X), _CUTILS_D1_38(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_D1_40(WHAT1, WHAT2, X, ...) WHAT1(X), WHAT2(X), _CUTILS_D1_39(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_D1_41(WHAT1, WHAT2, X, ...) WHAT1(X), WHAT2(X), _CUTILS_D1_40(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_D1_42(WHAT1, WHAT2, X, ...) WHAT1(X), WHAT2(X), _CUTILS_D1_41(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_D1_43(WHAT1, WHAT2, X, ...) WHAT1(X), WHAT2(X), _CUTILS_D1_42(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_D1_44(WHAT1, WHAT2, X, ...) WHAT1(X), WHAT2(X), _CUTILS_D1_43(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_D1_45(WHAT1, WHAT2, X, ...) WHAT1(X), WHAT2(X), _CUTILS_D1_44(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_D1_46(WHAT1, WHAT2, X, ...) WHAT1(X), WHAT2(X), _CUTILS_D1_45(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_D1_47(WHAT1, WHAT2, X, ...) WHAT1(X), WHAT2(X), _CUTILS_D1_46(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_D1_48(WHAT1, WHAT2, X, ...) WHAT1(X), WHAT2(X), _CUTILS_D1_47(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_D1_49(WHAT1, WHAT2, X, ...) WHAT1(X), WHAT2(X), _CUTILS_D1_48(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_D1_50(WHAT1, WHAT2, X, ...) WHAT1(X), WHAT2(X), _CUTILS_D1_49(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_D1_51(WHAT1, WHAT2, X, ...) WHAT1(X), WHAT2(X), _CUTILS_D1_50(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_D1_52(WHAT1, WHAT2, X, ...) WHAT1(X), WHAT2(X), _CUTILS_D1_51(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_D1_53(WHAT1, WHAT2, X, ...) WHAT1(X), WHAT2(X), _CUTILS_D1_52(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_D1_54(WHAT1, WHAT2, X, ...) WHAT1(X), WHAT2(X), _CUTILS_D1_53(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_D1_55(WHAT1, WHAT2, X, ...) WHAT1(X), WHAT2(X), _CUTILS_D1_54(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_D1_56(WHAT1, WHAT2, X, ...) WHAT1(X), WHAT2(X), _CUTILS_D1_55(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_D1_57(WHAT1, WHAT2, X, ...) WHAT1(X), WHAT2(X), _CUTILS_D1_56(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_D1_58(WHAT1, WHAT2, X, ...) WHAT1(X), WHAT2(X), _CUTILS_D1_57(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_D1_59(WHAT1, WHAT2, X, ...) WHAT1(X), WHAT2(X), _CUTILS_D1_58(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_D1_60(WHAT1, WHAT2, X, ...) WHAT1(X), WHAT2(X), _CUTILS_D1_59(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_D1_61(WHAT1, WHAT2, X, ...) WHAT1(X), WHAT2(X), _CUTILS_D1_60(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_D1_62(WHAT1, WHAT2, X, ...) WHAT1(X), WHAT2(X), _CUTILS_D1_61(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_D1_63(WHAT1, WHAT2, X, ...) WHAT1(X), WHAT2(X), _CUTILS_D1_62(WHAT1, WHAT2, __VA_ARGS__)
#define _CUTILS_D1_64(WHAT1, WHAT2, X, ...) WHAT1(X), WHAT2(X), _CUTILS_D1_63(WHAT1, WHAT2, __VA_ARGS__)

/**
 * For each element in the sequence, perform the operations \c name1 and \c name2
 *
 * @code
 * int a = 5;
 * char b = 'a';
 * float c = 5.2;
 * CUTILS_DUPLICATE_EXPR2(sizeof, typeof, a, b, c);
 * is expanded into:
 * sizeof(a), typeof(a), sizeof(b), typeof(b), sizeof(c), typeof(c)
 * @endcode
 *
 * @param[in] name1 the first operation to apply to each element of the sequence
 * @param[in] name2 the second operation to apply to each element of the sequence
 * @param[in] ... the sequence to operate with
 */
#define CUTILS_DUPLICATE_EXPR2(name1, name2, ...) _CUTILS_FETCH_NAME(__VA_ARGS__, \
	_CUTILS_D1_64, _CUTILS_D1_63, _CUTILS_D1_62, _CUTILS_D1_61, _CUTILS_D1_60, \
	_CUTILS_D1_59, _CUTILS_D1_58, _CUTILS_D1_57, _CUTILS_D1_56, _CUTILS_D1_55, _CUTILS_D1_54, _CUTILS_D1_53, _CUTILS_D1_52, _CUTILS_D1_51,_CUTILS_D1_50, 			\
    _CUTILS_D1_49, _CUTILS_D1_48, _CUTILS_D1_47,_CUTILS_D1_46,_CUTILS_D1_45,_CUTILS_D1_44,_CUTILS_D1_43,_CUTILS_D1_42,_CUTILS_D1_41,_CUTILS_D1_40, 								\
    _CUTILS_D1_39,_CUTILS_D1_38,_CUTILS_D1_37,_CUTILS_D1_36,_CUTILS_D1_35,_CUTILS_D1_34,_CUTILS_D1_33,_CUTILS_D1_32,_CUTILS_D1_31,_CUTILS_D1_30, 								\
    _CUTILS_D1_29,_CUTILS_D1_28,_CUTILS_D1_27,_CUTILS_D1_26,_CUTILS_D1_25,_CUTILS_D1_24,_CUTILS_D1_23,_CUTILS_D1_22,_CUTILS_D1_21,_CUTILS_D1_20, 								\
    _CUTILS_D1_19,_CUTILS_D1_18,_CUTILS_D1_17,_CUTILS_D1_16,_CUTILS_D1_15,_CUTILS_D1_14,_CUTILS_D1_13,_CUTILS_D1_12,_CUTILS_D1_11,_CUTILS_D1_10, 								\
    _CUTILS_D1_09,_CUTILS_D1_08,_CUTILS_D1_07,_CUTILS_D1_06,_CUTILS_D1_05,_CUTILS_D1_04,_CUTILS_D1_03,_CUTILS_D1_02,_CUTILS_D1_01, _CUTILS_D1_00)(name1, name2, __VA_ARGS__)

///@addtogroup index_name_mapping
///@{

/**
 * Represents the prefix of every identifier output by ::CUTILS_INDEX_NAME_MAPPING
 *
 */
#define _CUTILS_INDEX_NAME_MAPPING_DEFAULT_NAME qwerty

/**
 * Convert a number into an unique identifier.
 *
 * Useful if you want to generate a sequence of different identifiers
 *
 * @code
 * int CUTILS_INDEX_NAME_MAPPING(0, a) = 5; //produce an identifier
 * int CUTILS_INDEX_NAME_MAPPING(1, a) = 6; //produce another identifier
 * printf("%d + %d is %d\n", CUTILS_INDEX_NAME_MAPPING(0, a), CUTILS_INDEX_NAME_MAPPING(1, a), CUTILS_INDEX_NAME_MAPPING(0, a) + CUTILS_INDEX_NAME_MAPPING(1, a));
 * @endcode
 *
 * This macro is much more flexible than ::UV ; however, is much more longer than ::UV
 *
 * @param[in] index an integer representing the id of the identifier to build
 * @param[in] suffix a string allowing you to customize further the identifier (in case of name clashing)
 * @return an unique identifier
 */
#define CUTILS_INDEX_NAME_MAPPING(index, suffix) \
		P99_PASTE3(_CUTILS_INDEX_NAME_MAPPING_DEFAULT_NAME, index, suffix)

///@}

#endif /* MACROS_H_ */
