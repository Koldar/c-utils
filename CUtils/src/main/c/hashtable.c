/*
 * hashtable.c
 *
 *  Created on: Jan 24, 2017
 *      Author: koldar
 */

#include "hashtable.h"
#include "macros.h"
#include <stdbool.h>
#include "errors.h"

struct HT {
	HTCell* cell;
	payload_functions functions;
};

static HTCell* newHTCell(CU_NULLABLE const void* e, unsigned long key);
static void destroyHTCell(CU_NOTNULL const HTCell* htCell, CU_NULLABLE const struct var_args* context);

CU_NOTNULL HT* cuHTNew(payload_functions functions) {
	HT* retVal = CU_MALLOC(HT);
	if (retVal == NULL) {
		ERROR_MALLOC();
	}

	retVal->functions = functions;
	retVal->cell = NULL;

	return retVal;
}

CU_DEFINE_DEFAULT_VALUES(cuHTNew,
		cuPayloadFunctionsDefault()
);

int cuHTGetSize(CU_NOTNULL const HT* ht) {
	int retVal;
	retVal = HASH_COUNT(ht->cell);
	return retVal;
}

CU_NULLABLE void* cuHTGetItem(CU_NOTNULL const HT* ht, unsigned long key) {
	HTCell* tmp;

	HASH_FIND(hh, ht->cell, &key, sizeof(unsigned long), tmp);
	if (tmp == NULL) {
		return NULL;
	}
	return tmp->data;
}

bool cuHTContainsItem(CU_NOTNULL const HT* ht, unsigned long key) {
	HTCell* tmp;
	HASH_FIND(hh, ht->cell, &key, sizeof(unsigned long), tmp);
	return (tmp != NULL);
}

bool cuHTAddOrUpdateItem(CU_NOTNULL HT* ht, unsigned long key, CU_NOTNULL const void* data) {
	HTCell* tmp;

	HASH_FIND(hh, ht->cell, &key, sizeof(unsigned long), tmp);
	if (tmp == NULL) {
		cuHTAddItem(ht, key, data);
		return true;
	} else {
		tmp->data = (void*) data;
		return false;
	}
}

bool cuHTUpdateItem(CU_NOTNULL HT* ht, unsigned long key, CU_NULLABLE const void* data) {
	HTCell* tmp;

	HASH_FIND(hh, ht->cell, &key, sizeof(unsigned long), tmp);
	if (tmp == NULL) {
		return false;
	}
	tmp->data = (void*) data;
	return true;
}

void cuHTAddItem(CU_NOTNULL HT* ht, unsigned long key, CU_NULLABLE const void* data) {
	HTCell* add = newHTCell(data, key);
	HASH_ADD(hh, ht->cell, id, sizeof(unsigned long), add);
}

void cuHTDestroy(CU_NOTNULL HT* ht, CU_NULLABLE const struct var_args* context) {

	HTCell* s;
	HTCell* tmp;

	HASH_ITER(hh, ht->cell, s, tmp) {
		HASH_DEL(ht->cell, s);
		destroyHTCell(s, context);
	}
	CU_FREE(ht);
}

void cuHTDestroyWithElements(CU_NOTNULL HT* ht, destructor d) {
	HTCell* s;
	HTCell* tmp;

	HASH_ITER(hh, ht->cell, s, tmp) {
		HASH_DEL(ht->cell, s);
		d(s->data, NULL); //TODO context null
		destroyHTCell(s, NULL); //TODO contrext null
	}
	free(ht);
}

void cuHTDestroyWithElements2(CU_NOTNULL const HT* _ht, CU_NULLABLE const struct var_args* context) {
	HTCell* s;
	HTCell* tmp;
	HT* ht = (HT*)_ht;

	HASH_ITER(hh, ht->cell, s, tmp) {
		HASH_DEL(ht->cell, s);
		ht->functions.destroy(s->data, context);
		destroyHTCell(s, context);
	}
	CU_FREE(ht);
}

void cuHTDestroyCell(CU_NOTNULL HT* ht, CU_NOTNULL HTCell* htCell) {
	HASH_DEL(ht->cell, htCell);
	destroyHTCell(htCell, NULL); //TODO context null
}

void cuHTDestroyCellWithElement(CU_NOTNULL HTCell* htCell, destructor d) {
	d(htCell->data, NULL); //TODO context null
	free(htCell);
}

bool cuHTRemoveItem(CU_NOTNULL HT* ht, unsigned long key) {
	HTCell* tmp;

	HASH_FIND(hh, ht->cell, &key, sizeof(unsigned long), tmp);
	if (tmp == NULL) {
		return false;
	}
	HASH_DEL(ht->cell, tmp);
	destroyHTCell(tmp, NULL); //TODO context null
	return true;
}

