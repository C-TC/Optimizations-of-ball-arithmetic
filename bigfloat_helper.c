#include "bigfloat_helper.h"
#include "bigint_helper.h"
#include "bigint.h"


// the output bigfloat = 0.(decimal) x 10^expo
BigFloat* bigfloat_from_int_decimal(int decimal,long long expo){
    BigInteger* bi=biginteger_from_number(decimal);
    return bigfloat_from_biginteger(bi,expo);
}

// the output bigfloat = 0.(decimal) x 10^expo
BigFloat* bigfloat_from_biginteger(BigInteger* decimal,long long expo){
    BigFloat *res=(BigFloat*)malloc(sizeof(BigFloat));
    res->exponent=expo;
    res->mantissa=decimal;
    return res;
}


void bigfloat_print(BigFloat* p){
    printf("decimal part: ");
    biginteger_print(p->mantissa);
    printf("exponent: %lld\n",p->exponent);
}

void bigfloat_delete(BigFloat* p){
    biginteger_delete(p->mantissa);
    SAFE_FREE(p);
}


int bigfloat_abs_comp(BigFloat* p1,BigFloat* p2){
    //assume normalized
    if(p1->exponent>p2->exponent){
        return 1;
    }else if(p1->exponent<p2->exponent){
        return -1;
    }
    int comp=biginteger_abs_comp(p1->mantissa,p2->mantissa);
    return comp;
}

BigFloat* bigfloat_copy(BigFloat* p){
    if(p==NULL||p->mantissa==NULL||p->mantissa->array==NULL)
    return NULL;
    BigFloat *res=malloc(sizeof(BigFloat));
    res->mantissa=biginteger_copy(p->mantissa,NULL);
    res->exponent=p->exponent;
    return res;
}
