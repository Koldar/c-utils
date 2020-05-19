/**
 * @file
 *
 * Implements hashtables
 *
 * This module is just a front end to uthash
 *
 * Here's an example on how to use the hashtable. Of course you can exploit all macro available in \b ut_hash to find/loop and so on
 * even here. Those macros are explained <a href="https://troydhanson.github.io/uthash/userguide.html#_hash_operations">here</a>
 *
 * ```
 * HT* ht = cuHTNew();
 * double* d = NULL;
 *
 * d = malloc(sizeof(double)); *d = 5;
 * cuHTAddItem(ht,1, d);
 * d = malloc(sizeof(double)); *d = 127;
 * cuHTAddItem(ht,5, d);
 * d = malloc(sizeof(double)); *d = -4.5;
 * cuHTAddItem(ht,7, d);
 *
 * cuHTDestroyWithElements(ht, free);
 * ```
 *
 *  @date Jan 24, 2017
 *  @author koldar
 */

#ifndef HASHTABLE_H_
#define HASHTABLE_H_

#include <stdbool.h>
#include "uthash.h"
#include "typedefs.h"
#include "macros.h"
#include "var_args.h"
#include "payload_functions.h"

/**
 * A structure representing a cell of the hash table
 *
 * see <a href="http://troydhanson.github.io/uthash/userguide.html">uthash</a> for further information
 */
typedef struct HTCell {
	unsigned long id;
	void* data;
	UT_hash_handle hh;
} HTCell;

typedef struct HT HT;

#define CU_STANDARD_CONTAINER_TYPE HT
#define CU_STANDARD_CONTAINER_SUFFIX ht
#include "standardContainer.xmacro.h"

/**
 * an hashtable whose values are hashtables whose values are integers. Keys are unspecified
 */
typedef HT int_ht_in_ht;


//TODO remove default argument. This has been introduced to mantain backward compatibility with previous code
/**
 * Create a new hashtable in memory
 *
 * @param[in] functions a set of functions used to easily manage the payload
 * @return the new hashtable just created
 */
HT* cuHTNew(payload_functions functions);
CU_DECLARE_FUNCTION_WITH_DEFAULTS(HT*, cuHTNew, payload_functions);
#define cuHTNew(...) CU_CALL_FUNCTION_WITH_DEFAULTS(cuHTNew, 1, __VA_ARGS__)
CU_DECLARE_DEFAULT_VALUES(cuHTNew,
		cuPayloadFunctionsDefault()
);

/**
 * \note
 * This operation is a O(1)
 *
 * @param[in] ht the hash table involved
 * @return the number of elements inside the hash table
 */
int cuHTGetSize(CU_NOTNULL const HT* ht);

/**
 * get an element in the hashtable, given a certain key
 *
 * @param[in] ht the hashtable whwere we want to look int
 * @param[in] key the key associated to a certain element
 * @return
 * 	\li the element whose key is \c key;
 * 	\li NULL if it isn't present in the hashtable;
 */
void* cuHTGetItem(CU_NOTNULL const HT* ht, unsigned long key);

/**
 * @param[in] ht the ht involved
 * @param[in] key the key involved
 * @return
 * 	\li true if there is a value within the hashtable whoe key is \c key;
 * 	\li false otheriwse
 */
bool cuHTContainsItem(CU_NOTNULL const HT* ht, unsigned long key);

/**
 * Insert or alter an element inside the hash table
 *
 * If the hashtable doesn't have an element indexed with \c key, a new element will be created.
 * Otherwise we will update the existing element
 *
 * \attention
 * After this operation the old data will be totally overwritten! Be sure to still have a backup reference
 * of the previous object, otherwise memory leak will likely to happen!
 *
 * @param[in] ht the hashtable to analyze
 * @param[in] key the key of the element to update
 * @param[in] data the new data to overwrite the old one
 * @return
 * 	\li true if a new element is created;
 * 	\li false if we overwrote the previous one
 */
bool cuHTAddOrUpdateItem(CU_NOTNULL HT* ht, unsigned long key, CU_NULLABLE const void* data);

/**
 * Updates the value indexed by \c key to a new value
 *
 * \note
 * If there is no element indexed with \c key in the hashtable, this function does nothing
 *
 * @param[in] ht the hashtable to alter
 * @param[in] key the key referring to the element to update
 * @param[in] data the new data that will replace the old one
 * @return
 * 	\li true if we have update with success the data;
 * 	\li false if we couldn't find any cell indexed with \c key;
 */
