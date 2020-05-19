/**
 * @file
 *
 * Represents a list. The list implemented is a simple one (not double-linked).
 * In the memory, it might resemble to:
 *
 * \dot
 * 	digraph {
 * 		A;
 * 		B;
 * 		C;
 *
 * 		A -> B -> C;
 * 	}
 * \enddot
 *
 *
 *
 * @date May 17, 2017
 * @author koldar
 */

#ifndef LIST_H_
#define LIST_H_

#include "stdbool.h"
#include <stdlib.h>
#include "typedefs.h"
#include "macros.h"
#include "payload_functions.h"
#include "var_args.h"

typedef struct list_cell list_cell;

typedef struct list list;

#define CU_STANDARD_CONTAINER_SUFFIX list
#define CU_STANDARD_CONTAINER_TYPE list
#include "standardContainer.xmacro.h"

///**
// * a list of integers.
// * Integers are stored inside the pointer to a structure itself
// */
//typedef list int_list;
///**
// * A list of strings
// * The payload of the each list cell is a pointer to the string
// */
//typedef list string_list;

/**
 * Initialize a new list inside the memory.
 *
 * Use ::cuListDestroy or ::cuListDestroyWithElement to release the memory from the list
 *
 * @return the list requested
 */
list* cuListNew(payload_functions payloadFunctions);
CU_DECLARE_FUNCTION_WITH_DEFAULTS(list*, cuListNew, payload_functions);
#define cuListNew(...) CU_CALL_FUNCTION_WITH_DEFAULTS(cuListNew, 1, __VA_ARGS__)
CU_DECLARE_DEFAULT_VALUES(cuListNew,
	cuPayloadFunctionsDefault()
);



/**
 * Remove a list from the heap
 *
 * \note
 * the items in the list won't be removed from memory though
 *
 * @param[in] l the list to remove
 * @see cuListDestroyWithElement
 */
void cuListDestroy(CU_NOTNULL const list* l, CU_NULLABLE const struct var_args* context);
#define CU_FUNCTION_POINTER_destructor_void_cuListDestroy_voidConstPtr_var_argsConstPtr CU_DESTRUCTOR_ID

/**
 * Remove a list from the heap
 *
 * \note
 * the items in the list will be removed from memory though
 *
 * @param[in] l the list to remove
 * @see cuListDestroy
 */
void cuListDestroyWithElement(CU_NOTNULL const list* l, CU_NULLABLE const struct var_args* context);
#define CU_FUNCTION_POINTER_destructor_void_cuListDestroyWithElement_voidConstPtr_var_argsConstPtr CU_DESTRUCTOR_ID

/**
 * Clear the whole list
 *
 * \post
 * 	\li list is empty
 *
 * \note
 * the payload won't be freed at all. See ::cuListClearWithElements instead
 *
 * If this list is the only one referencing those elements, doing this will create memory leaks!
 *
 * @param[inout] l the list to free
 */
void cuListClear(CU_NOTNULL list* l);

/**
 * Clear the whole list with elements
 *
 * \post
 * 	\li list is empty;
 * 	\li all the payload is deleted as well
 *
 *
 * If this list is the only one referencing those elements, doing this **won't** create memory leaks!
 *
 * @param[inout] l the list to free
 */
void cuListClearWithElements(CU_NOTNULL list* l);

/**
 * Clone the given list
 *
 * The elements inside the list are copy **by value**: each element inside the list will be cloned
 * with \c c.
 * After this operation, if you release from memory the data inside the original list, the clone will
 * **not** become faulty
 *
 * @param[in] l the list to clone
 * @return the clone
 */
list* cuListCloneWithElements(CU_NOTNULL const list* l);

/**
 * Clone the given list
 *
 * The elements inside the list are copy **by reference**.
 * After this operation, if you release from memory the data inside the original list, the clone will
 * become faulty
 *
 * @param[in] l the list to clone
 * @return the clone
 */
