/**
 * @file
 *
 * @author koldar
 * @date Mar 30, 2018
 */

#include "naive_queue.h"
#include "defaultFunctions.h"
#include "errors.h"

struct naive_queue_cell {
	int evaluation;
	void* payload;
	struct naive_queue_cell* next;
	struct naive_queue_cell* before;
};

struct naive_queue {
	struct naive_queue_cell* head;
	int size;
	payload_functions functions;
	evaluator_function f;
	const var_args* va;
};

static struct naive_queue_cell* newNaiveQueueCell(int evaluation, CU_NULLABLE const void* payload, CU_NULLABLE struct naive_queue_cell* before,  CU_NULLABLE struct naive_queue_cell* next);
static void destroyNaiveQueueCell(CU_NOTNULL const struct naive_queue_cell* nqc, destructor payloadDestructor);
static void* _cuNaiveQueueRemoveItemPrivate(CU_NOTNULL naive_queue* q, finder f, CU_NULLABLE const var_args* va, destructor d);

naive_queue* cuNaiveQueueNew(payload_functions functions, evaluator_function f, CU_NULLABLE const var_args* va) {
	naive_queue* retVal = (naive_queue*) malloc(sizeof(naive_queue));
	if (retVal == NULL) {
		ERROR_MALLOC();
	}

	retVal->va = va;
	retVal->f = f;
	retVal->functions = functions;
	retVal->size = 0;
	retVal->head = NULL;

	return retVal;
}

void cuNaiveQueueDestroy(CU_NOTNULL const naive_queue* nq, CU_NULLABLE const struct var_args* context) {
	struct naive_queue_cell* nqc = nq->head;
	while (nqc != NULL) {
		struct naive_queue_cell* tmp = nqc->next;
		destroyNaiveQueueCell(nqc, cuDefaultFunctionDestructorNOP);
		nqc = tmp;
	}
	free((void*)nq);
}

void cuNaiveQueueDestroyWithElements(CU_NOTNULL const naive_queue* nq, CU_NULLABLE const struct var_args* context) {
	struct naive_queue_cell* nqc = nq->head;
	while (nqc != NULL) {
		struct naive_queue_cell* tmp = nqc->next;
		destroyNaiveQueueCell(nqc, nq->functions.destroy);
		nqc = tmp;
	}
	free((void*)nq);
}

void cuNaiveQueueAddItem(CU_NOTNULL naive_queue* q, CU_NULLABLE const void* el) {
	struct naive_queue_cell* nqc = q->head;

	int elEvaluation = q->f(el, q->va);
	if (nqc == NULL) {
		//the queue is actually empty
		struct naive_queue_cell* add = newNaiveQueueCell(elEvaluation, el, NULL, NULL);
		q->head = add;
		goto exit;
	}

	struct naive_queue_cell* before = NULL;
	struct naive_queue_cell** prevPointer = &q->head;
	while (nqc != NULL) {
		if (nqc->evaluation < elEvaluation) {
			prevPointer = &nqc->next;
			before = nqc;
			nqc = nqc->next;
			continue;
		}

		struct naive_queue_cell* add = newNaiveQueueCell(elEvaluation, el, before, nqc);
		nqc->before = add;
		*prevPointer = add;
		goto exit;
	}

	//add to the end of the queue
	struct naive_queue_cell* add = newNaiveQueueCell(elEvaluation, el, before, NULL);
	*prevPointer = add;

	exit:;
	q->size += 1;
}

void* _cuNaiveQueuePeekItem(CU_NOTNULL const naive_queue* q) {
	if (q->head == NULL) {
		return NULL;
	}
	return q->head->payload;
}

void* _cuNaiveQueuePopItem(CU_NOTNULL naive_queue* q) {
	if (q->head == NULL) {
		return NULL;
	}

	void* retVal = q->head->payload;
	cuNaiveQueueRemoveCell(q, q->head);

	return retVal;
}

naive_queue_cell* cuNaiveQueueFindCellItem(CU_NOTNULL const naive_queue* q, CU_NOTNULL void** payloadOutput, finder f, CU_NULLABLE const var_args* va) {
	naive_queue_cell* tmp = q->head;

	while (tmp != NULL) {
		void* payload = tmp->payload;
		if (f(payload, va)) {
			*payloadOutput = payload;
			return tmp;
		}

		tmp = tmp->next;
	}

	*payloadOutput = NULL;
	return NULL;
}

void cuNaiveQueueRemoveCell(CU_NOTNULL naive_queue* q, const naive_queue_cell* queueCell) {
	if (queueCell->before == NULL && queueCell->next == NULL) { //there is only one cell
		q->head = NULL;
	} else if (queueCell->before == NULL) { //head: hence q->head and queueCell are the same pointer
		queueCell->next->before = NULL;
		q->head = queueCell->next;
	} else if (queueCell->next == NULL) { //tail
		queueCell->before->next = NULL;
	} else {
		//in the middle
		queueCell->before->next = queueCell->next;
		queueCell->next->before = queueCell->before;
	}
	q->size -= 1;
	destroyNaiveQueueCell(queueCell, cuDefaultFunctionDestructorNOP);
}

