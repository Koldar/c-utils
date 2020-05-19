/**
 * @file
 *
 * Contains some function representing standard implementations of payload_functions
 *
 * @date May 18, 2017
 * @author koldar
 */

#ifndef DEFAULTFUNCTIONS_H_
#define DEFAULTFUNCTIONS_H_

#include <stdbool.h>
#include <stdio.h>
#include "macros.h"

struct var_args;

// ******************* COMPARATORS ************************

/**
 * @addtogroup comparators
 * @{
 * @brief all the functions which are suitable to be comparators
 *
 */

/**
 * Compare the 2 pointers by reference
 *
 * @param[in] a the first object to compare
 * @param[in] b the second object to compare
 * @return
 * @li true if <tt>a = b</tt>
 * @li false otherwise
 */
bool cuDefaultFunctionsComparatorObject(CU_NULLABLE const void* a, CU_NULLABLE const void* b);

/**
 * Compare 2 pointers actually holding 2 integers
 *
 * @code
 * cuDefaultFunctionsComparatorIntValue(5, 4); //false
 * cuDefaultFunctionsComparatorIntValue(5, 5); //true
 * @endcode
 *
 * @param[in] a the first integer to compare
 * @param[in] b the second integer to compare
 * @return
 * 	\li true if the 2 integers are equal;
 * 	\li false otherwise;
 */
bool cuDefaultFunctionsComparatorIntValue(const int a, const int b);

/**
 * Compare 2 pointer pointing to 2 integers
 *
 * @param[in] a the first pointer to integer to compare
 * @param[in] b the second pointer to integer to compare
 * @return
 * 	\li true if the 2 integers are equal;
 * 	\li false otherwise;
 */
bool cuDefaultFunctionsComparatorIntPtr(CU_NOTNULL const int* a, CU_NOTNULL const int* b);

/**
 * A dummy function which throws an error if called
 *
 * @note
 * this is a useful placeholder for objects where you don't want to set all the function before hand
 *
 * @param[in] a the first pointer to integer to compare
 * @param[in] b the second pointer to integer to compare
 * @return an error
 */
bool cuDefaultFunctionsComparatorNullObject(const void* a, const void* b);

/**
 * Compare 2 strings. Alias of @c strcmp
 *
 * @param[in] a the first string to compare
 * @param[in] b the second string to compare
 */
bool cuDefaultFunctionsComparatorString(const char* a, const char* b);

///@}

// ********************* DESTRUCTOR ************************

/**
 * @addtogroup destructor
 * @{
 * @brief all the functions which are suitable to be destructor
 *
 * Destructors removes from the heap initializes objects
 */

/**
 * Free the pointer \c payload
 *
 * @param[in] payload a points which refers to something on the heap to be removed. Can perform deep destruction as well
 */
void cuDefaultFunctionsDestructorObject(CU_NULLABLE const void* payload, CU_NULLABLE const struct var_args* context);
#define CU_FUNCTION_POINTER_destructor_void_cuDefaultFunctionsDestructorObject_voidConstPtr_var_argsConstPtr CU_DESTRUCTOR_ID

/**
 * A dummy function which throws an error if called
 *
 * @note
 * this is a useful placeholder for objects where you don't want to set all the function before hand
 *
 * @param[in] obj the object to deallocate
 */
void cuDefaultFunctionsDestructorNullObject(CU_NULLABLE const void* obj, CU_NULLABLE const struct var_args* context);
#define CU_FUNCTION_POINTER_destructor_void_cuDefaultFunctionsDestructorNullObject_voidConstPtr_var_argsConstPtr CU_DESTRUCTOR_ID

/**
 * A possible implementation of a ::destructor. Doe nothing
 *
 * \note
 * the function does nothing
 *
 * @param[in] payload the payload to free
 *
 */
void cuDefaultFunctionDestructorNOP(CU_NULLABLE const void* payload, CU_NULLABLE const struct var_args* context);
#define CU_FUNCTION_POINTER_destructor_void_cuDefaultFunctionDestructorNOP_voidConstPtr_var_argsConstPtr CU_DESTRUCTOR_ID

