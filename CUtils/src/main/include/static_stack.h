/**
 * @file
 *
 * Provides a stack whose capacity **cannot** be altered
 *
 * If you want to have a stack which can enlarge, please consider ::dynamic_stack
 * Common usage is the following:
 *
 * ```
 * static_stack* s = cuStaticStackNew(100);
 * cuStaticStackPush(s, 5);
 * cuStaticStackPush(s, 7);
 * cuStaticStackPop(s); //retrieves 7
 * cuStaticStackPop(s); //retrieves 5
 * cuStaticStackDestroy(s);
 * ```
 *
 * @date Mar 13, 2017
 * @author koldar
 */

#ifndef STATIC_STACK_H_
#define STATIC_STACK_H_

#include <stdbool.h>
#include "macros.h"
#include "var_args.h"

/**
 * A data structure representing a stack
 */
typedef struct static_stack static_stack;

/**
 * Initialize a new stack
 *
 * \attention
 * Be sure to free it from the heap by calling ::cuStaticStackDestroy when it is not useful anymore
 *
 * @param[in] maxSize the size of the stack
 * @return a stack in memory
 */
static_stack* cuStaticStackNew(int maxSize);

/**
 * Destory from the heap a stack created with ::cuStaticStackNew
 *
 * @param[in] stack the stack to remove from the memory
 */
void cuStaticStackDestroy(CU_NOTNULL const static_stack* stack, CU_NULLABLE const var_args* context);
#define CU_FUNCTION_POINTER_destructor_void_cuStaticStackDestroy_voidConstPtr_var_argsConstPtr CU_DESTRUCTOR_ID

/**
 * @param[in] stack the structure to check
 * @return the number of elements inside the stack
 */
int cuStaticStackSize(CU_NOTNULL const static_stack* stack);

/**
 * @param[in] s the stack to analyze
 * @return the maximum capability of the stack
 */
int cuStaticStackGetCapacity(CU_NOTNULL const static_stack* s);

/**
 * Stacks empty can perform no \b pop operation
 *
 * @param[in] stack the structure to check
 * @return
 * 	@li true if the stack has no elements inside it;
 * 	@li false otherwise
 */
bool cuStaticStackIsEmpty(CU_NOTNULL const static_stack* stack);

/**
 * Stacks full can perform no \b push operation
 *
 * @param[in] stack the structure to check
 * @return
 * 	@li true if the stack is completely full
 * 	@li false otherwise
 */
bool cuStaticStackIsFull(CU_NOTNULL const static_stack* stack);

/**
 * Pops an element from the stack
 *
 * The stack will decrease its size after this operation
 *
 * @param[in] stack the structure to check
 * @return
 * 	\li the element on the top of the stack;
 * 	\li NULL otherwise
 */
void* cuStaticStackPop(CU_NOTNULL static_stack* stack);

/**
 * Push an elemento into the stack
 *
 * The stack will increase its size after this operation
 *
 * \attention
 * 	The function will crash if you try to push an element to a full stack
 *
 * @param[in] stack the structure to check
 * @param[in] data the data to push to the stack
 */
void cuStaticStackPush(CU_NOTNULL static_stack* stack, CU_NULLABLE const void* data);

/**
 * like ::cuStaticStackPop, but it won't alter the stack itself
 */
void* cuStaticStackPeek(CU_NOTNULL const static_stack* stack);

/**
 * Empty the stack
 *
 * \note
 * If there is data on the stack, it not \b free automatically. It is simple removed. Be sure
 * there are other references in the program to avoid memory leaks
 *
 * @param[in] stack the stack to clear
 */
void cuStaticStackClear(CU_NOTNULL static_stack* stack);

/**
 * Get the n-th item in the stack
 *
 * @private
 *
 * @pre
 *  @li \f$ index \geq 0 \land index < size \f$;
 *
 * @param[in] stack the stack to handle
 * @param[in] index the index of the value to fetch
 * @return the valu fetched
 */
void* _cuStaticStackGetNthItem(CU_NOTNULL const static_stack* stack, int index);

/**
 * Iterate thorugh the stack without actually changing it
 *
 * @note
 * The iteration goes from the top of the stack till the bottom
 *
 * @param[in] _stack the stack to analyze
 * @param[in] _payload a name of the variable containing the payload of the stack
 * @param[in] _payload_type the type of the _payload variable
 */
#define CU_ITERATE_OVER_STATIC_STACK(_stack, _payload, _payload_type) \
	static_stack* UV(stack) = _stack; \
	bool UV(innerLoop); \
	for (int UV(i)=(cuStaticStackSize(UV(stack)) - 1); (UV(innerLoop) = true, UV(i) >= 0); UV(i)--) \
		for (_payload_type _payload=(_payload_type)(_cuStaticStackGetNthItem(stack, UV(i))); UV(innerLoop); UV(innerLoop)=false) \


#endif /* STATIC_STACK_H_ */
