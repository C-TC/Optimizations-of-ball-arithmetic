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

typedef union {
  double d;
  struct {
    unsigned long mantissa:52;
    unsigned int exponent : 11;
    unsigned int sign : 1;
  } parts;
} double_cast;

double big_float_to_double(const BigFloat bf) {
    if(bf.mantissa.data.bits[bf.mantissa.data.size - 1] == 0) {//if bf = 0 because we assume bf is normalized
        return 0.0;
    }
    double_cast result;
    result.parts.sign = bf.mantissa.sign >= 0? 0 : 1;
    // actually, we can't use double to represent bigfloat of power >32 or <-31, so this (long long) to (int) is safe
    if (bf.power > 32) {
        // infinity
        result.parts.mantissa = 0;
        #pragma GCC diagnostic push     // ignore this warning 
        #pragma GCC diagnostic ignored "-Woverflow"
        result.parts.exponent = 0xFFFFFFFF;
        #pragma GCC diagnostic pop
        return result.d;
    } else if (bf.power < -32) {
        //underflow
        printf("big_float_to_double underflow! return 0.0\n");
        return 0.0;
    }
    int expo = (int) bf.power * 32; 
    int size = bf. mantissa.data.size;
    unsigned int *bits = bf.mantissa.data.bits;
    unsigned int bit0 = bits[size - 1], bit1 = bits[size - 2], bit2 = bits[size - 3];
    unsigned long data1 = (unsigned long) bit0 << 32 | bit1, data2 = (unsigned long) bit1 << 32 | bit2;
    unsigned long mask1 = 0x8000000000000000, mask2 = 0xFFFFFFFFFFFFF000, mask3 = 0xFFFFFFFF00000000, mask4 = 0x00000000FFFFFFFF;
    if (bf.power == -32) {
        unsigned long res = data1 >> 2;
        res &= mask2;
        if (res == 0){
            //underflow
            printf("big_float_to_double underflow! return 0.0\n");
            return 0.0;
        } else {
            //denormalize case 1
            result.parts.exponent = 0;
            result.parts.mantissa = res >> 12;
            return result.d;
        }
    }
    while ((data1 & mask1) == 0) {
        data1 <<= 1;
        data2 <<= 1;
        expo--;
        if(expo == -1022) {
            //denormalize case 2
            result.parts.exponent = 0;
            result.parts.mantissa = ((data1 & mask3) | ((data2 & mask3) >> 32)) >> 12;
            return result.d;
        }
    }
    // normal case: attention! move the first 1 to the left of decimal
    data1 <<= 1;
    data2 <<= 1;
    expo--;
    expo += 1023; //double representation: exponent(unsigned) = power + 1023 
    result.parts.exponent = (unsigned int) expo;
    result.parts.mantissa = ((data1 & mask3) | ((data2 & mask3) >> 32)) >> 12;
    return result.d;
}