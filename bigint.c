#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "bigint.h"
#include "bigint_helper.h"

BigInteger* biginteger_add(BigInteger* p_biginteger1, BigInteger* p_biginteger2, BigInteger *p_biginteger_result){
    if(p_biginteger1->is_positive == p_biginteger2->is_positive){
        // addition
        if (!p_biginteger_result)
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
            if (!p_biginteger_result)
            return biginteger_zero();
            biginteger_set_inline(p_biginteger_result, 0);
            return p_biginteger_result;
        }
        if (!p_biginteger_result)
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
        tmp3 = biginteger_add(tmp2, p_biginteger_result, NULL);
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

int __naive_abs_div(BigInteger *p1, BigInteger *p2) {
    int result = 0;
    BigInteger *temp;
    while (1) {
        result += 1;
        temp = biginteger_mul_scalar(p2, result);
        int comp = biginteger_abs_comp(temp, p1);
        if (comp > 0) break;
    }
    return result - 1;
}

void __biginteger_abs_div(BigInteger *p1, BigInteger *p2, BigInteger *q) {
    q->array_size = p1->array_size - p2->array_size + 1;
    q->array = malloc(q->array_size * sizeof(int));
    BigInteger *cur_r = malloc(sizeof(BigInteger));
    BigInteger *low1  = malloc(sizeof(BigInteger));
    biginteger_split(p1, p1->array_size - p2->array_size, cur_r, low1);
    q->array[0] = __naive_abs_div(cur_r, p2);
    BigInteger *temp, *temp2;
    for (int pos = 1; pos < q->array_size; ++pos) {
        temp = biginteger_mul_scalar(p2, q->array[pos - 1]);
        temp->is_positive = !(temp->is_positive);
        temp2 = biginteger_add(cur_r, temp, NULL);
        SAFE_FREE(cur_r->array);
        cur_r->array_size += 1;
        cur_r->array = malloc( (1 + temp2->array_size)* sizeof(int));
        cur_r->array[0] = p1->array[pos];
        for (int i = 0; i < temp2->array_size; ++i) {
            cur_r->array[i + 1] = temp2->array[i];
        }
        q->array[pos] = __naive_abs_div(cur_r, p2); 
    }
    if (q->array_size > 1) {
        biginteger_delete(temp);
        biginteger_delete(temp2);
    }
    biginteger_delete(cur_r);
    biginteger_delete(low1);
}

int biginteger_div(BigInteger* p_biginteger1, BigInteger* p_biginteger2, BigInteger *q, BigInteger *r){
    //TODO: too many tedious manually mallocs, need to be fixed
    if (biginteger_is_zero(p_biginteger2)) return 0;
    if (biginteger_is_zero(p_biginteger1)) {
        biginteger_set_inline(q, 0);
        biginteger_set_inline(r, 0);
        return 1;
    }
    int comp = biginteger_abs_comp(p_biginteger1, p_biginteger2);
    if (comp == 0) {
        biginteger_set_inline(q, 1);
        biginteger_set_inline(r, 0);
        return 1;
    }
    if (comp < 0) {
        biginteger_set_inline(q, 0);
        BigInteger *ZERO_TEMP = biginteger_zero();
        biginteger_add(p_biginteger1, ZERO_TEMP, r);
        biginteger_delete(ZERO_TEMP);
        return 1;
    }
    __biginteger_abs_div(p_biginteger1, p_biginteger2, q);
    if (p_biginteger1->is_positive == p_biginteger2->is_positive) {
        q->is_positive = true;
    } else q->is_positive = false;
    BigInteger *pq = biginteger_mul(p_biginteger2, q);
    pq->is_positive = !(pq->is_positive);
    biginteger_add(p_biginteger1, pq, r);
    biginteger_delete(pq);
    return 1;
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
    BigInteger* tmp1 = biginteger_add(low1, high1, NULL);
    BigInteger* tmp2 = biginteger_add(low2, high2, NULL);
    BigInteger* z1 = karatsuba(tmp1, tmp2);
    BigInteger* z2 = karatsuba(high1, high2);

    // result = (z2 × 10 ^ (m2 × 2)) + ((z1 - z2 - z0) × 10 ^ m2) + z0
    BigInteger* tmp3 = biginteger_left_shift(z2, middle << 1);
    z2->is_positive = !z2->is_positive;
    BigInteger* tmp4 = biginteger_add(z1, z2, NULL);
    z0->is_positive = !z0->is_positive;
    BigInteger* tmp5 = biginteger_add(tmp4, z0, NULL);
    BigInteger* tmp6 = biginteger_left_shift(tmp5, middle);
    z0->is_positive = !z0->is_positive;
    z2->is_positive = !z2->is_positive;
    BigInteger* tmp7 = biginteger_add(tmp3, tmp6, NULL);
    BigInteger* result = biginteger_add(tmp7, z0, NULL);

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
