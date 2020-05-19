/*
 * queue.c
 *
 *  Created on: Aug 2, 2017
 *      Author: koldar
 */


#include "priority_queue.h"
#include "macros.h"
#include "log.h"
#include <stdlib.h>
#include <math.h>
#include "errors.h"
#include "file_utils.h"
#include "utility.h"

// a queue is implemented as an infinite heap

struct priority_queue_cell {
	void* payload;
	int priority;

	struct priority_queue_cell* parent;
	struct priority_queue_cell* left;
	struct priority_queue_cell* right;
};


struct priority_queue {
	struct priority_queue_cell* min;
	int size;
	payload_functions functions;
	int nextCellAvailable;
	CU_NULLABLE queue_findItem findItemImplementation;
	CU_NULLABLE queue_addItem addItemImplementation;
	CU_NULLABLE evaluator_function evaluateItemImplementation;
};

static CU_NOTNULL struct priority_queue_cell* newQueueCell();
static void destroyQueueCell(CU_NOTNULL const struct priority_queue_cell* qc, CU_NULLABLE const struct var_args* context);
static void destroyQueueCellWithElement(CU_NOTNULL const struct priority_queue_cell* qc, destructor d);
static int evaluateQueueCell(CU_NOTNULL const priority_queue* q, CU_NOTNULL const struct priority_queue_cell* qc);
static bool isParent(CU_NOTNULL const priority_queue* q, CU_NOTNULL const struct priority_queue_cell* qc);
static bool isLeaf(CU_NOTNULL const struct priority_queue_cell* qc);
static void percolateUp(CU_NOTNULL priority_queue* q, CU_NOTNULL struct priority_queue_cell* qc);
static void percolateDown(CU_NOTNULL priority_queue* q, CU_NOTNULL struct priority_queue_cell* qc);
static void gotoCellWithId(CU_NOTNULL const priority_queue* q, int idToRead, CU_NOTNULL struct priority_queue_cell** pointerOfCellReached, CU_NOTNULL struct priority_queue_cell** pointerOfParentCell, CU_NOTNULL struct priority_queue_cell*** pointerOfParentReachingNode);
static void _clearQueue(CU_NOTNULL const priority_queue* q, CU_NULLABLE const struct priority_queue_cell* qc, bool destroyPayload, const struct var_args* context);
static bool containsItemInQueueRecursive0(CU_NOTNULL const priority_queue* q, CU_NULLABLE const struct priority_queue_cell* qc, int evalData, CU_NULLABLE void* data);
static bool containsItemInQueueRecursive1(CU_NOTNULL const priority_queue* q, CU_NULLABLE const struct priority_queue_cell* qc, CU_NULLABLE void* data);
static CU_NULLABLE void* findItemInQueueRecursive(CU_NOTNULL const priority_queue* q, CU_NOTNULL const struct priority_queue_cell* qc, finder f, CU_NULLABLE const var_args* va);
static CU_NULLABLE struct priority_queue_cell** getQueueCellPtr(CU_NOTNULL const struct priority_queue_cell* qc);
static void swapQueueCells(CU_NOTNULL priority_queue* q, CU_NOTNULL struct priority_queue_cell* child, CU_NOTNULL struct priority_queue_cell* parent);
static void _cuQueueSavePNG_Nodes(FILE* fout, CU_NOTNULL const priority_queue* q, CU_NOTNULL const struct priority_queue_cell* qc);
static void _cuQueueSavePNG_Edges(FILE* fout, CU_NOTNULL const priority_queue* q, CU_NOTNULL const struct priority_queue_cell* qc);
static void _cuQueueCloneWithElements(CU_NOTNULL const priority_queue* q, CU_NULLABLE const struct priority_queue_cell* qc, CU_NOTNULL priority_queue* result, CU_NULLABLE cloner c);
static void _cuQueueToList(CU_NOTNULL const priority_queue* q, CU_NULLABLE const struct priority_queue_cell* qc, list* l);

static int globalIncrement = 0;

