#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define UNIT_CAPACITY 100000000
#define UNIT_LEN 8

typedef struct BigInteger{
    bool is_positive;
    int* array;
    size_t array_size;
}BigInteger;

// typedef struct ExactFloat{
//     bool is_positive;
//     BigInteger* numerator;
//     BigInteger* denominator;
// }ExactFloat;

BigInteger* biginteger_from_string(char* integer_str, size_t size){
    bool is_positive;
    if(integer_str[0] == '+'){
        is_positive = true;
        integer_str = integer_str + 1;
        size--;
    }else if(integer_str[0] == '-'){
        is_positive = false;
        integer_str = integer_str + 1;
        size--;
    }else{
        is_positive = true;
    }

    size_t array_size = (size_t)ceil((double)size / UNIT_LEN);
    BigInteger* p_biginteger = malloc(sizeof(BigInteger));
    p_biginteger->array = malloc(sizeof(int) * array_size);
    p_biginteger->array_size = array_size;
    p_biginteger->is_positive = is_positive;
    int cur = 0;
    int idx = 0;
    int coef = 1;
    for(int i=size-1;i>=0;i--){
        cur += (integer_str[i] - '0') * coef;
        if(coef >= UNIT_CAPACITY){
            p_biginteger->array[idx++] = cur % UNIT_CAPACITY;
            cur /= UNIT_CAPACITY;
            coef = 1;
        }
        coef *= 10;
    }
    p_biginteger->array[idx] = cur;
    // remove leading zero
    for(int i=array_size-1;i>0;i--){
        if(p_biginteger->array[i] == 0){
            p_biginteger->array_size--;
        }
    }
    return p_biginteger;
}

int biginteger_delete(BigInteger* p_biginteger){
    free(p_biginteger->array);
    free(p_biginteger);
    return 0;
}

int biginteger_print_debug(BigInteger* p_biginteger){
    printf("array_size:\t%lu, array:\t", p_biginteger->array_size);
    for(int i=0;i<p_biginteger->array_size;i++){
        printf("%d\t", p_biginteger->array[i]);
    }
    printf("\n");
    return 0;
}

bool biginteger_is_zero(BigInteger* p_biginteger){
    return p_biginteger->array_size == 1 && p_biginteger->array[0] == 0;
}

bool biginteger_is_one(BigInteger* p_biginteger){
    return p_biginteger->array_size == 1 && p_biginteger->array[0] == 1 && p_biginteger->is_positive;
}

bool biginteger_is_negative_one(BigInteger* p_biginteger){
    return p_biginteger->array_size == 1 && p_biginteger->array[0] == 1 && !p_biginteger->is_positive;
}

int biginteger_print(BigInteger* p_biginteger){
    if(!p_biginteger->is_positive && !biginteger_is_zero(p_biginteger)){
        printf("-");
    }
    for(int i=p_biginteger->array_size-1;i>=0;i--){
        if(i == p_biginteger->array_size-1)
            printf("%d", p_biginteger->array[i]);
        else
            printf("%08d", p_biginteger->array[i]);
    }
    printf("\n");
    return 0;
}

// compare their absolute value
// return 1 if p_biginteger1 is larger
// return -1 if p_biginteger2 is larger
// return 0 otherwise
int biginteger_abs_comp(BigInteger* p_biginteger1, BigInteger* p_biginteger2){
    if(p_biginteger1->array_size > p_biginteger2->array_size){
        return 1;
    }else if(p_biginteger1->array_size < p_biginteger2->array_size){
        return -1;
    }
    for(int i=p_biginteger1->array_size-1;i>=0;i--){
        if(p_biginteger1->array[i] > p_biginteger2->array[i]){
            return 1;
        }else if(p_biginteger1->array[i] < p_biginteger2->array[i]){
            return -1;
        }
    }
    return 0;
}

BigInteger* biginteger_zero(){
    BigInteger* p_biginteger_zero = malloc(sizeof(BigInteger));
    p_biginteger_zero->is_positive = true;
    p_biginteger_zero->array_size = 1;
    p_biginteger_zero->array = malloc(sizeof(int));
    p_biginteger_zero->array[0] = 0;
    return p_biginteger_zero;
}

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

