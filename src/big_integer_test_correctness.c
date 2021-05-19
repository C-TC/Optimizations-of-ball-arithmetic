#include <assert.h>
#include "big_integer.h"

int main(){
    BigInteger op1;
    BigInteger op2;
    BigInteger res;
    FILE *in = fopen("../data/hard_mul.txt", "r");
    for (int i = 0; i < 15; ++i) {
        op1 = big_integer_create_from_file(&in);
        op2 = big_integer_create_from_file(&in);
        res = big_integer_create_from_file(&in);
    
        // BigInteger res_ = big_integer_multiply(op1, op2);
        // big_integer_multiply_inplace_fixed_precision(&op1, op2, op1.data.size);
        // big_integer_multiply_inplace_fixed_precision_unflod(&op1, op2, op1.data.size);
        // big_integer_multiply_inplace_fixed_precision_verter_1x_unfold(&op1, op2, op1.data.size);
        // big_integer_multiply_inplace_fixed_precision_verter_2x_unfold(&op1, op2, op1.data.size);
        // big_integer_multiply_inplace_fixed_precision_verter_4x_unfold(&op1, op2, op1.data.size);
        // big_integer_multiply_inplace_fixed_precision_verter_8x_unfold(&op1, op2, op1.data.size);
        // big_integer_multiply_inplace_fixed_precision_verter_12x_unfold(&op1, op2, op1.data.size);
        big_integer_multiply_inplace_fixed_precision_verter_16x_unfold(&op1, op2, op1.data.size);

        for(int j=0;j<res.data.size;j++){
            assert(op1.data.bits[j] == res.data.bits[j+op1.data.size]);
        }

        printf("input size %d test passed\n", op1.data.size);
        big_integer_destroy(&op1);
        big_integer_destroy(&op2);
        big_integer_destroy(&res);
    }
    return 0;
}