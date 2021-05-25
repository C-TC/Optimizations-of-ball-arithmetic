#include <assert.h>
#include <stdlib.h>
#include "big_integer.h"

void test_sum_correctness(int num_op){
    BigInteger *ops = malloc(sizeof(BigInteger) * num_op);
    BigInteger res;
    char filename[100];
    sprintf(filename, "../data/hard_add_32bit_%d.txt", num_op);
    FILE *in = fopen(filename, "r");
    printf("***********************\n");
    printf("*  test: num_op = %2d  *\n", num_op);
    printf("***********************\n");
    for (int i = 0; i < 15; ++i) {
        for(int j=0;j<num_op;j++){
            ops[j] = big_integer_create_from_file(&in);
        }
        res = big_integer_create_from_file(&in);

        BigInteger res_;
        res_.data.capacity = ops[0].data.size + 1;
        res_.data.bits = calloc(res.data.capacity, 8);
        big_integer_sum(ops, 0, num_op, ops[0].data.size, &res_);

        int offset = res.data.size == ops[0].data.size ? 0 : 1;
        for(int j=0;j<res_.data.size;j++){
            assert(res_.data.bits[j] == res.data.bits[j+offset]);
        }

        printf("input size %d test passed\n", ops[0].data.size);
        for(int j=0;j<num_op;j++){
            big_integer_destroy(ops + j);
        }
        big_integer_destroy(&res);
        big_integer_destroy(&res_);
    }
    fclose(in);
}

int main(){
    int num_op[] = {2, 4, 8, 12, 16};
    for(int i=0;i<5;i++){
        test_sum_correctness(num_op[i]);
    }
    return 0;
}