#ifndef __BIGFLOAT__
#define __BIGFLOAT__

#include "bigint.h"
#include "bigint_helper.h"


// Note: we always assume that mantissa is "normalized". "Normalized" is intended to be vague
// for different bases
typedef struct BigFloat {
    BigInteger *mantissa;
    long long exponent;
}BigFloat;

/*
Base 10: sign of bigfloat is sign of mantissa
Normalized to 0.1~1 x 10^n
0: mantissa 0, exponent 0
*/


//==================== Operation Function ============
BigFloat *bigfloat_add(BigFloat *a, BigFloat *b, BigFloat *res);
BigFloat *bigfloat_sub(BigFloat *a, BigFloat *b, BigFloat *res);
BigFloat *bigfloat_mul(BigFloat *a, BigFloat *b, BigFloat *res);
BigFloat *bigfloat_div(BigFloat *a, BigFloat *b, BigFloat *res);
//====================================================

#endif
