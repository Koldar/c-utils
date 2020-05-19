/**
 * @file
 *
 * A module containing some defautl payload functions
 *
 * A paylaod_functions is basically a set of common functions. Assume there is a container (e.g., a list) which can handle a generic type:
 * for example it may contain strings. In order to successfully implement the generic list, every cell should have a pointer <tt>void*</tt>: this allow
 * to store in each cell a generic payload. However, some operations (e.g., destroy the list and all its content) needs to have some kind of information
 * about the payload (in this case, how to remove the paylaod from memory).
 *
 * By providing a payload_functions to a container, you pass to the container such information. The container will then generically use
 * the functions inside the payload_function to actually perform the operation.
 *
 * Each contined type should have a payload_functions associated. This module provide a paylaod_functions for common types.
 *
 * <h2>Increased flexibility</h2>
 *
 * payload_function allows to increase constructor flexibility as well.
 * A structure each container should have. Such structure contains several methods to control how the payload can be managed.
 *
 * For example if you need to get a string represenrtation of a container, you should have a string representation
 * method of the payload as well. This structure contains all the methods you might need to achieve this.
 *
 * It's more flexible than to feed those methods inside the new() because here you don't have to alter the parameters of the method at all.
 * For example assume you have:
 *
 * ```
 * cuListNew(destructor d, cloner c);
 * ```
 *
 * Then you need a ::buffer_string as well. With the current `cuListNew()` you need to change every reference to:
 *
 * ```
 * cuListNew(myDestructor, myCloner) //before
 * cuListNew(myDestructor, myCloner, myBufferString) //after
 * ```
 *
 * <h2>Semantic</h2>
 *
 * payload_function should be interpreted as a "java interface" each contained type shoudl conform to. Implementing all the functions inside a payload_functions
 * is not required, albeit strongly encouraged.
 *
 * @date Oct 11, 2017
 * @author koldar
 */

#ifndef PAYLOAD_FUNCTIONS_H_
#define PAYLOAD_FUNCTIONS_H_

#include "typedefs.h"

/**
 *
 *
 */
typedef struct {
	/**
	 * Function used to remove from memort the payload
	 */
	destructor destroy;
	/**
	 * Function used to clone (deep copy) the payload
	 */
	cloner clone;
	/**
	 * Function used to generate a string representation of the object
	 */
	buffer_string bufferString;
	/**
	 * Function used to determine which of 2 same object is the greater than the other one
	 */
	orderer order;
	/**
	 * Function used to determine whether or not 2 object are the same
	 */
	comparator compare;
	/**
	 * Function used to dump an object into a stream
	 */
	object_serializer serialize;
	/**
	 * Function used to fethc an object from a stream
	 */
	object_deserializer deserialize;
} payload_functions;

/**
 * The default payload functions.
 *
 * Assumes the payload is a pointer.
 *
 * The default has the following behaviour:
 *  @li destructor: if the value is not NULL, call free;
 *  @li cloner: simply return the reference, do not perform any malloc;
 *  @li buffer stringer: print the pointer;
 *  @li orderer and comparer: compare by pointer value;
 *  @li serializer and deserializer: serialize the pointer
 *
 */
payload_functions cuPayloadFunctionsDefault();

/**
 * A payload function which, not matter which function you execute, you get an error
 *
 * this is a useful starting point for creating your custom_payload function
 *
 * @return functions which, when called, raise an error
 */
payload_functions cuPayloadFunctionsError();

/**
 * functions tuned when the paylaod is actually an integer
 */
payload_functions cuPayloadFunctionsIntValue();

/**
 * functions tuned when the payload is a pointer to an integer
 */
payload_functions cuPayloadFunctionsIntPtr();

/**
 * Functions tunes when the payload is a string
 */
payload_functions cuPayloadFunctionsString();

/**
 * Functions tuned when the payload is a boolean
 */
payload_functions cuPayloadFunctionsBooleanValue();

#endif /* PAYLOAD_FUNCTIONS_H_ */
