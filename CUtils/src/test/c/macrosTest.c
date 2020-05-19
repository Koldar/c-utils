/*
 * macrosTest.c
 *
 *  Created on: Jun 27, 2017
 *      Author: koldar
 */

#include <assert.h>
#include "CuTest.h"
#include "macros.h"
#include <stdbool.h>
#include "log.h"
#include "string_utils.h"
#include "var_args.h"

#define MACRO_TEST(...) \
		CU_HAS_VARIADIC(__VA_ARGS__)

#define MACRO_TEST2(...) \
		CU_HANDLE_VARIADIC(__VA_ARGS__)(4)(3)

void testHasVariadic01(CuTest* tc) {
	assert(MACRO_TEST() == 0);
	assert(MACRO_TEST(5) == 1);
	assert(MACRO_TEST(pippo) == 1);
	assert(MACRO_TEST(6, 4) == 1);
	assert(MACRO_TEST(6, pippo) == 1);
}

void testHasVariadic02(CuTest* tc) {
	assert(MACRO_TEST2() == 3);
	assert(MACRO_TEST2(5) == 4);
	assert(MACRO_TEST2(pippo) == 4);
	assert(MACRO_TEST2(6, 4) == 4);
	assert(MACRO_TEST2(6, pippo) == 4);
}

void testHasComma(CuTest* tc) {
	int pippo;
	char mondo;

	info("has comma is %d", P99_HAS_COMMA());
	assert(P99_HAS_COMMA() == 0);

	info("has comma is %d", P99_HAS_COMMA(5));
	assert(P99_HAS_COMMA(5) == 0);

	info("has comma is %d", P99_HAS_COMMA(5,2));
	assert(P99_HAS_COMMA(5,2) == 1);

	info("has comma is %d", P99_HAS_COMMA(5,pippo, mondo));
	assert(P99_HAS_COMMA(5,pippo, mondo) == 1);

	info("has comma is %d", P99_HAS_COMMA("pippo, mondo"));
	assert(P99_HAS_COMMA("pippo, mondo") == 0);

	info("has comma is %d", P99_HAS_COMMA("pippo", "mondo"));
	assert(P99_HAS_COMMA("pippo", "mondo") == 1);
}

void testCountArgs(CuTest* tc) {

	info("count is %d", CU_COUNT_ARGS());
	assert(CU_COUNT_ARGS() == 0);

	info("count is %d", CU_COUNT_ARGS(pippo));
	assert(CU_COUNT_ARGS(pippo) == 1);

	info("count is %d", CU_COUNT_ARGS(pippo, 3));
	assert(CU_COUNT_ARGS(pippo, 3) == 2);

	info("count is %d", CU_COUNT_ARGS("ciao, mondo"));
	assert(CU_COUNT_ARGS("ciao, mondo") == 1);

	info("count is %d", CU_COUNT_ARGS("ciao", "mondo"));
	assert(CU_COUNT_ARGS("ciao", "mondo") == 2);
}

static int buildingSum;
static void foo(int a) {
	buildingSum += a;
}

static int buildingSum2;
static void bar(int a) {
	buildingSum2 += a;
}

static int increase(int a) {
	return a + 1;
}

static int identity(int a) {
	return a;
}

void testExpr(CuTest* tc) {
	buildingSum = 0;
	CUTILS_EXECUTE_EXPR1(foo, ;, 1);
	assert(buildingSum == 1);

	buildingSum = 0;
	CUTILS_EXECUTE_EXPR1(foo, ;, 1, 1, 1, 1);
	assert(buildingSum == 4);
}

void testExpr2(CuTest* tc) {
	buildingSum = 0;
	buildingSum2 = 0;
	CUTILS_EXECUTE_EXPR2(foo, bar, 1, 2);
	assert(buildingSum == 1);
	assert(buildingSum2 == 2);

	buildingSum = 0;
	buildingSum2 = 0;
	CUTILS_EXECUTE_EXPR2(foo, bar, 1, 2, 1, 2);
	assert(buildingSum == 2);
	assert(buildingSum2 == 4);
}

