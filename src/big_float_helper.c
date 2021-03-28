#include "big_float_helper.h"

BigFloat big_float_create(BigInteger bi, long long power) {
    BigFloat bf;
    bf.mantissa = bi;
    bf.power = power;
    return bf;
}

void big_float_destroy(BigFloat *bf) {
    big_integer_destroy(&bf->mantissa);
}