list* cuListClone(CU_NOTNULL const list* l);

/**
 * Adds an element at the beginning of the list
 *
 * It's a \f$O(1) \f$ time operation
 *
 * @post
 *  @li this operation will increase the size of the list by 1;
 *
 * @param[inout] l the list to update
 * @param[in] el the element to add
 */
void cuListAddHead(CU_NOTNULL list* l, CU_NULLABLE const void* el);

/**
 * Append a value to the end of the list
 *
 * It's a \f$O(1) \f$ time operation
 *
 * @post
 *  @li the list will have its length increased by 1
 *
 * @param[inout] l the list to update
 * @param[in] el the item to add
 */
void cuListAddTail(CU_NOTNULL list* l, CU_NULLABLE const void* el);

/**
 * the size of he lsit
 *
 * It's a a \f$O(1)\f$ time operation
 *
 * @param[in] l the list to handle
 * @return the number of elements within this list
 */
int cuListGetSize(CU_NOTNULL const list* l);

/**
 * Adds all the elements of \c src into \c dst
 *
 * The @c src elements will be put on the tail of @c dst. The operation is a \f$ O(1) \f$ time.
 *
 * \note
 * After this function, \c src will be empty
 *
 * @param[inout] dst the list that will accept all the elements inside \c scc;
 * @param[inout] src the list whose elements will be tranferred to \c
 */
void cuListMoveContent(CU_NOTNULL list* restrict dst, CU_NOTNULL list* restrict src);

/**
 * Get the tail of the list
 *
 * I't a \f$ O(1)\f$ time operation.
 *
 * @param [in] l the list to handle
 * @return the tail of the list or NULL if the list is empty
 */
void* cuListGetLastElement(CU_NOTNULL const list* l);

/**
 * Check if the given list is empty
 *
 * @param[in] l the list to analyze
 * @return
 * 	\li true if \c l is empty;
 * 	\li false otherwises
 */
bool cuListIsEmpty(CU_NOTNULL const list* l);

/**
 * Get and remove the head from the list \c l
 *
 * @param[inout] l the list involved
 * @return
 * 	\li the payload at the head of the list;
 * 	\li NULL if the list is empty;
 */
void* cuListPopFrom(CU_NOTNULL list* l);

/**
 * remove the head of the list, even from the memory
 *
 * @note
 * If the @c l is empty nothing will be executed
 *
 * @param[inout] l the list whose head needs to  remove
 */
void cuListRemoveHeadAndDestroyItem(CU_NOTNULL list* l, CU_NULLABLE const struct var_args* context);

/**
 * Alias for ::cuListGetHead0
 *
 * @param[in] l the list to handle
 * @return
 * 	\li the head fo the list;
 * 	\li NULL if \c l is empty;
 */
void* cuListPeek0(CU_NOTNULL const list* l);

/**
 * wrapper of ::cuListPeek0 which allows you to automatically cast the value peeked
 *
 * @param[in] l the ist involved
 * @param[in] type type whose valued peeked is casted to. Note that peek may return NULL
 */
#define cuListPeek1(l, type) ((type)cuListPeek0(l))

/**
 * Fetch the head of the list
 *
 * @param[in] l the list to handle
 * @return
 * 	\li the head fo the list;
 * 	\li NULL if \c l is empty;
 */
void* cuListGetHead0(CU_NOTNULL const list* l);

/**
 * wrapper of ::_cuListGetHerad allowing you to automatically cast the value peeked
 */
#define cuListGetHead1(l, type) ((type)cuListGetHead0(l))

/**
 * Fetch the tail of the list
 *
 * @param[in] l the list to analyze
 * @return
 * 	\li the tail of the list;
 * 	\li NULL if \c l is empty
 */
void* cuListGetTail0(const list* l);

