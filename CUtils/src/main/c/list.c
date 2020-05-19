/*
 * list.c
 *
 *  Created on: May 17, 2017
 *      Author: koldar
 */

#define _GNU_SOURCE
#include "list.h"
#include "macros.h"
#include "log.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "random_utils.h"
#include "defaultFunctions.h"
#include "errors.h"

struct list_cell {
	///represents the paylaod inside this cell of the list
	void* payload;
	///a pointer to the next element of the list. Can be NULL
	struct list_cell* next;
};

struct list {
	///number of elements inside the list. Allows count operation to be O(1)
	int size;
	///pointer to the first element of the list. Can be NULL
	list_cell* head;
	///pointer to the last element of the list. Can be NULL
	list_cell* tail;
	/**
	 * A set of functions used to easily manupilate each content of each cell
	 */
	payload_functions payloadFunctions;
};

static list_cell* newListCell(const void* p, const list_cell* next);

list* cuListNew(payload_functions payloadFunctions) {
	list* retVal = malloc(sizeof(list));
	if (retVal == NULL) {
		ERROR_MALLOC();
	}

	retVal->head = NULL;
	retVal->size = 0;
	retVal->tail = NULL;
	retVal->payloadFunctions = payloadFunctions;

	return retVal;
}
CU_DEFINE_DEFAULT_VALUES(cuListNew,
		cuPayloadFunctionsDefault()
);

void cuListDestroy(CU_NOTNULL const list* lst, CU_NULLABLE const struct var_args* context) {
	CU_ITERATE_OVER_LIST(lst, cell, value, void*) {
		free(cell);
	}
	free((void*)lst);
}

void cuListDestroyWithElement(CU_NOTNULL const list* _lst, CU_NULLABLE const struct var_args* context) {
	list* lst = (list*) _lst;

	CU_ITERATE_OVER_LIST(lst, cell, value, void*) {
		debug("deleting payload!!!!");
		_lst->payloadFunctions.destroy(cell->payload, context);
		free(cell);
	}
	CU_FREE(lst);
}

void cuListClear(CU_NOTNULL list* l) {
	CU_ITERATE_OVER_LIST(l, cell, value, void*) {
		free(cell);
	}
	l->head = NULL;
	l->size = 0;
	l->tail = NULL;
}

void cuListClearWithElements(CU_NOTNULL list* l) {
	CU_ITERATE_OVER_LIST(l, cell, value, void*) {
		l->payloadFunctions.destroy(cell->payload, NULL);
		free(cell);
	}

	l->head = NULL;
	l->size = 0;
	l->tail = NULL;
}

list* cuListCloneWithElements(CU_NOTNULL const list* l) {
	list* retVal = cuListNew();
	CU_ITERATE_OVER_LIST(l, cell, payload, void*) {
		cuListAddTail(retVal, l->payloadFunctions.clone(payload));
	}
	return retVal;
}

list* cuListClone(CU_NOTNULL const list* l) {
	list* retVal = cuListNew();
	CU_ITERATE_OVER_LIST(l, cell, payload, void*) {
		cuListAddTail(retVal, payload);
	}
	return retVal;
}

void cuListAddHead(CU_NOTNULL list* l, CU_NULLABLE const void* el) {
	list_cell* new_cell = malloc(sizeof(list_cell));
	if (new_cell == NULL) {
		ERROR_MALLOC();
	}

	new_cell->payload = (void*) el;
	new_cell->next = l->head;

	l->size++;
	l->head = new_cell;
	if (l->tail == NULL) {
		l->tail = new_cell;
	}
}

void cuListAddTail(CU_NOTNULL list* l, CU_NULLABLE const void* el) {
	list_cell* new_cell = malloc(sizeof(list_cell));
	if (new_cell == NULL) {
		ERROR_MALLOC();
	}

	new_cell->payload = (void*)el;
	new_cell->next = NULL;
	if (l->tail != NULL) {
		l->tail->next = new_cell;
	}

	l->size++;
	if (l->head == NULL) {
		l->head = new_cell;
	}
	l->tail = new_cell;

}

int cuListGetSize(const list* l) {
	return l->size;
}

