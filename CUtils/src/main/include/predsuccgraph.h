/**
 * @file
 *
 * @date Dec 22, 2016
 * @author koldar
 */

#ifndef PREDSUCCGRAPH_H_
#define PREDSUCCGRAPH_H_

#include "utility.h"
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include "typedefs.h"

#include "node.h"
#include "edge.h"

/**
 * A graph containing nodes. Edges are retrieved observing a node configuring as source
 */
typedef struct PredSuccGraph {
	/**
	 * An hastable containing all the nodes inside the graph.
	 *
	 * The keys of the hastables are the IDs of the nodes whilst the values inside the ahstables
	 * are the nodes themselves.
	 *
	 * The data inside this array has a meaning only till the ::PredSuccGraph::size -th cell
	 */
	NodeHT* nodes;
	///the actual size of the graph
	unsigned int size;
	payload_functions nodeFunctions;
	payload_functions edgeFunctions;
	/**
	 * if true, each node in the graph will populate not only the successors, but also their predecessors
	 *
	 * it's a strategy that will increase the amount of memory and CPU needed
	 */
	bool enablePredecessors;
	//TODO remove
//	/**
//	 * The function to use to compare the payload of 2 nodes
//	 *
//	 */
//	comparator nodePayloadComparator;
//	/**
//	 * The function to use to destroy the layload of an node
//	 *
//	 */
//	destructor nodePayloadDestructor;
//	/**
//	 * The function to use to compute the string representation of the paylaod of an node
//	 *
//	 */
//	buffer_string nodePayloadBufferStringer;
//	/**
//	 * The function to use to clone the payload of a node
//	 */
//	cloner nodePayloadCloner;
//	/**
//	 * The function to use when in images we need to choose a color for the node
//	 */
//	colorer nodePayloadColorer;
//	object_serializer nodePayloadSerializer;
//	object_deserializer nodePayloadDeserializer;
//	/**
//	 * The function to use to compare the payload of 2 edges
//	 *
//	 */
//	comparator edgePayloadComparator;
//	/**
//	 * The function to use to destroy the layload of an edge
//	 *
//	 */
//	destructor edgePayloadDestructor;
//	/**
//	 * The function to use to compute the string representation of the paylaod of an edge
//	 *
//	 */
//	buffer_string edgePayloadBufferStringer;
//	/**
//	 * The function to use to clone the payload of an edge
//	 *
//	 */
//	cloner edgePayloadCloner;
//	/**
//	 * The function to use when in images we need to choose a color for the edge
//	 */
//	colorer edgePayloadColorer;
//	object_serializer edgePayloadSerializer;
//	object_deserializer edgePayloadDeserializer;

} PredSuccGraph;

/**
 * Initialize a new graph
 *
 * @param[in] enablePredecessors true if you want that every nodes stores not only its successors, but also its predecessors;
 * @param[in] vertexPayload payload functions used to easily manage the payload of each vertex
 * @param[in] edgePaylaod paylaod functions used to easily manage paylaod of each edge
 * @return an instance of the newly created graph
 */
PredSuccGraph* cuPredSuccGraphNew(bool enablePredecessors, payload_functions vertexPayload, payload_functions edgePayload);
CU_DECLARE_FUNCTION_WITH_DEFAULTS(PredSuccGraph*, cuPredSuccGraphNew, bool, payload_functions, payload_functions);
#define cuPredSuccGraphNew(...) CU_CALL_FUNCTION_WITH_DEFAULTS(cuPredSuccGraphNew, 3, __VA_ARGS__)
CU_DECLARE_DEFAULT_VALUES(cuPredSuccGraphNew,
        false,
		cuPayloadFunctionsDefault(),
		cuPayloadFunctionsDefault()
);

/**
 * Completely copy the whole graph
 *
 * \attention ensure to call ::cuPredSuccGraphDestroyWithElements after using the return value of the function to avoid memory leaks
 *
 * @param[in] graph the graph to copy
 * @return a clone of the graph. Nodes, Edges wil be copied as well
 */
PredSuccGraph* cuPredSuccGraphClone(CU_NOTNULL const PredSuccGraph* graph);

/**
 * Destroy the whole graph
 *
 * \attention all the ::Node instances and the ::Edge instance of the graph will be removed as well!
 *
 * @param[in] g the graph to dispose
 */