/**
 * This function allows you to append within a list a new item
 *
 * After this operation the size of list is increased by one.
 *
 * \note
 * This function is incompatible with ::CU_VARIABLE_ITERATE_OVER_LIST .
 *
 *
 * @param[inout] l the list ot update
 * @param[inout] cell the cell after that you will add a new item
 * @param[in] item the item to append
 * @return
 *  the ::list_cell created
 */
list_cell* cuListAddItemAfter(list* l, list_cell* cell, const void* item);

/**
 * same as ::cuListAddItemAfter but we accept an index of the list_cell, instead of the list_cell itself
 *
 * The new item will be set in the position @c index. The element in such position will be translated next.
 *
 * ```
 * 0 1 2 3 4 //indexes
 * a b c d e //values
 * add "X" in position "3"
 * 0 1 2 3 4 5
 * a b c X d e
 * ```
 *
 * @note
 * This operation is time consuming! Complexity is \f$ O(n) \f$
 *
 * @param[inout] l the list to handle
 * @param[in] index the index where we we want to put the item in
 * @param[in] item the item to add
 */
void cuListAddItemAt(CU_NOTNULL list* l, int index, CU_NULLABLE const void* item);

/**
 * Set the i-th element of the list.
 *
 * The previous element is removed from the list but not from memory
 *
 * @param[inout] l the list to update
 * @param[in] index the index of the cell to update
 * @param[in] newItem the new content of the cell with index @c index
 * @return
 *  @li the previous value in the cell with index @c index
 *  @li NULL if the index is not associated to any value in the list
 */
void* cuListSetItemAt(CU_NOTNULL list* l, int index, CU_NULLABLE const void* newItem);

/**
 * Set the i-th element of the list.
 *
 * The previous element is removed from the list **as well from memory**
 *
 * @param[inout] l the list to update
 * @param[in] index the index of the cell to update
 * @param[in] newItem the new content of the cell with index @c index
 */
void cuListSetItemAtWithElement(CU_NOTNULL list* l, int index, CU_NULLABLE const void* newItem);

/**
 * Find an element inside the list
 *
 * The function will retrieve the **first** element satisfying the function \c f
 *
 * \note
 * This operation is O(n)
 *
 * @param[in] l the list under analysis
 * @param[in] f a function that returns true if an element satisfy your condition
 * @param[in] va context of the function \c f
 * @return
 * 	\li the first element satifying your condition;
 * 	\li NULL otherwise;
 */
void* cuListfindItem(CU_NOTNULL const list* l, finder f, CU_NOTNULL const var_args* va);

/**
 * Remove the tail of the list
 *
 * @note
 * This operation is \f$O(n)\f$
 *
 * @param[inout] l the list whose tail we need to pop
 * @return
 *  @li the tail of the list
 *  @li NULL if the list is empty
 */
void* cuListPopTail(CU_NOTNULL list* l);

/**
 * Remove the tail of the list and removes it from the memory
 *
 * The function does nothing if the list is empty.
 *
 * @note
 * This operation is \f$O(n)\f$
 *
 * @param[inout] l the list to manage
 */
void cuListRemoveTailAndDestroyItem(CU_NOTNULL list* l, CU_NULLABLE const struct var_args* context);

/**
 * Remove the element from the list.
 *
 * Nothing is done is the element is not found within the list.
 * Keep in mind that the element is **not** freed from the memory
 *
 * @param[inout] l the list to alter
 * @param[in] f a function used to check if the element has been found
 * @param[in] va the context of function \c f
 * @return
 * 	\li true if the element has been found inside the list;
 * 	\li false otherwise
 */
bool cuListRemoveItem(CU_NOTNULL list* l, finder f, CU_NOTNULL const var_args* va);

/**
 * Remove the element from the list.
 *
 * Nothing is done is the element is not found within the list.
 * **Keep in mind that the element is freed from the memory**
 *
 * @param[inout] l the list to alter
 * @param[in] f a function used to check if the element has been found
 * @param[in] va the context of function \c f
 * @return
 * 	\li true if the element has been found inside the list;
 * 	\li false otherwise
 */