void cuListMoveContent(CU_NOTNULL list* restrict dst, CU_NOTNULL list* restrict src) {
	//*********** DST **********
	dst->size += cuListGetSize(src);
	if (dst->head == NULL) {
		dst->head = src->head;
	} else {
		dst->tail->next = src->head;
	}
	dst->tail = src->tail;

	//************ SRC **************
	src->size = 0;
	src->head = NULL;
	src->tail = NULL;
}

void* cuListGetLastElement(CU_NOTNULL const list* l) {
	if (l->tail == NULL) {
		return NULL;
	}
	return l->tail->payload;
}

bool cuListIsEmpty(CU_NOTNULL const list* l) {
	return l->size == 0;
}

void* cuListPopFrom(CU_NOTNULL list* l) {
	if (cuListIsEmpty(l)) {
		return NULL;
	}

	list_cell* cell = l->head;
	void* retVal = cell->payload;
	l->head = l->head->next;
	l->size--;
	if (cuListIsEmpty(l)) {
		l->tail = NULL;
	}

	free(cell);
	return retVal;
}

void cuListRemoveHeadAndDestroyItem(CU_NOTNULL list* l, CU_NULLABLE const struct var_args* context) {
	if (cuListIsEmpty(l)) {
		return;
	}
	cuListRemoveDestroyNthItem(l, 0);
}

void* cuListPeek0(CU_NOTNULL const list* l) {
	return cuListGetHead0(l);
}

void* cuListGetHead0(CU_NOTNULL const list* l) {
	if (cuListIsEmpty(l)) {
		return NULL;
	}
	return l->head->payload;
}

void* cuListGetTail0(CU_NOTNULL const list* l) {
	if (cuListIsEmpty(l)) {
		return NULL;
	}
	return l->tail->payload;
}

list_cell* cuListAddItemAfter(CU_NOTNULL list* l, CU_NOTNULL list_cell* cell, CU_NULLABLE const void* item) {
	CU_ITERATE_OVER_LIST(l, acell, v, void*) {
		if (acell == cell) {
			list_cell* retVal = newListCell(item, acell->next);
			acell->next = retVal;
			l->size += 1;

			//if cell was the tail, we need to update the pointer as well
			if (retVal->next == NULL) {
				l->tail = retVal;
			}
			return retVal;
		}
	}
	return NULL;
}

void cuListAddItemAt(CU_NOTNULL list* l, int index, CU_NULLABLE const void* item) {
	int i=0;

	//it's safe to use CU_VARIABLE_ITERATE_OVER_LIST since when we change the list we terminate the iteration.
	CU_VARIABLE_ITERATE_OVER_LIST(l, prevCell, cell, data, void*) {
		if (i == index) {
			//ok, we need to add the item here

			if (index == 0) {
				//add on head
				cuListAddHead(l, item);
			} else if (cuListIsLastItem(l, cell)) {
				//add on tail
				cuListAddTail(l, item);
			} else {
				//add in the middle
				list_cell* newCell = newListCell(item, NULL);
				prevCell->next = newCell;
				newCell->next = cell;
				l->size += 1;
			}

			break;
		}
		i++;
	}
}

void* cuListSetItemAt(CU_NOTNULL list* l, int index, CU_NULLABLE const void* newItem) {
	int i = 0;
	void* result = NULL;
	CU_ITERATE_OVER_LIST(l, cell, data, void*) {
		if (index == i) {
			result = cell->payload;
			cell->payload = (void*)newItem;
			return result;
		} else {
			i++;
		}
	}

	return NULL;
}

void cuListSetItemAtWithElement(CU_NOTNULL list* l, int index, CU_NULLABLE const void* newItem) {
	int i = 0;
	void* result = NULL;
	CU_ITERATE_OVER_LIST(l, cell, data, void*) {
		if (index == i) {
			l->payloadFunctions.destroy(cell->payload, NULL);
			cell->payload = (void*)newItem;
			return;
		} else {
			i++;
		}
	}
}

void* cuListfindItem(CU_NOTNULL const list* l, finder f, CU_NOTNULL const var_args* va) {
	CU_ITERATE_OVER_LIST(l, cell, value, void*) {
		if (f(value, va)) {
			return value;
		}
	}
	return NULL;
}

