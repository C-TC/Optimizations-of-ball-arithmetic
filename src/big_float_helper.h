#ifndef BIG_FLOAT_HELPER
#define BIG_FLOAT_HELPER
#include "big_float.h"
#include "big_integer.h"

typedef union {
  double d;
  struct {
    unsigned long mantissa:52;
    unsigned int exponent : 11;
    unsigned int sign : 1;
  } parts;
} double_cast;

BigFloat big_float_create(BigInteger, long long);
BigFloat big_float_generate(const int, long long);
/* help func generate initial value for iteration methods */
BigFloat big_float_create_from_uint_fixed_precision(unsigned int value, const int precision);
void big_float_destroy(BigFloat *);
BigFloat big_float_deep_copy(const BigFloat);
double big_float_to_double(const BigFloat);
BigFloat double_to_big_float(double);
BigFloat double_to_big_float_fixed_precision(double, const int);
void big_float_print(BigFloat bf);
void big_float_print_msg(BigFloat bf, const char* msg);
#endif
