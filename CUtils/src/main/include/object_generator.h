/**
 * @file
 *
 * File implementing generators and coroutine
 *
 * Generators
 * ==========
 *
 * CUtils provides a naive implementation of generators. It is strongly recommended to keep the generators small, in particular with
 * local variables: their number should be small.
 *
 * @code
 * #include "object_generator.h"
 * //define a simple generator
 * int range(cu_generator* g) {
 *  //this sector will be called everytime: here, for example, we fetch the values passed from cuGeneratorNew
 *	int start = cuVarArgsGetItem(cuGeneratorGetVarArgs(g), 0, int);
 *	int step = cuVarArgsGetItem(cuGeneratorGetVarArgs(g), 1, int);
 *	int end = cuVarArgsGetItem(cuGeneratorGetVarArgs(g), 2, int);
 *	//define geneator body. After the generator, there is a sequence of pairs each representing the type and the name of each local variable
 *	CU_GENERATOR(g, int, i)
 * 		for (i=start; i<end; i+=step) {
 * 			//generator, then the value to yield, then a seuqnece of local variables, IN THE SAME ORDER OF CU_GENERATOR
 *			CU_YIELD(g, i, int, i);
 *		}
 *	CU_GENERATOR_END(g);
 * }
 * //define generator user
 * int main() {
 *  cuInitVarArgsOnStack(va, 10, 20, 100); //lioke python range function
 *	cu_generator* gen = cuGeneratorNew(CU_AS_GENERATOR(range), va);
 *
 *	assert(range(gen) == 10);
 *	assert(range(gen) == 30);
 *	assert(range(gen) == 50);
 *	assert(range(gen) == 70);
 *	//use cuGeneratorIsFinished(gen) to check if the generator, after a call, has returned something valid or not
 *	cuGeneratorDestroy(gen);
 * }
 * @endcode
 *
 * @author koldar
 * @date Jun 27, 2018
 */

#ifndef OBJECT_GENERATOR_H_
#define OBJECT_GENERATOR_H_

#include <stdbool.h>
#include "typedefs.h"
#include "var_args.h"

typedef struct cu_generator cu_generator;

#define _CU_LOCAL_SIZE 10

/**
 * Represents a function which is compliant to be a generator
 *
 * @param[in] g a generator value
 */
typedef void* (*generator_t)(cu_generator* g);

/**
 * Cast a type pointer into one compliant with ::generator_t type
 *
 * @param[in] x the type pointer to convert
 */
#define CU_AS_GENERATOR(x) ((generator_t)(x))

/**
 * Initialize a new generator on the heap
 *
 * @param[in] gen a function representing the generator. The function body should compliant with the syntax of the genrator
 * @param[in] va optional set of additional variables to pass to the generator
 * @return the allocated generator
 */
cu_generator* cuGeneratorNew(generator_t gen, CU_NULLABLE const var_args* va);

/**
 * Destroy a generator
 *
 * @param[in] generator the gnerator to free from memory
 */
void cuGeneratorDestroy(const CU_NOTNULL cu_generator* generator, CU_NULLABLE const struct var_args* context);
#define CU_FUNCTION_POINTER_destructor_void_cuGeneratorDestroy_voidConstPtr_var_argsConstPtr CU_DESTRUCTOR_ID

/**
 * Store a local variable current value
 *
 * @private
 *
 * @param[inout] generator the generator used to store the variable
 * @param[in] index the index where to store the value
 * @param[in] val the value to store
 *
 */
void _cuGeneratorStoreLocal(CU_NOTNULL cu_generator* generator, int index, union generic_t val);

/**
 * Load a local variable stored in the cu_generator
 *
 * @private
 *
 * @param[in] generator the generator involved
 * @param[in] index the index where the local variable is stored
 * @return a pointer where the value fo the variable is stored
 */
void* _cuGeneratorLoadLocal(CU_NOTNULL const cu_generator* generator, int index);

/**
 * Check if the generator generated a value from the last call
 *
 * @param[in] generator the generator involved
 * @return
 *  @li true if the last call of the generator was successful
 *  @li false if the last call of the generator wasn't successful
 */
bool cuGeneratorIsFinished(CU_NOTNULL const cu_generator* generator);

/**
 * Mark a generator as finished
 *
 * @private
 *
 * @note
 * A generator is finished if we know that it can't produce anymore values
 *
 * @param[inout] generator the generator to mark
 */
void _cuGeneratorMarkAsFinished(CU_NOTNULL cu_generator* generator);

/**
 * Check if the generator has been started or not
 *
 * @param[in] generator the generator to handle
 * @return
 *  @li true if the generator has been started;
 *  @li false otherwise
 */
bool cuGeneratorIsStarted(CU_NOTNULL const cu_generator* generator);

/**
 * Mark a generator as started
 *
 * @private
 *
 * @note
 * A generator is started if we have already call the generator at least once but we don't know if it can produce values or not
 *
 * @param[inout] generator the generator to mark
 */
void _cuGeneratorMarkAsStarted(CU_NOTNULL cu_generator* generator);

/**
 * Check if the generator has been initialized or not
 *
 * @param[in] generator the generator to handle
 * @return
 *  @li true if the generator has been initialized;
 *  @li false otherwise
 */
bool cuGeneratorIsInitialized(CU_NOTNULL const cu_generator* generator);

/**
 * Get the last value of the label the generator has set
 *
 * @private
 *
 * Each labels identifies a specific yield
 *
 * @param[in] generator the generator to test
 * @return the label
 */
int _cuGeneratorGetLabel(CU_NOTNULL const cu_generator* generator);