void testExpr3(CuTest* tc) {
	buildingSum = 0;
	buildingSum2 = 0;
	int array[] = {CUTILS_DUPLICATE_EXPR2(identity, increase, 1, 5, 7)};
	assert(array[0] == 1);
	assert(array[1] == 2);
	assert(array[2] == 5);
	assert(array[3] == 6);
	assert(array[4] == 7);
	assert(array[5] == 8);
}

void testWithMacro(CuTest* tc) {
	CU_WITH(int* a = malloc(sizeof(int)))(free(a)) {
		*a = 0;
		*a += 2;
		assert(*a == 2);
	}

	//	CU_WITH(int* a = malloc(sizeof(int)), free(a)) {
	//		*a = 0;
	//		*a += 2;
	//		assert(*a == 2);
	//	}
}

void testIndexNameMapping(CuTest* tc) {
	int CUTILS_INDEX_NAME_MAPPING(0, a) = 5;
	int CUTILS_INDEX_NAME_MAPPING(1, a) = 6;
	assert(CUTILS_INDEX_NAME_MAPPING(0, a) + CUTILS_INDEX_NAME_MAPPING(1, a) == 11);
}

int defaultFunction(int a, int b, int c, int d) {
	return a+b+c+d;
}

//let P99 know the prototype of the full function you want to have default values
P99_PROTOTYPE(int, defaultFunction, int, int, int, int);
//overwrite defaultFunction function with a macro: you need to give the name of the underlying function and the number of maximum parameters it accepts
#define defaultFunction(...) P99_CALL_DEFARG(defaultFunction, 4, __VA_ARGS__)
//let P99 know the default arguments of each parameter. Empty parameter means no default value (hence it is required)
P99_DECLARE_DEFARG(defaultFunction,
		/*NO DEFAULT FOR A*/,
		/*NO DEFAULT FOR B*/,
		5 /*default value for cd ..c*/,
		10 /* default value for d*/
);

void testDefaultArguments(CuTest* tc) {
	assert(defaultFunction(1, 2, 3, 4) == 10);
	assert(defaultFunction(1, 2, 3) == 16); //d=10
	assert(defaultFunction(1, 2) == 18); //c=5, d=10
	//assert(defaultFunction(1) == 17); //error
}

void testStringSwitch_01(CuTest* tc) {
	int a = 0;
	CU_STRING_SWITCH("ciao") {
		CU_CASE("ciao") {
			a = 1;
			break;
		}
		CU_CASE("world") {
			a = 2;
			break;
		}
		CU_DEFAULT() {
			a = 3;
			break;
		}
	}

	assert(a==1);
}

void testStringSwitch_02(CuTest* tc) {
	int a = 0;
	CU_STRING_SWITCH("world") {
		CU_CASE("ciao") {
			a = 1;
			break;
		}
		CU_CASE("world") {
			a = 2;
			break;
		}
		CU_DEFAULT() {
			a = 3;
			break;
		}
	}

	assert(a==2);
}

void testStringSwitch_03(CuTest* tc) {
	int a = 0;
	CU_STRING_SWITCH("hello") {
		CU_CASE("ciao") {
			a = 1;
			break;
		}
		CU_CASE("world") {
			a = 2;
			break;
		}
		CU_DEFAULT() {
			a = 3;
			break;
		}
	}

	assert(a==3);
}

void testMultiWrapCode_01(CuTest* tc) {
	int count = 0;
	CU_MULTI_WRAP_CODE_START(int a=4, CU_NOCODE) CU_MULTI_WRAP_CODE_END(char b='a', CU_NOCODE) {
		assert(a == 4);
		assert(b == 'a');
		count++;
	}
	assert(count == 1);
}

void testMultiWrapCode_02(CuTest* tc) {
	int count = 0;
	CU_MULTI_WRAP_CODE_START(int a=4, CU_NOCODE) CU_MULTI_WRAP_CODE(float c=0.5, CU_NOCODE)	CU_MULTI_WRAP_CODE_END(char b='a', CU_NOCODE) {
		assert(a == 4);
		assert(b == 'a');
		assert(c == 0.5);
		count++;
	}
	assert(count == 1);
}

