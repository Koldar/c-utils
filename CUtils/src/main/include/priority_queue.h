/**
 * @file
 *
 * Implements a priority queue using a function evaluation on the nodes.
 *
 * Objects with low evaluation are fetched first.
 * The implementation uses an heap to implement the queue.
 *
 * There are some optimization to be aware of:
 *  - fast add insertion: normally when adding item in the queue, you nee dto specify both value to add and its priority with ::cuPriorityQueueAddItem .
 *  	However, you can firstly call ::cuPriorityQueueEnableFastAddOperation to set a default evaluation function and call ::cuPriorityQueueAddItem1 to
 *  	automatically generate the priority level;
 *  - fast contain operation: during contains, we look in the whole head for the requested data. However, if you call ::cuPriorityQueueEnableFastContainOperation
 *  	you will be able to inject a function that we will call instead. You should use this function to store the ::priority_queue_cell storing your data
 *
 *
 *
 *
 * @date Aug 2, 2017
 * @author koldar
 */

#ifndef PRIORITY_QUEUE_H_
#define PRIORITY_QUEUE_H_

#include <stdbool.h>
#include "typedefs.h"
#include "var_args.h"
#include "payload_functions.h"
#include "list.h"


typedef struct priority_queue priority_queue;

struct priority_queue_cell;

/**
 * an ad-hoc function type which will be executed whenever somewhere ask "is item X inside queue Q?"
 *
 * @param[in] q the involved queue
 * @param[in] data the data whose containement we need to check
 * @return
 *  @li the pointer of the memory where cell representing containin the element is referenced in the heap if @c data is in queue @c q
 *  @li NULL if @c data is not in the queue @c q
 */
typedef struct priority_queue_cell* (*queue_findItem)(CU_NOTNULL const priority_queue* q, CU_NULLABLE const void* data);
#define CU_QUEUE_FINDITEM_ID 100
/**
 * Convert a finder function toathe correct finder signature
 */
#define CU_QUEUE_AS_FINDITEM(...) ((CU_ENSURE_FUNCTION_POINTER(CU_QUEUE_FINDITEM_ID, queue_findItem, priority_queue_cell, __VA_ARGS__, CU_FT_CONST_PTR(priority_queue), CU_FT_CONST_PTR(void))))

/**
 * an ad-hoc function type which will be executed whenever someone adds data inside a queue
 *
 * @param[in] q the queue which has a new item
 * @param[in] data the data just added
 * @param[in] priority a pointer representing the place where the queue records the priority of the data just added
 */
typedef void (*queue_addItem)(CU_NOTNULL priority_queue* q, CU_NULLABLE const void* data, struct priority_queue_cell* qc);
#define CU_QUEUE_ADDITEM_ID 101
/**
 * Convert a add function to the correct finder signature
 */
#define CU_QUEUE_AS_ADDITEM(...) ((CU_ENSURE_FUNCTION_POINTER(CU_QUEUE_ADDITEM_ID, queue_addItem, void, __VA_ARGS__, CU_FT_PTR(priority_queue), CU_FT_PTR(void), CU_FT_PTR(priority_queue_cell))))

/**
 * initialize an empty queue without any optimizations
 *
 * @param[in] p a structure containing several methods used to handle the payload of the queue
 * @return an instance of the queue
 */
CU_NOTNULL priority_queue* cuPriorityQueueNew(payload_functions p);

/**
 * Destroy the queue. The items in the queue won't be released
 *
 * @param[in] q the queue to deallocate
 */
void cuPriorityQueueDestroy(CU_NOTNULL const priority_queue* q, CU_NULLABLE const struct var_args* context);
#define CU_FUNCTION_POINTER_destructor_void_cuQueueDestroy_voidConstPtr_var_argsConstPtr CU_DESTRUCTOR_ID

/**
 * Set the contains operation specific for this queue
 *
 * @post
 *  @li contains optimaztion enabled
 *
 * When enabled, contains operation of an item inside the queue will **just** call the given function.
 * This is essential when you wish, for example, that information about containment in a queue is not stored in the queue itself,
 * but in the data (for example for faster contains checking).
 * Furthermore, we will call @c op2 as soon as someone adds a data inside the queue
 *
 * Assume for example that you want to fast check if a specific queue contains a specific item. Instead of searching the whole queue
 * you might want to setup the data inside the queue with a `bool isInQueue` field. cThen @c operation will just check such flag
 *
 * @param[in] q the queue involved
 * @param[in] op1 contain operation
 * @param[in] op2 add operation
 */
void cuPriorityQueueEnableFastContainOperation(CU_NOTNULL priority_queue* q, queue_findItem op1, queue_addItem op2);

