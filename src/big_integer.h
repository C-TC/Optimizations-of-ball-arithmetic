#ifndef BIG_INTEGER_H
#define BIG_INTEGER_H
#include <stdio.h>

/*
** big_integer.h
** Code largely based on this repo: <https://github.com/andreazevedo/biginteger>
** Modifications: support for arbitrary large numbers, bigint multiplication
**/

typedef struct BigIntegerData {
  unsigned long *bits;
  // assume the number would not be larger than 2^INT_MAX
  int size;
  int capacity;
} BigIntegerData;

typedef struct BigInteger {
  int sign;
  BigIntegerData data;
} BigInteger;

/* create a big integer number */
BigInteger big_integer_create(long long value);
/* set a big integer number with a long long value*/
void big_integer_set(long long value, BigInteger *pBigInt);

/* create a big integer number from file in the specific format:
 *     sign size bits[0] bits[1] ... bits[size-1]
 */
BigInteger big_integer_create_from_file(FILE **ppFile);
/* output a big integer number to file in the specific format:
 *     sign size bits[0] bits[1] ... bits[size-1]
 */
void big_integer_output_to_file(const BigInteger bigInt, FILE **ppFile);
/* print a big integer number to stdout in the specific format:
 *     sign size bits[0] bits[1] ... bits[size-1]
 */
void big_integer_print(const BigInteger bigInt, const char *msg);

/* free the allocated bits array and set size/sign/capacity to 0 */
void big_integer_destroy(BigInteger *pBigInt);

/* returns the big integer as int */
int big_integer_to_int(const BigInteger bigInt);

/* returns the big integer as long long */
long long big_integer_to_long_long(const BigInteger bigInt);

/* compare big integers */
int big_integer_compare(const BigInteger left, const BigInteger right);
/* compare two unsigned bigints, return 1 if left > right, 0 if =, & -1 if < */
int big_integer_compare_data(const BigIntegerData *, const BigIntegerData *);

/* adds two big integers together ( left + right ) */
BigInteger big_integer_add(const BigInteger left, const BigInteger right);
void big_integer_add_inplace(const BigInteger left, const BigInteger right,
                             BigInteger *pResult);
/*
 * Adds two fixed precision big integers inplacely
 * i.e. left += right
 *
 * !!! left and right can refer to the same object
 *
 * Assumptions:
 * !!! left.size >= precision
 * !!! right.size >= precision
 *  => almost no memory allocation
 * !!! left and right should not have leading zeros
 * carried is used in big float addition, =1/-1 if add makes size bigger by 1/-1.
 */
void big_integer_add_inplace_fixed_precision(BigInteger *left,
                                             const BigInteger right,
                                             const int precision,
                                             int *carried);

/* subtracts one big integer from another ( left - right ) */
BigInteger big_integer_subtract(const BigInteger left, const BigInteger right);
void big_integer_subtract_inplace(const BigInteger left, const BigInteger right,
                                  BigInteger *pResult);

/* increments the bigInteger by the amount specified */
void big_integer_increment(BigInteger *bigInt, const unsigned int value);

/* decrements the bigInteger by the amount specified */
void big_integer_decrement(BigInteger *bigInt, const unsigned int value);

/* multiply two big integers ( result = left * right ) */
BigInteger big_integer_multiply(const BigInteger left, const BigInteger right);
/* multiply two big integers ( result = left * right ), reduced memory copy */
BigInteger big_integer_multiply_opt(const BigInteger left,
                                    const BigInteger right);
/* multiply two big integers ( result = left * right ) */
void big_integer_multiply_inplace(const BigInteger left, const BigInteger right,
                                  BigInteger *pResult);
/* multiply_two_big_integers ( left = left * right) */
void big_integer_multiply_two_operands_opt(BigInteger *pLeft,
                                           const BigInteger right);
/* A divide-and-conquer algorithm to speedup multiplication of very large
 * numbers */
BigInteger big_integer_multiply_karatsuba(const BigInteger left,
                                          const BigInteger right);

/*
 * Multiply two fixed precision big integers inplacely
 * i.e. left *= right
 *
 * !!! left and right can refer to the same object
 *
 * Assumptions:
 * !!! left.size >= precision
 * !!! right.size >= precision
 *  => almost no memory allocation
 * !!! left and right should not have leading zeros
 */
void big_integer_multiply_inplace_fixed_precision(BigInteger *left,
                                                  const BigInteger right,
                                                  const int precision);
/* 
 * Assumptions: 
 * !!! left.size >= precision
 * !!! right.size >= precision
 */                                                  
