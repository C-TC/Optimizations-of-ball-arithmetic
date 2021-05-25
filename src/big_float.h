#ifndef BIG_FLOAT_H
#define BIG_FLOAT_H
#include "big_integer.h"

#define DIV_STEP 9
typedef struct BigFloat
{
    BigInteger mantissa;
    long long power;
} BigFloat;

BigFloat big_float_multiply(BigFloat, BigFloat);
BigFloat big_float_div(BigFloat, BigFloat);
BigFloat big_float_add(BigFloat, BigFloat);
BigFloat big_float_sub(BigFloat, BigFloat);

// lo = lo + ro
void big_float_add_inplace_fixed_precision(BigFloat *, BigFloat, const int);
// lo = lo - ro
void big_float_sub_inplace_fixed_precision(BigFloat *, BigFloat, const int);
// lo = lo * ro
void big_float_mul_inplace_fixed_precision(BigFloat *, BigFloat, const int);

BigFloat big_float_sqrt_fix_precision(BigFloat, const int, const int);
BigFloat big_float_reciprocal_sqrt_fix_precision(BigFloat, const int, const int);
#endif /* BIG_FLOAT_H */