CU_NOTNULL priority_queue* cuPriorityQueueNew(payload_functions p) {
	priority_queue* result = CU_MALLOC(priority_queue);
	if (result == NULL) {
		ERROR_MALLOC();
	}

	result->functions = p;
	result->min = NULL;
	result->size = 0;
	result->nextCellAvailable = 1;

	result->findItemImplementation = NULL;
	result->addItemImplementation = NULL;
	result->evaluateItemImplementation = NULL;

	return result;
}

void cuPriorityQueueDestroy(CU_NOTNULL const priority_queue* q, CU_NULLABLE const struct var_args* context) {
	_clearQueue(q, q->min, false, context);
	CU_FREE(q);
}

void cuPriorityQueueEnableFastContainOperation(CU_NOTNULL priority_queue* q, queue_findItem op1, queue_addItem op2) {
	q->addItemImplementation = op2;
	q->findItemImplementation = op1;
}

void cuPriorityQueueEnableFastAddOperation(CU_NOTNULL priority_queue* q, evaluator_function operation) {
	q->evaluateItemImplementation = operation;
}

void cuPriorityQueueAddItem(CU_NOTNULL priority_queue* q, CU_NULLABLE const void* data, long dataPriority) {
	//I need to position the new payload on the last leaf

	//level is the power of 2 of every node in the level
	//i.e level=3 --> ids of nodes will be 8,9,10,11,12,13,14,15
	//right-most node will be 2^n-1
	//left-most node will be 2^n
	//at level i, all the nodes in the left branch has the i-th bit (counting from the right) set to 0
	//at level i, all the nodes in the right branch has the i-th bit (counting from the right) set to 1
	int level = 0;
	int qcId = 1;
	struct priority_queue_cell* qc = NULL;
	struct priority_queue_cell* parent = NULL;
	struct priority_queue_cell** parentPointer = NULL;

	//we do that now because gotoCellWithId needs that
	q->size += 1;

	gotoCellWithId(q, q->nextCellAvailable, &qc, &parent, &parentPointer);

#ifdef DEBUG
	if (qc != NULL) {
		ERROR_IS_ALREADY_PRESENT(qc, "heap", "%p");
	}
#endif

	qc = newQueueCell();
	*parentPointer = qc;

	qc->payload = (void*)data;
	if (cuPriorityQueueIsEmpty(q)) {
		qc->parent = NULL;
	} else {
 		qc->parent = parent;
	}
	qc->priority = dataPriority;

	q->nextCellAvailable += 1;

	percolateUp(q, qc);

	if (q->addItemImplementation != NULL) {
		q->addItemImplementation(q, data, qc);
	}
}

void cuPriorityQueueAddItem1(CU_NOTNULL priority_queue* q, CU_NULLABLE const void* data, CU_NULLABLE const struct var_args* context) {
	cuPriorityQueueAddItem(q, data, q->evaluateItemImplementation(data, context));
}

CU_NULLABLE void* _cuPriorityQueuePeekItem(CU_NOTNULL const priority_queue* q) {
	return q->min != NULL ? q->min->payload : NULL;
}

CU_NULLABLE void* _cuPriorityQueuePopItem(CU_NOTNULL priority_queue* q) {
	void* retVal = NULL;
	struct priority_queue_cell* min = NULL;

	//queue is empty. Return nothing
	if (cuPriorityQueueIsEmpty(q)) {
		ERROR_OBJECT_IS_EMPTY("queue", "%p", q);
	}

	q->size -= 1;
	q->nextCellAvailable -= 1;
	min = q->min;
	retVal = q->min->payload;

	//queue has one element, return such element
	//remember: we have just remove size by 1!
	if (q->size == 0) {
		destroyQueueCell(q->min, NULL); //TODO context null
		q->min = NULL;
		return retVal;
	}
	//we have at least 2 values inside the queue

	struct priority_queue_cell* qc;
	struct priority_queue_cell* parent;
	struct priority_queue_cell** pointerOfParent;

	gotoCellWithId(q, q->nextCellAvailable, &qc, &parent, &pointerOfParent);
	struct priority_queue_cell* oldMin = q->min;

	swapQueueCells(q, q->min, qc);
	//SWAP(q->min->payload, qc->payload, void*);

	//TODO remove
	//min still points to the top of the heap and qc to the bottom. We need to remove the bottom
	//destroy the cell of the min (not used anymore)
	if (oldMin->parent->left == oldMin) {
		oldMin->parent->left = NULL;
	} else {
		oldMin->parent->right = NULL;
	}
	destroyQueueCell(oldMin, NULL); //TODO context null

	percolateDown(q, q->min);

	return retVal;
}

