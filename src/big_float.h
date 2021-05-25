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
BigFloat big_float_mul_fixed_precision(BigFloat, BigFloat, const int);
void big_float_mul_toplace_fixed_precision(BigFloat, BigFloat, BigFloat *, const int);
// BigFloat /= 2^power_of_two
void big_float_div_by_power_of_two_inplace_fixed_precision(BigFloat *, int power_of_two, const int precision);

BigFloat big_float_sqrt_fix_precision(BigFloat, const int, const unsigned int);
BigFloat big_float_reciprocal_sqrt_fix_precision(BigFloat, const int, const unsigned int);
BigFloat big_float_guided_reciprocal_sqrt_fix_precision(BigFloat, const int);
#endif /* BIG_FLOAT_H */