void cuPredSuccGraphDestroyWithElements(const PredSuccGraph* g, CU_NULLABLE const struct var_args* context);
#define CU_FUNCTION_POINTER_destructor_void_cuPredSuccGraphDestroyWithElements_voidConstPtr_var_argsConstPtr CU_DESTRUCTOR_ID

/**
 * Adds a node within the graph
 *
 * \pre
 * 	\li the node has unique identifier set to ::PredSuccGraph::size;
 * \post
 * 	\li ::PredSuccGraph::size increased by one;
 *
 * @param[in] g the graph involved
 * @param[in] n the node to add inside the graph
 */
//TODO remove
void _cuPredSuccGraphAddVertexInstanceInGraph(CU_NOTNULL PredSuccGraph* g, CU_NOTNULL const Node* n);

/**
 * Adds a node within the graph
 *
 * @param[in] g the graph to involved
 * @param[in] id the id of the node to create
 * @param[in] payload the paylaod associated to the node
 * @return the node you've just created
 */
CU_NOTNULL Node* cuPredSuccGraphAddNodeInGraphById(CU_NOTNULL PredSuccGraph* g, NodeId id, CU_NULLABLE const void* payload);

/**
 * Get the node in the graph
 *
 * @param[in] g the graph involved
 * @param[in] id the unique identifier of the node you want to fetch
 * @return
 * 	\li the ::Node instance;
 * 	\li NULL if there is no such node
 */
Node* cuPredSuccGraphGetNodeById(const PredSuccGraph* g, const NodeId id);

/**
 * Get the number of successors of the given node in the graph
 *
 * @param[in] g the graph to analyze
 * @param[in] id the id of the node to consider
 * @return the number of successors of the node with id \c id inside the graph \c g
 */
int cuPredSuccGraphGetNodeOutDegree(const PredSuccGraph* g, NodeId id);

/**
 * Check if one node is reachable from another one
 *
 * \note
 * the function check if it exists a path from \c sourceid till \c sinkId
 *
 * This is not a particular efficient routine, so do not use it if performances are important!
 *
 * @param[in] g the graph where the 2 nodes are located
 * @param[in] sourceId the id of the node where to start the reachability question
 * @param[in] sinkId the id of the node to reach
 * @param[in] traverser a function allowing the function to understand if an edge can be traversed or not
 * @return
 * 	\li \c true if it exist a path from \c sourceId till \c sinkId
 * 	\li \c false otherwise
 */
bool cuPredSuccGraphIsVertexReachableFromVertex(const PredSuccGraph* g, NodeId sourceId, NodeId sinkId, bool (*traverser)(const Edge* edge));

/**
 * Get the first node in a set of nodes that isn't a descendant of a given node
 *
 * @definition
 * Descendant: either a direct or an indirect successor of a particular node
 *
 * @pre
 *  @li therre cannot be a cycle that start from @c id and, through its successors, go to @c id again
 *
 * @dot
 * digraph {
 * 	rankdir="TB";
 * 	A [fillcolor="cyan"];
 * 	B [fillcolor="cyan"];
 * 	C [fillcolor="cyan"];
 * 	D, E, F;
 *
 * 	D -> E -> A -> B;
 * 	D -> F-> B;
 * 	C -> E;
 * }
 * @enddot
 *
 * In the figure, if the vertex is @c D and the possible descendants are <tt>A,B,C</tt>, the function would return <tt>C</tt>
 * since it's the only vertex which is not a descendant of <tt>D</tt>.
 *
 * @param[in] g the graph we need to operate on
 * @param[in] id the id of the vertex who we need to compute the descedants from
 * @param[in] possibleDescendantIds a list containing all the possible descendant we're handling
 * @param[in] traverser a function that, given an edge, detetermines if we can traverse the edge or not during the resolution of the function
 * @param[inout] output a pointer to the stack representing the output of the function. if it returns true, then this field will be populated with the id of the node in @c possibleDescendantsIds which is not a descendant of node with id @c id
 * @return
 *  @li true if @c output is populated;
 *  @li false otherwise
 *
 */
bool cuPredSuccGraphGetFirstVertexWhichIsNotDescendantOf(CU_NOTNULL const PredSuccGraph* g, NodeId id, const CU_NOTNULL pint_hash_set* possibleDescendantIds, bool (*traverser)(CU_NOTNULL const Edge* edge), CU_NOTNULL NodeId* output);

/**
 * get the first predecessor of a given node
 *
 * @note
 * no garantuees are made for which node is picked as return value if the node @c n has multiple rpedecessors
 *
 * @param[in] g the graph to consider
 * @param[in] n the node to consider
 * @return
 * 	@li a predecessor of node @c n;
 * 	@li NULL if @c n has no predecessors
 */
