/*
 * sccTest.c
 *
 *  Created on: Jun 13, 2017
 *      Author: koldar
 */

#include "CuTest.h"
#include <stdlib.h>
#include <assert.h>
#include "predsuccgraph.h"
#include "log.h"
#include "scc.h"

void testSCC01(CuTest* tc) {
	PredSuccGraph* g = cuPredSuccGraphNew();
	scc_graph* sccGraph = cuStronglyConnectedComponentsGraphNew(g, edge_traverser_alwaysAccept, false, NULL);
	assert(cuPredSuccGraphGetVertexNumber(cuStronglyConnectedComponentsGraphAsPredSuccGraph(sccGraph)) == 0);
	cuStronglyConnectedComponentsGraphDestroy(sccGraph, NULL);
	cuPredSuccGraphDestroyWithElements(g, NULL);
}

void testSCC02(CuTest* tc) {
	excludeLogger("redBlackTree.c");

	PredSuccGraph* g = cuPredSuccGraphNew();

	cuPredSuccGraphAddNodeInGraphById(g, 0, NULL);
	cuPredSuccGraphAddNodeInGraphById(g, 1, NULL);
	cuPredSuccGraphAddNodeInGraphById(g, 2, NULL);
	cuPredSuccGraphAddNodeInGraphById(g, 3, NULL);
	cuPredSuccGraphAddNodeInGraphById(g, 4, NULL);

	cuPredSuccGraphAddEdge(g, 0, 1, NULL);
	cuPredSuccGraphAddEdge(g, 1, 2, NULL);
	cuPredSuccGraphAddEdge(g, 2, 0, NULL);

	cuPredSuccGraphAddEdge(g, 3, 4, NULL);
	cuPredSuccGraphAddEdge(g, 4, 3, NULL);

	scc_graph* sccGraph = cuStronglyConnectedComponentsGraphNew(g, edge_traverser_alwaysAccept, false, NULL);

	info("number of scc inside the graph are %d", cuPredSuccGraphGetVertexNumber(cuStronglyConnectedComponentsGraphAsPredSuccGraph(sccGraph)));
	assert(cuPredSuccGraphGetVertexNumber(cuStronglyConnectedComponentsGraphAsPredSuccGraph(sccGraph)) == 2);

	PredSuccGraph* sccs = cuStronglyConnectedComponentsGraphAsPredSuccGraph(sccGraph);
	assert(cuStronglyConnectedComponentsIsNodeInside(sccGraph, 0, 0));
	assert(cuStronglyConnectedComponentsIsNodeInside(sccGraph, 1, 0));
	assert(cuStronglyConnectedComponentsIsNodeInside(sccGraph, 2, 0));

	assert(cuStronglyConnectedComponentsIsNodeInside(sccGraph, 3, 1));
	assert(cuStronglyConnectedComponentsIsNodeInside(sccGraph, 4, 1));

	cuStronglyConnectedComponentsGraphDestroy(sccGraph, NULL);
	cuPredSuccGraphDestroyWithElements(g, NULL);
}

void testSCC03(CuTest* tc) {
	excludeLogger("redBlackTree.c");

	PredSuccGraph* g = cuPredSuccGraphNew();

	cuPredSuccGraphAddNodeInGraphById(g, 0, NULL);
	cuPredSuccGraphAddNodeInGraphById(g, 1, NULL);
	cuPredSuccGraphAddNodeInGraphById(g, 2, NULL);
	cuPredSuccGraphAddNodeInGraphById(g, 3, NULL);
	cuPredSuccGraphAddNodeInGraphById(g, 4, NULL);

	cuPredSuccGraphAddEdge(g, 0, 1, NULL);
	cuPredSuccGraphAddEdge(g, 1, 2, NULL);
	cuPredSuccGraphAddEdge(g, 2, 0, NULL);

	cuPredSuccGraphAddEdge(g, 3, 4, NULL);
	cuPredSuccGraphAddEdge(g, 4, 3, NULL);

	cuPredSuccGraphAddEdge(g, 4, 2, NULL);
	cuPredSuccGraphAddEdge(g, 4, 0, NULL);
	cuPredSuccGraphAddEdge(g, 3, 1, NULL);

	scc_graph* sccGraph = cuStronglyConnectedComponentsGraphNew(g, edge_traverser_alwaysAccept, true, NULL);

	info("number of scc inside the graph are %d", cuPredSuccGraphGetVertexNumber(cuStronglyConnectedComponentsGraphAsPredSuccGraph(sccGraph)));
	assert(cuPredSuccGraphGetVertexNumber(cuStronglyConnectedComponentsGraphAsPredSuccGraph(sccGraph)) == 2);

	PredSuccGraph* sccs = cuStronglyConnectedComponentsGraphAsPredSuccGraph(sccGraph);
	assert(cuStronglyConnectedComponentsIsNodeInside(sccGraph, 0, 0));
	assert(cuStronglyConnectedComponentsIsNodeInside(sccGraph, 1, 0));
	assert(cuStronglyConnectedComponentsIsNodeInside(sccGraph, 2, 0));

	assert(cuStronglyConnectedComponentsIsNodeInside(sccGraph, 3, 1));
	assert(cuStronglyConnectedComponentsIsNodeInside(sccGraph, 4, 1));

	int sum = 0;
	Node* scc0 = cuPredSuccGraphGetNodeById(cuStronglyConnectedComponentsGraphAsPredSuccGraph(sccGraph), 0);
	assert(scc0 != NULL);
	assert(cuStronglyConnectedComponentsGraphGetNumberOfNodes(scc0) == 3);
	CU_ITERATE_OVER_HT_VALUES(cuStronglyConnectedComponentsGetComponentOfNode(sccGraph, 0)->successors, e1, Edge*) {
		EdgeList* l = getEdgePayloadAs(e1, EdgeList*);
		sum += cuListGetSize(l);
	}
	assert(sum == 0);

	sum = 0;
	int edgesNumber = 0;
	Node* scc1 = cuPredSuccGraphGetNodeById(cuStronglyConnectedComponentsGraphAsPredSuccGraph(sccGraph), 1);
	assert(scc1 != NULL);
	info("getNumberOfNodesInsideSCC(scc1) == %d", cuStronglyConnectedComponentsGraphGetNumberOfNodes(scc1));
	assert(cuStronglyConnectedComponentsGraphGetNumberOfNodes(scc1) == 2);
	CU_ITERATE_OVER_HT_VALUES(scc1->successors, e2, Edge*) {
		edgesNumber += 1;
		EdgeList* l = getEdgePayloadAs(e2, EdgeList*);
		sum += cuListGetSize(l);
	}
	info("sum is %d; edgesNumber is %d", sum, edgesNumber);
	assert(edgesNumber == 1);
	assert(sum == 3);

	cuStronglyConnectedComponentsGraphDestroy(sccGraph, NULL);
	cuPredSuccGraphDestroyWithElements(g, NULL);
}

