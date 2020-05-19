/**
 * @file
 *
 * Represents a priority queue implemented via a bidirectional list
 *
 * It's access time is slow, but you can easily fetch data from whenever in the queue
 *
 *
 * @author koldar
 * @date Mar 30, 2018
 */

#ifndef NAIVE_QUEUE_H_
#define NAIVE_QUEUE_H_

#include <stdbool.h>
#include "list.h"
#include "typedefs.h"
#include "payload_functions.h"
#include "var_args.h"

typedef struct naive_queue naive_queue;

/**
 * A queue containing integers
 */
typedef naive_queue int_naive_queue;

typedef struct naive_queue_cell naive_queue_cell;

/**
 * Initialie a new naive queue
 *
 * @param[in] functions a set of functions allowing you to easily interact with the payload
 * @param[in] f an evaluation function used to set the priority of each element in the queu
 * @param[in] va context of f function. May be NULL
 * @return a new naive queue initialized in the heap;
 */
naive_queue* cuNaiveQueueNew(payload_functions functions, evaluator_function f, CU_NULLABLE const var_args* va);

/**
 * destroy the naive queue from the memory
 *
 * \note
 * the items within the queue won't be touched
 *
 * @param[in] nq the queu to dispose of
 */
void cuNaiveQueueDestroy(CU_NOTNULL const naive_queue* nq, CU_NULLABLE const struct var_args* context);
#define CU_FUNCTION_POINTER_destructor_void_cuNaiveQueueDestroy_voidConstPtr_var_argsConstPtr CU_DESTRUCTOR_ID

/**
 * destroy the naive queue from the memory **and* its elements
 *
 * \note
 * the items within the queue won't be touched
 *
 * @param[in] nq the queue to dispose of
 */
void cuNaiveQueueDestroyWithElements(CU_NOTNULL const naive_queue* nq, CU_NULLABLE const struct var_args* context);

/**
 * Add a new item within the queue
 *
 * \note
 * items are automatically ordered bu a ::evaluator_function
 *
 * @param[inout] q the queue to update
 * @param[in] el the element to add
 */
void cuNaiveQueueAddItem(CU_NOTNULL naive_queue* q, CU_NULLABLE const void* el);

/**
 * peek the first item of the queue
 *
 * @private
 *
 * @param[in] q the queue involved
 * @return the first item of the queue
 */
void* _cuNaiveQueuePeekItem(CU_NOTNULL const naive_queue* q);

/**
 * get **and** removes from the queue the first item of the queue
 *
 * @private
 *
 * @param[inout] q the queue involved
 * @return the first item of the queue
 */
void* _cuNaiveQueuePopItem(CU_NOTNULL naive_queue* q);

/**
 * Look for an item inside the queue
 *
 * @param[in] q the queue where we need to look for
 * @param[in] f a function returning true if a given element is what we were looking for
 * @param[in] va additional variables that will be transparently injected into @c f function .Used to give @c f a context
 * @return
 *  @li the first element where @c f returns true
 *  @li NULL if no such element could be found.
 */
void* cuNaiveQueueFindItem(CU_NOTNULL const naive_queue* q, finder f, CU_NULLABLE const var_args* va);

/**
 * finds a naive queue cell satifsying the given contraint.
 *
 * If there are multiple cell satisfying the contraint, only the first s fetched
 *
 * @note
 * this operation is \f$ O(n)\f$
 *
 * @param[in] q the queue to manage
 * @param[inout] payloadOutput pointer of allocated <tt>void\*</tt> where to put the pointer for the payload (if criterion is satisfied). NULL if not.
 * @param[in] f the contraint to satisfy
 * @param[in] va an optional context to give to the @c f function
 * @return
 * @li the element requested;
 * @li NULL if no element satisfy the criterion
 */
naive_queue_cell* cuNaiveQueueFindCellItem(CU_NOTNULL const naive_queue* q, CU_NOTNULL void** payloadOutput, finder f, CU_NULLABLE const var_args* va);

/**
 * Removes a cell previously fetched via (for example) ::cuNaiveQueueFindCellItem
 *
 * @note
 * this operation won't free the actual payload of the queue cell
 *
 * @param[inout] q the queue to manage
 * @param[in] queueCell the cell to remove from memory
 */
void cuNaiveQueueRemoveCell(CU_NOTNULL naive_queue* q, const naive_queue_cell* queueCell);

/**
 * like ::cuNaiveQueueRemoveCell but it will remove the paylaod of the given queue cell as well
 *
 * @param[inout] q the queue to manage
 * @param[in] queueCell the cell to remove from memory
 */
void cuNaiveQueueRemoveCellWithElements(CU_NOTNULL naive_queue* q, const naive_queue_cell* queueCell);

/**
 * Remove the first item satisfying the finder
 *
 * \note
 * the element will be removed, but not from memory
 *
 * @param[inout] q the queue to alter
 * @param[in] f a function returning true if the delete criterion  is matched
 * @param[in] va context of function \c f
 * @return
 * 	\li the element remove
 * 	\li NULL otherwise
 */
void* cuNaiveQueueRemoveItem(CU_NOTNULL naive_queue* q, finder f, CU_NULLABLE const var_args* va);

/**
 * Remove the first item (also from memory) satisfying the finder
 *
 *
 * @param[inout] q the queue to alter
 * @param[in] f a function returning true if the delete criterion  is matched
 * @param[in] va context of function \c f
 * @return
 * 	\li true if we have removed something;
 * 	\li false otherwise
 */