int cuPriorityQueueGetSize(CU_NOTNULL const priority_queue* q) {
	return q->size;
}

bool cuPriorityQueueIsEmpty(CU_NOTNULL const priority_queue* q) {
	return q->size == 0;
}

void cuPriorityQueueClear(CU_NOTNULL priority_queue* q) {
	_clearQueue(q, q->min, false, NULL);
	q->size = 0;
	q->nextCellAvailable = 1;
}

void cuPriorityQueueClearWithElements(CU_NOTNULL priority_queue* q, CU_NULLABLE const struct var_args* context) {
	_clearQueue(q, q->min, true, context);
	q->size = 0;
	q->nextCellAvailable = 1;
}

void cuPriorityQueueDestroyWithElements(CU_NOTNULL priority_queue* q, CU_NULLABLE const struct var_args* context) {
	_clearQueue(q, q->min, true, context);
	free((void*)q);
}

CU_NULLABLE void* cuPriorityQueueFindItem(CU_NOTNULL const priority_queue* q, finder f, CU_NULLABLE const var_args* va) {
	return findItemInQueueRecursive(q, q->min, f, va);
}

static CU_NULLABLE void* findItemInQueueRecursive(CU_NOTNULL const priority_queue* q, CU_NOTNULL const struct priority_queue_cell* qc, finder f, CU_NULLABLE const var_args* va) {
	if (qc == NULL) {
		return NULL;
	}
	if (f(qc->payload, va) == true) {
		return qc->payload;
	}

	void* retVal = NULL;
	retVal = findItemInQueueRecursive(q, qc->left, f, va);
	if (retVal != NULL) {
		return retVal;
	}
	retVal = findItemInQueueRecursive(q, qc->right, f, va);
	if (retVal != NULL) {
		return retVal;
	}
	return NULL;
}


bool cuPriorityQueueContainsItem1(CU_NOTNULL const priority_queue* q, CU_NULLABLE void* data) {
	//we can always use this function, although it can be quite slow
	if (q->findItemImplementation != NULL) {
		return q->findItemImplementation(q, data) != NULL;
	} else {
		return containsItemInQueueRecursive1(q, q->min, data);
	}
}

static bool containsItemInQueueRecursive1(CU_NOTNULL const priority_queue* q, CU_NULLABLE const struct priority_queue_cell* qc, CU_NULLABLE void* data) {
	if (qc == NULL) {
		return false;
	}
	//TODO this function should be a comparator, not an orderer
	if (q->functions.compare(qc->payload, data) == 0) {
		return true;
	}

	if (qc->left != NULL) {
		if (containsItemInQueueRecursive1(q, qc->left, data)) {
			return true;
		}
	}

	if (qc->right != NULL) {
		if (containsItemInQueueRecursive1(q, qc->right, data)) {
			return true;
		}
	}

	return false;
}

bool cuPriorityQueueContainsItem0(CU_NOTNULL const priority_queue* q, CU_NULLABLE void* data, long priority) {
	if (q->findItemImplementation) {
		return q->findItemImplementation(q, data) != NULL;
	} else {
		return containsItemInQueueRecursive0(q, q->min, priority, data);
	}
}

int cuPriorityQueueChangePriority(CU_NOTNULL priority_queue* q, CU_NULLABLE const void* data, long newPriority) {
	if (q->findItemImplementation == NULL) {
		ERROR_IMPOSSIBLE_SCENARIO("you need to enable fast containment operatoin in order to use %s operation", __func__);
	}
	struct priority_queue_cell* qc = q->findItemImplementation(q, data);
	int oldPriority = qc->priority;
	qc->priority = newPriority;

	if (oldPriority == newPriority) {
		return oldPriority;
	} else if (oldPriority > newPriority) {
		//the data has a lower priority. Since this is a minheap the data may go towards the root
		percolateUp(q, qc);
	} else {
		CU_REQUIRE_LT(oldPriority, newPriority);
		//the data has greater priority. Sijnce this is a minheap the data may go towards the leaves
		percolateDown(q, qc);
	}

	return oldPriority;
}