/**
 * A function which removes from memory a string from the heap
 *
 * @param[in] str the string to remove from memory. Does nothing if @c str is NULL
 */
void cuDefaultFunctionsDesturctorString(CU_NULLABLE const char* str, CU_NULLABLE const struct var_args* context);
#define CU_FUNCTION_POINTER_destructor_void_cuDefaultFunctionsDesturctorString_voidConstPtr_var_argsConstPtr CU_DESTRUCTOR_ID

///@}

// ****************** ORDERER *************************

/**
 * @addtogroup orderer
 * @{
 * @brief functions to detect the ordering of 2 objects
 *
 * If an order function returns 0 it means that the ordering between 2 objects is the same (this may or may not imply that the objects are equal)
 */

/**
 * Decide which integer is less than the other
 *
 * the variable is stored in the address pointed by \c a and \c b
 *
 */
int cuDefaultFunctionsOrdererIntPtr(const int* a, const int* b);

/**
 * Decide which integer is less than the other
 *
 * the variable is stored in the pointer \c payload **itself!**
 *
 */
int cuDefaultFunctionsOrdererIntValue(const int _a, const int _b);

/**
 * The default orderer. Compare pointer values
 *
 */
int cuDefaultFunctionsOrdererObject(const void* a, const void* b);

/**
 * A dummy function which throws an error if called
 *
 * @note
 * this is a useful placeholder for objects where you don't want to set all the function before hand
 *
 * @param[in] a first object
 * @param[in] b second object
 * @return an error
 */
int cuDefaultFunctionsOrdererNullObject(const void* a, const void* b);

/**
 * decide which string comes fist
 *
 * @param[in] a the first string
 * @param[in] b the second string
 * @return
 *  @li a negative number if \f$ a < b \f$
 *  @li a positive number if \f$ a > b \f$
 *  @li 0 if \f$ a = b \f$ or the ordering between the 2 strings cannot be determined
 */
int cuDefaultFunctionsOrdererString(CU_NOTNULL const char* a, CU_NOTNULL const char* b);

///@}

// ************************* CLONER ************************

/**
 * @addtogroup cloner
 * @{
 * @brief all the function explicitly designed to be cloner
 */

/**
 * default clone operation. return the given pointer
 *
 * @note
 * no malloc is actually performed
 *
 * @param[in] payload the content to copy
 * @return @c payload
 */
void* cuDefaultFunctionsClonerObject(const void* payload);
#define CU_FUNCTION_POINTER_cloner_voidPtr_cuDefaultFunctionsClonerObject_voidConstPtr CU_CLONER_ID

/**
 * Creates a copy of a pointer to a integer
 *
 * @param[in] i the pointer to integer to clone
 * @return a new malloced memory address containing the same value of @c i
 */
int* cuDefaultFunctionClonerIntPtr(CU_NOTNULL const int* i);
#define CU_FUNCTION_POINTER_cloner_voidPtr_cuDefaultFunctionClonerIntPtr_voidConstPtr CU_CLONER_ID

/**
 * A dummy function which throws an error if called
 *
 * @note
 * this is a useful placeholder for objects where you don't want to set all the function before hand
 *
 * @param[in] payload the object to clone
 * @return an error
 */
void* cuDefaultFunctionsClonerNullObject(const void* payload);
#define CU_FUNCTION_POINTER_cloner_voidPtr_cuDefaultFunctionsClonerNullObject_voidConstPtr CU_CLONER_ID

/**
 * Clones a string
 *
 * @attention
 * use ::cuDefaultFunctionsDesturctorString to remove the clone from memory
 *
 * @param[in] str the string to clone
 * @return a new clone of @c str in the heap
 */
char* cuDefaultFunctionsClonerString(CU_NOTNULL const char* str);
#define CU_FUNCTION_POINTER_cloner_voidPtr_cuDefaultFunctionsClonerString_voidConstPtr CU_CLONER_ID

