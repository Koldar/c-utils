/**
 * @file
 *
 * API for compacting variadic values in a structure
 *
 * Suppose you have a variadic function:
 *
 * @code
 * int foo(...) {
 *   //do stuff here with va_start, va_list and va_end
 * }
 *
 * int main() {
 * 	foo("hello", 5, true);
 * }
 * @endcode
 *
 * This module provides you a structure to better inquire variadic arguments. The module sees them as an array.
 *
 * @code
 * int foo(va_args* variadic) {
 *   //you can cycle over the variadic
 *   CU_ITERATE_ON_VARIADIC(variadic, i, v) {
 *   	printf("the %d-th variadic is %p\n", i, v);
 *   }
 *   //or you can manually access them:
 *   int b = cuVarArgsGetItem(variadic, 1, int); //get the copy of the variadic
 *   int* a = cuVarArgsGetItemPtr(variadic, 1, int*); //get the pointer of the variadic
 *   printf("b is %d\n", b); //shows 5
 *   printf("a is %d\n", *a); //shows 5
 * }
 *
 * int main() {
 *  cuInitVarArgsOnStack(va, "hello", 5, true);
 * 	foo(va);
 * }
 * @endcode
 *
 * This modules is one of the few which generates its structure **on the stack, not on the heap**. In other word, if you exit from the scope where
 * you called ::cuInitVarArgsOnStack, the variable @c va will lead to an invalid lead. This design choice has been made to create a lightweight variadic structure,
 * since I assumed it could be used everywhere.
 *
 * If you need to create the variadic on the heap, you can easily do it via:
 *
 * @code
 * 	cuNewVarArgsOnHeap(va, "hello", 5, true);
 * 	foo(va);
 * 	cuDestroyVarArgs(va); //explicit deallocation of va
 * @endcode
 *
 * @author koldar
 * @date Mar 13, 2018
 */

#ifndef VAR_ARGS_H_
#define VAR_ARGS_H_

#include "macros.h"
#include "log.h"
#include "errors.h"

/**
 * Metadata used to store variadic arguments
 * We need to define it in the header because this structuure is one of the few which can be allocated on the stack
 */
typedef struct var_args {
	/**
	 * number of elements of the 2 arrays ::var_args::sizes and ::var_args::values
	 */
	int count;
	/**
	 * Pointer to an array representing, in each cell, the size of the data in the i-th cell of the array ::var_args::sizes
	 */
	size_t* sizes;
	/**
	 * Pointer to an array representing, in each cell, the value of the varargs
	 */
	char* values;
} var_args;


/**
 * Creates a new variadic structure
 *
 * @private
 *
 * @param[in] count the number of variadic elements
 * @param[in] size_array an array \c count element long which contains the size of the i-th cell of \c value_array. **Copy by reference**
 * @param[in] value_array an array \c count element long which contains the values of each variadic value. **Copy by reference**
 * @return an element on the  **stack** representing the variadic metadata
 */
var_args _cuInitVarArgsOnStack(int count, CU_NULLABLE const size_t* size_array, CU_NULLABLE const char* value_array);

/**
 * Creates a new variadic structure on the heap
 *
 * @private
 *
 * @param[in] count the number of variadic elements
 * @param[in] size_array an array \c count element long which contains the size of the i-th cell of \c value_array. **Copy by reference**
 * @param[in] value_array an array \c count element long which contains the values of each variadic value. **Copy by reference**
 * @return an element on the  **heap** representing the variadic metadata
 */
var_args* _cuNewVarArgsOnHeap(int count, CU_NULLABLE const size_t* size_array, CU_NULLABLE const char* value_array);

/**
 * Destroy an existing structure created with ::cuNewVarArgsOnHeap
 *
 * @note
 * the function will remove from memory @c size_array and @c value_array (parameters from ::_cuNewVarArgsOnHeap) as well
 *
 * @param[in] va the structure to remove from the heap
 */
void cuDestroyVarArgs(CU_NOTNULL const var_args* va, CU_NULLABLE const struct var_args* context);
#define CU_FUNCTION_POINTER_destructor_void_cuDestroyVarArgs_voidConstPtr_var_argsConstPtr CU_DESTRUCTOR_ID

/**
 * Fetch the pointer of the i-th varaidic element
 *
 * @private
 *
 * @param[in] va the variadic metadata to inquire
 * @param[in] index the index of the data to fetch
 * @return a pointer to the i-th element of the variadic metadata
 */
void* _cuVarArgsGetItem(const var_args* va, int index);

/**
 * The number of variadic elements
 *
 * @param[in] va the metadata to inquire
 * @return the number fo variadic parameters of this function
 */
int cuVarArgsGetSize(const var_args* va);