bool cuHTUpdateItem(CU_NOTNULL HT* ht, unsigned long key, CU_NULLABLE const void* data);

/**
 * Add a key-value mapping within this hash table
 *
 * @param[inout] ht the hash table where the mapping needs to be add
 * @param[in] key the key to add in the hashtable
 * @param[in] data the values associated to the key that needs to be added
 */
void cuHTAddItem(CU_NOTNULL HT* ht, unsigned long key, CU_NULLABLE const void* data);

/**
 * Deallocate the memory occupied by this hash table
 *
 * @note
 * the payload associated to this hashtable won't be removed at all
 *
 * @param[in] ht the hashtable to remove
 */
void cuHTDestroy(CU_NOTNULL HT* ht, CU_NULLABLE const struct var_args* context);
#define CU_FUNCTION_POINTER_destructor_void_cuHTDestroy_voidConstPtr_var_argsConstPtr CU_DESTRUCTOR_ID

//TODO remove
/**
 * Like ::cuHTDestroy but it destroys the paylaod of the hashtable as well
 *
 * @param[in] ht the hashtable to remove
 * @param[in] d the destructor function to use to remove from memory the paylaods of the hastable
 */
void cuHTDestroyWithElements(CU_NOTNULL HT* ht, destructor d);

/**
 * Like ::cuHTDestroy but it destroys the paylaod of the hashtable as well
 *
 * @param[in] ht the hashtable to remove
 */
void cuHTDestroyWithElements2(CU_NOTNULL const HT* ht, CU_NULLABLE const struct var_args* context);
#define CU_FUNCTION_POINTER_destructor_void_cuHTDestroyWithElements2_voidConstPtr_var_argsConstPtr CU_DESTRUCTOR_ID

/**
 * Delete a cell inside the hashtable
 *
 * \post
 * 	\li don't use \c htCell after this call because it will cause an invalid read
 *
 * @param[in] ht the hash table to change
 * @param[in] htCell the cell to remove
 * @see cuHTDestroyCellWithElement
 */
void cuHTDestroyCell(CU_NOTNULL HT* ht, CU_NOTNULL HTCell* htCell);

/**
 * like ::cuHTDestroyCell but it delete from the memory also the element inside the cell
 *
 * @param[in] htCell the cell to remove from the memory
 * @param[in] d the destructor to use to remove the data inside the \c htCell
 * @see cuHTDestroyCell
 */
void cuHTDestroyCellWithElement(CU_NOTNULL HTCell* htCell, destructor d);

/**
 * delete an item from the hash table
 *
 * The function does nothing if the hash table des not contain the key
 *
 * @param[inout] ht the hashtable involved
 * @param[in] key the key associate to the value we need to remove
 * @return
 *  @li true if we have removed something;
 *  @li false otherwise
 */
bool cuHTRemoveItem(CU_NOTNULL HT* ht, unsigned long key);

/**
 * like ::cuHTRemoveItem but it will also remove from memory the value associated with @c key
 *
 * @param[inout] ht the hashtable involved
 * @param[in] key the key associate to the value we need to remove
 * @param[in] d the function to use to remove from memory the value associated with @c key
 * @return
 *  @li true if we have removed something;
 *  @li false otherwise
 */
bool cuHTRemoveItemWithElement(CU_NOTNULL HT* ht, unsigned long key, destructor d);

/**
 * Chekc if the hash table contains no values
 *
 * @param[in] ht the hash table to consider
 * @return
 *  @li true if the hash table is empty;
 *  @li false otherwise;
 */
bool cuHTIsEmpty(CU_NOTNULL const HT* ht);

/**
 * Find an element inside the hash table.
 *
 * If you already know the key of the element, use ::cuHTGetItem because it is way faster
 *
 * @param[in] ht the hashtable to look into
 * @param[in] finder the lambda function to use to detect if there is an element we're interested in
 * @param[in] va the context to use for function \c finder
 * @return
 * 	\li the element we're looking for;
 * 	\li NULL otherwise;
 */
CU_NULLABLE void* cuHTFindItem(CU_NOTNULL const HT* ht, finder finder, CU_NULLABLE const var_args* va);

/**
 * Check if an item is inside the hashtable
 *
 * \note
 * if you know the key of the element, it's much more quick to use ::cuHTGetItem
 *
 * @param[in] ht the hashtable to evaluate
 * @param[in] finder the function used to scroll the hashtable
 * @param[in] va the context of function \c finder
 * @return
 * 	\li true if the item is present inside the hashtable;
 * 	\li false otherwise
 */
