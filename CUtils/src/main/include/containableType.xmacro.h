/**
 * @file
 *
 * XMacro file use to generate standard container alias for custom types.
 *
 * In the project, we define a generic type (e.g., node). When aggregating such custom type (e.g., list), for documentation purposes, you don't want to
 * use directly the list, but you would like to have types for "list of nodes", "list of edges", "list of graphs".
 * Basically you have a common container and you want to generate a bunch of useful typedefs with custom payloads.
 *
 * This xmacro file does exacly that. To use it, follow this code example:
 *
 * @code
 * #define CU_CONTAINABLE_TYPE node
 * #include "containableType.xmacro.h"
 * //CU_CONTAINABLE_TYPE is automatically undefined at the end of the xmacro
 * @endcode
 *
 * This will automatically create in your sources a list of new typedefs, like the following:
 *
 * @code
 * typedef list node_list;
 * typedef HT node_ht;
 * typedef hash_set node_hashset;
 * @endcode
 *
 * CU_CONTAINABLE_TYPE represents the type which will be contained in such common data structures.
 *
 * @attention
 * XMacro files does not contain guards since they can be added multiple times in the project!
 *
 * @author koldar
 * @date Sep 18, 2018
 */

#include "macros.h"
#include <P99/p99_paste.h>

//enable for debugging this single file
//#define CU_CONTAINABLE_TYPE node

#ifndef CU_CONTAINABLE_TYPE
#	error "To use this xmacro, you are require to define CU_CONTAINABLE_TYPE as well!"
#endif
#define _CU_CONTAINABLE_TYPE_PAIR_OP(context, index, current, next) current next
#define _CU_CONTAINABLE_TYPE_PAIR_FUNC(context, index, type, suffix) \
	struct type; \
	typedef struct type P99_PASTE(CU_CONTAINABLE_TYPE, _, suffix);



CU_FOR_PAIR(, _CU_CONTAINABLE_TYPE_PAIR_OP, _CU_CONTAINABLE_TYPE_PAIR_FUNC, \
		list, list, \
		hash_set, hashset, \
		HT,	ht, \
		heap, heap, \
		naive_queue, naivequeue, \
		queue, queue \
)

#undef CU_CONTAINABLE_TYPE