void testNewScopeWithVariables_01(CuTest* tc) {
	int count = 0;

	CU_NEWSCOPE_WITH_VARIABLES(int a=5, char b='a') {
		assert(a == 5);
		assert(b == 'a');
		count++;
	}
	assert(count == 1);
}

void testNewScopeWithVariables_02(CuTest* tc) {
	int count = 0;
	CU_NEWSCOPE_WITH_VARIABLES(int a=5) {
		critical("ciao2");
		assert(a == 5);
		count++;
	}
	assert(count == 1);
}

void testNewScopeWithVariables_03(CuTest* tc) {
	int count = 0;
	CU_NEWSCOPE_WITH_VARIABLES() {
		critical("ciao3");
		count++;
	}
	assert(count == 1);
}

void test_CU_NLAST_VARIADIC_01(CuTest* tc) {
	assert((CU_NLAST_VARIADIC(1, 'c')) == 'c');
	assert((CU_NLAST_VARIADIC(1, 'a', 'b', 'c')) == 'c');
	assert((CU_NLAST_VARIADIC(2, 'a', 'b', 'c')) == 'b');
	assert((CU_NLAST_VARIADIC(3, 'a', 'b', 'c')) == 'a');
}

void test_CU_SUBSETNLAST_VARIADIC_01(CuTest* tc) {
#undef OP
#undef FUNC
#define OP(NAME, I, REC, RES) REC + RES
#define FUNC(NAME, X, I) X

	assert((P99_FOR(, P99_NARG(CU_SUBSETNLAST_VARIADIC(0, 1, 2, 3 )), OP, FUNC, CU_SUBSETNLAST_VARIADIC(0, 1, 2, 3))) == 6);
	assert((P99_FOR(, P99_NARG(CU_SUBSETNLAST_VARIADIC(1, 1, 2, 3 )), OP, FUNC, CU_SUBSETNLAST_VARIADIC(1, 1, 2, 3))) == 3);
	assert((P99_FOR(, P99_NARG(CU_SUBSETNLAST_VARIADIC(2, 1, 2, 3 )), OP, FUNC, CU_SUBSETNLAST_VARIADIC(2, 1, 2, 3))) == 1);
}

void test_CU_FOR_PAIR_01(CuTest* tc) {
#undef OP
#undef FUNC
#define OP(CONTEXT, INDEX, CURRENT, NEXT) CURRENT + NEXT
#define FUNC(CONTEXT, INDEX, A, B) (A * B)

	assert(CU_FOR_PAIR(, OP, FUNC, 1, 2, 3, 4, 5, 6) ==  (1*2+3*4+5*6));
	assert(CU_FOR_PAIR(, OP, FUNC, 1, 2) ==  (1*2));
}

void test_CU_FOR_EACH_01(CuTest* tc) {
#undef OP
#undef FUNC
#define OP(CONTEXT, INDEX, CURRENT, NEXT) (CURRENT + NEXT)
#define FUNC(CONTEXT, INDEX, A) (A)

	assert(CU_FOR_EACH(, OP, FUNC, 1) == (1));
	assert(CU_FOR_EACH(, OP, FUNC, 1, 2) == (1+2));
	assert(CU_FOR_EACH(, OP, FUNC, 1, 2, 3) == (1+2+3));

#undef FUNC
#define FUNC(CONTEXT, INDEX, A) ((INDEX) * (A))

	assert(CU_FOR_EACH(, OP, FUNC, 1) == (1*1));
	assert(CU_FOR_EACH(, OP, FUNC, 1, 2) == (2*1+1*2));
	assert(CU_FOR_EACH(, OP, FUNC, 1, 2, 3) == (3*1+2*2+1*3));
}

void test_CU_FOR_TRIPLE_01(CuTest* tc) {
#undef OP
#undef FUNC
#define OP(CONTEXT, INDEX, CURRENT, NEXT) (CURRENT + NEXT)
#define FUNC(CONTEXT, INDEX, A, B, C) (A * B * C)

	assert(CU_FOR_TRIPLE(, OP, FUNC, 1, 2, 3, 4, 5, 6, 7, 8, 9) ==  (1*2*3+4*5*6+7*8*9));
	assert(CU_FOR_TRIPLE(, OP, FUNC, 1, 2, 3) ==  (1*2*3));
}