bool cuNaiveQueueRemoveItemWithElements(CU_NOTNULL naive_queue* q, finder f, CU_NULLABLE const var_args* va);

/**
 * Check if the queue is empty
 *
 * @param[in] q the queue to check
 * @return
 * 	\li true if the queue is empty;
 * 	\li false otherwise;
 */
bool cuNaiveQueueIsEmpty(CU_NOTNULL const naive_queue* q);

/**
 * get the number of element within the queue
 *
 * @param[in] q the queue to check
 * @return the number fo elements within the queue
 */
int cuNaiveQueueGetSize(CU_NOTNULL const naive_queue* q);

/**
 * Clear all the elements withint the queue
 *
 * @param[inout] q the queue to flush
 */
void cuNaiveQueueClear(CU_NOTNULL naive_queue* q);

/**
 * Clear all the elements withint the queue
 *
 * \note
 * this function will delete from the memory the payload as well!
 *
 * @param[inout] q the queue to flush
 */
void cuNaiveQueueClearWithElements(CU_NOTNULL naive_queue* q);

/**
 * Geneerate a string representation of the naive_queue
 *
 * @param[in] q the queue to print
 * @param[inout] buffer an already initialzied buffer where we print @c q
 * @return number of bytes filled in the buffer
 */
int cuNaiveQueueBufferStringer(CU_NOTNULL const naive_queue* q, CU_NOTNULL char* buffer);

/**
 * get the head of the queue
 *
 * @code
 * q = cuNaiveQueueNew(...);
 * struct point* p = cuNaiveQueuePeekItem(q, struct point*);
 * @endcode
 *
 * @param[in] q the queue whose head we need to peek
 * @param[in] type the type of the returned value of this function
 * @return the head of the queue
 */
#define cuNaiveQueuePeekItem(q, type) ((type)(_cuNaiveQueuePeekItem(q)))

/**
 * get the head of the queue and removes it from the queue
 *
 * @code
 * q = cuNaiveQueueNew(...);
 * struct point* p = cuNaiveQueuePopItem(q, struct point*);
 * @endcode
 *
 * After this function, the queue will be 1 element shorter
 *
 * @param[inout] q the queue whose head we need to peek
 * @param[in] type the type of the returned value of this function
 * @return the head of the queue
 */
#define cuNaiveQueuePopItem(q, type) ((type)(_cuNaiveQueuePopItem(q)))

struct naive_queue_cell;

/**
 * the first item of the queue
 *
 * @param[in] nq the queue involved
 * @return the head of the queue
 */
struct naive_queue_cell* _cuNaiveQueueGetHead(CU_NOTNULL const naive_queue* nq);

/**
 * get the evaluation of the paylaod inside a naive_queue_cell
 *
 * @private
 *
 * @param[in] nqc the cell invoved
 * @return the evaluation of the content of @c nqc
 */
int _cuNaiveQueueGetEvaluationOfCell(CU_NOTNULL const struct naive_queue_cell* nqc);

/**
 * get the next cell of a naive_queue_cell
 *
 * @private
 *
 * @param[in] nqc the cell invoved
 * @return the cell occuring just after @c nqc
 */
struct naive_queue_cell* _cuNaiveQueueGetNextOfCell(CU_NOTNULL const struct naive_queue_cell* nqc);

/**
 * get the paylaod of a naive_queue_cell
 *
 * @private
 *
 * @param[in] nqc the cell invoved
 * @return the paylaod associated to the cell
 */
void* _cuNaiveQueueGetPayloadOfCell(CU_NOTNULL const struct naive_queue_cell* nqc);

/**
 * Adds the entire given list inside queue
 *
 * items are copied by **reference**. The list @c l won't be touched
 *
 * @param[inout] queue the queue involved
 * @param[in] l the list involved
 */
void cuNaiveQueueAddContainer(CU_NOTNULL naive_queue* queue, CU_NOTNULL const list* l);

/**
 * like ::cuNaiveQueueAddContainer but this function will remove the elements from the list
 *
 * @post
 *  @li @c l empty
 *
 * @param[inout] queue the queue involved
 * @param[inout] l the list involved
 */
void cuNaiveQueueMoveContentsInContainer(CU_NOTNULL naive_queue* queue, CU_NOTNULL list* l);

#define CU_ITERATE_OVER_NAIVE_QUEUE(q, i, eval, v, type) \
	for (bool UV(count)=true; UV(count); ) \
		for (type v; UV(count); ) \
			for(int i=0, eval=0; UV(count); UV(count)=false) \
			for(struct naive_queue_cell* UV(queueCell) = _cuNaiveQueueGetHead(q); UV(queueCell) != NULL; ) \
			for ( \
				i = 0, \
				v = (type) _cuNaiveQueueGetPayloadOfCell(UV(queueCell)), \
				eval = _cuNaiveQueueGetEvaluationOfCell(UV(queueCell)); \
				\
				UV(queueCell) != NULL; \
				\
				UV(queueCell) = _cuNaiveQueueGetNextOfCell(UV(queueCell)), \
				i += 1, \
				eval = UV(queueCell) != NULL ? _cuNaiveQueueGetEvaluationOfCell(UV(queueCell)) : 0, \
				v = (type) (UV(queueCell) != NULL ? _cuNaiveQueueGetPayloadOfCell(UV(queueCell)) : NULL) \
		)


#endif /* NAIVE_QUEUE_H_ */