CU_NULLABLE Node* cuPredSuccGraphGetFirstPredecessorOfVertexInstance(CU_NOTNULL const PredSuccGraph* g, CU_NOTNULL const Node* n);

/**
 * get the first predecessor of a given node
 *
 * @note
 * no garantuees are made for which node is picked as return value if the node with @c id has multiple rpedecessors
 *
 * @param[in] g the graph to consider
 * @param[in] id the id of the node to consider
 * @return
 * 	@li a predecessor of the node whose id is @c id;
 * 	@li NULL if the node with id @c id has no predecessors
 */
CU_NULLABLE Node* cuPredSuccGraphGetFirstPredecessorOfVertex(CU_NOTNULL const PredSuccGraph* g, NodeId id);

/**
 * check if a node has any successors
 *
 * @param[in] g the graph to operate on
 * @param[in] id the id of the vertex to check
 * @return
 *  @li true if the vertex represented by @c id has no successors;
 *  @li false otherwise;
 */
bool cuPredSuccGraphHasVertexNoSuccessors(CU_NOTNULL const PredSuccGraph* g, NodeId id);

/**
 * check if a node has any successors
 *
 * @param[in] g the graph to operate on
 * @param[in] n the vertex to check
 * @return
 *  @li true if the vertex @c n has no successors;
 *  @li false otherwise;
 */
bool cuPredSuccGraphHasVertexInstanceNoSuccessors(CU_NOTNULL const PredSuccGraph* g, Node* n);

/**
 * check if a node has any predecessors
 *
 * @pre
 *  @li @c g must have enabled the rpedecessors feature
 *
 * @param[in] g the graph to operate on
 * @param[in] id the id of the vertex to check
 * @return
 *  @li true if the vertex represented by @c id has no predecessors;
 *  @li false otherwise;
 */
bool cuPredSuccGraphHasVertexNoPredecessors(CU_NOTNULL const PredSuccGraph* g, NodeId id);

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
int cuPredSuccGraphGetPredecessorNumberOfVertex(CU_NOTNULL const PredSuccGraph* g, NodeId id);

/**
 * Adds an edge between 2 nodes
 *
 * @dotfile edge01.dot
 *
 * @param[in] g the graph to alter
 * @param[in] edge the edge to add inside the graph.
 * @return \c edge itself
 */
Edge* _cuPredSuccGraphAddEdgeDirectly(PredSuccGraph* g, const Edge* edge);

/**
 * Adds an edge between 2 nodes
 *
 * \dotfile edge01.dot
 *
 * @param[inout] source the source node
 * @param[in] sink the sinnk node
 * @param[in] edgePayload the label on the edge we need to add
 * @return the edge just added
 */
CU_NOTNULL Edge* _cuPredSuccGraphAddEdge(CU_NOTNULL Node* restrict source, CU_NOTNULL Node* restrict sink, CU_NULLABLE const void* edgePayload);

/**
 * Adds an edge between 2 nodes
 *
 * \dotfile edge01.dot
 *
 * @param[inout] graph the graph to alter
 * @param[in] sourceId the id of the source node
 * @param[in] sinkId the id of the sink node
 * @param[in] edgePayload the label on the edge we need to add
 * @return the edge just added
 */
CU_NOTNULL Edge* cuPredSuccGraphAddEdge(CU_NOTNULL PredSuccGraph* graph, NodeId sourceId, NodeId sinkId, CU_NULLABLE const void* edgePayload);

/**
 * Removes an edge between 2 nodes
 *
 * \dotfile edge01.dot
 *
 * The function does nothing if there is not edge between \c source and \c sink
 *
 * \post
 * 	\li the edge between the 2 will be removed (even in the memory);
 *
 * @param[in] source the source node
 * @param[in] sink the sink node
 */
void _cuPredSuccGraphRemoveEdge(Node* restrict source, Node* restrict sink);

/**
 * Like ::_cuPredSuccGraphRemoveEdge but operates on ids, not on the concrete node instances.
 *
 * Useful if 2 different graphs shares the same layout
 *
 * \post
 * 	\li \c graph will not contain the edge "sink -> source";
 * 	\li if the edge was not present in the first place, nothing changes: the function does nothing;
 *
 * \note
 * The edge is removed from the memory as well;
 *
 * @param[inout] graph where we need to remove an edge;
 * @param[in] sourceId the id of the source of the edge;
 * @param[in] sinkId the id of the sink of the edge;
 * @param[in] removeFlipped true if tyou want to remove not only source->sink but also sink->source
 */