void test_CU_LASTLOOP_01() {
	char buffer[300];
	const char* const names[] = {"hello", "world", "!"};
	int j = 0;
	for (int i=0; i<3; i++) {
		j += sprintf(&buffer[j], "%s%s", names[i], CU_LASTLOOP(i, 3) ? "" : ", ");
	}
	info("obtained string is \"%s\"", buffer);
	assert(isStrCaseEqual(buffer, "hello, world, !"));
}

void test_CU_LASTLOOP_02() {
	char buffer[300];
	char* names[] = {"hello", "world", "!"};
	int j = 0;
	for (int i=0; i<1; i++) {
		j += sprintf(&buffer[j], "%s%s", names[i], CU_LASTLOOP(i, 1) ? "" : ", ");
	}
	info("obtained string is \"%s\"", buffer);
	assert(isStrCaseEqual(buffer, "hello"));
}

void test_CU_FIRSTLOOP_01() {
	char buffer[300];
	const char* const names[] = {"hello", "world", "!"};
	int j = 0;
	for (int i=0; i<3; i++) {
		j += sprintf(&buffer[j], "%s%s", CU_FIRSTLOOP(i, 0) ? "" : ", ", names[i]);
	}
	info("obtained string is \"%s\"", buffer);
	assert(isStrCaseEqual(buffer, "hello, world, !"));
}

void test_CU_FIRSTLOOP_02(CuTest* tc) {
	char buffer[300];
	char* names[] = {"hello", "world", "!"};
	int j = 0;
	for (int i=0; i<1; i++) {
		j += sprintf(&buffer[j], "%s%s", CU_FIRSTLOOP(i, 0) ? "" : ", ", names[i]);
	}
	info("obtained string is \"%s\"", buffer);
	assert(isStrCaseEqual(buffer, "hello"));
}

void test_repeat_until_01(CuTest* tc) {
	int a = 0;
	int loop = 0;
	CU_REPEAT {
		a++;
		loop += 1;
	} CU_UNTIL(a==5);

	assert(a == 5);
	assert(loop == 5);
}

void test_repeat_until_02(CuTest* tc) {
	int a = 0;
	int loop = 0;
	CU_REPEAT {
		a++;
		loop += 1;
	} CU_UNTIL(a==1);

	assert(a == 1);
	assert(loop == 1);
}

#define CU_IS_MACRO_DEFINED_TEST1

void test_CU_IS_MACRO_DEFINED(CuTest* tc) {
	assert(CU_IS_MACRO_DEFINED(CU_IS_MACRO_DEFINED_TEST1) == true);
	assert(CU_IS_MACRO_DEFINED(CU_IS_MACRO_DEFINED_TEST2) == false);
}

void test_CU_IF_MACRO_DEFINED(CuTest* tc) {
	int a = 0;
	CU_IF_MACRO_DEFINED(CU_IS_MACRO_DEFINED_TEST1)(a = 5)(a = -5);
	assert(a == 5);

	a = 0;
	CU_IF_MACRO_DEFINED(CU_IS_MACRO_DEFINED_TEST2)(a = 5)(a = -5);
	assert(a == -5);
}

typedef void (*dummy_destructor_type)(const void* p, const var_args* context);
#define CU_ENSURE_DESTRUCTOR(functionName) CU_ENSURE_FUNCTION_POINTER(1, dummy_destructor_type, void, functionName, CU_FT_CONST_PTR(void), CU_FT_CONST_PTR(var_args))

#define CU_FUNCTION_POINTER_dummy_destructor_type_void_destroyFoo_voidConstPtr_var_argsConstPtr 1
void destroyFoo(void* foo, const struct var_args* context) {
}

void destroyFoo2(void* foo, const struct var_args* context) {
}



void test_CU_ENSURE_FUNCTION_POINTER_01(CuTest* tc) {
	dummy_destructor_type d = CU_ENSURE_DESTRUCTOR(destroyFoo);
}