// cnt should be >= 0
BigInteger* biginteger_left_shift(BigInteger* p_biginteger, int cnt){
    if(biginteger_is_zero(p_biginteger)){
        return biginteger_zero();
    }

    BigInteger* p_biginteger_result = malloc(sizeof(BigInteger));
    size_t result_size = p_biginteger->array_size + cnt;
    p_biginteger_result->array = malloc(sizeof(int) * result_size);
    p_biginteger_result->array_size = result_size;
    p_biginteger_result->is_positive = p_biginteger->is_positive;

    for(int i=0;i<cnt;i++){
        p_biginteger_result->array[i] = 0; // change it to memset???
    } 

    for(int i=cnt;i<result_size;i++){
        p_biginteger_result->array[i] = p_biginteger->array[i-cnt]; // change it to memcpy???
    }

    return p_biginteger_result;
}

// cnt should be >= 0
BigInteger* biginteger_right_shift(BigInteger* p_biginteger, int cnt){
    size_t result_size = p_biginteger->array_size - cnt;
    if(result_size <= 0){
        return biginteger_zero();
    }
    BigInteger* p_biginteger_result = malloc(sizeof(BigInteger));
    p_biginteger_result->array = malloc(sizeof(int) * result_size);
    p_biginteger_result->array_size = result_size;
    p_biginteger_result->is_positive = p_biginteger->is_positive;
    memcpy(p_biginteger_result->array, p_biginteger->array+cnt, sizeof(int) * result_size);
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

BigInteger* biginteger_copy(BigInteger* p_biginteger){
    BigInteger* p_biginteger_result = malloc(sizeof(BigInteger));
    p_biginteger_result->array_size = p_biginteger->array_size;
    p_biginteger_result->array = malloc(sizeof(int) * p_biginteger->array_size);
    p_biginteger_result->is_positive = p_biginteger->is_positive;
    memcpy(p_biginteger_result->array, p_biginteger->array, sizeof(int) * p_biginteger->array_size);
    return p_biginteger_result;
}

BigInteger* biginteger_add_leading_zero(BigInteger* p_biginteger, int cnt){
    BigInteger* p_biginteger_result = malloc(sizeof(BigInteger));
    size_t result_size = p_biginteger->array_size + cnt;
    p_biginteger_result->array_size = result_size;
    p_biginteger_result->array = malloc(sizeof(int) * result_size);
    p_biginteger_result->is_positive = p_biginteger->is_positive;
    memcpy(p_biginteger_result->array, p_biginteger->array, sizeof(int) * p_biginteger->array_size);
    memset(p_biginteger_result->array + p_biginteger->array_size, 0, sizeof(int) * cnt);
    return p_biginteger_result;
}

// |number| shoule < UNIT_CAPACITY
BigInteger* biginteger_from_number(int number){
    BigInteger* p_biginteger = biginteger_zero();
    if(number < 0){
        number = -number;
        p_biginteger->is_positive = false;
    }
    p_biginteger->array[0] = number;
    return p_biginteger;
}

// TODO
int biginteger_div(BigInteger* p_biginteger1, BigInteger* p_biginteger2, BigInteger* p_quotient, BigInteger* p_remainder){
//     if(biginteger_is_zero(p_biginteger2)){
//         // divided by zero error
//         return 1;
//     }

//     if(biginteger_is_one(p_biginteger2)){
//         p_quotient = biginteger_copy(p_biginteger1);
//         p_remainder = biginteger_zero();
//         return 0;
//     }

//     if(biginteger_is_negative_one(p_biginteger2)){
//         p_quotient = biginteger_copy(p_biginteger1);
//         p_quotient->is_positive = !p_quotient->is_positive;
//         p_remainder = biginteger_zero();
//         return 0;
//     }

//     if(biginteger_abs_comp(p_biginteger1, p_biginteger2) < 0){
//         p_quotient = biginteger_zero();
//         p_remainder = biginteger_copy(p_biginteger1);
//         if(p_biginteger1->is_positive && p_biginteger2->is_positive){
//             return 0;
//         }
//         if(!p_biginteger1->is_positive && !p_biginteger2->is_positive){
//             p_remainder->is_positive = false;
//             return 0;
//         }
//         BigInteger* tmp1 = biginteger_copy(p_biginteger1);
//         tmp1->is_positive = false;
//         BigInteger* tmp2 = biginteger_add(p_remainder, tmp1);
//         biginteger_delete(tmp1);
//         biginteger_delete(p_remainder);
//         p_remainder = tmp2;
//         if(!p_biginteger2->is_positive){
//             return 0;
//         }
//         p_remainder->is_positive = true;
//         return 0;
//     }

//     BigInteger* tmp1 = biginteger_add_leading_zero(p_biginteger1, 1);
//     int n = tmp1->array_size;
//     int m = p_biginteger2->array_size;
//     int div_first_digit = p_biginteger2->array[m-1];
//     BigInteger* r = biginteger_right_shift(p_biginteger1, n-m);
//     r->is_positive = true;
//     p_quotient = biginteger_zero();
//     for(int i=0;i<n-m;i++){
//         long q = ((long)tmp1->array[n-i-1] * UNIT_CAPACITY + tmp1->array[n-i-2]) / div_first_digit;
//         BigInteger* tmp2 = biginteger_mul_scalar(p_biginteger2, q);
//         while(q > 0 && biginteger_abs_comp(tmp2, r) > 0){
//             q--;
//             biginteger_delete(tmp2);
//             tmp2 = biginteger_mul_scalar(p_biginteger2, q);
//         }
//         BigInteger* tmp3 = biginteger_left_shift(p_quotient, 1);
//         biginteger_delete(p_quotient);
//         BigInteger* tmp6 = biginteger_from_number(q);
//         p_quotient = biginteger_add(tmp3, tmp6);
//         biginteger_delete(tmp3);
//         biginteger_delete(tmp6);

//         tmp2->is_positive = false;
//         BigInteger* tmp4 = biginteger_add(r, tmp2);
//         biginteger_delete(tmp2);
//         biginteger_delete(r);
//         if(i + 1 < n - m){
//             BigInteger* tmp5 = biginteger_left_shift(tmp4, 1);
//             biginteger_delete(tmp4);
//             BigInteger* tmp7 = biginteger_from_number(p_biginteger1->array[n-i-5]);
//             r = biginteger_add(tmp5, tmp7);
//         }else{
//             p_remainder = r;
//         }
//     }

    return 0;
}

// int exactfloat_delete(ExactFloat* p_exactfloat){
//     biginteger_delete(p_exactfloat->numerator);
//     biginteger_delete(p_exactfloat->denominator);
//     free(p_exactfloat);
//     return 0;
// }

// TODO
// BigInteger* gcd(BigInteger* a, BigInteger* b){
//     BigInteger* t;
//     while(!biginteger_is_zero(b)){
//         t = b
//         b := a mod b
//         a = t
//     }
//     return a;
// }

// int exactfloat_reduce(ExactFloat* p_exactfloat){
//     // TODO
//     return 0;
// }

// ExactFloat* exactfloat_mul(ExactFloat* p_exactfloat1, ExactFloat* p_exactfloat2){
//     ExactFloat* p_exactfloat_result = malloc(sizeof(ExactFloat));
//     p_exactfloat_result->numerator = biginteger_mul(p_exactfloat1->numerator, p_exactfloat2->numerator);
//     p_exactfloat_result->denominator = biginteger_mul(p_exactfloat1->denominator, p_exactfloat2->denominator);
//     exactfloat_reduce(p_exactfloat_result);
//     return p_exactfloat_result;
// }

// ExactFloat* exactfloat_add(ExactFloat* p_exactfloat1, ExactFloat* p_exactfloat2){
//     ExactFloat* p_exactfloat_result = malloc(sizeof(ExactFloat));
//     BigInteger* tmp1 = biginteger_mul(p_exactfloat1->numerator, p_exactfloat2->denominator);
//     BigInteger* tmp2 = biginteger_mul(p_exactfloat2->numerator, p_exactfloat1->denominator);
//     p_exactfloat_result->numerator = biginteger_add(tmp1, tmp2);
//     p_exactfloat_result->denominator = biginteger_mul(p_exactfloat1->denominator, p_exactfloat2->denominator);
//     exactfloat_reduce(p_exactfloat_result);
//     biginteger_delete(tmp1);
//     biginteger_delete(tmp2);
//     return p_exactfloat_result;
// }

// The second argument of the split_at function specifies 
// the number of digits to extract from the right
// for example, split_at("12345", 3) will extract the 3 final digits, giving: high="12", low="345".
int biginteger_split(BigInteger* p_biginteger, int m, BigInteger* high, BigInteger* low){
    size_t high_size = p_biginteger->array_size - m;
    size_t low_size = m;
    high->array_size = high_size;
    low->array_size = low_size;
    high->array = malloc(sizeof(int) * high_size);
    low->array = malloc(sizeof(int) * low_size);
    high->is_positive = p_biginteger->is_positive;    
    low->is_positive = p_biginteger->is_positive;    
    memcpy(high->array, p_biginteger->array + low_size, high_size * sizeof(int));
    memcpy(low->array, p_biginteger->array, low_size * sizeof(int));
    return 0;
}

#define KARATSUBA_SWITCH 20
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


int main(){
    // char str1[10000];
    // char str2[10000];
    // printf("integer1: ");
    // scanf("%s", str1);
    // printf("integer2: ");
    // scanf("%s", str2);
    // BigInteger* p_biginteger1 = biginteger_from_string(str1, strlen(str1));
    // BigInteger* p_biginteger2 = biginteger_from_string(str2, strlen(str2));
    // biginteger_print_debug(p_biginteger1);
    // biginteger_print(p_biginteger1);
    // biginteger_print_debug(p_biginteger2);
    // biginteger_print(p_biginteger2);
    // BigInteger* p_biginteger3 = biginteger_add(p_biginteger1, p_biginteger2);
    // biginteger_print_debug(p_biginteger3);
    // biginteger_print(p_biginteger3);
    // biginteger_delete(p_biginteger1);
    // biginteger_delete(p_biginteger2);
    // biginteger_delete(p_biginteger3);

    // char str1[10000];
    // char str2[10000];
    // while(scanf("%s %s", str1, str2) == 2){
    //     BigInteger* p_biginteger1 = biginteger_from_string(str1, strlen(str1));
    //     // printf("%lu\n", strlen(str1));
    //     // biginteger_print(p_biginteger1);
    //     BigInteger* p_biginteger2 = biginteger_from_string(str2, strlen(str2));
    //     // biginteger_print(p_biginteger2);
    //     BigInteger* p_biginteger3 = biginteger_add(p_biginteger1, p_biginteger2);
    //     biginteger_print(p_biginteger3);
    //     biginteger_delete(p_biginteger1);
    //     biginteger_delete(p_biginteger2);
    //     biginteger_delete(p_biginteger3);
    // }

    // char str1[10000];
    // int scalar;
    // printf("integer1: ");
    // scanf("%s", str1);
    // printf("scalar: ");
    // scanf("%d", &scalar);
    // BigInteger* p_biginteger1 = biginteger_from_string(str1, strlen(str1));
    // biginteger_print_debug(p_biginteger1);
    // biginteger_print(p_biginteger1);
    // BigInteger* p_biginteger2 = biginteger_mul_scalar(p_biginteger1, scalar);
    // biginteger_print_debug(p_biginteger2);
    // biginteger_print(p_biginteger2);
    // biginteger_delete(p_biginteger1);
    // biginteger_delete(p_biginteger2);

    // char str1[10000];
    // int scalar;
    // while(scanf("%s %d", str1, &scalar) == 2){
    //     BigInteger* p_biginteger1 = biginteger_from_string(str1, strlen(str1));
    //     BigInteger* p_biginteger2 = biginteger_mul_scalar(p_biginteger1, scalar);
    //     biginteger_print(p_biginteger2);
    //     biginteger_delete(p_biginteger1);
    //     biginteger_delete(p_biginteger2);
    // }

    // char str1[10000];
    // int shift;
    // printf("integer1: ");
    // scanf("%s", str1);
    // printf("shift: ");
    // scanf("%d", &shift);
    // BigInteger* p_biginteger1 = biginteger_from_string(str1, strlen(str1));
    // biginteger_print_debug(p_biginteger1);
    // biginteger_print(p_biginteger1);
    // BigInteger* p_biginteger2 = biginteger_left_shift(p_biginteger1, shift);
    // biginteger_print_debug(p_biginteger2);
    // biginteger_print(p_biginteger2);
    // biginteger_delete(p_biginteger1);
    // biginteger_delete(p_biginteger2);


    // char str1[10000];
    // char str2[10000];
    // printf("integer1: ");
    // scanf("%s", str1);
    // printf("integer2: ");
    // scanf("%s", str2);
    // BigInteger* p_biginteger1 = biginteger_from_string(str1, strlen(str1));
    // BigInteger* p_biginteger2 = biginteger_from_string(str2, strlen(str2));
    // biginteger_print_debug(p_biginteger1);
    // biginteger_print(p_biginteger1);
    // biginteger_print_debug(p_biginteger2);
    // biginteger_print(p_biginteger2);
    // BigInteger* p_biginteger3 = biginteger_mul(p_biginteger1, p_biginteger2);
    // biginteger_print_debug(p_biginteger3);
    // biginteger_print(p_biginteger3);
    // biginteger_delete(p_biginteger1);
    // biginteger_delete(p_biginteger2);
    // biginteger_delete(p_biginteger3);

    char str1[10000];
    char str2[10000];
    while(scanf("%s %s", str1, str2) == 2){
        BigInteger* p_biginteger1 = biginteger_from_string(str1, strlen(str1));
        BigInteger* p_biginteger2 = biginteger_from_string(str2, strlen(str2));
        BigInteger* p_biginteger3 = biginteger_mul(p_biginteger1, p_biginteger2);
        biginteger_print(p_biginteger3);
        biginteger_delete(p_biginteger1);
        biginteger_delete(p_biginteger2);
        biginteger_delete(p_biginteger3);
    }


    // char str1[10000];
    // int split;
    // printf("integer1: ");
    // scanf("%s", str1);
    // printf("split: ");
    // scanf("%d", &split);
    // BigInteger* p_biginteger1 = biginteger_from_string(str1, strlen(str1));
    // biginteger_print_debug(p_biginteger1);
    // biginteger_print(p_biginteger1);
    // BigInteger* p_biginteger2 = malloc(sizeof(BigInteger));
    // BigInteger* p_biginteger3 = malloc(sizeof(BigInteger));
    // biginteger_split(p_biginteger1, split, p_biginteger2, p_biginteger3);
    // printf("p_biginteger2->array_size %lu\n", p_biginteger2->array_size);
    // printf("p_biginteger3->array_size %lu\n", p_biginteger3->array_size);
    // biginteger_print_debug(p_biginteger2);
    // biginteger_print_debug(p_biginteger3);
    // biginteger_print(p_biginteger2);
    // biginteger_print(p_biginteger3);
    // biginteger_delete(p_biginteger1);
    // biginteger_delete(p_biginteger2);
    // biginteger_delete(p_biginteger3);

    // char str1[10000];
    // char str2[10000];
    // printf("integer1: ");
    // scanf("%s", str1);
    // printf("integer2: ");
    // scanf("%s", str2);
    // BigInteger* p_biginteger1 = biginteger_from_string(str1, strlen(str1));
    // BigInteger* p_biginteger2 = biginteger_from_string(str2, strlen(str2));
    // biginteger_print_debug(p_biginteger1);
    // biginteger_print(p_biginteger1);
    // biginteger_print_debug(p_biginteger2);
    // biginteger_print(p_biginteger2);
    // BigInteger* p_biginteger3 = biginteger_mul(p_biginteger1, p_biginteger2);
    // BigInteger* p_biginteger4 = karatsuba(p_biginteger1, p_biginteger2);
    // biginteger_print_debug(p_biginteger3);
    // biginteger_print(p_biginteger3);
    // biginteger_print_debug(p_biginteger4);
    // biginteger_print(p_biginteger4);
    // biginteger_delete(p_biginteger1);
    // biginteger_delete(p_biginteger2);
    // biginteger_delete(p_biginteger3);
    // biginteger_delete(p_biginteger4);

    // char str1[10000];
    // char str2[10000];
    // while(scanf("%s %s", str1, str2) == 2){
    //     BigInteger* p_biginteger1 = biginteger_from_string(str1, strlen(str1));
    //     BigInteger* p_biginteger2 = biginteger_from_string(str2, strlen(str2));
    //     BigInteger* p_biginteger3 = karatsuba(p_biginteger1, p_biginteger2);
    //     biginteger_print(p_biginteger3);
    //     biginteger_delete(p_biginteger1);
    //     biginteger_delete(p_biginteger2);
    //     biginteger_delete(p_biginteger3);
    // }
    return 0;
}