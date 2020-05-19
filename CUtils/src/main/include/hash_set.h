/**
 * @file
 *
 * An hash set is an implementation of a set.
 *
 * A set is an unordered group of elements. You can check if an element belong to the set in O(1).
 * This module implement the set via an hashtable.
 *
 * Common usage as follows:
 *
 * @code
 * //init
 * hash_set* s = cuHashSetNew(getPayloadFunctionsInt());
 * //insert operation
 * cuHashSetAddItem(s, 4);
 * cuHashSetAddItem(s, 5);
 * //remove operation
 * cuHashSetRemoveItem(s, 4);
 * //contain operation
 * bool contains = cuHashSetContainsItem(s, 5); //return true
 * //destroy
 * cuHashSetDestroyWithElements(s);
 * @endcode
 *
 * @date Dec 21, 2017
 * @author koldar
 */

#ifndef HASH_SET_H_
#define HASH_SET_H_

#include <stdbool.h>
#include "hashtable.h"
#include "payload_functions.h"
#include "var_args.h"

typedef struct hash_set hash_set;

/**
 * A set containining, instead of \c void*, the actual int paylaod
 */
typedef hash_set pint_hash_set;
/**
 * A set containing, instead of @c void*, the actual boolean payload
 */
typedef hash_set pbool_hash_set;
/**
 * A set containing strings
 */
typedef hash_set string_hash_set;

/**
 * Initialize an hash set
 *
 * @param[in] functions support function to manage the payload of this container
 * @return the set desired
 */
hash_set* cuHashSetNew(payload_functions functions);

/**
 * Adds an item inside the set
 *
 * @param[inout] set the set where to add the item
 * @param[in] item the item to add
 * @return
 * 	\li true if we have added \c item in the set;
 * 	\li false if \c item was already inside the set;
 */
bool cuHashSetAddItem(CU_NOTNULL hash_set* set, const void* item);

/**
 * Check if \c item is inside the set
 *
 * @param[in] set the set to manage
 * @param[in] item the item to check
 * @return
 * 	\li true if \c item is inside \c set;
 * 	\li false otherwise;
 */
bool cuHashSetContainsItem(CU_NOTNULL const hash_set* set, const void* item);

/**
 * Remove an item inside the set
 *
 * \note
 * The function does nothing if \c item is not inside the set from the beginning
 *
 * @param[inout] set the set to manage
 * @param[in] item the element to remove from the set
 */
void cuHashSetRemoveItem(CU_NOTNULL hash_set* set, const void* item);

/**
 * Release the memory from this set
 *
 * \note
 * Paylaod won't be release from the memory at all
 *
 * @param[in] set the set to release from memory;
 */
void cuHashSetDestroy(CU_NOTNULL const hash_set* set, CU_NULLABLE const struct var_args* context);
#define CU_FUNCTION_POINTER_destructor_void_cuHashSetDestroy_voidConstPtr_var_argsConstPtr CU_DESTRUCTOR_ID

/**
 * Release the memory from this set
 *
 * \note
 * The function will remove from the memory the payload as well!
 *
 * @param[in] set the set to release from memory;
 */
void cuHashSetDestroyWithElements(CU_NOTNULL const hash_set* set, CU_NULLABLE const struct var_args* context);
#define CU_FUNCTION_POINTER_destructor_void_ccuHashSetDestroyWithElements_voidConstPtr_var_argsConstPtr CU_DESTRUCTOR_ID

/**
 * the size of the set
 *
 * @param[in] set the set to manage
 * @return the size of the set
 */
size_t cuHashSetGetSize(CU_NOTNULL const hash_set* set);

/**
 * true if the set is empty
 *
 * @param[in] set the set to manage
 * @return
 * 	\li true if the set has no items within it;
 * 	\li false otherwise
 */
bool cuHashSetIsEmpty(CU_NOTNULL const hash_set* set);

/**
 * Compute the \f$ set_{1} \cup set_{2} \f$
 *
 * \note
 * \c set1 and \c set2 can't be the same!
 *
 * @param[inout] set1 the first set to consider. If \c inPlace is true, this set **will be modified!**
 * @param[in] set2 the second set to consider;
 * @param[in] inPlace if false, the 2 sets won't be changed and the result will be the output. If \c inPlace is true, the return
 * 	value will be the same reference of \c set1, and \c set1 variable itself will be changed
 * @return the union of the 2 sets. **Payloads are copied by reference**.
 */
hash_set* cuHashSetGetUnionOfHashSets(CU_NOTNULL hash_set* restrict set1, CU_NOTNULL const hash_set* restrict set2, bool inPlace);

/**
 * Compute the \f$ set_{1} \cap set_{2} \f$
 *
 * \note
 * \c set1 and \c set2 can't be the same!
 *
 * @param[inout] set1 the first set to consider. If \c inPlace is true, this set **will be modified!**
 * @param[in] set2 the second set to consider;
 * @param[in] inPlace if false, the 2 sets won't be changed and the result will be the output. If \c inPlace is true, the return
 * 	value will be the same reference of \c set1, and \c set1 variable itself will be changed
 * @return the intersection of the 2 sets. **Payloads are copied by reference**.
 */