bool cuListRemoveAndDestroyItem(CU_NOTNULL list* l, finder f, CU_NOTNULL const var_args* va);

/**
 * like ::cuListfindItem but it look for an item whose reference is the one specified by \c obj
 *
 * \note
 * Of course this is not powerful like ::cuListfindItem, but it can work in many cases
 * (for example when the payload is inside the cell pointer itself)
 *
 * @param[in] l the list where we need to look at
 * @param[in] obj the reference we're looking for
 * @return
 * 	\li true if the reference \c obj is inside the list;
 * 	\li false otherwise
 */
bool cuListContainsItemByReference(CU_NOTNULL const list* l, CU_NOTNULL const void* obj);

/**
 * like ::cuListRemoveItem, but it removes the object if the reference is the same.
 *
 * \note
 * Of course this is not powerful like ::cuListRemoveItem, but it can work in many cases
 * (for example when the payload is inside the cell pointer itself)
 *
 * If the obj is not found, the function does nothing
 * If multiple instances of \c obj are presents inside the list, only the first is removed.
 *
 * \note
 * No release of the memory occupied by the payload is done
 *
 * @param[inout] l the list to handle
 * @param[in] obj the object to find
 * @return
 * 	\li true if the object has been removed;
 * 	\li false otherwise;
 */
bool cuListRemoveItemByReference(CU_NOTNULL list* l, CU_NOTNULL const void* obj);

/**
 * Get a string representation of the given list
 *
 * @param[in] lst the list to represents
 * @param[inout] buffer the buffer used to store the string representation of the list
 */
int cuListBufferString(CU_NOTNULL const list* lst, CU_NOTNULL char* buffer);

/**
 * Get the n-th element of the list
 *
 * \attention
 * This method is **highly** inefficient and it should be used only for debugging purposes, not in production!
 *
 * The operation take \f$O(n)\f$ time
 *
 * @param[in] list the list to analyze
 * @param[in] index the value in the \c index th position
 * @return
 * 	\li the payload
 * 	\li NULL if the length of \c list is smaller than \c index itself;
 */
void* _cuListGetNthItem(CU_NOTNULL const list* list, int index);

/**
 * Get the n-th element of the list
 *
 * \attention
 * This method is **highly** inefficient and it should be used only for debugging purposes, not in production!
 *
 * The operation take \f$O(n)\f$ time
 *
 * @param[in] list the list to analyze
 * @param[in] index the value in the \c index th position
 * @param[in] type the type used to automatically cast the nth item
 * @return
 * 	\li the payload
 * 	\li NULL if the length of \c list is smaller than \c index itself;
 */
#define cuListGetNthItem(list, index, type) ((type)_cuListGetNthItem(list, index))

/**
 * Removes the  n-th element of the list
 *
 * \attention
 * This method is **highly** inefficient and it should be used only for debugging purposes, not in production!
 *
 * The operation takes \f$O(n)\f$ time
 *
 * @param[inout] lst the list to handle
 * @param[in] index the index of the cell to remove
 * @return
 * 	\li true if the element is removed;
 * 	\li false otherwise;
 */
bool cuListRemoveNthItem(CU_NOTNULL list* lst, int index);

/**
 * like ::cuListRemoveNthItem but it also remove the element from memory as well
 *
 * @param[inout] lst the list to handle
 * @param[in] index the index of the cell to remove
 * @return
 * 	\li true if the element is removed;
 * 	\li false otherwise;
 */
bool cuListRemoveDestroyNthItem(CU_NOTNULL list* lst, int index);

/**
 * Removes the cell \c cellToRemove in the list
 *
 * \attention
 * You should use this function only inside ::CU_VARIABLE_ITERATE_OVER_LIST. Other uses may (and often will) lead to **undefined behaviour**!
 *
 * This function will allow you to dynamically remove elements inside a list when you're iterating it.
 * A typical example might be:
 *
 * \code
 * CU_VARIABLE_ITERATE_OVER_LIST(&list, previous, cell, payload, void*) {
 * 	if (conditionToPayload()) {
 * 		cuListRemoveItem(&list, &previous, cell);
 * 	}
 * }
 * \endcode
 *
 * @param[in] l the list to change
 * @param[inout] previousCell the cell that is positioned just before the one to remove. After this, previousCell pointer will contain NULL
 * @param[in] cellToRemove the cell you want to remove
 *
 */