///@}

// ************************* COLORER *****************************

/**
 * @addtogroup colorers
 * @{
 * @brief a function that, given an object, computes the associated color to it
 */

/**
 * @return always black
 *
 * \ingroup colorer
 */
int cuDefaultFunctionsColorerObject(void* payload);

///@}


//************************ BUFFER STRINGER ****************************

/**
 * @addtogroup bufferStringer
 * @{
 * @brief a buffer stringer is a function which computes a string representation of an object
 */

/**
 * A naive implementation of a ::buffer_string. Prints the pointer of the payload
 *
 * @param[in] payload a pointer containing the payload to print
 * @param[inout] buffer the buffer where to print into the number
 * @return the number of characters filled in the buffer
 */
int cuDefaultFunctionsBufferStringerObject(const void* payload, CU_NOTNULL char* buffer);

/**
 * Print an integer
 *
 * @param[in] i a pointer containing the **actual** integer
 * @param[inout] buffer the buffer where to print into the number
 * @return the number of characters filled in the buffer
 */
int cuDefaultFunctionsBufferStringerIntValue(const int i, char* buffer);

/**
 * Generate the string representation of a pointer of integer
 *
 * @c j is actually a pointer to a integer, not an integer itself
 *
 * @param[in] j a pointer to the  **actual** integer
 * @param[inout] buffer the buffer where to print into the number
 * @return the number of characters filled in the buffer
 */
int cuDefaultFunctionsBufferStringerIntPtr(CU_NOTNULL const int* j, CU_NOTNULL char* buffer);

/**
 * A dummy function which throws an error if called
 *
 * @note
 * this is a useful placeholder for objects where you don't want to set all the function before hand
 *
 * @param[in] payload a pointer containing the payload to print
 * @param[inout] buffer the buffer where to print into the number
 * @return the number of characters filled in the buffer
 * @return an error
 */
int cuDefaultFunctionsBufferStringerNullObject(CU_NULLABLE const void* payload, CU_NOTNULL char* buffer);

/**
 * Populate the buffer with a string represwsentation of a string
 *
 * @param[in] str the string to put in the buffer
 * @param[inout] buffer an initialized buffer which will containt the string representation of the string
 * @return the number of character filled in the buffer
 */
int cuDefaultFunctionsBufferStringerString(CU_NULLABLE const char* str, CU_NOTNULL char* buffer);

/**
 * Populate the buffer with a string represwsentation of a boolean
 *
 * @param[in] j the boolean to put in the buffer
 * @param[inout] buffer an initialized buffer which will containt the string representation of the string
 * @return the number of character filled in the buffer
 */
int cuDefaultFunctionsBufferStringerBoolValue(const bool j, CU_NOTNULL char* buffer);

///@}

//********************************* SERIALIZERS **********************************

/** @addtogroup serializers
 * @{
 * @brief functions allowing to persist data on a stream
 */

/**
 * The default serializer. It writes the pointer itself
 *
 * @pre
 *  @li @c f open with "wb"
 *
 * @param[inout] f the stream to update
 * @param[in] payload a pointer variable to clone
 */
int cuDefaultFunctionsSerializerObject(FILE* f, const void* payload);

/**
 * A dummy function which throws an error if called
 *
 * @pre
 *  @li @c f open with "wb"
 *
 * @note
 * this is a useful placeholder for objects where you don't want to set all the function before hand
 *
 * @param[inout] f the stream to update
 * @param[in] payload a pointer variable to clone
 * @return an error
 */
int cuDefaultFunctionsSerializerNullObject(CU_NOTNULL FILE* f, CU_NULLABLE const void* payload);

/**
 * Store inside the file the given integer
 *
 * The method might not work if the <tt>sizeof(int)>sizeof(void*)</tt>
 *
 * @pre
 *  @li @c f open with "wb"
 *
 * \note
 * We won't save the bytes containing an int, so cross platforming may not work
 *
 * @param[inout] f the stream to update
 * @param[in] n a pointer variable which **actually contains an integer**
 */