bool cuHTContainsItems(CU_NOTNULL const HT* ht, finder finder, CU_NULLABLE const var_args* va);

/**
 * Fetch the first item the program can find within the hashtable.
 *
 * Nothing is said about what the softwar epicks up: <b>don't assume it was the first one you have added in the hashtable!</b>
 *
 * @param[in] ht the ht invovled
 * @return
 * 	\li an item inside the \c ht;
 * 	\li NULL if the \c ht is empty;
 */
CU_NULLABLE void* cuHTGetFirstItem(CU_NOTNULL const HT* ht);

/**
 * Swap the contents of the values indexed by \c key1 and \c keys
 *
 * If one key does not exist in the hashtable, after this operation it will exist whilst the
 * former one will be removed
 *
 * @param[in] ht the ht to handle
 * @param[in] key1 the first key
 * @param[in] key2 the second key
 * @return
 * 	\li true if a swap is performed;
 * 	\li false if both keys are not present;
 */
bool cuHTSwapValues(CU_NOTNULL HT* ht, unsigned long key1, unsigned long key2);

//TODO remove in order to do it you need to move all edge.h and node.h in predsuccgraph.h, using the payload_functions from graph for serialize nodes/edges
/**
 * store an hashtable inside a file
 *
 * \pre
 * 	\li \c f open in binary writing `wb`;
 *
 * @param[inout] f the file we want to write on;
 * @param[in] ht the hashtable to store
 * @param[in] payload the serializer to use to serialize the payload of the hashtable
 */
void cuHTStoreInFile(CU_NOTNULL FILE* f, CU_NOTNULL const HT* ht, object_serializer payload);

/**
 * store a hashtable in a file
 *
 * like ::cuHTStoreInFile but it uses the payload_functions in the hashtabole to serialize the payload
 *
 * \pre
 * 	\li \c f open in binary writing `wb`;
 *
 * @param[inout] f the file we want to write on;
 * @param[in] ht the hashtable to store
 */
void cuHTStoreInFile2(CU_NOTNULL FILE* f, CU_NOTNULL const HT* ht);

/**
 * load a hashtable from a file
 *
 * \pre
 * 	\li \c f open in binary reading
 * @post
 *  @li @c f cursor has been moved to next character after the serialization of the hashtable
 *
 *
 * @param[inout] f the file we want to read on
 * @param[in] functions set of function to easily manage the payload of the newly loaded hashtable
 * @return the hashtable loaded from the file
 *
 */
CU_NOTNULL HT* cuHTLoadFromFile(CU_NOTNULL FILE* f, payload_functions functions);


/**
 * Clear all the elements inside the given hashtable.
 *
 * The values inside the hashtable won't be touched; however make sure
 * you have other reference of them otherwise memory leak might happen
 *
 * @param[inout] ht the hashtable to clear
 */
void cuHTClear(CU_NOTNULL HT* ht);

/**
 * Clone a specified hashtable
 *
 * Elements are cloned by reference
 *
 * @param[in] ht the hastable to clone.
 * @return the clone pointer
 */
CU_NOTNULL HT* cuHTClone(CU_NOTNULL const HT* ht);
#define CU_FUNCTION_POINTER_cloner_voidPtr_cuHTClone_voidConstPtr CU_CLONER_ID

/**
 * Clone a specified hashtable
 *
 * Elements are cloned according to the paylaod function of ::HT
 *
 * @param[in] ht the hashtable to clone
 * @return the clone pointer
 */
CU_NOTNULL HT* cuHTCloneWithElements(CU_NOTNULL const HT* ht);
#define CU_FUNCTION_POINTER_cloner_voidPtr_cuHTCloneWithElements_voidConstPtr CU_CLONER_ID

/**
 * Clear all the elements inside the given hashtable
 *
 * Contrarly to ::cuHTClear, this function will release the memory of the paylaod inside the hashtable as well!
 *
 * \post
 * 	\li hashtable empty;
 * 	\li elements in hashtable released from memory
 *
 * @param[inout] ht the hashtable to clear
 * @param[in] d the function to use to destroy the paylaod of the hashtable
 */
void cuHTClearWithElements(CU_NOTNULL HT* ht, destructor d);

