/**
 * @file
 *
 * Contains all the instruction to operate with edges
 *
 *  Created on: Dec 22, 2016
 *      Author: koldar
 */

#ifndef EDGE_H_
#define EDGE_H_

#include "hashtable.h"
#include "list.h"
#include "var_args.h"
#include <stdbool.h>


struct Node;
struct PredSuccGraph;

/**
 * Represents a direct edge
 *
 * \dotfile edge01.dot
 */
typedef struct Edge {
	struct Node* source;
	struct Node* sink;
	void* payload;
} Edge;

typedef list EdgeList;
typedef HT EdgeHT;

/**
 * Creates an edge
 *
 * @param[in] source the source of the edge
 * @param[in] sink the sink of the edge
 * @param[in] payload the label of the edge
 * @return an edge in the heap. Be sure to clear it out with ::destroyEdge when not needed anymore
 */
Edge* newEdge(const struct Node* restrict source, const struct Node* restrict sink, const void* payload);

/**
 * Clone an edge. Payload is copied by referend though.
 *
 * If you want to clone the payload as well, please use ::cloneEdgeWithPayload
 * \note
 * ::Edge::source,  ::Edge::sink and ::Edge::payload are not cloned; only copied by reference
 *
 * @param[in] e the edge to clone
 * @return a new copy of the edge
 */
Edge* cloneEdge(const Edge* e);

/**
 * Clone an edge. Payload included
 *
 * \note
 * ::Edge::source,  ::Edge::sink are not cloned; only copied by reference
 *
 * @param[in] e the edge to clone
 * @param[in] payloadCloner the function to use to clone the edge payload
 * @return a new copy of the edge
 */
Edge* cloneEdgeWithPayload(const Edge* e, cloner payloadCloner);

/**
 * Destroy an edge
 *
 * \warning
 * The function <b>won't free</b> the memory for the soource nor the sink of the edges. It will simply remove the edge itself
 *
 * @param[in] e the edge to remove
 */
void destroyEdge(const Edge* e, CU_NULLABLE const struct var_args* context);
#define CU_FUNCTION_POINTER_destructor_void_destroyEdge_voidConstPtr_var_argsConstPtr CU_DESTRUCTOR_ID

/**
 * Destroy an edge with the associated payload
 *
 * \warning
 * The function <b>won't free</b> the memory for the source nor the sink of the edges. It will simply remove the edge itself
 *
 * @param[in] e the edge to remove
 * @param[in] payloadDestructor the function to use to destroy the payload
 */
void destroyEdgeWithPayload(const Edge* e, destructor payloadDestructor);

/**
 * \note
 * Hash does not take into account payload at all
 *
 * @param[in] e the edge involved
 * @return an hash representation of the edge \c e
 */
int getHashOfEdge(const Edge* e);

/**
 * \note
 * note that the edge will check only the source and the sink of the 2 edges, not the ::Edge::payload!
 *
 * The function **won't check** the payload content, only the source and the sink of the edges.
 * If you want to also check the payload, please use ::getEdgeCmpWithPayload
 *
 * @param[in] e1 the first edge. Can be  NULL
 * @param[in] e2 the second edge. Can be NULL
 * @return
 * 	\li true if the edges are equals;
 * 	\li false otherwise
 */
bool getEdgeCmp(const Edge* e1, const Edge* e2);

/**
 * Check that 2 edges are the same **and** with the same payload
 *
 * @param[in] e1 the first edge. Can be NULL
 * @param[in] e2 the second edge. Can be NULL
 * @param[in] payloadComparator the function to use to compare 2 payload edges
 * @return
 * 	\li true if the edges are equals (and with the same payload);
 * 	\li false otherwise
 */
bool getEdgeCmpWithPayload(const Edge* e1, const Edge* e2, comparator payloadComparator);

/**
 * retrieve the buffer popolated by the string representation of \c e
 *
 * @param[in] e the edge whose string representation we're interested in
 * @param[inout] buffer the buffer that will accept the representation
 * @return the number of characters we have consumed in the buffer (or pointer to '\0')
 */
int getBufferStringOfEdge(const Edge* e, char* buffer);

void storeEdgeInFile(FILE* f, const Edge* e, object_serializer payload);

Edge* loadEdgeFromFile(FILE* f, struct PredSuccGraph* g, object_deserializer edgeDeserializer);

/**
 * A set of function allowing you to easily manage ::Edge structure when it's stored in a container
 *
 * @return the associated ::payload_functions
 */
payload_functions cuEdgeGetPayoadFunctions();

/**
 * Get the payload of the edge already casted
 *
 * @param[in] edge a point of ::Edge
 * @param[in] type the type of the payload. example may be \c int, \c Foo
 */
#define getEdgePayloadAs(edge, type) ((type)(edge)->payload)

#endif /* EDGE_H_ */
