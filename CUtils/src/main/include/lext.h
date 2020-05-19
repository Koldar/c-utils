#ifndef LEXT_H__
#define LEXT_H__

#include <stdbool.h>

union lext {
	bool bval;
	char cval;
	int ival;
	long lval;
	float fval;
	double dval;
	char* sval;
};

union lext getEmptyLexT();

#endif
