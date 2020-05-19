/**
 * @file
 *
 * The module contains several types representing pair, triples and, more generally, tuples.
 *
 * @date: Jan 22, 2018
 * @author: koldar
 */

#ifndef TUPLE_H_
#define TUPLE_H_

#include "macros.h"
#include "typedefs.h"
#include "payload_functions.h"
#include "var_args.h"

/**
 * A pair of generic pointers.
 */
struct pointer_pair_t {
	/**
	 * the first value of the pair
	 */
	//TODO this should be generic_t!
	CU_NULLABLE void* first;
	/**
	 * gthe second value of the pair
	 */
	//TODO this should be generic_t!
	CU_NULLABLE void* second;
	/**
	 * the first set of functions operating on the first value
	 */
	payload_functions firstFunctions;
	/**
	 * the second set of functions operating on the second value
	 */
	payload_functions secondFunctions;
};

/**
 * Initialize a new pair of void pointers
 *
 * @param[in] first the first pointer to initialize the first field of the pair
 * @param[in] second the second pointer to initialize the second field of the pair
 * @param[in] firstFunctions the set of useful functions dealing with the first value
 * @param[in] secondFunctions the set of useful functions dealing with the second value
 * @return the pair of generic pointers
 */
struct pointer_pair_t* cuPointerPairNew(CU_NULLABLE const void* first, CU_NULLABLE const void* second, payload_functions firstFunctions, payload_functions secondFunctions);

/**
 * Setup a new pair of void pointers
 *
 * @param[in] first the first pointer to initialize the first field of the pair
 * @param[in] second the second pointer to initialize the second field of the pair
 * @param[in] firstFunctions the set of useful functions dealing with the first value
 * @param[in] secondFunctions the set of useful functions dealing with the second value
 * @return the pair of generic pointers
 */
struct pointer_pair_t cuPointerPairSetup(CU_NULLABLE const void* first, CU_NULLABLE const void* second, payload_functions firstFunctions, payload_functions secondFunctions);

/**
 * Destroy from memory the pair of pointers.
 *
 * \note
 * Paylaod won't be released from memory though
 *
 * @param[in] p the pair to destroy
 */
void cuPointerPairDestroy(CU_NOTNULL const struct pointer_pair_t* p, CU_NULLABLE const struct var_args* context);
#define CU_FUNCTION_POINTER_destructor_void_cuPointerPairDestroy_voidConstPtr_var_argsConstPtr CU_DESTRUCTOR_ID

/**
 * like ::cuPointerPairDestroy but it destory also the payload of the pair
 *
 * @param[in] p the pair to destroy
 */
void cuPointerPairDestroyWithElements(CU_NOTNULL const struct pointer_pair_t* p, CU_NULLABLE const struct var_args* context);
#define CU_FUNCTION_POINTER_destructor_void_cuPointerPairDestroyWithElements_voidConstPtr_var_argsConstPtr CU_DESTRUCTOR_ID

/**
 * generate a set of functions to easily abstractally manage the payloads of the pair
 *
 * @return a set of function to maamnge the payload of the pair
 */
payload_functions cuPointerPairGetPayloadFunctions();

/**
 * the first element of the pair
 *
 * @code
 * cuPointerPairGetFirst(p, double*)
 * @endcode
 *
 * @param[in] p the pair involved
 * @param[in] type the type you want to cast the first value of ther pair
 * @return the first value (casted accordingly)
 */
#define cuPointerPairGetFirst(p, type) ((type)(p->first))

/**
 * the first element of the pair
 *
 * @code
 * cuPointerPairGetFirst(p, double*)
 * @endcode
 *
 * @param[in] p the pair involved
 * @param[in] type the type you want to cast the first value of ther pair
 * @return the first value (casted accordingly)
 */
#define cuPointerPairGetX(p, type) cuPointerPairGetFirst(p, type)

/**
 * the second element of the pair
 *
 * @code
 * cuPointerPairGetSecond(p, double*)
 * @endcode
 *
 * @param[in] p the pair involved
 * @param[in] type the type you want to cast the second value of ther pair
 * @return the second value (casted accordingly)
 */
#define cuPointerPairGetSecond(p, type) ((type)(p->second))

/**
 * the second element of the pair
 *
 * @code
 * cuPointerPairGetSecond(p, double*)
 * @endcode
 *
 * @param[in] p the pair involved
 * @param[in] type the type you want to cast the second value of ther pair
 * @return the second value (casted accordingly)
 */
#define cuPointerPairGetTY(p, type) cuPointerPairGetSecond(p, type)

#endif /* TUPLE_H_ */