bool cuHTRemoveItemWithElement(CU_NOTNULL HT* ht, unsigned long key, destructor d) {
	HTCell* tmp;

	HASH_FIND(hh, ht->cell, &key, sizeof(unsigned long), tmp);
	if (tmp == NULL) {
		return false;
	}
	HASH_DEL(ht->cell, tmp);
	d(tmp->data, NULL); //TODO context null
	destroyHTCell(tmp, NULL); //TODO context null
	return true;
}

bool cuHTIsEmpty(CU_NOTNULL const HT* ht) {
	return ht->cell == NULL;
}

CU_NULLABLE void* cuHTFindItem(CU_NOTNULL const HT* ht, finder finder, CU_NULLABLE const var_args* va) {
	HTCell* el;
	HTCell* tmp;
	HASH_ITER(hh, ht->cell, el, tmp) {
		if (finder(el->data, va)){
			return el->data;
		}
	}
	return NULL;
}

bool cuHTContainsItems(CU_NOTNULL const HT* ht, finder finder, CU_NULLABLE const var_args* va) {
	return cuHTFindItem(ht, finder, va) != NULL;
}

CU_NULLABLE void* cuHTGetFirstItem(CU_NOTNULL const HT* ht) {
	CU_ITERATE_OVER_HT_VALUES(ht, data, void*) {
		return data;
	}
	return NULL;
}

bool cuHTSwapValues(CU_NOTNULL HT* ht, unsigned long key1, unsigned long key2) {
	HTCell* tmp1;
	HTCell* tmp2;

	HASH_FIND(hh, ht->cell, &key1, sizeof(unsigned long), tmp1);
	HASH_FIND(hh, ht->cell, &key2, sizeof(unsigned long), tmp2);

	if (tmp1 != NULL && tmp2 != NULL) {
		//they are both inside the hashtable
		void* p1 = tmp1->data;
		void* p2 = tmp2->data;
		HASH_DEL(ht->cell, tmp1);
		HASH_DEL(ht->cell, tmp2);

		cuHTAddItem(ht, key1, p2);
		cuHTAddItem(ht, key2, p1);
		return true;
	} else if (tmp1 != NULL) {
		//key2 is not inside the hashtable
		cuHTAddItem(ht, key2, tmp1->data);
		HASH_DEL(ht->cell, tmp1);
		return true;
	} else if (tmp2 != NULL) {
		//key1 is not inside the hashtable
		cuHTAddItem(ht, key1, tmp2->data);
		HASH_DEL(ht->cell, tmp2);
		return true;
	}

	return false;
}

void cuHTStoreInFile(CU_NOTNULL FILE* f, CU_NOTNULL const HT* ht, object_serializer payload) {
	char buffer[BUFFER_SIZE];
	int size;
	int htSize = cuHTGetSize(ht);
	HTCell* el;
	HTCell* tmp;


	fwrite(&htSize, sizeof(htSize), 1, f);
	HASH_ITER(hh, ht->cell, el, tmp) {
		fwrite(&el->id, sizeof(el->id), 1, f);
		payload(f, el->data);
	}
//	TODO remove CU_ITERATE_OVER_HT(ht, el) {
//		fwrite(&el->id, sizeof(el->id), 1, f);
//		payload(f, el->data);
//	}
}

void cuHTStoreInFile2(CU_NOTNULL FILE* f, CU_NOTNULL const HT* ht) {
	cuHTStoreInFile(f, ht, ht->functions.serialize);
}

CU_NOTNULL HT* cuHTLoadFromFile(CU_NOTNULL FILE* f, payload_functions functions) {
	HT* retVal = cuHTNew(functions);
	int size;
	unsigned long key;
	//we fill all the pointer with 0bits because the data to deserailize might be less than sizeof(void*)!
	void* data = NULL;
	int sizeRead = fread(&size, sizeof(size), 1, f);
	for(int i=0; i<size; i++) {
		int keyRead = fread(&key, sizeof(key), 1, f);
		functions.deserialize(f, &data);
		cuHTAddItem(retVal, key, data);
	}

	return retVal;
}

void cuHTClear(CU_NOTNULL HT* ht) {
	HTCell* s;
		HTCell* tmp;

	HASH_ITER(hh, ht->cell, s, tmp) {
		HASH_DEL(ht->cell, s);
		destroyHTCell(s, NULL); //TODO context null
	}
	ht->cell = NULL;
}