/**
 * Get the i-th variadic argument as a **value**
 *
 * \note
 * If you want the pointer, use ::cuVarArgsGetItemPtr
 *
 * @param[in] va the structure to inquire. The type is <tt>var_args*</tt>
 * @param[in] index the index of the varaidic value to fetch
 * @param[in] type of the value to fetch. For instance \c int or <tt>long</tt>
 * @return the value contained in the variadic metadata
 * @see cuVarArgsGetItemPtr
 */
#define cuVarArgsGetItem(va, index, type) (*((type*)_cuVarArgsGetItem((var_args*)va, index)))

/**
 * Get the i-th variadic argument as a **pointer**
 *
 * \note
 * If you want the value, use ::cuVarArgsGetItem
 *
 * @param[in] va the structure to inquire. The type is <tt>var_args*</tt>
 * @param[in] index the index of the varaidic value to fetch
 * @param[in] type of the value to fetch. For instance <tt>char*</tt> or <tt>int*</tt>
 * @return the pointer to the variadic metadata
 * @see cuVarArgsGetItem
 */
#define cuVarArgsGetItemPtr(va, index, type) (((type*)_cuVarArgsGetItem((var_args*)va, index)))

/**
 * Create a new variadic arguments
 *
 * @code
 * int foo(const var_args* variadic) {
 *   //perform stuff here
 * }
 *
 * int main() {
 *   varArgs(output, 3, 'a', (struct point){3,2});
 *   foo(output);
 * }
 * @endcode
 *
 * \note
 * Note that this macro may execute the parameters in the variadic element multiple times. If you have injected values with side-effect
 * of big to copy values, be sure to replace them with identifiers!
 *
 * The values put into the structure won't be the real values, but **copy** of them.
 *
 * @attention
 * This macro does not need to call ::cuDestroyVarArgs at all.
 *
 * @param[inout] outputname the name of an identifier that will be created contanining the specified variadic arguments
 * @param[in] ... all the variadic values
 */
#define cuInitVarArgsOnStack(outputname, ...) \
	var_args UV(_outputname); \
	var_args* outputname = & UV(_outputname); \
	CU_HANDLE_VARIADIC(__VA_ARGS__)(\
			_CU_GENERATE_VARARGS(outputname, _CU_GENERATE_ARRAYS_ON_STACK, _CU_GENERATE_MEMCPYS_ON_STACK, __VA_ARGS__); \
			*outputname = _cuInitVarArgsOnStack(CU_COUNT_ARGS(__VA_ARGS__), UV(_size_of_array_variable), UV(_value_array_variable)); \
	)(\
			*outputname = _cuInitVarArgsOnStack(0, NULL, NULL) \
	) \

/**
 * like ::cuInitVarArgsOnStack, but it will create a structure on the heap
 *
 * @attention
 * Be sure to call ::cuDestroyVarArgs to prevent memory leaks
 *
 * @param[inout] outputname the name of an identifier that will be created contanining the specified variadic arguments
 * @param[in] ... all the variadic values
 */
#define cuNewVarArgsOnHeap(outputname, ...) \
	var_args* outputname = NULL; \
	CU_HANDLE_VARIADIC(__VA_ARGS__)( \
		_CU_GENERATE_VARARGS(outputname, _CU_GENERATE_ARRAYS_ON_HEAP, _CU_GENERATE_MEMCPYS_ON_HEAP, __VA_ARGS__); \
		outputname = _cuNewVarArgsOnHeap(CU_COUNT_ARGS(__VA_ARGS__), UV(_size_of_array_variable), UV(_value_array_variable)); \
	)( \
		outputname = _cuNewVarArgsOnHeap(0, NULL, NULL); \
	)

//macro initialization

/**
 * Macro generating the populated array to put as parameter in ::_cuInitVarArgsOnStack function
 *
 * @private
 *
 * \post
 *  \li array <tt>UV(_value_array_variable)</tt> containing copies of the variadic values created;
 *  \li array <tt>UV(_size_of_array_variable)</tt> containing copies of the variadic value sizes created;
 *
 * @param[in] outputname piece of variable name used as suffix for new variable definitions in order to avoid conflicts
 * @param[in] generateArraysMacro a macro specifying the code to generate the arrays used to store variadic arguments. The macro needs to accept
 * 	a variadic arugment containing the lvalues used to represent each input variadic value
 * @param[in] generateMemcpysMacro a macro specyfin the code to generate a bunch of memcpys over the arrays created in @c generateArraysMacro. The macro
 * 	needs to accepts the list of variables used to represents the lvalues representing the variadic arguments
 * @param[in] ... the variadic value to manage
 */