/**
 * @param[in] ht the hashtable to print
 * @param[inout] buffer the place in memory where to save the string conversion of the hashtable
 * @return the number of bytes the string conversion filled in the buffer
 */
int cuHTBufferString(CU_NOTNULL const HT* ht, CU_NOTNULL char* buffer);

/**
 * Check if 2 hashatable are the same
 *
 * We check both keys and values.
 *
 * @pre
 *  @li hashatable are over the same payload type
 *
 * @param[in] a first hashtable
 * @param[in] b second hashtable
 * @return
 *  @li true if all the key-mapping are the same;
 *  @li false otherwise
 */
bool cuHTCompare(CU_NOTNULL const HT* a, CU_NOTNULL const HT* b);

/**
 * computes the payload functions when the contained element of a container is actually an hashtable
 *
 * @note
 * This actually generates a composition of payload functions: assume the container containing the hastable is a list.
 * The list has ::payload_functions which handle the hashtable (destroy hastable, compare hashatable and so on). Each of those functions
 * calls the ::payload_function of the inner hash table
 *
 * @return the payload function desired
 */
payload_functions cuHTGetPayloadFunction();

//TODO change name
/**
 * @private
 *
 * @param[in] ht the hashtable to handle
 * @return the first cell of the hashtable
 */
CU_NOTNULL HTCell* _cuHTGetCell(CU_NOTNULL const HT* ht);

/**
 * Get the key associated to a HTCell
 *
 * @private
 *
 * @param[in] cell the cell to consider
 * @return the key associated to the HTCell
 */
unsigned long _cuHTCellGetKey(CU_NOTNULL const HTCell* cell);

/**
 * Get the value associated to a HTCell
 *
 * @private
 *
 * @param[in] cell the cell to consider
 * @return the value associated to the HTCell
 */
CU_NULLABLE void* _cuHTCellGetValue(CU_NOTNULL const HTCell* cell);

/**
 * Get the next HTCell of the given HTCell
 *
 * @private
 *
 * @param[in] cell the cell to consider
 * @return the next HtCell associated to the HTCell
 */
CU_NULLABLE HTCell* _cuHTCellGetNext(CU_NOTNULL const HTCell* cell);

#define CU_ITERATE_OVER_HT(_head,el)   \
	HTCell* UV(head) = _cuHTGetCell(_head);\
	HTCell* el = NULL; \
	HTCell* UV(tmp) = NULL;\
	for(((el)=(UV(head))), ((*(char**)(&(UV(tmp))))=(char*)((UV(head)!=NULL)?(UV(head))->hh.next:NULL)); \
  (el) != NULL; ((el)=(UV(tmp))), ((*(char**)(&(UV(tmp))))=(char*)((UV(tmp)!=NULL)?(UV(tmp))->hh.next:NULL)))

/**
 * Macro allowing you to go through every element of hashtable
 *
 * An example of usage might be:
 *
 * @code
 * HT* ht =cuHTNew(cuPayloadFunctionDefault());
 * cuHTAddItem(ht, 5, "hello");
 * cuHTAddItem(ht, 7, "world");
 * CU_ITERATE_OVER_HASHTABLE(ht, key, value, char*) {
 * 	printf("string associated to key %ld: %s\n", key, value);
 * }
 * @endcode
 *
 * The output should be:
 *
 * @code
 * 	string associated to key 7: world
 * 	string associated to key 5: hello
 * @endcode
 *
 * The order used is **not** garantueed
 *
 * @param[in] ht the hashtable you want to consider. Use a local varaible containing the hashtable itself
 * @param[in] key a variable identifier that will have type `unsigned long` containing, at each iteration, the key of an entry of the hashtable
 * @param[in] value a variable identifier that will have type @c valueType containing, at each iteration, the value of the entry associated to the key @c key
 * @param[in] type the type of value @c value
 */
#define CU_ITERATE_OVER_HASHTABLE(ht, key, value, valueType) \
	for (bool UV(htLoop) = true; UV(htLoop);) \
		for (HTCell *UV(head) = _cuHTGetCell(ht), *UV(el)=NULL, *UV(next)=NULL; UV(htLoop); ) \
			for (unsigned long key = 0; UV(htLoop); ) \
				for (valueType value = (valueType)0; UV(htLoop); UV(htLoop) = false) \
					for ( \
						UV(el) = UV(head), \
						UV(next) = UV(el) != NULL ? _cuHTCellGetNext(UV(el)) : NULL, \
						key = UV(el) != NULL ? _cuHTCellGetKey(UV(el)) : key, \
						value = UV(el) != NULL ? ((valueType)_cuHTCellGetValue(UV(el))) : value \
						; \
						UV(el) != NULL \
						; \
						UV(el) = UV(next), \
						UV(next) = UV(next) != NULL ? _cuHTCellGetNext(UV(next)) : NULL, \
						key = UV(el) != NULL ? _cuHTCellGetKey(UV(el)) : key, \
						value = UV(el) != NULL ? ((valueType)_cuHTCellGetValue(UV(el))) : value \
					)