void cuHTClearWithElements(CU_NOTNULL HT* ht, destructor d) {
	HTCell* s;
	HTCell* tmp;

	HASH_ITER(hh, ht->cell, s, tmp) {
		HASH_DEL(ht->cell, s);
		d(s->data, NULL); //TODO context nulkl
		destroyHTCell(s, NULL); //TODO context null
	}
	ht->cell = NULL;
}

int cuHTBufferString(CU_NOTNULL const HT* ht, CU_NOTNULL char* buffer) {
	int i = 0;
	int j = 0;
	void* payload;
	HTCell* cell;
	HTCell* tmp;

	i += sprintf(&buffer[i], "{");
	HASH_ITER(hh, ht->cell, cell, tmp) {
		j++;
		i += sprintf(&buffer[i], "<%ld: ", cell->id);
		i += ht->functions.bufferString(cell->data, &buffer[i]);
		i += sprintf(&buffer[i], ">");

		if ((cell->hh.next) != NULL) {
			i += sprintf(&buffer[i], ",");
		}
	}
	i += sprintf(&buffer[i], "}");
	return i;
}

bool cuHTCompare(CU_NOTNULL const HT* a, CU_NOTNULL const HT* b) {
	if (a == b) {
		return true;
	}
	if (cuHTGetSize(a) != cuHTGetSize(b)) {
		return false;
	}

	CU_ITERATE_OVER_HASHTABLE(a, keyA, valueA, void*) {
		if (!cuHTContainsItem(b, keyA)) {
			return false;
		}
		void* valueB = cuHTGetItem(b, keyA);
		if (!a->functions.compare(valueA, valueB)) {
			return false;
		}
	}

	return true;
}

payload_functions cuHTGetPayloadFunction(payload_functions valueFunctions) {
	payload_functions result = cuPayloadFunctionsError();

	result.destroy = CU_AS_DESTRUCTOR(cuHTDestroyWithElements2);
	result.bufferString = CU_AS_BUFFERSTRINGER(cuHTBufferString);
	result.clone = CU_AS_CLONER(cuHTCloneWithElements);
	result.compare = CU_AS_COMPARER(cuHTCompare);
	result.deserialize = CU_AS_DESERIALIZER(cuHTLoadFromFile);
	result.serialize = CU_AS_SERIALIZER(cuHTStoreInFile2);

	return result;
}

CU_NOTNULL HTCell* _cuHTGetCell(CU_NOTNULL const HT* ht) {
	return ht->cell;
}

unsigned long _cuHTCellGetKey(CU_NOTNULL const HTCell* cell) {
	return cell->id;
}

CU_NULLABLE void* _cuHTCellGetValue(CU_NOTNULL const HTCell* cell) {
	return cell->data;
}

CU_NULLABLE HTCell* _cuHTCellGetNext(CU_NOTNULL const HTCell* cell) {
	return (HTCell*)cell->hh.next;
}

CU_NOTNULL HT* cuHTClone(CU_NOTNULL const HT* ht) {
	HT* retVal = cuHTNew();
	HTCell* cell;
	HTCell* tmp;

	HASH_ITER(hh, ht->cell, cell, tmp) {
		cuHTAddItem(retVal, cell->id, cell->data);
	}

	return retVal;
}

CU_NOTNULL HT* cuHTCloneWithElements(CU_NOTNULL const HT* ht) {
	HT* retVal = cuHTNew();

	HTCell* el;
	HTCell* tmp;

	HASH_ITER(hh, ht->cell, el, tmp) {
		cuHTAddItem(retVal, el->id, ht->functions.clone(el->data));
	}

	return retVal;
}

/**
 *
 * \note
 * The data is added in the cell <b>by reference</b>
 *
 * @param[in] e the data to put inside the cell
 * @param[in] f a function with one parameter <tt>void*</tt> returning the hash of the value, aka an int
 * @return a cell of an hash table. You still need to manually add it in the hash table though
 */
static HTCell* newHTCell(const void* e, unsigned long key) {
	HTCell* retVal = malloc(sizeof(HTCell));
	if (retVal == NULL) {
		ERROR_MALLOC();
	}

	retVal->id = key;
	retVal->data = (void*) e;

	return retVal;
}

/**
 * Frees from the memory the structure \c htCell
 *
 * \attention
 * the function <b>won't free the pointer</b> ::HTCell::data
 *
 * @param[in] htCell the cell to remove from the memory
 */
static void destroyHTCell(CU_NOTNULL const HTCell* htCell, CU_NULLABLE const struct var_args* context) {
	CU_FREE(htCell);
}