void cuPredSuccGraphRemoveEdge(PredSuccGraph* graph, const NodeId sourceId, const NodeId sinkId, const bool removeFlipped);

/**
 *
 * Removes an edge from the graph
 *
 * The function won't delete the edge from memory however
 *
 * @dotfile edge01.dot
 *
 * @param[in] source the source of the node
 * @param[in] sink the sink of the node
 * @return
 * 	\li the edge between the 2;
 * 	\li NULL if there is no edge between the 2
 */
Edge* _cuPredSuccGraphGetEdge(const Node* restrict source, const Node* restrict sink);

/**
 * Like ::_cuPredSuccGraphGetEdge, but uses ::Node id instead of ::Node intances
 *
 * @param[in] graph the graph involved
 * @param[in] sourceId the id of the source node
 * @param[in] sinkId the id of the sink node
 * @return
 * 	\li the edge between the 2;
 * 	\li NULL if  there is no edge between the 2
 */
Edge* cuPredSuccGraphGetEdgeInGraph(const PredSuccGraph* graph, const NodeId sourceId, const NodeId sinkId);

/**
 * get the payload of the edge going from `sourceId` till `sinkId`.
 *
 * The function will raise an error if the edge does not exist
 *
 * @param[in] graph graph involved
 * @param[in] sourceId the id of the source node
 * @param[in] sinkId the id of the sink node
 * @return pointer to the edge payload
 */
void* _cuPredSuccGraphGetEdgePayloadInGraph(CU_NOTNULL const PredSuccGraph* graph, NodeId sourceId, NodeId sinkId);

/**
 * like ::_cuPredSuccGraphGetEdgePayloadInGraph but it automatically cast the return value
 *
 * @param[in] graph graph involved
 * @param[in] sourceId the id of the source node
 * @param[in] sinkId the id of the sink node
 * @param[in] payloadType type to cast the value in the edge
 * @return pointer to the edge payload
 */
#define cuPredSuccGraphGetEdgePayloadInGraph(graph, sourceId, sinkId, payloadType) \
		((payloadType)_cuPredSuccGraphGetEdgePayloadInGraph(graph, sourceId, sinkId))

/**
 * @param[in] graph the graph to check
 * @param[in] sourceId the id of the source of the edge to check
 * @param[in] sinkId the id of the sink of the edge to check
 * @return
 * 	\li true if there is an edge going from \c sourceId to \c sinkid
 * 	\li false otherwise
 */
bool cuPredSuccGraphContainsEdgeInGraph(const PredSuccGraph* graph, NodeId sourceId, NodeId sinkId);

/**
 * Generates an image of the graph
 *
 * The function creates a temporary "*.dot" file, but such file is also automatically deleted.
 *
 * \attention
 * this function required "circo" to be installed on the system. Please install it
 * via <tt>sudo apt-get install graphviz</tt>
 *
 * \post
 * 	\li in the CWD a .png file is present;
 *
 * @param[in] graph the graph involved
 * @param[in] fileName the filename (with no extensions) to create
 */
void cuPredSuccGraphPrintGraph(PredSuccGraph* graph, const char* fileName);

/**
 * Like ::cuPredSuccGraphPrintGraph but it accept an additional string
 *
 * Useful for debugging purposes
 *
 * @param[in] graph the graph involved
 * @param[in] fileName the filename (with no extensions) to create
 * @param[in] additionalName an additional string to attach at the end of the @c filename (but before the extension)
 */
void _cuPredSuccGraphPrintGraphWithAdditionalName(PredSuccGraph* graph, const char* fileName, const char* additionalName);

/**
 * Prints a graph with the most generic possible prototype
 *
 * \attention
 * this function requires GraphViz installed in order to work". Please install it
 * via <tt>sudo apt-get install graphviz</tt>
 *
 * \post
 * 	\li in the CWD a .png file is present;
 *
 * @param[in] graph the graph involved
 * @param[in] cmd the "dot" command to use. If in doubt, use ::DC_DOT
 * @param[in] fileName the filename (with no extensions) to create
 * @param[in] additionalName another name to add to the filename of the PNG to create
 * @param[in] highlightedNode the node to highlight in the graph
 */
