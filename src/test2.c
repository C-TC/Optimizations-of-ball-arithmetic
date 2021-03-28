#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include "bigint.h"
#include "bigint_helper.h"
#include "bigfloat_helper.h"
#include "bigfloat.h"

/* Only tested small mantissa, because big mantissa is tested in biginteger. 
Here we focus on bigfloat add and mul.
*/

void test_print(){
    BigFloat *a=bigfloat_from_int_decimal(123,2);
    bigfloat_print(a);
    bigfloat_delete(a);
}

void test_add(){
    BigFloat *a,*b,*res,*correct;

    a=bigfloat_from_int_decimal(0,0);
    b=bigfloat_from_int_decimal(0,0);
    res=bigfloat_add(a,b,NULL);
    correct=bigfloat_from_int_decimal(0,0);
    assert(bigfloat_abs_comp(res,correct)==0);
    bigfloat_delete(a);
    bigfloat_delete(b);
    bigfloat_delete(res);
    bigfloat_delete(correct);

    a=bigfloat_from_int_decimal(1,0);
    b=bigfloat_from_int_decimal(2,0);
    res=bigfloat_add(a,b,NULL);
    correct=bigfloat_from_int_decimal(3,0);
    assert(bigfloat_abs_comp(res,correct)==0);
    bigfloat_delete(a);
    bigfloat_delete(b);
    bigfloat_delete(res);
    bigfloat_delete(correct);

    a=bigfloat_from_int_decimal(1,0);
    b=bigfloat_from_int_decimal(9,0);
    res=bigfloat_add(a,b,NULL);
    correct=bigfloat_from_int_decimal(10,1);
    assert(bigfloat_abs_comp(res,correct)==0);
    bigfloat_delete(a);
    bigfloat_delete(b);
    bigfloat_delete(res);
    bigfloat_delete(correct);

    a=bigfloat_from_int_decimal(0,0);
    b=bigfloat_from_int_decimal(9,0);
    res=bigfloat_add(a,b,NULL);
    correct=bigfloat_from_int_decimal(9,0);
    assert(bigfloat_abs_comp(res,correct)==0);
    bigfloat_delete(a);
    bigfloat_delete(b);
    bigfloat_delete(res);
    bigfloat_delete(correct);

    a=bigfloat_from_int_decimal(-1,1);
    b=bigfloat_from_int_decimal(9,1);
    res=bigfloat_add(a,b,NULL);
    correct=bigfloat_from_int_decimal(8,1);
    assert(bigfloat_abs_comp(res,correct)==0);
    bigfloat_delete(a);
    bigfloat_delete(b);
    bigfloat_delete(res);
    bigfloat_delete(correct);

    a=bigfloat_from_int_decimal(10,1);
    b=bigfloat_from_int_decimal(9,1);
    res=bigfloat_add(a,b,NULL);
    correct=bigfloat_from_int_decimal(100,2);
    assert(bigfloat_abs_comp(res,correct)==0);
    bigfloat_delete(a);
    bigfloat_delete(b);
    bigfloat_delete(res);
    bigfloat_delete(correct);

    a=bigfloat_from_int_decimal(10,2);
    b=bigfloat_from_int_decimal(9,1);
    res=bigfloat_add(a,b,NULL);
    correct=bigfloat_from_int_decimal(19,2);
    assert(bigfloat_abs_comp(res,correct)==0);
    bigfloat_delete(a);
    bigfloat_delete(b);
    bigfloat_delete(res);
    bigfloat_delete(correct);

    a=bigfloat_from_int_decimal(100,1);
    b=bigfloat_from_int_decimal(-9,0);
    res=bigfloat_add(a,b,NULL);
    correct=bigfloat_from_int_decimal(10,0);
    assert(bigfloat_abs_comp(res,correct)==0);
    bigfloat_delete(a);
    bigfloat_delete(b);
    bigfloat_delete(res);
    bigfloat_delete(correct);

    a=bigfloat_from_int_decimal(100,3);
    b=bigfloat_from_int_decimal(-9,0);
    res=bigfloat_add(a,b,NULL);
    correct=bigfloat_from_int_decimal(991,2);
    assert(bigfloat_abs_comp(res,correct)==0);
    bigfloat_delete(a);
    bigfloat_delete(b);
    bigfloat_delete(res);
    bigfloat_delete(correct);
}

void test_multiply(){
    BigFloat *a,*b,*res,*correct;

    a=bigfloat_from_int_decimal(0,0);
    b=bigfloat_from_int_decimal(0,0);
    res=bigfloat_mul(a,b,NULL);
    correct=bigfloat_from_int_decimal(0,0);
    assert(bigfloat_abs_comp(res,correct)==0);
    bigfloat_delete(a);
    bigfloat_delete(b);
    bigfloat_delete(res);
    bigfloat_delete(correct);

    a=bigfloat_from_int_decimal(0,0);
    b=bigfloat_from_int_decimal(5,1);
    res=bigfloat_mul(a,b,NULL);
    correct=bigfloat_from_int_decimal(0,0);
    assert(bigfloat_abs_comp(res,correct)==0);
    bigfloat_delete(a);
    bigfloat_delete(b);
    bigfloat_delete(res);
    bigfloat_delete(correct);

    a=bigfloat_from_int_decimal(1,0);
    b=bigfloat_from_int_decimal(1,0);
    res=bigfloat_mul(a,b,NULL);
    correct=bigfloat_from_int_decimal(1,-1);
    assert(bigfloat_abs_comp(res,correct)==0);
    bigfloat_delete(a);
    bigfloat_delete(b);
    bigfloat_delete(res);
    bigfloat_delete(correct);

    a=bigfloat_from_int_decimal(1,0);
    b=bigfloat_from_int_decimal(-2,0);
    res=bigfloat_mul(a,b,NULL);
    correct=bigfloat_from_int_decimal(-2,-1);
    assert(bigfloat_abs_comp(res,correct)==0);
    bigfloat_delete(a);
    bigfloat_delete(b);
    bigfloat_delete(res);
    bigfloat_delete(correct);

    a=bigfloat_from_int_decimal(5,1);
    b=bigfloat_from_int_decimal(4,-1);
    res=bigfloat_mul(a,b,NULL);
    correct=bigfloat_from_int_decimal(20,0);
    assert(bigfloat_abs_comp(res,correct)==0);
    bigfloat_delete(a);
    bigfloat_delete(b);
    bigfloat_delete(res);
    bigfloat_delete(correct);
}


// int main(int argc, const char **argv)
// {

//     //test_print();
// 	test_add();
//     printf("test_add pass\n");
// 	test_multiply();
//     printf("test_multiply pass\n");

// 	printf("\nAll tests passed!\n");
// 	return EXIT_SUCCESS;
// };