int cuDefaultFunctionsSerializerIntValue(FILE* f, const int n);

/**
 * Store inside the file the given integer
 *
 * @pre
 *  @li @c f open with "wb"
 *
 * @param[inout] f the stream to update
 * @param[in] n a pointer variable to the **actual** integer
 * @return the number of bytes filled
 */
int cuDefaultFunctionsSerializerIntPtr(FILE* f, CU_NOTNULL const int* n);

/**
 * Store inside the file the given string
 *
 * @pre
 *  @li @c f open with "wb"
 *
 * @param[inout] f the stream to update
 * @param[in] str the string to persist in the file
 * @return the number fo bytes filled
 */
int cuDefaultFunctionsSerializerString(FILE* f, CU_NOTNULL const char* str);

///@}

// ******************************* DESERIALIZERS *************************************

/** @addtogroup deserializers
 * @{
 * @brief functions used to generate inmemory data from a persisted value
 */

/**
 * The default deserializer. It reads the pointer itself
 *
 * Calling this function will move th cursor of the file itself!
 *
 * @pre
 *  @li @c f open with "rb"
 *
 * @param[in] f the stream where you need to read object from
 * @param[inout] payload a pointer to the object we need to populate from the stream
 * @return the number of byte read from the stream
 */
int cuDefaultFunctionsDeserializerObject(FILE* f, void* payload);

/**
 * A dummy function which throws an error if called
 *
 * @note
 * this is a useful placeholder for objects where you don't want to set all the function before hand
 *
 * @pre
 *  @li @c f open with "rb"
 *
 * @param[in] f the stream where you need to read object from
 * @param[inout] payload a pointer to the object we need to populate from the stream
 * @return an error
 */
int cuDefaultFunctionsDeserializerNullObject(CU_NOTNULL FILE* f, CU_NULLABLE const void* payload);

/**
 * Load an int from the file
 *
 * \note
 * If your platform use a different number of bytes to allocate an int, this simple deserializer
 * won't work
 *
 * @pre
 *  @li @c f open with "rb"
 *
 * @param[in] f the stream where you need to read object from
 * @param[inout] n a pointer to the object to populate. The pointer refers to a area of memory allocated to an int, either in the heap or stack
 * @return the number of byte read from the stream
 */
int cuDefaultFunctionsDeserializerIntValue(FILE* f, int* n);

/**
 * Load an int from the file
 *
 * \note
 * If your platform use a different number of bytes to allocate an int, this simple deserializer
 * won't work
 *
 * @pre
 *  @li @c f open with "rb"
 *
 * @param[in] f the stream where you need to read object from
 * @param[inout] n a pointer to <tt>int*</tt> to create. After this function is called, the pointer will reference a malloced area
 * @return the number of bytes read from the stream
 */
int cuDefaultFunctionsDeserializerIntPtr(FILE* f, int** n);

/**
 * Load a string from the file
 *
 * @note
 * the string will be created in the heap.
 *
 * @pre
 *  @li @c f open with "rb"
 *
 * @param[inout] f the stream where you need to read object from
 * @param[inout] pstr a pointer to a <tt>char*</tt> that will be alter to point a malloced area where the string will reside
 * @return the number of bytes read from the stream
 */
int cuDefaultFunctionsDeserializerString(CU_NOTNULL FILE* f, CU_NOTNULL char** pstr);

///@}

// ******************************* EVALUATORS *************************************

/**
 * The default evaluator. It reads the pointer itself and convert it to a number
 *
 * \ingroup evaluator
 *
 * @param[in] p the object to evaluate
 * @param[in] va the context of the evaluator to support the function body
 * @return an evaluation of the object \c p;
 */
int cuDefaultFunctionsEvaluatorObject(const void* p, const struct var_args* va);

// ******************************* HASH FUNCTIONS **********************************

unsigned long hashString(const char* str);


#endif /* DEFAULTFUNCTIONS_H_ */