void cuPriorityQueueRemoveItem(CU_NOTNULL priority_queue* q, CU_NULLABLE const void* data) {
	if (q->findItemImplementation == NULL) {
		ERROR_IMPOSSIBLE_SCENARIO("you need to enable fast containment operatoin in order to use %s operation", __func__);
	}

	if (q->min == NULL) {
		ERROR_IMPOSSIBLE_SCENARIO("the queue is empty!");
	}

	int bestPriority = q->min->priority;
	//change the given data to be on the head of the queue
	cuPriorityQueueChangePriority(q, data, bestPriority - 1);
	//pop item
	cuPriorityQueuePopItem(q, void*);
}

int cuPriorityQueueCellGetPriority(CU_NOTNULL const struct priority_queue_cell* qc) {
	return qc->priority;
}

list* cuPriorityQueueToList(CU_NOTNULL const priority_queue* q) {
	list* result = cuListNew(q->functions);

	_cuQueueToList(q, q->min, result);

	return result;
}


priority_queue* cuPriorityQueueClone(CU_NOTNULL const priority_queue* q, CU_NULLABLE const struct var_args* context) {
	priority_queue* result = cuPriorityQueueNew(q->functions);
	_cuQueueCloneWithElements(q, q->min, result, NULL);
	return result;
}

priority_queue* cuPriorityQueueCloneWithElements(CU_NOTNULL const priority_queue* q, CU_NULLABLE const struct var_args* context) {
	priority_queue* result = cuPriorityQueueNew(q->functions);
	_cuQueueCloneWithElements(q, q->min, result, q->functions.clone);
	return result;
}

void cuPriorityQueueSavePNG(CU_NOTNULL const priority_queue* q, const char* template, ...) {
	char filename[BUFFER_SIZE];
	va_list va;
	va_start(va, template);
	vsnprintf(filename, BUFFER_SIZE, template, va);
	va_end(va);
	FILE* dotFile = cuFileUtilsOpen("w", "%s.dot", filename);
	va_end(va);

	cuFileUtilsTabbedWriteln(dotFile, 0, "digraph {");
	cuFileUtilsTabbedWriteln(dotFile, 1, "rankdir=\"TB\";");
	_cuQueueSavePNG_Nodes(dotFile, q, q->min);
	_cuQueueSavePNG_Edges(dotFile, q, q->min);
	cuFileUtilsTabbedWriteln(dotFile, 0, "}");
	fclose(dotFile);

	cuUtilsCallSystem("dot -Tsvg -o %s.svg %s.dot", filename, filename);
	cuUtilsCallSystem("rm %s.dot", filename);
}

CU_NULLABLE queue_findItem cuPriorityQueueGetFindItemOperation(CU_NOTNULL const priority_queue* q) {
	return q->findItemImplementation;
}

CU_NULLABLE queue_addItem cuPriorityQueueGetAddItemOperation(CU_NOTNULL const priority_queue* q) {
	return q->addItemImplementation;
}

static void _cuQueueCloneWithElements(CU_NOTNULL const priority_queue* q, CU_NULLABLE const struct priority_queue_cell* qc, CU_NOTNULL priority_queue* result, CU_NULLABLE cloner c) {
	if (qc == NULL) {
		return;
	}
	void* payload = c != NULL ? c(qc->payload) : qc->payload;
	cuPriorityQueueAddItem(result, payload, qc->priority);
	_cuQueueCloneWithElements(q, qc->left, result, c);
	_cuQueueCloneWithElements(q, qc->right, result, c);
}