#define _CU_GENERATE_VARARGS(outputname, generateArraysMacro, generateMemcpysMacro, ...) \
	_CU_GENERATE_VARIABLES_DEFINITIONS(outputname, __VA_ARGS__); \
	generateArraysMacro(_CU_GENERATE_VARIABLES_SEQUENCE(outputname, __VA_ARGS__)); \
	{ \
		int UV(_buffer_next_fill) = 0; \
		generateMemcpysMacro(_CU_GENERATE_VARIABLES_SEQUENCE(outputname, __VA_ARGS__)); \
	} \

#define _CU_GENERATE_VARIABLES_DEFINITIONS_SEPARATOR(NAME, I, REC, RES) REC; RES
#define _CU_GENERATE_VARIABLES_DEFINITIONS_OPERATION(VAR_NAME, X, I) typeof(X) CUTILS_INDEX_NAME_MAPPING(I, VAR_NAME) = (X)
/**
 * A macro generating for every **value** of the sequence a new definition of variable on the stack
 *
 * @code
 * GENERATE_VARIABLES(5, 'a', 4.3);
 * //generates the following (name variables are much complex, but they are unique)
 * int a1 = 5;
 * char b1 = 'a';
 * float c1 = 4.3;
 * @endcode
 *
 * @param[in] variableName piece of variable name used as suffix for new variable definitions in order to avoid conflicts
 * @param[in] ... the values for each of them we need to create a variable definition
 */
#define _CU_GENERATE_VARIABLES_DEFINITIONS(variableName, ...) P99_FOR( \
		/*a data (macro or other stuff) which is available during all iterations*/ \
		variableName, \
		/*the number of times the iteration needs to happen*/ \
		CU_COUNT_ARGS(__VA_ARGS__), \
		/*reduce operation*/ \
		_CU_GENERATE_VARIABLES_DEFINITIONS_SEPARATOR, \
		/*operation to perform to every value in the sequence*/ \
		_CU_GENERATE_VARIABLES_DEFINITIONS_OPERATION, \
		/*sequence*/ \
		__VA_ARGS__ \
)

/**
 * Generates the definitions of the 2 arrays to put in ::_cuInitVarArgsOnStack
 *
 * @private
 *
 * The macro will simply generate the following code:
 * @code
 * char array[M];
 * size_t arraySize[N];
 * @endcode
 * where <tt>M, N</tt> are set to be a proper value
 *
 * @param[in] ... the variadic containing lvalues each of them representing a variadic argument
 */
#define _CU_GENERATE_ARRAYS_ON_STACK(...) \
	char UV(_value_array_variable)[0 + _CU_GENERATE_SUM_OF_SIZEOF(__VA_ARGS__)]; \
	size_t UV(_size_of_array_variable)[CU_COUNT_ARGS(__VA_ARGS__)]; \

#define _CU_GENERATE_ARRAYS_ON_HEAP(...) \
	char* UV(_value_array_variable) = (char*) malloc(_CU_GENERATE_SUM_OF_SIZEOF(__VA_ARGS__)); \
	if (UV(_value_array_variable) == NULL) { \
		ERROR_MALLOC(); \
	} \
	size_t* UV(_size_of_array_variable) = malloc((CU_COUNT_ARGS(__VA_ARGS__))*sizeof(size_t)); \
	if (UV(_size_of_array_variable) == NULL) { \
		ERROR_MALLOC(); \
	}

#define _CU_GENERATE_VARIABLES_SEQUENCE_SEPARATOR(NAME, I, REC, RES) REC, RES
#define _CU_GENERATE_VARIABLES_SEQUENCE_OPERATION(VAR_NAME, X, I) CUTILS_INDEX_NAME_MAPPING(I, VAR_NAME)
/**
 * Generate a sequence of unique variables each contanining a specifci variadic value
 *
 * @code
 * _CU_GENERATE_VARIABLES_SEQUENCE(output, 4, 'a', 3.4);
 * //will generate
 * @endcode
 *
 * \note
 * In this way we have lvalues of each variadic value
 *
 * @param[in] variableName the suffix each varaible defined will have
 * @param[in] ... variadic values each variables defined will have
 */
#define _CU_GENERATE_VARIABLES_SEQUENCE(variableName, ...) P99_FOR( \
		variableName, \
		CU_COUNT_ARGS(__VA_ARGS__), \
		_CU_GENERATE_VARIABLES_SEQUENCE_SEPARATOR, \
		_CU_GENERATE_VARIABLES_SEQUENCE_OPERATION, \
		__VA_ARGS__, \
)

#define _CU_GENERATE_SUM_OF_SIZEOF_SEPARATOR(NAME, I, REC, RES) REC + RES
#define _CU_GENERATE_SUM_OF_SIZEOF_OPERATION(NAME, X, I) sizeof(X)
/**
 * Generate the sum of the `sizeof` operators applied to each value in the sequence
 *
 * @code
 * int a;
 * int b;
 * char c;
 * size_t total = GENERATE_SUM_OF_SIZEOF(a, b, c);
 * //should return 4+4+1
 * @endcode
 */