hash_set* cuHashSetGetIntersectionOfHashSets(CU_NOTNULL hash_set* restrict set1, CU_NOTNULL const hash_set* restrict set2, bool inPlace);

/**
 * Compute the \f$ set_{1} / set_{2} \f$
 *
 * \note
 * \c set1 and \c set2 can't be the same!
 *
 * @param[inout] set1 the first set to consider. If \c inPlace is true, this set **will be modified!**
 * @param[in] set2 the second set to consider;
 * @param[in] inPlace if false, the 2 sets won't be changed and the result will be the output. If \c inPlace is true, the return
 * 	value will be the same reference of \c set1, and \c set1 variable itself will be changed
 * @return the difference of the 2 sets. **Payloads are copied by reference**.
 */
hash_set* cuHashSetGetDifferenceOfHashSets(CU_NOTNULL hash_set* restrict set1, CU_NOTNULL const hash_set* restrict set2, bool inPlace);

/**
 * Empty the hash set
 *
 * @param[in] set the set to manage
 */
void cuHashSetClear(CU_NOTNULL const hash_set* set);

/**
 * Empty the hash set
 *
 * \note
 * this function will release from the memory all the items as well!
 *
 * @param[in] set the set to manage
 */
void cuHashSetClearWithElements(CU_NOTNULL const hash_set* set);

/**
 * compute a string representation of the set
 *
 * @param[in] set the set to manage
 * @param[inout] buffer the buffer where to store the string representation
 * @return the number of bytes which have been filled in \c buffer
 */
int cuHashSetGetBufferString(CU_NOTNULL const hash_set* set, char* buffer);

/**
 * Clone the set by copy the payloads within the set by reference
 *
 * This means that no deep copy is performed; however now the same data is accessible through 2 different sets;
 *
 * @param[in] set the set to manage
 */
hash_set* cuHashSetCloneByReference(CU_NOTNULL const hash_set* set);

/**
 * Clone the set by deep copying it
 *
 * This mean a **deep copy** is performed (depending on ::payload_functions implementation)
 *
 * @param[in] set the set to manage
 */
hash_set* cuHashSetClone(CU_NOTNULL const hash_set* set);

/**
 * Get a value in the set
 *
 * @attention
 * no garantuees are made about the randomness of this method nor the outcome.
 * The only garantuee this method offers is the return of an item inside the set.
 *
 * @param[in] set the set to deal with
 * @return
 *  @li an item inside the @c set;
 *  @li NULL if the set is empty;
 */
void* cuHashSetGetAnItem(CU_NOTNULL const hash_set* set);

/**
 * Get the hashtable under this set implementation
 *
 * \note
 * protected function. Shouldn't be used at all
 *
 * @param[in] set the set to manage
 * @return the hash table representing the set
 */
HT* _cuHashSetGetUnderlyingHashTable(CU_NOTNULL const hash_set* set);

/**
 * Macro to comfortably loop over the values inside the set
 *
 * @code
 * hash_set set = cuHashSetNew(...);
 * //add some values in "set"
 * CU_ITERATE_OVER_HASHSET(set, var, double) {
 * 	//var will be of type "double*"
 * 	printf("the set contains %f\n", *var);
 * }
 * @endcode
 *
 * \note
 * the ordering of iteration is **not garantueed to be deterministic**.
 *
 * @param[in] aset the set to loop over
 * @param[in] name the name of the variable which will contain the value to iterate over
 * @param[in] type the type the variable \c name will have within the cycle. It will always be a pointer of \c type.
 */
#define CU_ITERATE_OVER_HASHSET(aset, name, type) \
	for (bool UV(setloop)=true; UV(setloop); ) \
		for (const hash_set* UV(set)=(aset); UV(setloop); ) \
			for (const HT* UV(ht)=_cuHashSetGetUnderlyingHashTable(UV(set)); UV(setloop); UV(setloop)=false) \
				CU_ITERATE_OVER_HT_VALUES(UV(ht), name, type)

/**
 * Macro to comfortably loop over the values inside the set
 *
 * This allows you to add elements while looping as well
 *
 * @note
 * the order of iteration is **not garantueed to be deterministic**.
 *
 * @param[in] aset the set to loop over
 * @param[in] name the name of the variable which will contain the value to iterate over
 * @param[in] type the type the variable \c name will have within the cycle. It will always be a pointer of \c type.
 */
#define CU_VARIABLE_ITERATE_OVER_HASHSET(aset, name, type) \
	for (bool UV(setloop)=true; UV(setloop); ) \
		for (hash_set* UV(set)=((hash_set*)aset); UV(setloop); ) \
			for (HT* UV(ht)=_cuHashSetGetUnderlyingHashTable(UV(set)); UV(setloop); UV(setloop)=false) \
				CU_ITERATE_OVER_HT_VALUES(UV(ht), name, type)


#endif /* HASH_SET_H_ */