static void _cuQueueToList(CU_NOTNULL const priority_queue* q, CU_NULLABLE const struct priority_queue_cell* qc, list* l) {
	if (qc == NULL) {
		return;
	}
	cuListAddTail(l, qc->payload);
	_cuQueueToList(q, qc->left, l);
	_cuQueueToList(q, qc->right, l);
}

static void _cuQueueSavePNG_Nodes(FILE* fout, CU_NOTNULL const priority_queue* q, CU_NOTNULL const struct priority_queue_cell* qc) {
	char buffer[BUFFER_SIZE];
	q->functions.bufferString(qc->payload, buffer);
	cuFileUtilsTabbedWriteln(fout, 1, "N%04d [label=\"%s\\n(%d)\"];", qc->priority, buffer, qc->priority);

	if (qc->left != NULL) {
		_cuQueueSavePNG_Nodes(fout, q, qc->left);
	}
	if (qc->right != NULL) {
		_cuQueueSavePNG_Nodes(fout, q, qc->right);
	}
}

static void _cuQueueSavePNG_Edges(FILE* fout, CU_NOTNULL const priority_queue* q, CU_NOTNULL const struct priority_queue_cell* qc) {
	char buffer[BUFFER_SIZE];
	q->functions.bufferString(qc->payload, buffer);
	if (qc->left != NULL) {
		cuFileUtilsTabbedWriteln(fout, 1, "N%04d -> N%04d;", qc->priority, qc->left->priority);
		_cuQueueSavePNG_Edges(fout, q, qc->left);
	}
	if (qc->right != NULL) {
		cuFileUtilsTabbedWriteln(fout, 1, "N%04d -> N%04d;", qc->priority, qc->right->priority);
		_cuQueueSavePNG_Edges(fout, q, qc->right);
	}

}

static bool containsItemInQueueRecursive0(CU_NOTNULL const priority_queue* q, CU_NULLABLE const struct priority_queue_cell* qc, int evalData, CU_NULLABLE void* data) {
	if (qc == NULL) {
		return false;
	}
	//TODO this function should be a comparator, not an orderer
	if (q->functions.compare(qc->payload, data) == 0) {
		return true;
	}

	if (qc->left != NULL) {
		int evalLeft = evaluateQueueCell(q, qc->left);
		if (evalData > evalLeft) {
			if (containsItemInQueueRecursive0(q, qc->left, evalData, data)) {
				return true;
			}
		}
	}

	if (qc->right != NULL) {
		int evalRight = evaluateQueueCell(q, qc->right);
		if (evalData > evalRight) {
			if (containsItemInQueueRecursive0(q, qc->right, evalData, data)) {
				return true;
			}
		}
	}

	return false;
}

static void gotoCellWithId(CU_NOTNULL const priority_queue* q, int idToRead, CU_NOTNULL struct priority_queue_cell** pointerOfCellReached, CU_NOTNULL struct priority_queue_cell** pointerOfParentCell, CU_NOTNULL struct priority_queue_cell*** pointerOfParentReachingNode) {
	int qcId = 1;
	struct priority_queue_cell *const * parentPointer = &q->min;
	const struct priority_queue_cell* qc = q->min;
	//level of the node under analysis
	int level = 0;
	int size = (int) floor(log2(idToRead));

	*pointerOfParentCell = NULL;
	*pointerOfCellReached = NULL;
	*pointerOfParentReachingNode = NULL;

	//level is the power of 2 of every node in the level
	//i.e level=3 --> ids of nodes will be 8,9,10,11,12,13,14,15
	//right-most node will be 2^n-1
	//left-most node will be 2^n
	//at level i, all the nodes in the left branch has the i-th bit (counting from the right) set to 0
	//at level i, all the nodes in the right branch has the i-th bit (counting from the right) set to 1

	while(true) {
		if (qcId == idToRead) {
			//we need to put the new element here
			*pointerOfCellReached = (struct priority_queue_cell*)qc;
			*pointerOfParentReachingNode = (struct priority_queue_cell**)parentPointer;
			return;
		} else if ((idToRead & (1 << (size - level - 1))) > 0) {
			parentPointer = &qc->right;
			*pointerOfParentCell = (struct priority_queue_cell*)qc;
			qc = qc->right;
			qcId = 2 * qcId + 1;
		} else {
			parentPointer = &qc->left;
			*pointerOfParentCell = (struct priority_queue_cell*)qc;
			qc = qc->left;
			qcId = 2 * qcId + 0;
		}
		level++;
	}
}