void cuNaiveQueueRemoveCellWithElements(CU_NOTNULL naive_queue* q, const naive_queue_cell* queueCell) {
	q->functions.destroy(queueCell->payload, NULL);
	cuNaiveQueueRemoveCell(q, queueCell);
}

void* cuNaiveQueueRemoveItem(CU_NOTNULL naive_queue* q, finder f, CU_NULLABLE const var_args* va) {
	return _cuNaiveQueueRemoveItemPrivate(q, f, va, cuDefaultFunctionDestructorNOP);
}

bool cuNaiveQueueRemoveItemWithElements(CU_NOTNULL naive_queue* q, finder f, CU_NULLABLE const var_args* va) {
	void* retVal = _cuNaiveQueueRemoveItemPrivate(q, f, va, cuDefaultFunctionDestructorNOP);
	q->functions.destroy(retVal, va);
	return retVal != NULL;
}

bool cuNaiveQueueIsEmpty(CU_NOTNULL const naive_queue* q) {
	return q->size == 0;
}

int cuNaiveQueueGetSize(CU_NOTNULL const naive_queue* q) {
	return q->size;
}

void cuNaiveQueueClear(CU_NOTNULL naive_queue* q) {
	struct naive_queue_cell* nqc = q->head;
	while (nqc != NULL) {
		struct naive_queue_cell* tmp = nqc->next;
		destroyNaiveQueueCell(nqc, cuDefaultFunctionDestructorNOP);
		CU_FREE(nqc);
		nqc = tmp;
	}
	q->size = 0;
	q->head = NULL;
}

void cuNaiveQueueClearWithElements(CU_NOTNULL naive_queue* q) {
	struct naive_queue_cell* nqc = q->head;
	while (nqc != NULL) {
		struct naive_queue_cell* tmp = nqc->next;
		destroyNaiveQueueCell(nqc, q->functions.destroy);
		CU_FREE(nqc);
		nqc = tmp;
	}
	q->size = 0;
	q->head = NULL;
}

int cuNaiveQueueBufferStringer(CU_NOTNULL const naive_queue* q, CU_NOTNULL char* buffer) {
	int i = 0;

	i += sprintf(&buffer[i], "[");

	CU_ITERATE_OVER_NAIVE_QUEUE(q, j, evaluation, value, void*) {
		i += sprintf(&buffer[i], "%d: ", evaluation);
		i += q->functions.bufferString(value, &buffer[i]);
		if (cuNaiveQueueGetSize(q) == (j+1)) {
			//this is the last element
			i += sprintf(&buffer[i], " ");
		} else {
			//there are other elements
			i += sprintf(&buffer[i], ", ");
		}
	}

	i += sprintf(&buffer[i], "]");

	return i;
}

struct naive_queue_cell* _cuNaiveQueueGetHead(CU_NOTNULL const naive_queue* nq) {
	return nq->head;
}

int _cuNaiveQueueGetEvaluationOfCell(CU_NOTNULL const struct naive_queue_cell* nqc) {
	return nqc->evaluation;
}

struct naive_queue_cell* _cuNaiveQueueGetNextOfCell(CU_NOTNULL const struct naive_queue_cell* nqc) {
	return nqc != NULL ? nqc->next : NULL;
}

void* _cuNaiveQueueGetPayloadOfCell(const struct naive_queue_cell* nqc) {
	return nqc->payload;
}

void cuNaiveQueueAddContainer(CU_NOTNULL naive_queue* queue, CU_NOTNULL const list* l) {
	CU_ITERATE_OVER_LIST(l, cell, payload, void*) {
		cuNaiveQueueAddItem(queue, payload);
	}
}

void cuNaiveQueueMoveContentsInContainer(CU_NOTNULL naive_queue* queue, CU_NOTNULL list* l) {
	while (!cuListIsEmpty(l)) {
		void* item = cuListPopFrom(l);
		cuNaiveQueueAddItem(queue, item);
	}
}

//TODO useless parameter d
static void* _cuNaiveQueueRemoveItemPrivate(CU_NOTNULL naive_queue* q, finder f, CU_NULLABLE const var_args* va, destructor d) {
	struct naive_queue_cell* tmp = q->head;
	struct naive_queue_cell* next = NULL;
	void* result = NULL;

	while (tmp != NULL) {
		if (f(tmp->payload, va)) {
			result = tmp->payload;
			cuNaiveQueueRemoveCell(q, tmp);
			return result;
		}
		tmp = tmp->next;
	}

	return NULL;
}

static struct naive_queue_cell* newNaiveQueueCell(int evaluation, CU_NULLABLE const void* payload, CU_NULLABLE struct naive_queue_cell* before,  CU_NULLABLE struct naive_queue_cell* next) {
	struct naive_queue_cell* result = (struct naive_queue_cell*) malloc(sizeof(struct naive_queue_cell));
	if (result == NULL) {
		ERROR_MALLOC();
	}

	result->evaluation = evaluation;
	result->before = before;
	result->next = next;
	CU_SAFE_ASSIGN(result->payload, payload);

	return result;
}

static void destroyNaiveQueueCell(CU_NOTNULL const struct naive_queue_cell* nqc, destructor payloadDestructor) {
	payloadDestructor(nqc->payload, NULL); //TODO context null
	free((void*)nqc);
}
