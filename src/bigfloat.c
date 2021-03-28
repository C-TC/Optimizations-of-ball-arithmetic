#include "bigfloat.h"
#include "bigfloat_helper.h"
BigFloat *bigfloat_add(BigFloat *a, BigFloat *b, BigFloat *res) {
    if (!res) res = malloc(sizeof(BigFloat));
    if(biginteger_is_zero(a->mantissa)){
        res->mantissa=biginteger_copy(b->mantissa,NULL);
        res->exponent=b->exponent;
        return res;
    }else if (biginteger_is_zero(b->mantissa)){
        res->mantissa=biginteger_copy(a->mantissa,NULL);
        res->exponent=a->exponent;
        return res;
    }
    // do something here
    // don't forget to do garbage collection outside if used malloc
    // use macro SAFE_FREE instead of free, see bigint_helper.h

    //assume a.exp>=b.exp
    if(a->exponent<b->exponent){
        BigFloat* temp=a;
        a=b;
        b=temp;
    }

    //need to align mantissa to use BigInt add
    int num_zeros_add_to_a=a->exponent-b->exponent-biginteger_num_valid_digits(a->mantissa)+biginteger_num_valid_digits(b->mantissa);
    BigInteger *a_man_aligned, *b_man_aligned;
    //maybe optimize memory alloc and dealloc here
    if(num_zeros_add_to_a>0){
        a_man_aligned=biginteger_left_shift_digits(a->mantissa,num_zeros_add_to_a);
        b_man_aligned=biginteger_copy(b->mantissa,NULL);
    }else if (num_zeros_add_to_a<0){
        a_man_aligned=biginteger_copy(a->mantissa,NULL);
        b_man_aligned=biginteger_left_shift_digits(b->mantissa,-num_zeros_add_to_a);
    }else{
        a_man_aligned=biginteger_copy(a->mantissa,NULL);
        b_man_aligned=biginteger_copy(b->mantissa,NULL);
    }
    /* printf("a aligned:%d\n",a_man_aligned->array[0]);
    printf("b aligned:%d\n",b_man_aligned->array[0]); */
    int a_man_ali_valid_bits=biginteger_num_valid_digits(a_man_aligned);
    res->mantissa=biginteger_add(a_man_aligned,b_man_aligned,NULL);

    //renormalize: calculate exponent
    res->exponent=a->exponent+biginteger_num_valid_digits(res->mantissa)-a_man_ali_valid_bits;
    
    if(biginteger_is_zero(res->mantissa)){
        res->exponent=0;
    }

    SAFE_FREE(a_man_aligned->array);
    SAFE_FREE(a_man_aligned);
    SAFE_FREE(b_man_aligned->array);
    SAFE_FREE(b_man_aligned);
    return res;
}
BigFloat *bigfloat_sub(BigFloat *a, BigFloat *b, BigFloat *res) {
    if (!res) res = malloc(sizeof(BigFloat));
    // do something here
    // don't forget to do garbage collection outside if used malloc
    // use macro SAFE_FREE instead of free, see bigint_helper.h
    SAFE_FREE(res);
    res = NULL; 
    return res;
}
BigFloat *bigfloat_mul(BigFloat *a, BigFloat *b, BigFloat *res) {
    if (!res) res = malloc(sizeof(BigFloat));
    // do something here
    // don't forget to do garbage collection outside if used malloc
    // use macro SAFE_FREE instead of free, see bigint_helper.h
    res->mantissa=biginteger_mul(a->mantissa,b->mantissa);
    res->exponent=a->exponent+b->exponent;
    //renormalize
    res->exponent-=biginteger_num_valid_digits(a->mantissa)+biginteger_num_valid_digits(b->mantissa)-biginteger_num_valid_digits(res->mantissa);

    if(biginteger_is_zero(res->mantissa)){
        res->exponent=0;
    }
    return res;
}
BigFloat *bigfloat_div(BigFloat *a, BigFloat *b, BigFloat *res) {
    if (!res) res = malloc(sizeof(BigFloat));
    // do something here
    // don't forget to do garbage collection outside if used malloc
    // use macro SAFE_FREE instead of free, see bigint_helper.h
    SAFE_FREE(res);
    res = NULL; 
    return res;
}