void _cuPredSuccGraphPrintGraphWithOptions(const PredSuccGraph* graph, dot_commands cmd, const char* fileName, const char* additionalName, NodeId highlightedNode);

/**
 * @param[in] graph the graph involved
 * @return the number of nodes inside the graph
 */
int cuPredSuccGraphGetVertexNumber(const PredSuccGraph* graph);

/**
 * Creates a list of all the edges in the graph
 *
 * The list created contains the actual edges, so be sure to know what you're doing if you alter them
 *
 * \attention
 * The function \b does create the list in the heap, but it doesn't explicitly free it. Se be sure to call
 * ::cuListDestroy to free the list from the memory
 *
 * @param[in] graph the graph to analyze
 * @return the list fo edges in the graph
 */
EdgeList* cuPredSuccGraphGetEdgeList(const PredSuccGraph* graph);

/**
 * the edges which has as source the given node
 *
 * @param[in] graph the graph to work on
 * @param[in] sourceId the id of the node that configures itself as a source
 * @return a list of edges whose source has id @c sourceId
 */
CU_NOTNULL EdgeList* cuPredSuccGraphGetVertexOutEdges(CU_NOTNULL const PredSuccGraph* graph, NodeId sourceId);

/**
 * If you want to fetch the edges themselves, and not just the number, see ::cuPredSuccGraphGetEdgeList
 *
 * @param[in] graph the graph to analyze
 * @return the number of edges inside the graph
 */
int cuPredSuccGraphGetEdgesNumber(const PredSuccGraph* graph);

/**
 * \attention
 * This operation **heavily** depends on graph size
 *
 * @param[in] g1 graph involved
 * @param[in] g2 graph involved
 * @return true if \f$ g1 = g2 \f$, false otherwise
 */
bool cuPredSuccGraphCompare(const PredSuccGraph* restrict g1, const PredSuccGraph* restrict g2);

/**
 * Save a whole graph inside a file
 *
 * \pre
 * 	\li \c f open for writing binary
 *
 * \note
 *  This function **won't** store the function callbacks. This because function cannot be serialized!
 *
 * @param[inout] f the file to write on.
 * @param[in] g the graph to save
 * @return
 * 	\li true if the operation is successful;
 * 	\li false otherwise;
 */
void cuPredSuccGraphSerialize(FILE* f, const PredSuccGraph* g);

/**
 * Load a graph from a file
 *
 * \pre
 * 	\li \c f open for reading binary
 *
 * \note
 *  This function **won't** load the function callbacks. This because function cannot be serialized!
 *
 * @param[inout] f the file to read from
 * @param[in] nodeDeserializer a function used to deserialize nodes from a stream
 * @param[in] edgeDeserializer a function used to deserialize edges from a stream
 * @return
 * 	\li the instance of graph read;
 * 	\li NULL if we couldn't read the file at al;
 */
//PredSuccGraph* cuPredSuccGraphDeserialize(FILE* f, object_deserializer nodeDeserializer, object_deserializer edgeDeserializer);
//TODO load or deserailize? decide the convention!
PredSuccGraph* cuPredSuccGraphDeserialize(FILE* f, payload_functions vertexPayload, payload_functions edgePayload);

/**
 * @defgroup traverser
 * A list of functions representing possible taverser functions, namely functions telling you if during search in the graph
 * you can pass over an edge or not
 * @{
 */

/**
 * Always go over the edge
 *
 * @param[in] e the edge involved
 * @return
 * 	@li true if we need to trav erse the edge;
 * 	@li false false otherwise
 */
bool cuAlwaysTraverse(CU_NOTNULL const Edge* e);

///@}

/**
 * check if the predecessors of every node is active in this instance of graph
 *
 * @param[in] g the graph to check;
 * @return
 *  @li true if the nodes in the graph support the predecessors,
 *  @li false otheriwse;
 */
bool cuPredSuccGraphHasPredecessorsActive(CU_NOTNULL const PredSuccGraph* g);

/**
 * Iterate over the successors of a node in the graph
 *
 * @code
 * CU_ITERATE_OVER_PREDSUCC_SUCCESSORS(graph, anode, edge, edgePayload, int) {
 * 	printf("node %ld is connected to %ld and the label on the edge is %d\n", anode->id, edge->sink->id, edgePayload);
 * }
 * @endcode
 *
 * @param[in] graph the graph where the node is positioned
 * @param[in] node a <tt>Node\*</tt> instance of the node to fetch
 * @param[in] edge a variable name that will represents the edge whose source is @c node
 * @param[in] sinkPayload a variable name that will represents the data of the sink of edge @c edge
 * @param[in] sinkPayloadType type of variable @c sinkPayload
 */