#define _CU_GENERATE_SUM_OF_SIZEOF(...) P99_FOR( \
	, \
	CU_COUNT_ARGS(__VA_ARGS__), \
	_CU_GENERATE_SUM_OF_SIZEOF_SEPARATOR, \
	_CU_GENERATE_SUM_OF_SIZEOF_OPERATION, \
	__VA_ARGS__ \
)


#define _CU_GENERATE_MEMCPYS_ON_STACK_SEPARATOR(NAME, I, REC, RES) REC ; RES
#define _CU_GENERATE_MEMCPYS_ON_STACK_OPERATION(NAME, X, I) \
	UV(_size_of_array_variable)[I] = sizeof(X), \
	memcpy(((char*)(&UV(_value_array_variable))) + UV(_buffer_next_fill), &X, sizeof(X)), \
	UV(_buffer_next_fill) += sizeof(X) \

/**
 * Generates a list of calls of <tt>memcpy</tt> which populated 2 array
 *
 * The 2 arrays are:
 * \li <tt>UV(_value_array_variable)</tt>
 * \li <tt>UV(_size_of_array_variable)</tt>
 *
 * The function generates the following (supposing 4 variadic arguments):
 * @code
 * arraySize[0] = sizeof(a); memcpy(array + i, &a, sizeof(a)); i += sizeof(a);
 * arraySize[1] = sizeof(b); memcpy(array + i, &b, sizeof(b)); i += sizeof(b);
 * arraySize[2] = sizeof(c); memcpy(array + i, &c, sizeof(c)); i += sizeof(c);
 * arraySize[3] = sizeof(d); memcpy(array + i, &d, sizeof(d)); i += sizeof(d);
 * @endcode
 *
 * It is assumed a variables called <tt>UV(_buffer_next_fill)</tt> is present
 *
 * @param[in] ... lvalues we need to copy into the arrays
 */
#define _CU_GENERATE_MEMCPYS_ON_STACK(...) P99_FOR( \
		, \
		CU_COUNT_ARGS(__VA_ARGS__), \
		_CU_GENERATE_MEMCPYS_ON_STACK_SEPARATOR, \
		_CU_GENERATE_MEMCPYS_ON_STACK_OPERATION, \
		__VA_ARGS__ \
)

#define _CU_GENERATE_MEMCPYS_ON_HEAP_SEPARATOR(NAME, I, REC, RES) REC ; RES
#define _CU_GENERATE_MEMCPYS_ON_HEAP_OPERATION(NAME, X, I) \
	UV(_size_of_array_variable)[I] = sizeof(X), \
	memcpy(((char*)(UV(_value_array_variable))) + UV(_buffer_next_fill), &X, sizeof(X)), \
	UV(_buffer_next_fill) += sizeof(X) \

/**
 * Generates a list of calls of <tt>memcpy</tt> which populated 2 array
 *
 * The 2 arrays are:
 * \li <tt>UV(_value_array_variable)</tt>
 * \li <tt>UV(_size_of_array_variable)</tt>
 *
 * The function generates the following (supposing 4 variadic arguments):
 * @code
 * arraySize[0] = sizeof(a); memcpy(array + i, &a, sizeof(a)); i += sizeof(a);
 * arraySize[1] = sizeof(b); memcpy(array + i, &b, sizeof(b)); i += sizeof(b);
 * arraySize[2] = sizeof(c); memcpy(array + i, &c, sizeof(c)); i += sizeof(c);
 * arraySize[3] = sizeof(d); memcpy(array + i, &d, sizeof(d)); i += sizeof(d);
 * @endcode
 *
 * It is assumed a variables called <tt>UV(_buffer_next_fill)</tt> is present
 *
 * @param[in] ... lvalues we need to copy into the arrays
 */
#define _CU_GENERATE_MEMCPYS_ON_HEAP(...) P99_FOR( \
		, \
		CU_COUNT_ARGS(__VA_ARGS__), \
		_CU_GENERATE_MEMCPYS_ON_HEAP_SEPARATOR, \
		_CU_GENERATE_MEMCPYS_ON_HEAP_OPERATION, \
		__VA_ARGS__ \
)

/**
 * Iterate over all the variadic elements
 *
 * @param[in] va a <tt>var_args*</tt> variable representing the variadic metadata
 * @param[in] i an **unused** C identifier representing the incremental counter of the variadic item (0, 1, 2 and so on)
 * @param[in] v an **unused** C identifier representing the i-th variadic value
 */
#define CU_ITERATE_ON_VARIADIC(va, i, v) \
	for (int i=0; i<cuVarArgsGetSize((var_args*)va); i++) \
		CU_WRAP_CODE(void* v = cuVarArgsGetItemPtr((var_args*)va, i, void*), CU_NOCODE) \

#endif /* VAR_ARGS_H */
