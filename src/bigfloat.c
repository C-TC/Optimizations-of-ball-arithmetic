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
BigFloat *bigfloat_div(BigFloat *lo, BigFloat *rotemp, BigFloat *res) {
    if (!res) res = malloc(sizeof(BigFloat));
    BigFloat *ro = bigfloat_copy(rotemp);
    if (biginteger_is_zero(ro->mantissa)) {
        exit(-1);
    }
    if (biginteger_is_zero(lo->mantissa)) {
        res->exponent = 0;
        res->mantissa = biginteger_zero();
        return res;
    }
    //x initialization
    BigFloat *x = malloc(sizeof(BigFloat));
    x->exponent = -1;
    ro->exponent = -1;
    BigFloat *x1, *x2, *x3, *x4;
    BigFloat *fone = malloc(sizeof(BigFloat));
    fone->exponent = 0;
    fone->mantissa = biginteger_from_number(1);
    res->exponent = lo->exponent - ro->exponent;
    for (int i = 0; i < DIV_STEP; ++i) {
        x1 = bigfloat_mul(ro, x, NULL);
        x1->mantissa->is_positive = -x1->mantissa->is_positive;
        x2 = bigfloat_add(fone, x1, NULL);
        x3 = bigfloat_mul(x, x2, NULL);
        x4 = bigfloat_add(x, x3, NULL);
        bigfloat_delete(x);
        x = bigfloat_copy(x4);
    }
    bigfloat_delete(x1);
    bigfloat_delete(x2);
    bigfloat_delete(x3);
    bigfloat_delete(x4);
    bigfloat_delete(fone);
    res->mantissa = (bigfloat_mul(x, lo, NULL))->mantissa;
    size_t rel_lo_size = lo->mantissa->array_size;
    size_t rel_ro_size = ro->mantissa->array_size;
    while(lo->mantissa->array[lo->mantissa->array_size - (rel_lo_size--)] == 0) {}
    while(ro->mantissa->array[ro->mantissa->array_size - (rel_ro_size--)] == 0) {}
    BigFloat *temp1 = malloc(sizeof(BigFloat));
    biginteger_split(lo->mantissa, lo->mantissa->array_size - rel_lo_size, temp1->mantissa, NULL);
    BigFloat *temp2 = malloc(sizeof(BigFloat));
    biginteger_split(ro->mantissa, ro->mantissa->array_size - rel_ro_size, temp2->mantissa, NULL);
    if (biginteger_abs_comp(lo->mantissa, ro->mantissa) == -1) --res->exponent;
    bigfloat_delete(x);
    bigfloat_delete(ro);
    bigfloat_delete(temp1);
    bigfloat_delete(temp2);
    return res;
}
