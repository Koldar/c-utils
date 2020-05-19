/*
 * hash_set.c
 *
 *  Created on: Dec 21, 2017
 *      Author: koldar
 */

#include "hash_set.h"
#include "hashtable.h"
#include "macros.h"
#include "log.h"
#include "errors.h"

struct hash_set {
	HT* hashTable;
	payload_functions functions;
};


hash_set* cuHashSetNew(payload_functions functions) {
	hash_set* retVal = malloc(sizeof(hash_set));
	if (retVal == NULL) {
		ERROR_MALLOC();
	}

	retVal->hashTable = cuHTNew();
	retVal->functions = functions;

	return retVal;
}

bool cuHashSetAddItem(CU_NOTNULL hash_set* set, const void* item) {
	return cuHTAddOrUpdateItem(set->hashTable, (unsigned long)item, item);
}

bool cuHashSetContainsItem(CU_NOTNULL const hash_set* set, const void* item) {
	return cuHTContainsItem(set->hashTable, (unsigned long)item);
}

void cuHashSetRemoveItem(CU_NOTNULL hash_set* set, const void* item) {
	cuHTRemoveItem(set->hashTable, (unsigned long)item);
}

void cuHashSetDestroy(CU_NOTNULL const hash_set* set, CU_NULLABLE const struct var_args* context) {
	cuHTDestroy(set->hashTable, context);
	CU_FREE(set);
}

void cuHashSetDestroyWithElements(const CU_NOTNULL hash_set* set, CU_NULLABLE const struct var_args* context) {
	cuHTDestroyWithElements(set->hashTable, set->functions.destroy);
	CU_FREE(set);
}

size_t cuHashSetGetSize(CU_NOTNULL const hash_set* set) {
	return cuHTGetSize(set->hashTable);
}

bool cuHashSetIsEmpty(CU_NOTNULL const hash_set* set) {
	return cuHTIsEmpty(set->hashTable);
}

hash_set* cuHashSetGetUnionOfHashSets(CU_NOTNULL hash_set* restrict set1, CU_NOTNULL const hash_set* restrict set2, bool inPlace) {
	hash_set* retVal;

	if (inPlace) {
		retVal = set1;

		CU_ITERATE_OVER_HASHSET(set2, var, void*) {
			if (!cuHashSetContainsItem(retVal, var)) {
				cuHashSetAddItem(retVal, var);
			}
		}
	} else {
		const hash_set* minSizedSet = cuHashSetGetSize(set1) < cuHashSetGetSize(set2) ? set1 : set2;
		const hash_set* maxSizedSet = cuHashSetGetSize(set1) >= cuHashSetGetSize(set2) ? set1 : set2;

		retVal = cuHashSetCloneByReference(maxSizedSet);

		CU_ITERATE_OVER_HASHSET(minSizedSet, var, void*) {
			if (!cuHashSetContainsItem(retVal, var)) {
				cuHashSetAddItem(retVal, var);
			}
		}
	}

	return retVal;
}


hash_set* cuHashSetGetIntersectionOfHashSets(CU_NOTNULL hash_set* restrict set1, CU_NOTNULL const hash_set* restrict set2, bool inPlace) {
	hash_set* retVal;

	if (inPlace) {
		retVal = set1;

		CU_VARIABLE_ITERATE_OVER_HASHSET(retVal, var, void*) {
			if (!cuHashSetContainsItem(set2, var)) {
				cuHashSetRemoveItem(retVal, var);
			}
		}
	} else {
		retVal = cuHashSetNew(set1->functions);

		//the ">=" handle the scenario where the sets have the same size
		const hash_set* minSizedSet = cuHashSetGetSize(set1) < cuHashSetGetSize(set2) ? set1 : set2;
		const hash_set* maxSizedSet = cuHashSetGetSize(set1) >= cuHashSetGetSize(set2) ? set1 : set2;

		CU_VARIABLE_ITERATE_OVER_HASHSET(minSizedSet, var, void*) {
			if (cuHashSetContainsItem(maxSizedSet, var)) {
				cuHashSetAddItem(retVal, var);
			}
		}
	}

	return retVal;
}

hash_set* cuHashSetGetDifferenceOfHashSets(CU_NOTNULL hash_set* restrict set1, CU_NOTNULL const hash_set* restrict set2, bool inPlace) {
	hash_set* retVal;

	if (inPlace) {
		retVal = set1;
	} else {
		retVal = cuHashSetCloneByReference(set1);
	}

	CU_ITERATE_OVER_HASHSET(retVal, var, void*) {
		if (cuHashSetContainsItem(set2, var)) {
			cuHashSetRemoveItem(retVal, var);
			if (cuHashSetIsEmpty(retVal)) {
				return retVal;
			}
		}
	}

	return retVal;
}

void cuHashSetClear(const hash_set* set) {
	cuHTClear(set->hashTable);
}

void cuHashSetClearWithElements(CU_NOTNULL const hash_set* set) {
	cuHTClearWithElements(set->hashTable, set->functions.destroy);
}

int cuHashSetGetBufferString(CU_NOTNULL const hash_set* set, char* buffer) {
	int i=0;

	i += sprintf(&buffer[i], "{");

	int count = 0;
	CU_ITERATE_OVER_HASHSET(set, var, void*) {
		count++;
		i += set->functions.bufferString(var, &buffer[i]);
		if (count != cuHashSetGetSize(set)) {
			i += sprintf(&buffer[i], ", ");
		}
	}

	i += sprintf(&buffer[i], "}");

	return i;
}

hash_set* cuHashSetCloneByReference(CU_NOTNULL const hash_set* set) {
	hash_set* retVal = cuHashSetNew(set->functions);

	CU_ITERATE_OVER_HASHSET(set, var, void*) {
		cuHashSetAddItem(retVal, var);
	}

	return retVal;
}

hash_set* cuHashSetClone(CU_NOTNULL const hash_set* set) {
	hash_set* retVal = cuHashSetNew(set->functions);

	CU_ITERATE_OVER_HASHSET(set, var, void*) {
		cuHashSetAddItem(retVal, set->functions.clone(var));
	}

	return retVal;
}

void* cuHashSetGetAnItem(CU_NOTNULL const hash_set* set) {
	return cuHTGetFirstItem(set->hashTable);
}

HT* _cuHashSetGetUnderlyingHashTable(CU_NOTNULL const hash_set* set) {
	return set->hashTable;
}