/**
 * Set the evaluator operation specific for this queue
 *
 * @post
 *  @li add optimization evaluator function
 *
 * When enabled you can use the function ::cuPriorityQueueAddItem1
 *
 * @param[in] q the queue involved
 * @param[in] operation
 */
void cuPriorityQueueEnableFastAddOperation(CU_NOTNULL priority_queue* q, evaluator_function operation);



/**
 * Adds an item in the queue.
 *
 * @post
 *  @li The item with the lowest score in the queue will be placed on top
 *
 * @param[inout] q the queue involved
 * @param[in] data a new payload to add in the queue
 * @param[in] dataPriority a vlaue used to sort the data withi nthe queue. small @c dataPriority means that @c data
 * 	is likely to be chosen first
 */
void cuPriorityQueueAddItem(CU_NOTNULL priority_queue* q, CU_NULLABLE const void* data, long dataPriority);

/**
 * Adds an item in the queue.
 *
 * @pre
 *  @li ::cuQueueSetEvaluator needs to be called
 *
 * @param[inout] q the queue involved
 * @param[in] data a new payload to add in the queue
 * @param[in] context context to evaluator function
 */
void cuPriorityQueueAddItem1(CU_NOTNULL priority_queue* q, CU_NULLABLE const void* data, CU_NULLABLE const struct var_args* context);

/**
 * Peek the item with the lowest score in the queue
 *
 * @param[in] q the queue involved
 * @return
 *  @li the data with the lowest score in the queue
 *  @li NULL if the queue is empty
 */
CU_NULLABLE void* _cuPriorityQueuePeekItem(CU_NOTNULL const priority_queue* q);

/**
 * Pop from the queue the item with the lowest score
 *
 * @post
 *  @li @c q is one item smaller
 *
 * @param[inout] q the queue involved
 * @return
 *  @li the data with the lowest score in the queue
 *  @li NULL if @c q is empty
 */
CU_NULLABLE void* _cuPriorityQueuePopItem(CU_NOTNULL priority_queue* q);

/**
 * the number of elements in the queue
 *
 * @param[in] q the queue involved
 * @return the number of items inside the queue
 */
int cuPriorityQueueGetSize(CU_NOTNULL const priority_queue* q);

/**
 * check if the queue is empty
 *
 * @param[in] q the queue involved
 * @return
 *  @li true if the queue has no items;
 *  @li false otherwise;
 */
bool cuPriorityQueueIsEmpty(CU_NOTNULL const priority_queue* q);

/**
 * Remove from the queue (but not from memory) the items
 *
 * @param[inout] q the queue to clear
 */
void cuPriorityQueueClear(CU_NOTNULL priority_queue* q);

/**
 * Remove from the queue **and* from memory the items
 *
 * @param[inout] q the queue to clear
 */
void cuPriorityQueueClearWithElements(CU_NOTNULL priority_queue* q, CU_NULLABLE const struct var_args* context);

/**
 * Remove from the queue **as well as from memory** the items
 *
 * @param[inout] q the queue to clear
 */
void cuPriorityQueueDestroyWithElements(CU_NOTNULL priority_queue* q, CU_NULLABLE const struct var_args* context);
#define CU_FUNCTION_POINTER_destructor_void_cuQueueDestroyWithElements_voidConstPtr_var_argsConstPtr CU_DESTRUCTOR_ID

/**
 * Find an element in the queue
 *
 * \note
 * the operation is $O(n)$.
 *
 * The function **won't exploit** any score inside the queue
 *
 * @param[in] q the queue to analyze
 * @param[in] f a function allowing you to decide if you've found what tou need
 * @param[in] va variadic argument for the function f
 * @return
 * 	\li the **first** item satisfiying the criterion \c f;
 * 	\li NULL otherwise
 */
void* cuPriorityQueueFindItem(CU_NOTNULL const priority_queue* q, finder f, CU_NULLABLE const var_args* va);

/**
 * Checks if the queue contains an item
 *
 * @note
 * This function will exploit the scores of the queue. For example, if @c data has a score that is less than the
 * minimum no search will ever be started since no items can have a score less than the top ones. This will return false
 * even if @c data is indeed stored in the queue but has a different evaluation score.
 *
 * The function is faster than ::cuPriorityQueueContainsItem1
 *
 * if ::cuQueueSetFindOperation has been called, the quicker version of containment will be invoked instead
 *
 *
 * @param[in] q the queue to analyze
 * @param[in] data the data to check
 * @return
 *  @li true if we are able to find @c data in the queue
 *  @li false otherwise
 */