/**
 * Macro allowing you to go through every element of hashtable (you can also remove the current cell in the loop)
 *
 * An example of usage might be:
 *
 * @code
 * HT* ht =cuHTNew(cuPayloadFunctionDefault());
 * cuHTAddItem(ht, 5, "hello");
 * cuHTAddItem(ht, 7, "world");
 * CU_VARIABLE_ITERATE_OVER_HASHTABLE(ht, cell, key, value, char*) {
 * 	printf("string associated to key %ld: %s\n", key, value);
 * 	cuHTDestroyCell(ht, cell);
 * }
 * @endcode
 *
 * The output should be:
 *
 * @code
 * 	string associated to key 7: world
 * 	string associated to key 5: hello
 * @endcode
 *
 * The order used is **not** garantueed
 *
 * @param[in] ht the hashtable you want to consider. Use a local varaible containing the hashtable itself
 * @param[in] key a variable identifier that will have type `unsigned long` containing, at each iteration, the key of an entry of the hashtable
 * @param[in] value a variable identifier that will have type @c valueType containing, at each iteration, the value of the entry associated to the key @c key
 * @param[in] type the type of value @c value
 */
#define CU_VARIABLE_ITERATE_OVER_HASHTABLE(ht, cell, key, value, valueType) \
	for (bool UV(htLoop) = true; UV(htLoop);) \
		for (HTCell *UV(head) = _cuHTGetCell(ht), *cell=NULL, *UV(next)=NULL; UV(htLoop); ) \
			for (unsigned long key = 0; UV(htLoop); ) \
				for (valueType value = (valueType)0; UV(htLoop); UV(htLoop) = false) \
					for ( \
						cell = UV(head), \
						UV(next) = cell != NULL ? _cuHTCellGetNext(cell) : NULL, \
						key = cell != NULL ? _cuHTCellGetKey(cell) : key, \
						value = cell != NULL ? ((valueType)_cuHTCellGetValue(cell)) : value \
						; \
						cell != NULL \
						; \
						cell = UV(next), \
						UV(next) = UV(next) != NULL ? _cuHTCellGetNext(UV(next)) : NULL, \
						key = cell != NULL ? _cuHTCellGetKey(cell) : key, \
						value = cell != NULL ? ((valueType)_cuHTCellGetValue(cell)) : value \
					)



/**
 * Macro allowing you to go through every element of hashtable
 *
 * An example of usage might be:
 *
 * <pre><code>
 * HT* ht;
 * CU_ITERATE_OVER_HT_VALUES(&ht, el, int) {
 * 	printf("value %d\n", *el);
 * }
 * </code></pre>
 *
 * @param[in] _head double point to an hashtable to go through
 * @param[in] _data the name of the variable that will contain a value in the iteration
 * @param[in] type the type of \c _data. pointer is automatically added. So if you put \c int, data will have type <tt>int*</tt>
 */
#define CU_ITERATE_OVER_HT_VALUES(_head,_data,type) \
		for (bool UV(htLoop)=true; UV(htLoop); ) \
			for (HTCell* UV(head) = (_cuHTGetCell(_head)), *UV(el)=NULL, *UV(tmp)=NULL; UV(htLoop); ) \
				for (type _data = (UV(head) != NULL)? ((type)UV(head)->data) : _data; UV(htLoop); UV(htLoop)=false) \
					for(\
						(UV(el)=(UV(head))), \
						((*(char**)(&(UV(tmp))))=(char*)((UV(head) != NULL)?(UV(head))->hh.next:NULL)) \
						; \
						UV(el) != NULL \
						; \
						(UV(el)=(UV(tmp))), \
						_data=(UV(el) != NULL) ? ((type)UV(el)->data) : _data,\
						((*(char**)(&(UV(tmp))))=(char*)((UV(tmp)!=NULL)?(UV(tmp))->hh.next:NULL))\
					)


#endif /* HASHTABLE_H_ */
