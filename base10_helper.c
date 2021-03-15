#include "base10_helper.h"

int biginteger_delete(BigInteger* p_biginteger){
    SAFE_FREE(p_biginteger->array);
    SAFE_FREE(p_biginteger);
    return 0;
}

void bigfloat_delete(BigFloat *p) {
    if (p && p->base)
    biginteger_delete((BigInteger*)p->base);
    SAFE_FREE(p);
}

int biginteger_print(BigInteger* p_biginteger){
    if(!p_biginteger->is_positive && !biginteger_is_zero(p_biginteger)){
        printf("-");
    }
    for(int i=p_biginteger->array_size-1;i>=0;i--){
        if(i == p_biginteger->array_size-1)
            printf("%d", p_biginteger->array[i]);
        else{
            char format_str[10] = "%0 d";
            format_str[2] = UNIT_LEN + '0';
            printf(format_str, p_biginteger->array[i]);
        }
    }
    printf("\n");
    return 0;
}

void bigfloat_print(BigFloat *p) {
    if (!p) return;
    if (p->is_zero) {
        printf("0.");
        for (int i = 0; i < DIV_PRE; ++i) {
            printf("0");
        }
        printf("\n");
        return;
    }

}

BigInteger* biginteger_zero(){
    BigInteger* p_biginteger_zero = malloc(sizeof(BigInteger));
    p_biginteger_zero->is_positive = true;
    p_biginteger_zero->array_size = 1;
    p_biginteger_zero->array = malloc(sizeof(int));
    p_biginteger_zero->array[0] = 0;
    return p_biginteger_zero;
}

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

BigInteger* biginteger_copy(BigInteger* p_biginteger){
    BigInteger* p_biginteger_result = malloc(sizeof(BigInteger));
    p_biginteger_result->array_size = p_biginteger->array_size;
    p_biginteger_result->array = malloc(sizeof(int) * p_biginteger->array_size);
    p_biginteger_result->is_positive = p_biginteger->is_positive;
    memcpy(p_biginteger_result->array, p_biginteger->array, sizeof(int) * p_biginteger->array_size);
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
    long cur = 0;
    int idx = 0;
    long coef = 1;
    for(int i=size-1;i>=0;i--){
        cur += (integer_str[i] - '0') * coef;
        if(coef >= UNIT_CAPACITY){
            p_biginteger->array[idx++] = (int)(cur % UNIT_CAPACITY);
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

int biginteger_print_debug(BigInteger* p_biginteger){
    printf("array_size:\t%lu, array:\t", p_biginteger->array_size);
    for(int i=0;i<p_biginteger->array_size;i++){
        printf("%d\t", p_biginteger->array[i]);
    }
    printf("\n");
    return 0;
}
