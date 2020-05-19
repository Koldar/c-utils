/**
 * @file
 *
 * A node inside a graph
 *
 * Suppose you have a graph, like this one:
 *
 * @dot
 * digraph {
 * 	X;Y;Z
 *
 * 	X->Y;
 * 	Y->Z;
 * 	X->Z;
 * }
 * @enddot
 *
 * <tt>X,Y,Z</tt> are nodes and they are handled by this module. The main structure is ::Node.
 *
 *  Created on: Dec 22, 2016
 *      Author: koldar
 */

#ifndef NODE_H_
#define NODE_H_

#include <stdlib.h>
#include <stdbool.h>

#include "list.h"
#include "edge.h"
#include "utility.h"
#include "hashtable.h"
#include "hash_set.h"

struct PredSuccGraph;

/**
 * The type of the ::Node::id
 */
typedef unsigned long NodeId;

//TODO remove
typedef list NodeList;
//TODO remove
typedef HT NodeHT;

#define CU_CONTAINABLE_TYPE node
#include "containableType.xmacro.h"

/**
 * A single node within the graph
 *
 * This structure is used to represents the graph via a successors/predecessors notation, not via a adjacent matrix.
 *
 * Nodes may have a payload, aka some information natively associated to the graph vertex itself.
 * While it is not mandatory to have a payload (it might be NULL), having a payload can help exploit graph sturcture for your
 * need.
 *
 * You can access the payload with ::getNodePayloadAs.
 * If you have a payload, be sure to populate correctly the following global variables:
 * \li ::PredSuccGraph::nodeFunctions;
 *
 * They are used inside the graph code during graph destruction, graph comparation and other stuff.
 * Failing to do will likely to generate unknown behaviour or memory leaks.
 */
typedef struct Node {
	///ID that uniquely identiies the node throughout all the graph
	NodeId id;
	/**
	 * Hash table containing all the edges starting from this node
	 *
	 * The key of the hashtable are the sink id of the edges while the values of the hashtables are the edges themselves
	 */
	EdgeHT* successors;
	/**
	 * Hash table containing all the edges ending in this node
	 *
	 * The key of the hashtable are the source id of the edges while the values are the edges themselves.
	 * This value is used only when the node is created via
	 */
	CU_NULLABLE EdgeHT* predecessors;
	/**
	 * the payload associated to this node. Can be NULL
	 */
	void* payload;
} Node;

/**
 * Create a new node
 *
 * @param[in] id the id of the node. Must be unique for every nodes
 */
Node* newNode(const NodeId id);

/**
 * like ::newNodeWithPayload but we can enable both predecessors and successors link
 *
 * if @c predecessorsEnable is true, the memory usage will increment
 *
 * @param[in] id the id of the node to create
 * @param[in] payload the data to assign to this node
 * @param[in] predecessorsEnable true if we want to keep trace of the predecessor of a node as well: false otherwise;
 * @return a new instance of the node
 */
Node* newPredSuccNode(NodeId id, CU_NULLABLE const void* payload, bool predecessorsEnable);

/**
 * Like ::newNode but it set also the payload
 *
 * @param[in] id the id of the node. Must be unique for every nodes
 * @param[in] payload the payload to associate to this node
 */
CU_NOTNULL Node* newNodeWithPayload(const NodeId id, CU_NULLABLE const void* payload);

/**
 * like ::newNode but the id is assigned automatically
 */
Node* newNodeAuto();

/**
 * Destroy the node (payload included)
 *
 * The function will remove the node and the edges of the node. Its successors will be left untouched:
 * this <b>will lead to inconsistencies inside ::Node::successors structure!</b>
 *
 * \attention
 * Use this function only if you want to delete not only this node, but also its successors and predecessors!
 *
 * @param[in] node the node to remove from memory
 * @param[in] nodePayloadDestructor the function to use to free the payload of the node from the memory
 * @param[in] edgePayloadDestructor the function to use to free the paylaod of the edges
 */
void destroyNodeWithPayload(const Node* node, destructor nodePayloadDestructor, destructor edgePayloadDestructor);

/**
 * Add an edge in the successor list of the node
 *
 * \note if an edge is already present betwen the 2 nodes, it will be replaced
 *
 * @param[inout] source the source of the edge
 * @param[in] sink the sink of the edge
 * @param[in] edgePayload the label we need to put on the edge to add
 * @return the edge just added
 */
CU_NOTNULL Edge* addEdgeInNode(CU_NOTNULL Node* restrict source, const CU_NOTNULL Node* restrict sink, const CU_NULLABLE void* edgePayload);