void test_CU_FT_conventions_01(CuTest* tc) {
	int intPtr = 0;
	int intPtrPtr = 0;
	int intPtrPtrConst = 0;
	int intConstPtr = 0;
	int intConstPtrPtr = 0;

	assert(CU_FT_PTR(int) == 0);
	assert(CU_FT_PTR(CU_FT_PTR(int)) == 0);
	assert(CU_FT_CONST(CU_FT_PTR(CU_FT_PTR(int))) == 0);

	assert(CU_FT_CONST_PTR(int) == 0);

	assert(CU_FT(int, CONST, PTR, PTR) == 0);
}

void test_multiline_01(CuTest* tc) {
	char* test = CU_MULTILINE_N(
			"hello",
			"world!");
	assert(isStrCaseEqual(test, "hello\nworld!"));
}

void test_CU_SEQUENCE_01(CuTest* tc) {
	int a[] = {CU_SEQUENCE(5)};
	assert(a[0] == 0);
	assert(a[1] == 1);
	assert(a[2] == 2);
	assert(a[3] == 3);
	assert(a[4] == 4);
}

void test_CU_SEQUENCE_02(CuTest* tc) {
	int b[] = {CU_SEQUENCE(1)};
	assert(b[0] == 0);
}

void test_CU_RANGE_01(CuTest* tc) {
	int a[] = {CU_RANGE(0, 3)};

	assert(a[0] == 0);
	assert(a[1] == 1);
	assert(a[2] == 2);
}

void test_CU_RANGE_02(CuTest* tc) {
	int a[] = {CU_RANGE(2, 5)};

	assert(a[0] == 2);
	assert(a[1] == 3);
	assert(a[2] == 4);
}


//#define LAST_N(N, ...) P99_SUB(P99_MINUS(P99_NARG(__VA_ARGS__), N), N, ## __VA_ARGS__)
//
//#define FOR_PAIR(CONTEXT, OP, FUNC, ...) P99_PASTE2(_BASE_FOR_PAIR_, P99_NARG(__VA_ARGS__))(CONTEXT, OP, FUNC, ## __VA_ARGS__)
//
//#define _BASE_FOR_PAIR_2(CONTEXT, OP, FUNC, value1, value2) FUNC(CONTEXT, 1, value1, value2)
//#define _BASE_FOR_PAIR_4(CONTEXT, OP, FUNC, value1, value2, ...) OP(CONTEXT, 1, FUNC(CONTEXT, 2, value1, value2), _BASE_FOR_PAIR_2(CONTEXT, OP, FUNC, ## __VA_ARGS__))
//#define _BASE_FOR_PAIR_6(CONTEXT, OP, FUNC, value1, value2, ...) OP(CONTEXT, 2, FUNC(CONTEXT, 3, value1, value2), _BASE_FOR_PAIR_4(CONTEXT, OP, FUNC, ## __VA_ARGS__))
//#define _BASE_FOR_PAIR_8(CONTEXT, OP, FUNC, value1, value2, ...) OP(CONTEXT, 3, FUNC(CONTEXT, 4, value1, value2), _BASE_FOR_PAIR_6(CONTEXT, OP, FUNC, ## __VA_ARGS__))
//#define _BASE_FOR_PAIR_10(CONTEXT, OP, FUNC, value1, value2, ...) OP(CONTEXT, 4, FUNC(CONTEXT, 5, value1, value2), _BASE_FOR_PAIR_8(CONTEXT, OP, FUNC, ## __VA_ARGS__))
//
//
//#define _METADATA_REDUCE(NAME, I, REC, RES) REC, RES
//#define _METADATA_MAP(context, length, type, value) #value
//#define _GENERATE_FIELDS_NAME(structName, ...) FOR_PAIR(, _METADATA_REDUCE, _METADATA_MAP, ## __VA_ARGS__)
//
//#define _STRUCT_REDUCE(NAME, I, REC, RES) REC; RES
//#define _STRUCT_MAP(context, length, type, value) type value
//#define _GENERATE_STRUCT_FIELDS(structName, ...) FOR_PAIR(, _STRUCT_REDUCE, _STRUCT_MAP, ## __VA_ARGS__)
//
//#define DEFINE_STRUCT(structName, ...) \
//	static const int P99_PASTE(_, structName, _, fieldCount) = P99_DIV(P99_NARG(__VA_ARGS__), 2); \
//	static const char* P99_PASTE(_, structName, _, fieldNames)[] = { _GENERATE_FIELDS_NAME(structName, __VA_ARGS__) }; \
//	\
//	struct structName { \
//		_GENERATE_STRUCT_FIELDS(structName, __VA_ARGS__); \
//	}
//
//#define GET_FIELD_NAME(structName, id) P99_PASTE(_, structName, _, fieldNames)[id]
//
//DEFINE_STRUCT(struct_name, char, char_1, char, char_2);
//
//void testaaaa(CuTest* tc){
//        struct struct_name struct_name;
//        const char* member_name = NULL;
//        member_name = GET_FIELD_NAME(struct_name, 1); // member_name = "char_2"
//        printf("second member name is %s\n", member_name);
//        exit(0);
//}

