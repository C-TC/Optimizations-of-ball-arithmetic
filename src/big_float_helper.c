#include "big_float_helper.h"
#include "big_integer.h"
BigFloat big_float_create(BigInteger bi, long long power) {
    BigFloat bf;
    bf.mantissa = bi;
    bf.power = power;
    return bf;
}

void big_float_destroy(BigFloat *bf) {
    if (&bf->mantissa) big_integer_destroy(&bf->mantissa);
}

BigFloat big_float_deep_copy(const BigFloat bf){
    BigFloat res;
    res.power = bf.power;
    res.mantissa = big_integer_deepcopy(bf.mantissa);
    return res;
}