void cuListRemoveItemInListCell(CU_NOTNULL list* l, CU_NOTNULL list_cell** restrict previousCell, CU_NOTNULL list_cell* restrict cellToRemove);

/**
 * **Alter** the list order by scrambling the elements
 *
 * For example if the list is \f$[1,2,3]\f$ after calling this function the list may have become \f$[3,1,2]\f$
 *
 * @param[inout] l the list to scramble
 */
void cuListScramble(CU_NOTNULL list* l);

/**
 * **Alter** the list order by order the element in a certain way
 *
 * @param[inout] l the list to change
 * @param[in] order the function to use to establish which will be the order to use to reorganize the list itself
 */
void cuListSort(CU_NOTNULL list* l, orderer order);

/**
 * Get a random item in the given list
 *
 * @param[in] l the list where we need to pick from
 * @return
 * 	\li the item retrieved;
 * 	\li NULL if the list is empty;
 */
void* cuListPickRandomItem(CU_NOTNULL const list* l);

/**
 * Store a list inside a file pointer
 *
 * \pre
 * 	\li \c f open in binary mode;
 *
 * @param[inout] f the file that will accept a serialization of the list
 * @param[in] l the list to store
 */
void cuListStoreInFile(CU_NOTNULL FILE* f, CU_NOTNULL const list* l);

/**
 * Load a list from a file
 *
 * \pre
 * 	\li \c f open for reading binary
 *
 * @param[inout] f the file to read from
 * @param[in] deserializer the function to use to deserialize the paylaod of the list
 * @return
 * 	\li the instance of list read;
 * 	\li NULL if we couldn't read the file at all;
 */
list* cuListLoadFromFile(CU_NOTNULL FILE* f, object_deserializer deserializer);

/**
 * Check if the given cell is the last element of the list
 *
 * The function is useful with ::CU_ITERATE_OVER_LIST, for example
 *
 * @code
 * CU_ITERATE_OVER_LIST(l, cell, payload, void*) {
 * 	if (cuListIsLastItem(l, cell)) {
 * 		printf("Last element!\n");
 * 	} else {
 * 		printf("Not last element!\n");
 * 	}
 * }
 * @endcode
 *
 * @param[in] l the list involved
 * @param[in] cell the cell to consider
 * @return
 * 	\li true if this is the last cell in the list;
 * 	\li false otherwise
 */
bool cuListIsLastItem(CU_NOTNULL const list* l, CU_NOTNULL const list_cell* cell);

/**
 * the payload of a list cell
 *
 * @private
 *
 * @param[in] cell the cell to handle
 * @return the associated payload
 */
void* _cuListGetPayloadOfCell(CU_NOTNULL const list_cell* cell);

/**
 * The next list_cell of a given cell
 *
 * @private
 *
 * @param[in] cell the cell to handle
 * @return the cell next to the given one
 */
list_cell* _cuListGetNextOfCell(CU_NOTNULL const list_cell* cell);

/**
 * The head cell of a list
 *
 * @private
 *
 * @param[in] l the list to manage
 * @return the head of the list, as a list_cell
 */
list_cell* _cuListGetHeadCell(CU_NOTNULL const list* l);

/**
 * Allows you to transparently iterate through a list
 *
 * This macro is *slightly* faster than ::CU_VARIABLE_ITERATE_OVER_LIST but it doesn't allow you to
 * safely remove ::list_cell while iterating
 *
 * @param[in] _l a double point to the list you want to iterate through;
 * @param[in] cell name of the variable that will represent the cell under analysis
 * @param[in] _payload name of the variable representing <tt>cell->payload</tt>
 * @param[in] type type of the variable _payload
 */