void testSCC04(CuTest* tc) {
	excludeLogger("redBlackTree.c");

	PredSuccGraph* g = cuPredSuccGraphNew();

	cuPredSuccGraphAddNodeInGraphById(g, 0, NULL);
	cuPredSuccGraphAddNodeInGraphById(g, 1, NULL);
	cuPredSuccGraphAddNodeInGraphById(g, 2, NULL);

	cuPredSuccGraphAddEdge(g, 0, 1, NULL);
	cuPredSuccGraphAddEdge(g, 1, 2, NULL);

	scc_graph* sccGraph = cuStronglyConnectedComponentsGraphNew(g, edge_traverser_alwaysAccept, true, NULL);

	info("number of scc inside the graph are %d", cuPredSuccGraphGetVertexNumber(cuStronglyConnectedComponentsGraphAsPredSuccGraph(sccGraph)));
	assert(cuPredSuccGraphGetVertexNumber(cuStronglyConnectedComponentsGraphAsPredSuccGraph(sccGraph)) == 3);

	PredSuccGraph* sccs = cuStronglyConnectedComponentsGraphAsPredSuccGraph(sccGraph);
	assert(cuStronglyConnectedComponentsGraphGetNumberOfNodes(cuStronglyConnectedComponentsGetComponentOfNode(sccGraph, 0)) == 1);
	assert(cuStronglyConnectedComponentsGraphGetNumberOfNodes(cuStronglyConnectedComponentsGetComponentOfNode(sccGraph, 1)) == 1);
	assert(cuStronglyConnectedComponentsGraphGetNumberOfNodes(cuStronglyConnectedComponentsGetComponentOfNode(sccGraph, 2)) == 1);

	int sum = 0;
	int edgesNumber = 0;

	Node* scc = cuStronglyConnectedComponentsGetComponentOfNode(sccGraph, 0);
	assert(scc != NULL);
	assert(cuStronglyConnectedComponentsGraphGetNumberOfNodes(scc) == 1);
	CU_ITERATE_OVER_HT_VALUES(scc->successors, e1, Edge*) {
		edgesNumber += 1;
		EdgeList* l = getEdgePayloadAs(e1, EdgeList*);
		sum += cuListGetSize(l);
	}
	debug("sum is %d", sum);
	assert(sum == 1);
	assert(edgesNumber == 1);

	sum = 0;
	edgesNumber = 0;
	scc = cuStronglyConnectedComponentsGetComponentOfNode(sccGraph, 1);
	assert(scc != NULL);
	assert(cuStronglyConnectedComponentsGraphGetNumberOfNodes(scc) == 1);
	CU_ITERATE_OVER_HT_VALUES(scc->successors, e2, Edge*) {
		edgesNumber += 1;
		EdgeList* l = getEdgePayloadAs(e2, EdgeList*);
		sum += cuListGetSize(l);
	}
	assert(sum == 1);
	assert(edgesNumber == 1);

	sum = 0;
	edgesNumber = 0;
	scc = cuStronglyConnectedComponentsGetComponentOfNode(sccGraph, 2);
	assert(scc != NULL);
	assert(cuStronglyConnectedComponentsGraphGetNumberOfNodes(scc) == 1);
	CU_ITERATE_OVER_HT_VALUES(scc->successors, e3, Edge*) {
		edgesNumber += 1;
		EdgeList* l = getEdgePayloadAs(e3, EdgeList*);
		sum += cuListGetSize(l);
	}
	assert(sum == 0);
	assert(edgesNumber == 0);


	cuStronglyConnectedComponentsGraphDestroy(sccGraph, NULL);
	cuPredSuccGraphDestroyWithElements(g, NULL);
}

CuSuite* CuSCCSuite() {
	CuSuite* suite = CuSuiteNew();

	SUITE_ADD_TEST(suite, testSCC01);
	SUITE_ADD_TEST(suite, testSCC02);
	SUITE_ADD_TEST(suite, testSCC03);
	SUITE_ADD_TEST(suite, testSCC04);


	return suite;
}