/**
 * Set the value of qa given generator
 *
 * @private
 *
 * @param[inout] generator the structure to manage
 * @param[in] label the new value of the label
 */
void _cuGeneratorSetLabel(CU_NOTNULL cu_generator* generator, int label);

/**
 * Retrieve the var args set in the constructor of the ::cu_generator
 *
 * @param[in] generator the generator to handle
 * @return the variadic used in the constructor. read only
 */
CU_NULLABLE const var_args* cuGeneratorGetVarArgs(CU_NOTNULL const cu_generator* generator);

/**
 * Macro to create a new local variable in the generator memory
 *
 * @private
 *
 * @param[inout] generator the generator to manage
 * @param[in] index the cell where you want to store the local value in the generator. Starts with 0
 * @param[in] type the type of the variable to store
 * @param[in] value the value fo the variable to store
 */
#define _CU_DEFINE_LOCAL(generator, index, type, value) _cuGeneratorStoreLocal(generator, index, {value})

/**
 * Macro to load from the generator internal memory a variable value
 *
 * @private
 *
 * @param[in] generator the generator to load variable values from
 * @param[in] index the index where the variable you want to load is positioned
 * @param[in] type type of the variable you want to load
 */
#define _CU_LOAD_LOCAL(generator, index, type) (*((type*)_cuGeneratorLoadLocal(generator, index)))

#define _CU_STORE_LOCAL(generator, index, type, value) \
	memset(&UV(generic), 0, sizeof(union generic_t)); \
	type UV(value) = value; \
	memcpy(&UV(generic), &UV(value), sizeof(UV(value))); \
	_cuGeneratorStoreLocal(generator, index, UV(generic))

#define _CU_DEFINE_LOCALS_OP(generator, index, current, next) current; next
#define _CU_DEFINE_LOCALS_FUNC(generator, index, type, variableName) type variableName
#define _CU_DEFINE_LOCALS(generator, ...) CU_FOR_PAIR(generator, _CU_DEFINE_LOCALS_OP, _CU_DEFINE_LOCALS_FUNC, __VA_ARGS__)

#define _CU_LOAD_LOCALS_OP(generator, index, current, next) current; next
#define _CU_LOAD_LOCALS_FUNC(generator, index, type, variableName) variableName = _CU_LOAD_LOCAL(generator, index, type)
#define _CU_LOAD_LOCALS(generator, ...) CU_FOR_PAIR(generator, _CU_LOAD_LOCALS_OP, _CU_LOAD_LOCALS_FUNC, __VA_ARGS__)

#define _CU_STORE_LOCALS_OP(generator, index, current, next) current; next
#define _CU_STORE_LOCALS_FUNC(generator, index, type, value) _CU_STORE_LOCAL(generator, index, type, value)
#define _CU_STORE_LOCALS(generator, ...) CU_FOR_PAIR(generator, _CU_STORE_LOCALS_OP, _CU_STORE_LOCALS_FUNC, __VA_ARGS__)


//TODO continue
///**
// * @param[in] ... a sequence of pairs, each representing a local vartiable in the generator. the first value of the pair is the type of a local variable while
// * 	the second value is the name of the local variable
// */
//#define CU_DEFINE_GENERATOR(returnType, generatorName, generator, ...) \
//	returnType generatorName(cu_generator* generator) { \
//		_CU_GENERATOR_INIT_STATEMENTS /*follows () by the developer */
//
//#define _CU_GENERATOR_INIT_STATEMENTS(initStatements) \
//	initStatements; \
//	CU_GENERATOR(generator, __VA_ARGS__) \
//	_CU_GENERATOR_GENERATOR_CODE /* follow (with code) by the developer */
//
//#define _CU_GENERATOR_GENERATOR_CODE(generatorCode) \
//		generatorCode \
//		CU_GENERATOR_END(generator)
//
//
//
//
//CU_DEFINE_GENERATOR(int, range, generator, int, b)(
//		int start=0
//)(
//		CU_YIELD(0, int, b);
//);

/**
 * Mark the funciton as a generator
 *
 * @param[in] _generator the generator involved
 * @param[in] ... a pairs of types and variable name. the type represents the type of the second value of the pair, a local variable name to define.
 * 	Such variables will be automatically loaded when the generated is called
 */
#define CU_GENERATOR(_generator, ...) \
		cu_generator* UV(generator) = (_generator); \
		_CU_DEFINE_LOCALS(UV(generator), __VA_ARGS__); \
		if (cuGeneratorIsInitialized(UV(generator))) { \
			_cuGeneratorMarkAsStarted(UV(generator)); \
		} else if (cuGeneratorIsStarted(UV(generator))) { \
			/*load every locals*/ \
			_CU_LOAD_LOCALS(UV(generator), __VA_ARGS__); \
		} \
		\
		switch(_cuGeneratorGetLabel(UV(generator))) { \
		case 0: do {} while(0); /*useless statement*/

#define CU_YIELD(generator, val, ...) \
	do { \
		_cuGeneratorSetLabel(generator, __LINE__); \
		union generic_t UV(generic); \
		_CU_STORE_LOCALS(generator, __VA_ARGS__); \
		return (val); \
		case __LINE__: \
		if (!cuGeneratorIsStarted(generator)) { \
			goto generator_end; \
		} \
		_CU_LOAD_LOCALS((generator), __VA_ARGS__); \
	} while (0)

#define CU_GENERATOR_END(generator) \
	generator_end:; \
	_cuGeneratorMarkAsFinished(generator); \
	}

#endif /* OBJECT_GENERATOR_H_ */
