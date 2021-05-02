#include "big_float_helper.h"
#include "big_integer.h"
#include <assert.h>
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
    unsigned int bit0,bit1,bit2;
    assert(size > 0);
    if (size >= 3) {
        bit0 = bits[size - 1];
        bit1 = bits[size - 2];
        bit2 = bits[size - 3];
    } else if(size == 2) {
        bit0 = bits[size - 1];
        bit1 = bits[size - 2];
        bit2 = 0;
    } else {
        bit0 = bits[size - 1];
        bit1 = 0;
        bit2 = 0;
    } 
    unsigned long data1 = (unsigned long) bit0 << 32 | bit1, data2 = (unsigned long) bit1 << 32 | bit2;
    unsigned long mask1 = 0x8000000000000000, mask2 = 0xFFFFFFFFFFFFF000, mask3 = 0xFFFFFFFF00000000;
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

BigFloat double_to_big_float(double data) {
    double_cast d_c;
    d_c.d = data;
    // assume data is not NAN or INF
    if (d_c.parts.exponent == 0 && d_c.parts.mantissa == 0) {
        // zero
        BigInteger bi = big_integer_create(0);
        BigFloat bf = big_float_create(bi, 0);
        return bf;
    } else if (d_c.parts.exponent == 0 && d_c.parts.mantissa != 0) {
        // denormalize  (−1)^s × 0.m × 2^−1022  
        unsigned int data1 = (unsigned int) (d_c.parts.mantissa >> 50), data2 = (unsigned int) (d_c.parts.mantissa >> 18), data3 = (unsigned int) d_c.parts.mantissa & 0x0003FFFF;
        long long power = -32;
        if (data1 != 0) {
            // left shift 32 bit
            power++;
            BigInteger bi = big_integer_create(0x0000FFFF0000FFFF); // to get a big int of size 2
            assert(bi.data.size == 2);
            bi.data.size++; //TODO: maybe need bigint resize here
            bi.data.bits[0] = data3;
            bi.data.bits[1] = data2;
            bi.data.bits[2] = data1;
            BigFloat bf = big_float_create(bi, power);
            bf.mantissa.sign = d_c.parts.sign == 0 ? 1 : -1;
            return bf;
        } else if (data2 != 0) {
            // no shift
            BigInteger bi = big_integer_create(0x0000FFFF0000FFFF); // to get a big int of size 2
            assert(bi.data.size == 2);
            bi.data.bits[0] = data3;
            bi.data.bits[1] = data2;
            BigFloat bf = big_float_create(bi, power);
            bf.mantissa.sign = d_c.parts.sign == 0 ? 1 : -1;
            return bf;
        }
    } else {
        // normal
        int expo = ((int) d_c.parts.exponent) - 1023 + 1; //double representation: exponent(unsigned) = power + 1023, take the "1" left to decimal point into consideration
        //printf("expo: %d\n",expo);
        unsigned long man = d_c.parts.mantissa | 0x0010000000000000;
        man <<= 11; //64-53
        int power = expo / 32;
        //printf("power: %d\n",power);
        int remainder = expo % 32;
        if (remainder < 0) {
            //modulo for negative value
            power--;
            remainder+=32; 
        }
        //printf("remainder: %d\n",remainder);
        unsigned long part1 = man >> 32, part2 = man;
        part1 <<= remainder;
        part2 <<= remainder;
        unsigned int data1 = (unsigned int) (part1 >> 32), data2 = (unsigned int) (part2 >> 32), data3 = (unsigned int) (part2 & 0x00000000FFFFFFFF);
        if (data1 != 0) {
            // left shift 32 bit
            power++;
            BigInteger bi = big_integer_create(0x0000FFFF0000FFFF); // to get a big int of size 2
            assert(bi.data.size == 2);
            bi.data.size++; //TODO: maybe need bigint resize here
            bi.data.bits[0] = data3;
            bi.data.bits[1] = data2;
            bi.data.bits[2] = data1;
            BigFloat bf = big_float_create(bi, power);
            bf.mantissa.sign = d_c.parts.sign == 0 ? 1 : -1;
            return bf;
        } else {
            // no shift
            BigInteger bi = big_integer_create(0x0000FFFF0000FFFF); // to get a big int of size 2
            assert(bi.data.size == 2);
            bi.data.bits[0] = data3;
            bi.data.bits[1] = data2;
            BigFloat bf = big_float_create(bi, power);
            bf.mantissa.sign = d_c.parts.sign == 0 ? 1 : -1;
            return bf;
        }
    }
    assert(0); // input is NAN or INF!
}

void big_float_print(BigFloat bf) {
    int print_precision = 5;
    if (bf.mantissa.sign < 0) {
        printf("-0. ");
    } else {
        printf("0. ");
    }
    int i = bf.mantissa.data.size - 1, j;
    printf("[%u]",bf.mantissa.data.bits[i]);
    for (j = i - 1; j >= i - print_precision + 1 && j >= 0; j--) {
        printf(" -- [%u]",bf.mantissa.data.bits[j]);
    }
    if (j >= 0) {
        printf(" -- ...");
    }
    printf(" x 2^(32 x %lld) \n",bf.power);
}