void* cuListPopTail(CU_NOTNULL list* l) {
	if (l->tail == NULL) {
		return NULL;
	}

	void* result = l->tail;
	CU_VARIABLE_ITERATE_OVER_LIST(l, prevCell, cell, data, void*) {
		if (cuListIsLastItem(l, cell)) {
			cuListRemoveItemInListCell(l, &prevCell, cell);
			l->tail = prevCell;
			break;
		}
	}

	return result;
}

void cuListRemoveTailAndDestroyItem(CU_NOTNULL list* l, CU_NULLABLE const struct var_args* context) {
	void* payload = cuListPopTail(l);
	if (payload != NULL) {
		l->payloadFunctions.destroy(l, context);
	}
}

bool cuListRemoveItem(CU_NOTNULL list* l, finder f, CU_NOTNULL const var_args* va) {
	bool removed = false;
	CU_VARIABLE_ITERATE_OVER_LIST(l, prevCell, cell, payload, void*) {
		if (f(payload, va)) {
			cuListRemoveItemInListCell(l, &prevCell, cell);
			removed = true;
			break;
		}
	}

	return removed;
}

bool cuListRemoveAndDestroyItem(CU_NOTNULL list* l, finder f, CU_NOTNULL const var_args* va) {
	bool removed = false;
	CU_VARIABLE_ITERATE_OVER_LIST(l, prevCell, cell, payload, void*) {
		if (f(payload, va)) {
			l->payloadFunctions.destroy(payload, va);
			cuListRemoveItemInListCell(l, &prevCell, cell);
			removed = true;
			break;
		}
	}

	return removed;
}

bool cuListContainsItemByReference(CU_NOTNULL const list* l, CU_NOTNULL const void* obj) {
	CU_ITERATE_OVER_LIST(l, cell, payload, void*) {
		if (payload == obj) {
			return true;
		}
	}
	return false;
}

bool cuListRemoveItemByReference(CU_NOTNULL list* l, CU_NOTNULL const void* obj) {
	bool removed = false;
	CU_VARIABLE_ITERATE_OVER_LIST(l, prevCell, cell, payload, void*) {
		if (payload == obj) {
			cuListRemoveItemInListCell(l, &prevCell, cell);
			removed = true;
			break;
		}
	}

	return removed;
}

int cuListBufferString(CU_NOTNULL const list* lst, CU_NOTNULL char* buffer) {
	int i = 0;
	int j = 0;
	i += sprintf(&buffer[i], "[");
	CU_ITERATE_OVER_LIST(lst, cell, payload, void*) {
		j++;
		i += sprintf(&buffer[i], "%d:(", j);
		i += lst->payloadFunctions.bufferString(payload, &buffer[i]);
		i += sprintf(&buffer[i], ")");

		if ((cell->next) != NULL) {
			i += sprintf(&buffer[i], ",");
		}
	}
	i += sprintf(&buffer[i], "]");
	return i;
}

void* _cuListGetNthItem(CU_NOTNULL const list* l, int index) {
	CU_ITERATE_OVER_LIST(l, cell, payload, void*) {
		if (index == 0) {
			return payload;
		}
		index--;
	}
	return NULL;
}

bool cuListRemoveNthItem(CU_NOTNULL list* lst, int index) {
	if (index >= cuListGetSize(lst)) {
		return false;
	}

	CU_VARIABLE_ITERATE_OVER_LIST(lst, prevCell, cell, payload, void*) {
		if (index == 0) {
			cuListRemoveItemInListCell(lst, &prevCell, cell);
			return true;
		}
		index--;
	}

	return false;
}

bool cuListRemoveDestroyNthItem(CU_NOTNULL list* lst, int index) {
	if (index >= cuListGetSize(lst)) {
		return false;
	}

	CU_VARIABLE_ITERATE_OVER_LIST(lst, prevCell, cell, payload, void*) {
		if (index == 0) {
			lst->payloadFunctions.destroy(cell->payload, NULL);
			cuListRemoveItemInListCell(lst, &prevCell, cell);
			return true;
		}
		index--;
	}

	return false;
}

