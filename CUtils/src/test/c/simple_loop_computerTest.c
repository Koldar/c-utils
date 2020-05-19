/*
 * simple_loop_computerTest.c
 *
 *  Created on: Jul 31, 2017
 *      Author: koldar
 */


#include "CuTest.h"
#include <assert.h>
#include "simple_loops_computer.h"
#include "log.h"
#include "defaultFunctions.h"

///no loops
void testSimpleLoopComputer01(CuTest* tc) {
	PredSuccGraph* g = cuPredSuccGraphNew();
	johnson_support* support = cuJohnsonSupportNew();

	cuPredSuccGraphAddNodeInGraphById(g, 0, NULL);
	cuPredSuccGraphAddNodeInGraphById(g, 1, NULL);
	cuPredSuccGraphAddNodeInGraphById(g, 2, NULL);
	cuPredSuccGraphAddNodeInGraphById(g, 3, NULL);


	cuPredSuccGraphAddEdge(g, 0, 1, NULL);
	cuPredSuccGraphAddEdge(g, 1, 2, NULL);
	cuPredSuccGraphAddEdge(g, 2, 3, NULL);

	loop_list* simpleLoops = cuJohnsonSupportComputeSimpleLoops(support, g, edge_traverser_alwaysAccept);

	assert(cuListIsEmpty(simpleLoops));

	cuListDestroy(simpleLoops, NULL);
	cuJohnsonSupportDestroy(support, NULL);
	cuPredSuccGraphDestroyWithElements(g, NULL);
}

///one loop
void testSimpleLoopComputer02(CuTest* tc) {
	PredSuccGraph* g = cuPredSuccGraphNew();
	johnson_support* support = cuJohnsonSupportNew();

	cuPredSuccGraphAddNodeInGraphById(g, 0, NULL);
	cuPredSuccGraphAddNodeInGraphById(g, 1, NULL);
	cuPredSuccGraphAddNodeInGraphById(g, 2, NULL);
	cuPredSuccGraphAddNodeInGraphById(g, 3, NULL);


	cuPredSuccGraphAddEdge(g, 0, 1, NULL);
	cuPredSuccGraphAddEdge(g, 1, 2, NULL);
	cuPredSuccGraphAddEdge(g, 2, 3, NULL);
	cuPredSuccGraphAddEdge(g, 3, 0, NULL);

	loop_list* simpleLoops = cuJohnsonSupportComputeSimpleLoops(support, g, edge_traverser_alwaysAccept);

	assert(!cuListIsEmpty(simpleLoops));
	critical("getLengthOfList(simpleLoops) = %d", cuListGetSize(simpleLoops));
	assert(cuListGetSize(simpleLoops) == 1);
	loop* l = cuListGetNthItem(simpleLoops, 0, loop*);
	int sum = 0;
	CU_ITERATE_OVER_LIST(l, cell, n, Node*) {
		sum += n->id;
	}
	assert(sum == 6);

	cuListDestroy(simpleLoops, NULL);
	cuJohnsonSupportDestroy(support, NULL);
	cuPredSuccGraphDestroyWithElements(g, NULL);
}