/**
 * Add an edge in the successor list of the node
 *
 * \note
 * if an edge is already present between the 2 nodes, it will be replaced
 *
 * @param[inout] source the node that will contain the new edge
 * @param[in] e the edge to add
 * @return \c e itself;
 */
CU_NOTNULL Edge* addEdgeDirectlyInNode(CU_NOTNULL Node* source, const CU_NOTNULL Edge* e);

/**
 * Removes an edge from the 2 nodes
 *
 * \post
 * 	\li the edge is removed from the memory as well
 *
 * @param[in] source the source of the edge
 * @param[in] sink the sink of the edge
 */
void removeEdgeInNode(Node* restrict source, Node* restrict sink);

/**
 * Find the edge between 2 node
 *
 * @param[in] source the source fo the edge
 * @param[in] sink the sink of the edge
 * @return
 * 	\li the edge between 2 nodes;
 * 	\li NULL if there is no edge between the 2 nodes;
 */
Edge* findEdgeInNode(const Node* restrict source, const Node* restrict sink);

/**
 * Get all the edges whose source is \c source
 *
 * \attention
 * do not edit this hashtable, becuase such changes will be made to the actual node!
 *
 * @param[in] source the node involved
 * @return a hashtable containing all the edges whose source is \c source
 */
EdgeHT* getEdgeHTOfNode(const Node* source);

/**
 * fetch the number of successors of the node \c source
 *
 * @param[in] source the node to analyze
 * @return the size of the output of ::getEdgeHTOfNode
 */
int getNumberOfEdgesInNode(const Node* source);

/**
 * get the number fo predecessor a vertex has
 *
 * @pre
 *  @li @c g must have enabled the rpedecessors feature
 *
 * @param[in] g the graph to operate on
 * @param[in] id the id of the vertex to check
 * @return the number of predecessors the vertex has
 */
int cuNodeGetPredecessorNumberOfVertex(CU_NOTNULL const Node* n);

/**
 * get the only predecessor the given node has
 *
 * @pre
 *  @li the node @c n has only one predecessor
 *
 * @param[in] n the node to handle
 * @return
 *  @li the predecessor of @c n
 */
CU_NOTNULL Node* cuNodeGetPredecessor(CU_NOTNULL const Node* n);

/**
 * Get all the edges whose source is \c source
 *
 * \note
 * You can add and remove edges in this list, but don't edit
 * the original edges, since you will alter the original structure!
 *
 * \attention
 * be sure to destroy the list after the use!
 *
 * @param[in] source the node involved
 * @return a list containing all the edges whose source is \c source
 */
EdgeList* getEdgeListOfNode(const Node* source);

/**
 * The has of a node
 *
 * @param[in] n the node involved
 * @return \c n hash code
 */
int getHashOfNode(const Node* n);

/**
 * Clone node
 *
 * \note
 * The payload will be copied by reference. If you want to copy the payload as well use ::cloneNodeWithPayload
 *
 * @param[in] n the node to clone
 * @return the cloned node
 */
Node* cloneNode(const Node* n);

/**
 * Clone node (payload included
 *
 *
 * @param[in] n the node to clone
 * @param[in] payloadCloner the function to use to clone the node payload
 * @return the cloned node
 */
Node* cloneNodeWithPayload(const Node* n, cloner payloadCloner);

/**
 * Compare 2 different nodes
 *
 * @param[in] a the first node to compare
 * @param[in] b the second node to compare
 * @param[in] payloadComparator the function to use to compare the nodes payloads
 * @return
 * 	\li true if the 2 nodes are the same;
 * 	\li false otherwise
 */
bool compareNodes(const Node* a, const Node* b, comparator payloadComparator);

/**
 * Load a node from file
 *
 * @param[inout] f the file where to read the node
 * @param[in] enablePredecessors true if you want that this node will have capabilities to store not only successors, but also predecessors;
 * @param[in] nodeDeserializer a function allowing you to read from the file the payload of the node
 * @return a new heap value containing the node (and its payload as well);
 */
Node* loadNodeFromFileWithoutEdges(FILE* f, bool enablePredecessors, object_deserializer nodeDeserializer);

void storeNodeInFileWithoutEdges(FILE* f, const Node* n, object_serializer nodeSerializer);

/**
 * Check if the given node has the predecessors active
 *
 * @param[in] n the node to check
 * @return
 *  @li true if we have enabled the predecessors;
 *  @li false otherwise;
 */
bool nodeHasPredecessorsActive(CU_NOTNULL const Node* n);

/**
 * Get the payload of the node already casted
 *
 * @param[in] node a point of ::Node
 * @param[in] type the type of the payload. example may be \c int, \c Foo
 */
#define getNodePayloadAs(node, type) ((type)node->payload)

#endif /* NODE_H_ */