void cuListRemoveItemInListCell(CU_NOTNULL list* l, CU_NOTNULL list_cell** restrict previousCell, CU_NOTNULL list_cell* restrict cellToRemove) {
	list* lst = l;
	list_cell* previous = *previousCell;

	if (previous == NULL) {
		//we're removing the head
		debug("removing head");
		cuListPopFrom(l);
	} else if (cellToRemove->next == NULL){
		//we're removing the tail
		debug("removing tail");
		previous->next = NULL;
		lst->size--;
		lst->tail = previous;
		free(cellToRemove);
	} else {
		debug("removing middle element %p %p", previous, cellToRemove);
		debug("nexts are %p %p", previous->next, cellToRemove->next);
		//we're removing an element inside the list
		previous->next = cellToRemove->next;
		lst->size--;
		free(cellToRemove);
	}

	*previousCell = NULL;
}

void cuListScramble(CU_NOTNULL list* l) {
	//to scramble the list, we apply a bubble sort on the list with a random comparator
	list_cell* cell2;
	CU_ITERATE_OVER_LIST(l, cell1, payload1, void*) {
		if (cell1->next == NULL) {
			return;
		}
		cell2 = cell1->next;

		while (cell2 != NULL) {
			if ((rand() % 2) == 0) {
				SWAP(cell1->payload, cell2->payload, void*);
			}
			cell2 = cell2->next;
		}
	}
}

void cuListSort(CU_NOTNULL list* l, orderer order) {
	if (cuListGetSize(l) < 2) {
		return;
	}

	list_cell* cell1 = l->head;
	list_cell* cell2 = NULL;;

	while (cell1 != NULL) {
		if (cell1->next == NULL) {
			return;
		}
		cell2 = cell1->next;

		while (cell2 != NULL) {
			if (order(cell1->payload, cell2->payload) > 0) {
				SWAP(cell1->payload, cell2->payload, void*);
			}
			cell2 = cell2->next;
		}

		cell1 = cell1->next;
	}
}

void* cuListPickRandomItem(CU_NOTNULL const list* l) {
	if (cuListIsEmpty(l)) {
		return NULL;
	}

	int rnd = cuRandomUtilsGetInRange(0, cuListGetSize(l), true, false);
	list_cell* cell = l->head;
	while (rnd > 0) {
		cell = cell->next;
		rnd--;
	}

	return cell->payload;
}

void cuListStoreInFile(CU_NOTNULL FILE* f, CU_NOTNULL const list* l) {
	//store length of list
	fwrite(&l->size, sizeof(l->size), 1, f);
	//store payloads
	CU_ITERATE_OVER_LIST(l, cell, paylaod, void*) {
		l->payloadFunctions.serialize(f, paylaod);
	}
}

list* cuListLoadFromFile(CU_NOTNULL FILE* f, object_deserializer deserializer) {
	list* retVal = cuListNew();

	//read length of list
	int size = 0;
	//to make a warning disappear
	int objectRead = fread(&size, sizeof(size), 1, f);
	//read paylods
	critical("size read is %d", size);
	void* payloadRead = NULL;
	for (int i=0; i<size; i++) {
		deserializer(f, &payloadRead);
		cuListAddTail(retVal, payloadRead);
	}

	return retVal;
}

bool cuListIsLastItem(CU_NOTNULL const list* l, CU_NOTNULL const list_cell* cell) {
	return cell->next == NULL;
}

void* _cuListGetPayloadOfCell(CU_NOTNULL const list_cell* cell) {
	return cell->payload;
}

list_cell* _cuListGetNextOfCell(CU_NOTNULL const list_cell* cell) {
	return cell->next;
}

list_cell* _cuListGetHeadCell(CU_NOTNULL const list* l) {
	return l->head;
}


static list_cell* newListCell(const void* p, const list_cell* next) {
	list_cell* retVal = malloc(sizeof(list_cell));
	if (retVal == NULL) {
		ERROR_MALLOC();
	}

	retVal->payload = (void*) p;
	retVal->next = (struct list_cell*) next;

	return retVal;
}