///2 loops
void testSimpleLoopComputer03(CuTest* tc) {
	PredSuccGraph* g = cuPredSuccGraphNew();
	johnson_support* support = cuJohnsonSupportNew();

	cuPredSuccGraphAddNodeInGraphById(g, 0, NULL);
	cuPredSuccGraphAddNodeInGraphById(g, 1, NULL);
	cuPredSuccGraphAddNodeInGraphById(g, 2, NULL);
	cuPredSuccGraphAddNodeInGraphById(g, 3, NULL);


	cuPredSuccGraphAddEdge(g, 0, 1, NULL);
	cuPredSuccGraphAddEdge(g, 1, 2, NULL);
	cuPredSuccGraphAddEdge(g, 2, 3, NULL);
	cuPredSuccGraphAddEdge(g, 3, 0, NULL);
	cuPredSuccGraphAddEdge(g, 2, 0, NULL);

	loop_list* simpleLoops = cuJohnsonSupportComputeSimpleLoops(support, g, edge_traverser_alwaysAccept);

	assert(!cuListIsEmpty(simpleLoops));
	critical("getLengthOfList(simpleLoops) = %d", cuListGetSize(simpleLoops));
	assert(cuListGetSize(simpleLoops) == 2);
	loop* l;
	int sum = 0;

	l = cuListGetNthItem(simpleLoops, 0, loop*);
	sum = 0;
	CU_ITERATE_OVER_LIST(l, cell, n, Node*) {
		sum += n->id;
	}
	assert(sum == 6);

	l = cuListGetNthItem(simpleLoops, 1, loop*);
	sum = 0;
	CU_ITERATE_OVER_LIST(l, cell2, n2, Node*) {
		sum += n2->id;
	}
	assert(sum == 3);

	cuListDestroy(simpleLoops, NULL);
	cuJohnsonSupportDestroy(support, NULL);
	cuPredSuccGraphDestroyWithElements(g, NULL);
}

static int orderer_by_id(const Node* n1, const Node* n2) {
	return n1->id - n2->id;
}

static payload_functions getNodePayloadFunctions() {
	payload_functions result;

	result.destroy = CU_AS_DESTRUCTOR(cuDefaultFunctionDestructorNOP);
	result.order = CU_AS_ORDERER(orderer_by_id);

	return result;
}

