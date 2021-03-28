#include "big_float_helper.h"

BigFloat big_float_create(BigInteger bi) {
    BigFloat bf;
    bf.mantissa = bi;
    return bf;
}

void big_float_destroy(BigFloat *bf) {
    big_integer_destroy(&bf->mantissa);
}