static void _clearQueue(CU_NOTNULL const priority_queue* q, CU_NULLABLE const struct priority_queue_cell* qc, bool destroyPayload, const struct var_args* context) {
	if (qc == NULL) {
		return;
	}

	_clearQueue(q, qc->left, destroyPayload, context);
	_clearQueue(q, qc->right, destroyPayload, context);
	if (destroyPayload) {
		q->functions.destroy(qc->payload, context);
	}
	destroyQueueCell(qc, context);
}

static CU_NULLABLE struct priority_queue_cell** getQueueCellPtr(CU_NOTNULL const struct priority_queue_cell* qc) {
	if (qc->parent == NULL) {
		return NULL;
	}
	return qc->parent->left == qc ? &qc->parent->left : &qc->parent->right;
}

static CU_NULLABLE struct priority_queue_cell* getOtherQueueCellOfParent(CU_NOTNULL const struct priority_queue_cell* qc) {
	return qc->parent->left == qc ? qc->parent->right : qc->parent->left;
}

/**
 * @param[in] qc the ::struct priority_queue_cell to analyze
 * @return
 * 	\li true if \c qc is the top of the heap;
 * 	\li false otherwise;
 */
static bool isParent(CU_NOTNULL const priority_queue* q, CU_NOTNULL const struct priority_queue_cell* qc) {
	return q->min == qc;
}

static bool isLeaf(CU_NOTNULL const struct priority_queue_cell* qc) {
	return qc->left == NULL;
}

static int evaluateQueueCell(CU_NOTNULL const priority_queue* q, CU_NOTNULL const struct priority_queue_cell* qc) {
	return qc->priority;
}

/**
 * swap 2 queue cells in the binary tree which is the heap
 *
 * @pre
 *  @li @c a is near the top of the root
 *  @li @c b is near the leaves of the root
 *
 * @param[in] q the queue holding these queue_cells
 * @param[in] a queue cell to swap (it is near the root)
 * @param[in] b queue cell to swap (it is near the leaves)
 */
static void swapQueueCells(CU_NOTNULL priority_queue* q, CU_NOTNULL struct priority_queue_cell* a, CU_NOTNULL struct priority_queue_cell* b) {
	struct priority_queue_cell* parentA = a->parent;
	struct priority_queue_cell** parentLinkA = getQueueCellPtr(a);
	struct priority_queue_cell* leftChildA = a->left;
	struct priority_queue_cell* rightChildA = a->right;

	struct priority_queue_cell* parentB = b->parent;
	struct priority_queue_cell** parentLinkB = getQueueCellPtr(b);
	struct priority_queue_cell* leftChildB = b->left;
	struct priority_queue_cell* rightChildB = b->right;

	//is a the parent of b?
	bool isAParentOfB = b->parent == a;

	//parent of A now links to B
	if (parentA != NULL) {
		*parentLinkA = b;
		b->parent = parentA;
	} else {
		//A was the root. Now B is the root
		q->min = b;
		b->parent = NULL;
	}
	//children of A now are children of B: note a child of A might be B!
	if (leftChildA != b) {
		b->left = leftChildA;
		if (leftChildA != NULL) {
			leftChildA->parent = b;
		}
	} else {
		//child of A is actually B
		b->left = a;
		a->parent = b;
	}
	if (rightChildA != b) {
		b->right = rightChildA;
		if (rightChildA != NULL) {
			rightChildA->parent = b;
		}
	} else {
		//child of A is actually B
		b->right = a;
		a->parent = b;
	}

	//parent of B now points to A (we alerady set the parent if B is a child of A
	if (!isAParentOfB) {
		if (parentB != NULL) {
			*parentLinkB = a;
			a->parent = parentB;
		}
	}
	//children of B now are children of A
	a->left = leftChildB;
	if (leftChildB != NULL) {
		leftChildB->parent = a;
	}
	a->right = rightChildB;
	if (rightChildB != NULL) {
		rightChildB->parent = a;
	}

//	struct priority_queue_cell* grandparent = parent->parent;
//	struct priority_queue_cell* otherChild = getOtherQueueCellOfParent(child);
//	struct priority_queue_cell** grandParentLink = getQueueCellPtr(parent);
//	struct priority_queue_cell* leftSubChild = child->left;
//	struct priority_queue_cell* rightSubChild = child->right;
//	//gp -> child
//	if (grandParentLink != NULL) {
//		*grandParentLink = child;
//		child->parent = grandparent;
//	} else {
//		q->min = child;
//		child->parent = NULL;
//	}
//	//c -> parent
//	child->left = parent;
//	parent->parent = child;
//	//c -> other child of parent
//	child->right = otherChild;
//	if (otherChild != NULL) {
//		otherChild->parent = child;
//	}
//	//left child of c -> left child of parent
//	parent->left = leftSubChild;
//	if (leftSubChild != NULL) {
//		leftSubChild->parent = parent;
//	}
//	//right child of c -> right child of parent
//	parent->right = rightSubChild;
//	if (rightSubChild != NULL) {
//		rightSubChild->parent = parent;
//	}

}

