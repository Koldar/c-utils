#include "lext.h"

union lext getEmptyLexT() {
	union lext retVal;
	retVal.dval = 0.0;
	return retVal;
}
