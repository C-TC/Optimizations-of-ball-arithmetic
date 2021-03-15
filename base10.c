#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "base10.h"
#include "base10_helper.h"

BigInteger* biginteger_add(BigInteger* p_biginteger1, BigInteger* p_biginteger2){
    BigInteger* p_biginteger_result;
    if(p_biginteger1->is_positive == p_biginteger2->is_positive){
        // addition
        p_biginteger_result = malloc(sizeof(BigInteger));
        size_t result_size = p_biginteger1->array_size > p_biginteger2->array_size 
            ? p_biginteger1->array_size : p_biginteger2->array_size;
        p_biginteger_result->array = malloc(sizeof(int) * (result_size+1));
        p_biginteger_result->array_size = result_size;
        p_biginteger_result->is_positive = p_biginteger1->is_positive;
        
        int cur = 0;
        int idx = 0;
        for(int i=0;i<result_size;i++){
            if(i < p_biginteger1->array_size){
                cur += p_biginteger1->array[i];
            }
            if(i < p_biginteger2->array_size){
                cur += p_biginteger2->array[i];
            }
            p_biginteger_result->array[idx++] = cur % UNIT_CAPACITY;
            cur /= UNIT_CAPACITY;
        }
        if(cur != 0){
            p_biginteger_result->array_size++;
            p_biginteger_result->array[idx] = cur;
        }
    }else{
        // subtraction
        int comp_result = biginteger_abs_comp(p_biginteger1, p_biginteger2);
        size_t result_size;
        if(comp_result == 0){
            // result is 0;
            return biginteger_zero();
        }
        p_biginteger_result = malloc(sizeof(BigInteger));
        if(comp_result < 0){
            // swap p_biginteger1 and p_biginteger2
            BigInteger* tmp = p_biginteger1;
            p_biginteger1 = p_biginteger2;
            p_biginteger2 = tmp;
        }
        p_biginteger_result->is_positive = p_biginteger1->is_positive;
        result_size = p_biginteger1->array_size;
        p_biginteger_result->array_size = result_size;
        p_biginteger_result->array = malloc(sizeof(int) * result_size);

        bool borrow = false;
        for(int i=0;i<result_size;i++){
            int cur = p_biginteger1->array[i];
            if(i < p_biginteger2->array_size){
                cur -= p_biginteger2->array[i];
            }
            if(borrow){
                cur--;
            }
            if(cur < 0){
                borrow = true;
                cur += UNIT_CAPACITY;
            }else{
                borrow = false;
            }
            p_biginteger_result->array[i] = cur;
        }

        // remove leading zero
        for(int i=p_biginteger_result->array_size-1;i>0;i--){
            if(p_biginteger_result->array[i] == 0){
                p_biginteger_result->array_size--;
            }
        }

    }

    return p_biginteger_result;
}

// |scalar| should be less than UNIT_CAPACITY
BigInteger* biginteger_mul_scalar(BigInteger* p_biginteger, int scalar){
    if(scalar == 0){
        // result is 0
        return biginteger_zero();
    }

    BigInteger* p_biginteger_result = malloc(sizeof(BigInteger));
    size_t result_size = p_biginteger->array_size;
    p_biginteger_result->array = malloc(sizeof(int) * (result_size+1));
    p_biginteger_result->array_size = result_size;
    if(scalar > 0){
        p_biginteger_result->is_positive = p_biginteger->is_positive;
    }else{
        p_biginteger_result->is_positive = !p_biginteger->is_positive;
        scalar = -scalar;
    }
    
    long cur = 0;
    for(int i=0;i<p_biginteger->array_size;i++){
        cur += (long)scalar * p_biginteger->array[i];
        p_biginteger_result->array[i] = (int)(cur % UNIT_CAPACITY);
        cur /= UNIT_CAPACITY;
    }
    if(cur != 0){
        p_biginteger_result->array_size++;
        p_biginteger_result->array[result_size] = cur;
    }

    return p_biginteger_result;
}

BigInteger* biginteger_mul(BigInteger* p_biginteger1, BigInteger* p_biginteger2){
    BigInteger* p_biginteger_result = biginteger_zero();

    BigInteger *tmp1, *tmp2, *tmp3;
    for(int i=0;i<p_biginteger1->array_size;i++){
        tmp1 = biginteger_mul_scalar(p_biginteger2, p_biginteger1->array[i]);
        tmp2 = biginteger_left_shift(tmp1, i);
        tmp3 = biginteger_add(tmp2, p_biginteger_result);
        biginteger_delete(tmp1);
        biginteger_delete(tmp2);
        biginteger_delete(p_biginteger_result);
        p_biginteger_result = tmp3;
    }

    if(p_biginteger1->is_positive == p_biginteger2->is_positive){
        p_biginteger_result->is_positive = true;
    }else{
        p_biginteger_result->is_positive = false;
    }
    
    return p_biginteger_result;
}

BigFloat *biginteger_div(BigInteger* p_biginteger1, BigInteger* p_biginteger2){
    return NULL;
}

// https://en.wikipedia.org/wiki/Karatsuba_algorithm
BigInteger* karatsuba(BigInteger* p_biginteger1, BigInteger* p_biginteger2){
    if(p_biginteger1->array_size <= KARATSUBA_SWITCH || p_biginteger2->array_size <= KARATSUBA_SWITCH){
        return biginteger_mul(p_biginteger1, p_biginteger2);
    }
    int middle = p_biginteger1->array_size < p_biginteger2->array_size ? p_biginteger1->array_size : p_biginteger2->array_size;
    middle >>= 1;

    BigInteger* high1 = malloc(sizeof(BigInteger));
    BigInteger* low1 = malloc(sizeof(BigInteger));
    BigInteger* high2 = malloc(sizeof(BigInteger));
    BigInteger* low2 = malloc(sizeof(BigInteger));
    biginteger_split(p_biginteger1, middle, high1, low1);
    biginteger_split(p_biginteger2, middle, high2, low2);

    BigInteger* z0 = karatsuba(low1, low2);
    BigInteger* tmp1 = biginteger_add(low1, high1);
    BigInteger* tmp2 = biginteger_add(low2, high2);
    BigInteger* z1 = karatsuba(tmp1, tmp2);
    BigInteger* z2 = karatsuba(high1, high2);

    // result = (z2 × 10 ^ (m2 × 2)) + ((z1 - z2 - z0) × 10 ^ m2) + z0
    BigInteger* tmp3 = biginteger_left_shift(z2, middle << 1);
    z2->is_positive = !z2->is_positive;
    BigInteger* tmp4 = biginteger_add(z1, z2);
    z0->is_positive = !z0->is_positive;
    BigInteger* tmp5 = biginteger_add(tmp4, z0);
    BigInteger* tmp6 = biginteger_left_shift(tmp5, middle);
    z0->is_positive = !z0->is_positive;
    z2->is_positive = !z2->is_positive;
    BigInteger* tmp7 = biginteger_add(tmp3, tmp6);
    BigInteger* result = biginteger_add(tmp7, z0);

    biginteger_delete(tmp1);
    biginteger_delete(tmp2);
    biginteger_delete(tmp3);
    biginteger_delete(tmp4);
    biginteger_delete(tmp5);
    biginteger_delete(tmp6);
    biginteger_delete(tmp7);
    biginteger_delete(high1);
    biginteger_delete(low1);
    biginteger_delete(high2);
    biginteger_delete(low2);
    biginteger_delete(z0);
    biginteger_delete(z1);
    biginteger_delete(z2);
    return result;
}