CuSuite* CuMacroSuite() {
	CuSuite* suite = CuSuiteNew();

	SUITE_ADD_TEST(suite, testHasComma);
	SUITE_ADD_TEST(suite, testWithMacro);
	SUITE_ADD_TEST(suite, testCountArgs);
	SUITE_ADD_TEST(suite, testExpr);
	SUITE_ADD_TEST(suite, testExpr2);
	SUITE_ADD_TEST(suite, testExpr3);
	SUITE_ADD_TEST(suite, testIndexNameMapping);
	SUITE_ADD_TEST(suite, testHasVariadic01);
	SUITE_ADD_TEST(suite, testHasVariadic02);
	SUITE_ADD_TEST(suite, testDefaultArguments);
	SUITE_ADD_TEST(suite, testStringSwitch_01);
	SUITE_ADD_TEST(suite, testStringSwitch_02);
	SUITE_ADD_TEST(suite, testStringSwitch_03);
	SUITE_ADD_TEST(suite, testMultiWrapCode_01);
	SUITE_ADD_TEST(suite, testMultiWrapCode_02);
	SUITE_ADD_TEST(suite, testNewScopeWithVariables_01);
	SUITE_ADD_TEST(suite, testNewScopeWithVariables_02);
	SUITE_ADD_TEST(suite, testNewScopeWithVariables_03);


	SUITE_ADD_TEST(suite, test_CU_NLAST_VARIADIC_01);
	SUITE_ADD_TEST(suite, test_CU_SUBSETNLAST_VARIADIC_01);
	SUITE_ADD_TEST(suite, test_CU_FOR_EACH_01);
	SUITE_ADD_TEST(suite, test_CU_FOR_PAIR_01);
	SUITE_ADD_TEST(suite, test_CU_FOR_TRIPLE_01);

	SUITE_ADD_TEST(suite, test_CU_LASTLOOP_01);
	SUITE_ADD_TEST(suite, test_CU_LASTLOOP_02);
	SUITE_ADD_TEST(suite, test_CU_FIRSTLOOP_01);
	SUITE_ADD_TEST(suite, test_CU_FIRSTLOOP_02);

	SUITE_ADD_TEST(suite, test_repeat_until_01);
	SUITE_ADD_TEST(suite, test_repeat_until_02);

	SUITE_ADD_TEST(suite, test_CU_IS_MACRO_DEFINED);
	SUITE_ADD_TEST(suite, test_CU_IF_MACRO_DEFINED);
	SUITE_ADD_TEST(suite, test_CU_ENSURE_FUNCTION_POINTER_01);
	SUITE_ADD_TEST(suite, test_CU_FT_conventions_01);
	SUITE_ADD_TEST(suite, test_multiline_01);

	SUITE_ADD_TEST(suite, test_CU_SEQUENCE_01);
	SUITE_ADD_TEST(suite, test_CU_SEQUENCE_02);

	SUITE_ADD_TEST(suite, test_CU_RANGE_01);
	SUITE_ADD_TEST(suite, test_CU_RANGE_02);

	return suite;
}
