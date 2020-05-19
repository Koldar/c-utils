/*
 * typedefs.h
 *
 *  Created on: May 17, 2017
 *      Author: koldar
 */

#ifndef TYPEDEFS_H_
#define TYPEDEFS_H_

#include <stdio.h>
#include <stdbool.h>
#include "macros.h"

//TODO this structure is present in other parts of the project as well!!!
/**
 * an union that might contain every possibile primitive value
 */
union generic_t {
	bool bval;
	short sval;
	int ival;
	unsigned int uival;
	char cval;
	unsigned char ucval;
	long lval;
	unsigned long ulval;
	long long llval;
	unsigned long long ullval;
	float fval;
	double dval;
	void* pval;
};

/**
 * Represents a pair of integers
 */
typedef struct int_pair {
	///the first integer
	int x;
	///the second integer
	int y;
} int_pair;

/**
 * Represents the command we need to execute when drawing the graph with ::_cuPredSuccGraphPrintGraphWithOptions
 */
typedef enum {
	/**
	 * We will use "dot" command, useful for standard direct graphs
	 */
	DC_DOT,
	/**
	 * We will use "circo" command, useful for circular graphs
	 */
	DC_CIRCO,
	/**
	 * We will use "neato" command, useful for undirect graphs
	 */
	DC_NEATO,
	/**
	 * We will use "sfdp" command, useful to large undirect graphs
	 */
	DC_SFDP
} dot_commands;

struct var_args;

// *************************** FINDER ****************************************************

/**
 * A function that check if a particular object satisfies a criterion
 *
 * @param[in] obj the object to check
 * @param[in] context of function
 * @return
 * 	\li \c true if \c obj satisfies a certain criterion (specified in the body of the function);
 * 	\li \c false otherwise
 */
typedef bool (*finder)(const void* obj, const struct var_args* context);

#define CU_FINDER_ID 0
/**
 * Convert a finder function to athe correct finder signature
 */
#define CU_AS_FINDER(...) ((CU_ENSURE_FUNCTION_POINTER(CU_FINDER_ID, finder, boolean, __VA_ARGS__, CU_FT_CONST_PTR(void), CU_FT_CONST_PTR(var_args))))

// *************************** DESTRUCTOR ****************************************************

/**
 * A function that destroy an object from the heap
 *
 * @post
 *  @li after calling this function, @c object is not present in the memory anymore. Nothing is said about the values it owns
 *
 * @param[in] object the object to remove from the heap
 * @param[in] context context of the function. Can be NULL
 *
 */
typedef void (*destructor)(const void* object, CU_NULLABLE const struct var_args* context);

#define CU_DESTRUCTOR_ID 1
//#define CU_MARK_AS_DESTRUCTOR(define, functionName) CU_CREATE_NEW_MACRO(define, P99_PASTE(CU_FUNCTION_POINTER_, void, _, functionName, _, voidConstPtr, _, var_argsConstPtr), 1)

/**
 * Convert a destructor function to athe correct destructor signature
 */
#define CU_AS_DESTRUCTOR(...) (CU_ENSURE_FUNCTION_POINTER(CU_DESTRUCTOR_ID, destructor, void, __VA_ARGS__, CU_FT_CONST_PTR(void), CU_FT_CONST_PTR(var_args)))

// *************************** CLONER ****************************************************

/**
 * a function which clone a certain object
 *
 * @param[in] obj the object to clone
 * @return a clone of the object. Note that deep clone or clone are not specified at all in this contract
 */
typedef CU_NULLABLE void* (*cloner)(CU_NULLABLE const void* obj);

#define CU_CLONER_ID 2

/**
 * Convert a cloner function to the correct clone signature
 */
#define CU_AS_CLONER(...) (CU_ENSURE_FUNCTION_POINTER(CU_CLONER_ID, cloner, CU_FT_PTR(void), __VA_ARGS__, CU_FT_CONST_PTR(void)))


// *************************** CONTAINER ****************************************************

/**
 * Function which checks if an item is inside a container
 *
 * @param[in] obj the container we need to check
 * @param[in] item the element we need to check if it belongs to @c container
 * @param[in] context additional parameters which can be useful for computing containment
 * @return
 *  @li true if @c item is inside @c obj;
 *  @li false otherwise
 */
typedef bool (*container_t)(CU_NOTNULL const void* container, CU_NULLABLE const void* item, CU_NULLABLE const struct var_args* context);

#define CU_CONTAINER_ID 3

#define CU_AS_CONTAINER(...) (CU_ENSURE_FUNCTION_POINTER(CU_CONTAINER_ID, container_t, boolean, __VA_ARGS__, CU_FT_CONST_PTR(void), CU_FT_CONST_PTR(void), CU_FT_CONST_PTR(var_args)))

// **************************** HASH FUNCTION **************************************************

/**
 * function which generate the hash of an item.
 *
 * @param[in] obj the object whose hash we need to generate
 * @param[in] context additional info to fine tuning the hash function computation
 * @return the hash value of an object
 */