static void percolateUp(CU_NOTNULL priority_queue* q, CU_NOTNULL struct priority_queue_cell* qc) {
	if (isParent(q, qc)) {
		return;
	}

	int evalThis = evaluateQueueCell(q, qc);
	int evalParent = evaluateQueueCell(q, qc->parent);

	//qc->priority pointer is passed to optimization functions
	//so we can't just swap the content of payload and priority.
	//it's better to swap left,right,parent

	//TODO remove struct priority_queue_cell* parent = qc->parent;
	if (evalThis >= evalParent) {
		return;
	}
	swapQueueCells(q, qc->parent, qc);
	cuPriorityQueueSavePNG(q, "%s-%d", __func__, globalIncrement);
	globalIncrement += 1;
	percolateUp(q, qc);
	//TODO remove percolateUp(q, parent);
}

static void percolateDown(CU_NOTNULL priority_queue* q, CU_NOTNULL struct priority_queue_cell* qc) {
	if (qc == NULL) {
		return;
	}
	if (isLeaf(qc)) {
		return;
	}
	struct priority_queue_cell* small;

	if (qc->left == NULL && qc->right == NULL) {
		return;
	}

	int evalLeft;
	int evalRight;
	int evalSmall;

	if (qc->right == NULL) {
		small = qc->left;
		evalSmall = evaluateQueueCell(q, qc->left);
	} else {
		evalLeft = evaluateQueueCell(q, qc->left);
		evalRight = evaluateQueueCell(q, qc->right);

		if (evalLeft < evalRight) {
			evalSmall = evalLeft;
			small = qc->left;
		} else {
			evalSmall = evalRight;
			small = qc->right;
		}
	}

	int evalCurrent = evaluateQueueCell(q, qc);
	if (evalCurrent < evalSmall) {
		return;
	}
	swapQueueCells(q, qc, small);

	//TODO remove
//	SWAP(small->payload, qc->payload, void*);
//	SWAP(small->priority, qc->priority, int);

//	percolateDown(q, small);
	percolateDown(q, qc);
}

static CU_NOTNULL struct priority_queue_cell* newQueueCell() {
	struct priority_queue_cell* result = CU_MALLOC(struct priority_queue_cell);
	if (result == NULL) {
		ERROR_MALLOC();
	}

	result->left = NULL;
	result->right = NULL;
	result->parent = NULL;

	result->payload = NULL;
	result->priority = 0;

	return result;
}

void destroyQueueCell(CU_NOTNULL const struct priority_queue_cell* qc, CU_NULLABLE const struct var_args* context) {
	free((void*)qc);
}

void destroyQueueCellWithElement(CU_NOTNULL const struct priority_queue_cell* qc, destructor d) {
	d(qc->payload, NULL); //TODo context null
	CU_FREE(qc);
}