///test graph in https://youtu.be/johyrWospv0
void testSimpleLoopComputer04(CuTest* tc) {
	PredSuccGraph* g = cuPredSuccGraphNew();
	johnson_support* support = cuJohnsonSupportNew();

	for (NodeId i=0; i<9; i++) {
		cuPredSuccGraphAddNodeInGraphById(g, i, NULL);
	}

	cuPredSuccGraphAddEdge(g, 0, 1, NULL);
	cuPredSuccGraphAddEdge(g, 0, 4, NULL);
	cuPredSuccGraphAddEdge(g, 0, 7, NULL);
	cuPredSuccGraphAddEdge(g, 1, 2, NULL);
	cuPredSuccGraphAddEdge(g, 1, 8, NULL);
	cuPredSuccGraphAddEdge(g, 1, 6, NULL);
	cuPredSuccGraphAddEdge(g, 2, 1, NULL);
	cuPredSuccGraphAddEdge(g, 2, 0, NULL);
	cuPredSuccGraphAddEdge(g, 2, 5, NULL);
	cuPredSuccGraphAddEdge(g, 2, 3, NULL);
	cuPredSuccGraphAddEdge(g, 3, 4, NULL);
	cuPredSuccGraphAddEdge(g, 4, 1, NULL);
	cuPredSuccGraphAddEdge(g, 5, 3, NULL);
	cuPredSuccGraphAddEdge(g, 7, 8, NULL);
	cuPredSuccGraphAddEdge(g, 8, 7, NULL);

	loop_list* simpleLoops = cuJohnsonSupportComputeSimpleLoops(support, g, edge_traverser_alwaysAccept);

	assert(!cuListIsEmpty(simpleLoops));
	critical("getLengthOfList(simpleLoops) = %d", cuListGetSize(simpleLoops));
	assert(cuListGetSize(simpleLoops) == 6);
	loop* l;
	node_heap* tmph;
	tmph = cuHeapNew(100, getNodePayloadFunctions());

	//first cycle
	cuHeapClear(tmph);
	l = cuListGetNthItem(simpleLoops, 0, loop*);
	CU_ITERATE_OVER_LIST(l, cell1, n1, Node*) {
		cuHeapInsertItem(tmph, n1);
	}
	assert(cuHeapGetSize(tmph) == 3);
	assert(cuHeapContainsItem(tmph, cuPredSuccGraphGetNodeById(g, 0)));
	assert(cuHeapContainsItem(tmph, cuPredSuccGraphGetNodeById(g, 1)));
	assert(cuHeapContainsItem(tmph, cuPredSuccGraphGetNodeById(g, 2)));

	//third cycle
	cuHeapClear(tmph);
	l = cuListGetNthItem(simpleLoops, 1, loop*);
	CU_ITERATE_OVER_LIST(l, cell3, n3, Node*) {
		cuHeapInsertItem(tmph, n3);
	}
	assert(cuHeapGetSize(tmph) == 4);
	assert(cuHeapContainsItem(tmph, cuPredSuccGraphGetNodeById(g, 0)));
	assert(cuHeapContainsItem(tmph, cuPredSuccGraphGetNodeById(g, 4)));
	assert(cuHeapContainsItem(tmph, cuPredSuccGraphGetNodeById(g, 1)));
	assert(cuHeapContainsItem(tmph, cuPredSuccGraphGetNodeById(g, 2)));

	//fourth cycle
	cuHeapClear(tmph);
	l = cuListGetNthItem(simpleLoops, 2, loop*);
	CU_ITERATE_OVER_LIST(l, cell4, n4, Node*) {
		cuHeapInsertItem(tmph, n4);
	}
	assert(cuHeapGetSize(tmph) == 2);
	assert(cuHeapContainsItem(tmph, cuPredSuccGraphGetNodeById(g, 1)));
	assert(cuHeapContainsItem(tmph, cuPredSuccGraphGetNodeById(g, 2)));

	//sixth cycle
	cuHeapClear(tmph);
	l = cuListGetNthItem(simpleLoops, 3, loop*);
	CU_ITERATE_OVER_LIST(l, cell6, n6, Node*) {
		cuHeapInsertItem(tmph, n6);
	}
	assert(cuHeapGetSize(tmph) == 5);
	assert(cuHeapContainsItem(tmph, cuPredSuccGraphGetNodeById(g, 1)));
	assert(cuHeapContainsItem(tmph, cuPredSuccGraphGetNodeById(g, 2)));
	assert(cuHeapContainsItem(tmph, cuPredSuccGraphGetNodeById(g, 5)));
	assert(cuHeapContainsItem(tmph, cuPredSuccGraphGetNodeById(g, 3)));
	assert(cuHeapContainsItem(tmph, cuPredSuccGraphGetNodeById(g, 4)));

	//fifth cycle
	cuHeapClear(tmph);
	l = cuListGetNthItem(simpleLoops, 4, loop*);
	CU_ITERATE_OVER_LIST(l, cell5, n5, Node*) {
		cuHeapInsertItem(tmph, n5);
	}
	assert(cuHeapGetSize(tmph) == 4);
	assert(cuHeapContainsItem(tmph, cuPredSuccGraphGetNodeById(g, 1)));
	assert(cuHeapContainsItem(tmph, cuPredSuccGraphGetNodeById(g, 2)));
	assert(cuHeapContainsItem(tmph, cuPredSuccGraphGetNodeById(g, 3)));
	assert(cuHeapContainsItem(tmph, cuPredSuccGraphGetNodeById(g, 4)));

	//second cycle
	cuHeapClear(tmph);
	l = cuListGetNthItem(simpleLoops, 5, loop*);
	CU_ITERATE_OVER_LIST(l, cell2, n2, Node*) {
		cuHeapInsertItem(tmph, n2);
	}
	assert(cuHeapGetSize(tmph) == 2);
	assert(cuHeapContainsItem(tmph, cuPredSuccGraphGetNodeById(g, 7)));
	assert(cuHeapContainsItem(tmph, cuPredSuccGraphGetNodeById(g, 8)));

	cuHeapDestroy(tmph, NULL);
	cuListDestroyWithElement(simpleLoops, NULL);
	cuJohnsonSupportDestroy(support, NULL);
	cuPredSuccGraphDestroyWithElements(g, NULL);
}

CuSuite* CuSimpleLoopComputerSuite() {
	CuSuite* suite = CuSuiteNew();

	//	SUITE_ADD_TEST(suite, testSimpleLoopComputer01);
	//	SUITE_ADD_TEST(suite, testSimpleLoopComputer02);
	//	SUITE_ADD_TEST(suite, testSimpleLoopComputer03);
	SUITE_ADD_TEST(suite, testSimpleLoopComputer04);


	return suite;
}