typedef unsigned long (*hashfunction_t)(CU_NULLABLE const void* obj, CU_NULLABLE const struct var_args* context);
#define CU_HASHFUNCTION_ID 4

/**
 * cast a function to ::hashfunction_t
 *
 * The operation will compile time fail if a macro compliant with ::CU_ENSURE_FUNCTION_POINTER is not provided
 */
#define CU_AS_HASHFUNCTION(...) (CU_ENSURE_FUNCTION_POINTER( \
		CU_HASHFUNCTION_ID, container_t, \
		unsigned_long, \
		__VA_ARGS__, \
		CU_FT_CONST_PTR(void), CU_FT_CONST_PTR(var_args)))

/**
 * A function that checks if 2 objects are equal or not
 *
 * This function doesn't compute the ordering between the 2 object. For that reason, use ::orderer
 *
 * @param[in] _a the first object
 * @param[in] _b the second object
 * @return
 * 	\li true if the 2 objects are the same;
 * 	\li false otherwise
 */
typedef bool (*comparator)(const void* _a, const void* _b);

/**
 * Convert a compator function to the correct signature
 */
#define CU_AS_COMPARER(...) ((comparator)(__VA_ARGS__))

//TODO this buffer string whould have a parameter telling you how many characters are left in the buffer.
//or, in alternatives, you should use stringBuilder.h to create a string representation
/**
 * Represents anything that complies with buffer string.
 *
 * A buffer string function is something that accepts the thing whose string representation you
 * want to obtain and a buffer that will store such string representation.
 *
 * The return value represents the number of bytes consumed by the first parameter inside the buffer
 *
 * \note
 * This function is heavily **not** thread-safe
 */
typedef int (*buffer_string)(const void*,char*);

/**
 * Convert a buffer stringer function to the correct signature
 */
#define CU_AS_BUFFERSTRINGER(...) ((buffer_string)(__VA_ARGS__))

/**
 * Given an object, the function will compute its color
 *
 * The function is useful while debugging
 */
typedef int (*colorer)(void*);

/**
 * Convert a colorer function to the correct signature
 */
#define CU_AS_COLORER(...) ((colorer)(__VA_ARGS__))

/**
 * Given 2 object, determine if the first is less, equal of greater than the second one
 *
 * \note
 * if this function returns 0 it simply means that the 2 objects are in the same level. This **doesn't** mean they are the same though.
 * For that scope, use ::comparator
 *
 * @param[in] _a the first object to compare
 * @param[in] _b the second object to compare
 * @return
 * \li a negative number if \f$ a < b \f$;
 * \li 0 if  \f$ a = b\f$;
 * \li a positive number if \f$ a > b \f$;
 * @see ::comparator
 */
typedef int (*orderer)(const void* _a, const void* _b);

/**
 * Convert a buffer stringer function to the correct signature
 */
#define CU_AS_ORDERER(...) ((orderer)(__VA_ARGS__))

/**
 * Given a stream of char, parse the first characters read from the stream into an object.
 *
 * The first function parameter is the stream to read from; the second parameter is a pointer that we
 * will alter: after the call, it will contained the parsed data; the return value represents the number of
 * characters read from the stream.
 *
 * @param[inout] stream the stream where we need to load data from
 * @param[inout] obj an already usable area of memory to fill with the content fetched from the stream
 * @return the number of byte read from the stream
 */
typedef int (*object_deserializer)(FILE* stream, const void* obj);

/**
 * Convert a deserializer function to the correct signature
 */
#define CU_AS_DESERIALIZER(...) ((object_deserializer)(__VA_ARGS__))

/**
 * Given a stream, serialize the object within the stream
 *
 * The stream **will be changed**. After the call, the stream will contain the object serialized
 *
 * @param[inout] stream the stream where to store the file to
 * @param[in] obj the object to store. It needs to be a reference of something
 * @return the number of byte used to serialize \c obj
 */
typedef int (*object_serializer)(FILE* stream, const void* obj);

/**
 * Convert a serializer function to the correct signature
 */
#define CU_AS_SERIALIZER(...) ((object_serializer)(__VA_ARGS__))

/**
 * A type for any function giving something a value
 *
 * The evalutor function does not enforce a unique return value. Several totally different \c x
 * may share the same evaluation
 *
 * @param[in] x the object we need to evaluate
 * @param[in] context the context of the function
 * @return a number evaluating the object \c x
 */
typedef int (*evaluator_function)(const void* x, const struct var_args* context);

/**
 * Convert aevaluator function to the correct signasture
 */
#define CU_AS_EVALUATOR(...) ((evaluator_function)(__VA_ARGS__))

/**
 * A type of any function which simply checks for some condition
 *
 * @return
 *  @li true if a general condition is satisfied
 *  @li false otherwise
 */
typedef bool (*check_function)();


#endif /* TYPEDEFS_H_ */
