/**
 * @file
 *
 * @author koldar
 * @date May 10, 2018
 */


#include "CuTest.h"
#include <assert.h>
#include "dynamic_nary_tree.h"
#include "payload_functions.h"

///test dynamic tree allocation and deallocation
void testDynamicNaryTree01(CuTest* tc) {
	payload_functions intFunctions = cuPayloadFunctionsIntValue();
	struct dynamic_nary_tree* tree = cuNaryTreeNew(NULL, &intFunctions);

	cuDestroyNaryTreeWithElements(tree, NULL);
}

///add data on head to an empty node
void testDynamicNaryTree02(CuTest* tc) {
	payload_functions intFunctions = cuPayloadFunctionsIntValue();
	struct dynamic_nary_tree* tree = cuNaryTreeNew(NULL, &intFunctions);

	cuNaryTreeAddNodeOnHead(tree, cuNaryTreeGetRoot(tree), CU_CAST_INT2PTR(3));

	cuDestroyNaryTreeWithElements(tree, NULL);
}

///add data on tail to an empty node
void testDynamicNaryTree03(CuTest* tc) {
	payload_functions intFunctions = cuPayloadFunctionsIntValue();
	struct dynamic_nary_tree* tree = cuNaryTreeNew(NULL, &intFunctions);

	cuNaryTreeAddNodeOnTail(tree, cuNaryTreeGetRoot(tree), CU_CAST_INT2PTR(3));

	cuDestroyNaryTreeWithElements(tree, NULL);
}

///add multipel data on tail to an empty node
void testDynamicNaryTree04(CuTest* tc) {
	payload_functions intFunctions = cuPayloadFunctionsIntValue();
	struct dynamic_nary_tree* tree = cuNaryTreeNew(NULL, &intFunctions);

	cuNaryTreeAddNodeOnTail(tree, cuNaryTreeGetRoot(tree), CU_CAST_INT2PTR(2));
	cuNaryTreeAddNodeOnTail(tree, cuNaryTreeGetRoot(tree), CU_CAST_INT2PTR(3));
	cuNaryTreeAddNodeOnTail(tree, cuNaryTreeGetRoot(tree), CU_CAST_INT2PTR(4));

	cuDestroyNaryTreeWithElements(tree, NULL);
}

CuSuite* CuDynamicNaryTreeSuite() {
	CuSuite* suite = CuSuiteNew();

	SUITE_ADD_TEST(suite, testDynamicNaryTree01);
	SUITE_ADD_TEST(suite, testDynamicNaryTree02);
	SUITE_ADD_TEST(suite, testDynamicNaryTree03);
	SUITE_ADD_TEST(suite, testDynamicNaryTree04);

	return suite;
}
