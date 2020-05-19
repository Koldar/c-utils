/**
 * @file
 *
 * XMacro file use to generate standard alias of common types for custom containers.
 *
 * In the project, we define a generic list. But then, for documentation purposes, you don't want to
 * use directly the list, but you would like to have types for "list of integer", "list of strings", "list of nodes".
 * Basically you have a generalized container and you want to generate a bunch of useful typedefs with common payloads.
 *
 * This xmacro file does exacly that. To use it, follow this code example:
 *
 * @code
 * #defein CU_STANDARD_CONTAINER_TYPE list
 * #define CU_STANDARD_CONTAINER_SUFFIX list
 * #include "standardContainer.xmacro.h"
 * //CU_STANDARD_CONTAINER_SUFFIX and CU_STANDARD_CONTAINER_TYPE is automatically undefined at the end of the xmacro
 * @endcode
 *
 * This will automatically create in your sources a list of new typedefs, like the following:
 *
 * @code
 * ///a list of integer in the generic pointer
 * typedef list pint_list;
 * ///a list of integer in their own memory space
 * typedef list int_list;
 * ///a list of boolean in the generic pointer
 * typedef list pbool_list;
 * ///a list of nodes
 * typedef list node_list;
 * @endcode
 *
 * CU_STANDARD_CONTAINER_TYPE and CU_STANDARD_CONTAINER_SUFFIX macro represent, respectively:
 *
 * @li CU_STANDARD_CONTAINER_TYPE: the name of the container type. This should be a valid C type identifier;
 * @li CU_STANDARD_CONTAINER_SUFFIX: the name you want to suffix to every typedef (e.g., if it's `pippo` we will generate `int_pippo` and so on).
 *
 * @attention
 * XMacro files does not contain guards since they can be added multiple times in the project!
 *
 * @author koldar
 * @date Sep 18, 2018
 */

#include "macros.h"
#include <P99/p99_paste.h>

//uncomment to debug this single file
//struct list;
//#define CU_STANDARD_CONTAINER_TYPE list
//#define CU_STANDARD_CONTAINER_SUFFIX list


#ifndef CU_STANDARD_CONTAINER_SUFFIX
#	error "To use this xmacro, you are require to define CU_STANDARD_CONTAINER_SUFFIX as well!"
#endif
#ifndef CU_STANDARD_CONTAINER_TYPE
#	error "To use this xmacro, you are require to define CU_STANDARD_CONTAINER_TYPE as well!"
#endif
#define _CU_STANDARDCONTAINER_FOREACH_OP(context, index, current, next) current next
#define _CU_STANDARDCONTAINER_FOREACH_FUNC(context, index, x) \
	typedef CU_STANDARD_CONTAINER_TYPE P99_PASTE(x, CU_STANDARD_CONTAINER_SUFFIX);

/*
 * if we have included <stdbool.h> before including this header the "bool" entry in the for each will automatically
 * become _Bool, hence you will have _Bool_list, _Bool_ht and so on. This is clearly wrong.
 * To patch it, we add in the foreach not the type names, but the type names suffixed with "_", to prevent macro expansion
 */


CU_FOR_EACH(, _CU_STANDARDCONTAINER_FOREACH_OP, _CU_STANDARDCONTAINER_FOREACH_FUNC, \
		int_,  /**integer stored in a newly allocated area accessible though the container*/ \
		pint_, /**integer stored in the void* reference of the container*/ \
		bool_, /**boolean  stored in a newly allocated area accessible though the container*/ \
		pbool_, /**boolean stored in the void* reference of the container*/ \
		char_, /**char stored in a newly allocated area accessible though the container*/ \
		pchar_, /**char stored in the void* reference of the container*/ \
		short_, /**short stored in a newly allocated area accessible though the container*/ \
		pshort_, /**short stored in the void* reference of the container*/ \
		float_, /**float stored in a newly allcoated area accessible though the container*/ \
		double_, /**double stored in a newly allocated area accessible though the container*/ \
		string_, /**string stored in a newly allocated area accessible though the container*/ \
		list_in_ /**list stored in a newly allocated area accessible through an element of this container*/ \
)


#undef CU_STANDARD_CONTAINER_SUFFIX
#undef CU_STANDARD_CONTAINER_TYPE
