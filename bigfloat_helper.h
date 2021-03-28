#ifndef __BIGFLOAT_HELPER__
#define __BIGFLOAT_HELPER__
// Any helper function goes here
#include "bigfloat.h"
BigFloat* bigfloat_from_int_decimal(int,long long);
BigFloat* bigfloat_from_biginteger(BigInteger*,long long);
void bigfloat_print(BigFloat*);
void bigfloat_delete(BigFloat*);
int bigfloat_abs_comp(BigFloat*,BigFloat*);
BigFloat* bigfloat_copy(BigFloat*);
#endif