void big_integer_multiply_inplace_fixed_precision_unflod(BigInteger *left,
                                                  const BigInteger right,
                                                  const int precision);
void big_integer_multiply_inplace_fixed_precision_verter_1x_unfold(BigInteger *left,
                                                         const BigInteger right,
                                                         const int precision);
void big_integer_multiply_inplace_fixed_precision_verter_2x_unfold(BigInteger *left,
                                                         const BigInteger right,
                                                         const int precision);
void big_integer_multiply_inplace_fixed_precision_verter_4x_unfold(BigInteger *left,
                                                         const BigInteger right,
                                                         const int precision);
void big_integer_multiply_inplace_fixed_precision_verter_8x_unfold(BigInteger *left,
                                                         const BigInteger right,
                                                         const int precision);
void big_integer_multiply_inplace_fixed_precision_verter_12x_unfold(BigInteger *left,
                                                         const BigInteger right,
                                                         const int precision);
void big_integer_multiply_inplace_fixed_precision_verter_16x_unfold(BigInteger *left,
                                                         const BigInteger right,
                                                         const int precision);
void big_integer_multiply_inplace_quad_double(BigInteger *left, const BigInteger right);
void big_integer_multiply_inplace_fixed_precision_verter_16x_unfold_reduce_intop_1x(BigInteger *left,
                                                         const BigInteger right,
                                                         const int precision);
void big_integer_multiply_inplace_fixed_precision_verter_16x_unfold_reduce_intop_2x(BigInteger *left,
                                                         const BigInteger right,
                                                         const int precision);
void big_integer_multiply_inplace_fixed_precision_verter_16x_unfold_reduce_intop_4x(BigInteger *left,
                                                         const BigInteger right,
                                                         const int precision);
void big_integer_multiply_inplace_fixed_precision_verter_16x_unfold_reduce_intop_8x(BigInteger *left,
                                                         const BigInteger right,
                                                         const int precision);

/* 
 * Assumptions: 
 * !!! input have same precision (length)
 * !!! input have same sign
 * !!! precision = input.size
 * !!! res.size >= input.size + 1
 * !!! data in res.bits are all zeros
 */    
void big_integer_sum_2(const BigInteger op1, const BigInteger op2, const int precision, BigInteger* res);                                                         
void big_integer_sum_3(const BigInteger op1, const BigInteger op2, const BigInteger op3, const int precision, BigInteger* res);                                                         
void big_integer_sum_3_unfold_1x(const BigInteger op1, const BigInteger op2, const BigInteger op3, const int precision, BigInteger* res);                                                         
void big_integer_sum_3_unfold_2x(const BigInteger op1, const BigInteger op2, const BigInteger op3, const int precision, BigInteger* res);                                                         
void big_integer_sum_3_unfold_4x(const BigInteger op1, const BigInteger op2, const BigInteger op3, const int precision, BigInteger* res);                                                         
void big_integer_sum_3_unfold_8x(const BigInteger op1, const BigInteger op2, const BigInteger op3, const int precision, BigInteger* res);                                                         
void big_integer_sum_5(const BigInteger op1, const BigInteger op2, const BigInteger op3, const BigInteger op4, const BigInteger op5, const int precision, BigInteger* res);                                                         
void big_integer_sum_5_unfold_1x(const BigInteger op1, const BigInteger op2, const BigInteger op3, const BigInteger op4, const BigInteger op5, const int precision, BigInteger* res);                                                         
void big_integer_sum_5_unfold_2x(const BigInteger op1, const BigInteger op2, const BigInteger op3, const BigInteger op4, const BigInteger op5, const int precision, BigInteger* res);                                                         
void big_integer_sum_5_unfold_4x(const BigInteger op1, const BigInteger op2, const BigInteger op3, const BigInteger op4, const BigInteger op5, const int precision, BigInteger* res);                                                         
void big_integer_sum_5_unfold_8x(const BigInteger op1, const BigInteger op2, const BigInteger op3, const BigInteger op4, const BigInteger op5, const int precision, BigInteger* res);                                                         

/* create a deep copy of a bigint/bigintdata */
BigInteger big_integer_deepcopy(const BigInteger other);

/*e.g. 1-2-3-4 to 1-2-3-4-0-0, untested bigfloat helperfunc */
BigInteger big_integer_add_trailing_zeros(const BigInteger, int);
void big_integer_add_trailing_zeros_inplace(BigInteger *, int);

#ifdef DEBUG
void big_integer_dump(const BigInteger bigInt);
#endif

#endif /* BIG_INTEGER_H */