#define CU_ITERATE_OVER_PREDSUCC_SUCCESSORS(graph, node, edge, sinkPayload, sinkPayloadType) \
	CU_NEWSCOPE_WITH_VARIABLES(sinkPayloadType sinkPayload) \
			CU_ITERATE_OVER_HT_VALUES((node)->successors, edge, Edge*) \
				CU_WITH(sinkPayload=getNodePayloadAs(edge->sink, sinkPayloadType))(CU_NOCODE)

/**
 * Iterate over the successors of a node in the graph
 *
 * @pre
 *  @li @c graph needs to have the prededcessors enabled. Check it via ::cuPredSuccGraphHasPredecessorsActive
 *
 * @code
 * CU_ITERATE_OVER_PREDDSUCC_PREDECESSORS(graph, anode, edge, edgePayload, int) {
 * 	printf("node %ld is connected to %ld and the label on the edge is %d\n", anode->id, edge->sink->id, edgePayload);
 * }
 * @endcode
 *
 * @param[in] graph the graph where the node is positioned
 * @param[in] node a <tt>Node\*</tt> instance of the node to fetch
 * @param[in] edge a variable name that will represents the edge whose source is @c node
 * @param[in] sourcePayload a variable name that will represents the payload of the source of edge @c edge
 * @param[in] sourcePayloadType type of variable @c sourcePayload
 */
#define CU_ITERATE_OVER_PREDDSUCC_PREDECESSORS(graph, node, edge, sourcePayload, sourcePayloadType) \
	CU_NEWSCOPE_WITH_VARIABLES(sourcePayloadType sourcePayload) \
		CU_ITERATE_OVER_HT_VALUES((node)->predecessors, edge, Edge*) \
			CU_WITH(sourcePayload=getNodePayloadAs(edge->sink, sourcePayloadType))(CU_NOCODE)

/**
 * Iterate over all the nodes in the graph
 *
 * @code
 * 	CU_ITERATE_OVER_PREDSUCC_NODES(graph, id, value, char*) {
 * 		printf("the node of id %ld has the name \"%s\"\n", id, value);
 * 	}
 * @endcode
 *
 * @note
 * the order of the nodes is not garantueed
 *
 * @param[in] graph the graph where to iterate over
 * @param[in] node a variable representing the node we're looping over
 * @param[in] id a variable (that will be create within the loop) specifying the id of the node under analysis in the loop
 * @param[in] payload a variable specyfing the payload associated to the node under analysis
 * @param[in] type of the varaible \c payload
 *
 */
#define CU_ITERATE_OVER_PREDSUCC_NODES(graph, node, id, payload, type) \
	CU_NEWSCOPE_WITH_VARIABLES(NodeId id, type payload) \
		CU_ITERATE_OVER_HT_VALUES(graph->nodes, node, Node*) \
			CU_WRAP_CODE(id=node->id CU_COMMA payload=getNodePayloadAs(node, type), CU_NOCODE)

/**
 * Iterate over all the edges in the graph
 *
 * @note
 * The order of the edges is not garantueed
 *
 * @code
 * 	CU_ITERATE_OVER_PREDSUCC_EDGES(graph, e, source, sink, value, char*) {
 * 		printf("the edge of id %ld has the name \"%s\"\n", id, value);
 * 	}
 * @endcode
 *
 * @param[in] graph the graph to analyze
 * @param[in] e a temporary edge
 * @param[in] source a pointer to ::Node representing the source of an edge
 * @param[in] sink a pointer to ::Node representing the sink of an edge
 * @param[in] payload the variable containing the paylod of the edge
 * @param[in] type the type of variable \c paylod
 */
#define CU_ITERATE_OVER_PREDSUCC_EDGES(graph, e, source, sink, payload, type) \
	CU_WRAP_CODE(Node* source, CU_NOCODE) \
		CU_WRAP_CODE(Node* sink, CU_NOCODE) \
			CU_ITERATE_OVER_HT_VALUES(graph->nodes, node, Node*) \
				CU_ITERATE_OVER_HT_VALUES(node->successors, e, Edge*) \
					CU_WRAP_CODE(type payload = getEdgePayloadAs(e, type), CU_NOCODE) \


#endif /* PREDSUCCGRAPH_H_ */
