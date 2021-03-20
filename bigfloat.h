#ifndef __BIGFLOAT__
#define __BIGFLOAT__

#include "bigint.h"
#include "bigint_helper.h"


// Note: we always assume that mantissa is "normalized". "Normalized" is intended to be vague
// for different bases
typedef struct BigFloat {
    BigInteger *mantissa;
    long long power;
}BigFloat;

//==================== Operation Function ============
BigFloat *bigfloat_add(BigFloat *a, BigFloat *b, BigFloat *res);
BigFloat *bigfloat_sub(BigFloat *a, BigFloat *b, BigFloat *res);
BigFloat *bigfloat_mul(BigFloat *a, BigFloat *b, BigFloat *res);
BigFloat *bigfloat_div(BigFloat *a, BigFloat *b, BigFloat *res);
//====================================================

#endif