#define CU_ITERATE_OVER_LIST(_l, cell, _payload, type) \
	for (bool UV(listLoop)=true; UV(listLoop); ) \
		for (const list* UV(l) = (_l); UV(listLoop); ) \
			for(list_cell* cell = _cuListGetHeadCell(UV(l)); UV(listLoop); ) \
				for(list_cell* UV(next) = NULL; UV(listLoop); ) \
					for (type _payload = _cuListGetHeadCell(UV(l)) != NULL ? ((type)_cuListGetPayloadOfCell(_cuListGetHeadCell(UV(l)))) : _payload; UV(listLoop); ) \
						for (UV(next) = ((_cuListGetHeadCell(UV(l)) != NULL) && (_cuListGetNextOfCell(_cuListGetHeadCell(UV(l))) != NULL)) ? _cuListGetNextOfCell(_cuListGetHeadCell(UV(l))) : UV(next); UV(listLoop); UV(listLoop) = false) \
						for ( \
							; \
							cell != NULL \
							; \
							cell = UV(next), \
							UV(next) = (cell != NULL) ? _cuListGetNextOfCell(cell) : NULL, \
							_payload = (type)((cell != NULL) ? _cuListGetPayloadOfCell(cell) : NULL) \
						)

/**
 * Allows you to transparently iterate through a list
 *
 * This macro is *slightly* slower than ::CU_ITERATE_OVER_LIST but allows you to
 * safely remove ::list_cell while iterating.
 *
 * A typical example might be:
 *
 * \code
 * CU_VARIABLE_ITERATE_OVER_LIST(&list, previous, cell, payload, void*) {
 * 	if (conditionToPayload()) {
 * 		cuListRemoveItem(&list, &previous, cell);
 * 	}
 * }
 * \endcode
 *
 * @param[in] _l a double point to the list you want to iterate through;
 * @param[in] previousCell name of the variable that will represent the cell just analyzed.
 * @param[in] cell name of the variable that will represent the cell under analysis
 * @param[in] _payload name of the variable representing <tt>cell->payload</tt>
 * @param[in] type type of the variable _payload
 */
#define CU_VARIABLE_ITERATE_OVER_LIST(_l, previousCell, cell, _payload, type) \
	for (bool UV(listLoop)=true; UV(listLoop); ) \
		for (list* UV(l) = (_l); UV(listLoop); ) \
				for(list_cell *cell = _cuListGetHeadCell(UV(l)), *UV(next) = NULL, *previousCell = NULL, *UV(previousCellTmp)=NULL; UV(listLoop); ) \
					for (type _payload = (_cuListGetHeadCell(UV(l)) != NULL ? ((type)_cuListGetPayloadOfCell(_cuListGetHeadCell(UV(l)))) : _payload); UV(listLoop); ) \
						for (int UV(prevLength) = cuListGetSize(UV(l)); UV(listLoop); ) \
							for (UV(next) = ((_cuListGetHeadCell(UV(l)) != NULL) && (_cuListGetNextOfCell(_cuListGetHeadCell(UV(l))) != NULL)) ? _cuListGetNextOfCell(_cuListGetHeadCell(UV(l))) : UV(next); UV(listLoop); UV(listLoop) = false) \
								for ( \
									; \
									cell != NULL \
									; \
									UV(previousCellTmp) = (UV(prevLength) != cuListGetSize(UV(l))) ? UV(previousCellTmp) : cell, \
									previousCell = UV(previousCellTmp), \
									UV(prevLength) = cuListGetSize(UV(l)), \
									cell = UV(next), \
									UV(next) = (cell != NULL) ? _cuListGetNextOfCell(cell) : NULL, \
									_payload = (type)((cell != NULL) ? _cuListGetPayloadOfCell(cell) : NULL) \
								)


#endif /* LIST_H_ */
