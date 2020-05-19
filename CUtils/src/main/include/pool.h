/*
 * @file
 *
 * An implementation of the Pool pattern
 *
 *  Created on: Jul 29, 2019
 *      Author: koldar
 */

#ifndef POOL_H_
#define POOL_H_

#include "var_args.h"

struct pool;

struct pool* _cuPoolNew(int poolSize, size_t elementSize, hashfunction_t hashFunction);

void cuPoolDestroy(const struct pool* p);

void cuPoolDestroyWithElements(const struct pool* p, CU_NULLABLE const struct var_args* context);

void cuPoolAddItem(const struct pool* p, CU_NULLABLE const void* item);

void cuPoolClear(const struct pool* p);

bool cuPoolHasItem(const struct pool* p, const void* item, CU_NULLABLE const);

#endif /* POOL_H_ */