bool cuPriorityQueueContainsItem0(CU_NOTNULL const priority_queue* q, CU_NULLABLE void* data, long priority);

/**
 * Check if the queue contains an item
 *
 * @note
 * This function is **much slower** than ::cuPriorityQueueContainsItem0 when :cuQueueSetFindOperation has not been called.
 * The function will return sound results no matter the priority
 *
 * if ::cuQueueSetFindOperation has been called, the quicker version of containment will be invoked instead
 *
 * @param[in] q the queue involved
 * @param[in] data the data we need to check
 * @return
 *  @li @true if @c data is stored in the queue;
 *  @li @false otherwise
 */
bool cuPriorityQueueContainsItem1(CU_NOTNULL const priority_queue* q, CU_NULLABLE void* data);


/**
 * update the priority of the given data
 *
 * @pre
 *  @li ::cuPriorityQueueNew initialized with @c enableUpdatePriorityOperation set to @true
 *
 * @param[inout] q the involved
 * @param[in] data the data whose priority needs to be updated
 * @param[in] newPriority a number which represents the new priority @c data has
 */
int cuPriorityQueueChangePriority(CU_NOTNULL priority_queue* q, CU_NULLABLE const void* data, long newPriority);

/**
 * retrieve the item inside the queue and removes it
 *
 * @pre
 *  @li ::cuPriorityQueueNew initialized with @c enableUpdatePriorityOperation set to @true
 *  @li queue not empty and containing @c data
 *
 * @param[inout] q the queue involved
 * @param[in] data the element in the queue that needs to be removed
 */
void cuPriorityQueueRemoveItem(CU_NOTNULL priority_queue* q, CU_NULLABLE const void* data);

/**
 * @param[in] qc the cell containing a data
 * @return the priority of this queue cell
 */
int cuPriorityQueueCellGetPriority(CU_NOTNULL const struct priority_queue_cell* qc);

/**
 * generate on the heap a list from queue
 *
 * @note
 * Destroying the list is responsible of the caller!
 *
 * The list won't be sorted by priority
 *
 * @param[in] q the queue to involve
 * @return a list on the heap
 */
list* cuPriorityQueueToList(CU_NOTNULL const priority_queue* q);

/**
 * clone the queue. Elements in the container is copied by reference
 *
 * @param[in] q the queue to clone
 * @param[in] context context to pass to clone function
 * @return the cloned queue on the heap
 */
priority_queue* cuPriorityQueueClone(CU_NOTNULL const priority_queue* q, CU_NULLABLE const struct var_args* context);

/**
 * clone the queue. Elements in the container is copied by function payload
 *
 * @param[in] q the queue to clone
 * @param[in] context context to pass to clone function
 * @return the cloned queue on the heap
 */
priority_queue* cuPriorityQueueCloneWithElements(CU_NOTNULL const priority_queue* q, CU_NULLABLE const struct var_args* context);

/**
 * Generate a png representing the queue
 *
 * @param[in] q the queue involved
 * @param[in] filename name of the filename to generate (no extension required!)
 * @param[in] ... format parameters of @c template
 */
void cuPriorityQueueSavePNG(CU_NOTNULL const priority_queue* q, const char* template, ...);

/**
 * @param[in] q the queue involved
 * @return
 *  @li the operation used to quickly check if an itemc is inside the queue
 *  @li NULL if ::cuPriorityQueueEnableFastContainOperation has not been called yet
 */
CU_NULLABLE queue_findItem cuPriorityQueueGetFindItemOperation(CU_NOTNULL const priority_queue* q);

/**
 * @param[in] q the queue involved
 * @return
 *  @li the operation used to listen for adding an item is inside the queue
 *  @li NULL if ::cuPriorityQueueEnableFastContainOperation has not been called yet
 */
CU_NULLABLE queue_addItem cuPriorityQueueGetAddItemOperation(CU_NOTNULL const priority_queue* q);

/**
 * fetch the item with lowest priority in the queue without removing it from the queue
 *
 * @param[in] q the queue involved
 * @param[in] type of the value the queue returns
 * @return the data with lowest queue, casted to @c type
 */
#define cuPriorityQueuePeekItem(q, type) ((type)_cuPriorityQueuePeekItem(q))

/**
 * fetch the item with lowest priority in the queue removing it from the queue
 *
 * @param[in] q the queue involved
 * @param[in] type of the value the queue returns
 * @return the data with lowest queue, casted to @c type
 */
#define cuPriorityQueuePopItem(q, type) ((type)_cuPriorityQueuePopItem(q))

#endif /* PRIORITY_QUEUE_H_ */
