#include <assert.h>
#include <stdlib.h>
#include "big_integer.h"


int main(){
    int num_op;
    FILE *in;
    BigInteger *ops;
    BigInteger res;
    char filename[100];
    
    num_op = 16;
    ops = malloc(sizeof(BigInteger) * num_op);
    sprintf(filename, "../data/hard_add_32bit_%d.txt", num_op);
    in = fopen(filename, "r");
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
        big_integer_sum_16(
            ops[0], ops[1], ops[2], ops[3],
            ops[4], ops[5], ops[6], ops[7],
            ops[8], ops[9], ops[10], ops[11],
            ops[12], ops[13], ops[14], ops[15], 
            ops[0].data.size, &res_
        );

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
    free(ops);
    
    num_op = 12;
    ops = malloc(sizeof(BigInteger) * num_op);
    sprintf(filename, "../data/hard_add_32bit_%d.txt", num_op);
    in = fopen(filename, "r");
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
        big_integer_sum_12(
            ops[0], ops[1], ops[2], ops[3],
            ops[4], ops[5], ops[6], ops[7],
            ops[8], ops[9], ops[10], ops[11],
            ops[0].data.size, &res_
        );

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
    free(ops);
    
    num_op = 8;
    ops = malloc(sizeof(BigInteger) * num_op);
    sprintf(filename, "../data/hard_add_32bit_%d.txt", num_op);
    in = fopen(filename, "r");
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
        big_integer_sum_8(
            ops[0], ops[1], ops[2], ops[3],
            ops[4], ops[5], ops[6], ops[7],
            ops[0].data.size, &res_
        );

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
    free(ops);
    
    num_op = 4;
    ops = malloc(sizeof(BigInteger) * num_op);
    sprintf(filename, "../data/hard_add_32bit_%d.txt", num_op);
    in = fopen(filename, "r");
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
        big_integer_sum_4(
            ops[0], ops[1], ops[2], ops[3],
            ops[0].data.size, &res_
        );

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
    free(ops);
    
    num_op = 2;
    ops = malloc(sizeof(BigInteger) * num_op);
    sprintf(filename, "../data/hard_add_32bit_%d.txt", num_op);
    in = fopen(filename, "r");
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
        big_integer_sum_2(
            ops[0], ops[1],
            ops[0].data.size, &res_
        );

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
    free(ops);

    num_op = 8;
    ops = malloc(sizeof(BigInteger) * num_op);
    sprintf(filename, "../data/hard_add_32bit_%d.txt", num_op);
    in = fopen(filename, "r");
    printf("*******************************\n");
    printf("* big_integer_sum_8_unfold_1x *\n");
    printf("*      test: num_op = %2d      *\n", num_op);
    printf("*******************************\n");
    for (int i = 0; i < 15; ++i) {
        for(int j=0;j<num_op;j++){
            ops[j] = big_integer_create_from_file(&in);
        }
        res = big_integer_create_from_file(&in);

        BigInteger res_;
        res_.data.capacity = ops[0].data.size + 1;
        res_.data.bits = calloc(res.data.capacity, 8);
        big_integer_sum_8_unfold_1x(
            ops[0], ops[1], ops[2], ops[3],
            ops[4], ops[5], ops[6], ops[7],
            ops[0].data.size, &res_
        );

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
    free(ops);

    num_op = 8;
    ops = malloc(sizeof(BigInteger) * num_op);
    sprintf(filename, "../data/hard_add_32bit_%d.txt", num_op);
    in = fopen(filename, "r");
    printf("*******************************\n");
    printf("* big_integer_sum_8_unfold_2x *\n");
    printf("*      test: num_op = %2d      *\n", num_op);
    printf("*******************************\n");
    for (int i = 0; i < 15; ++i) {
        for(int j=0;j<num_op;j++){
            ops[j] = big_integer_create_from_file(&in);
        }
        res = big_integer_create_from_file(&in);

        BigInteger res_;
        res_.data.capacity = ops[0].data.size + 1;
        res_.data.bits = calloc(res.data.capacity, 8);
        big_integer_sum_8_unfold_2x(
            ops[0], ops[1], ops[2], ops[3],
            ops[4], ops[5], ops[6], ops[7],
            ops[0].data.size, &res_
        );

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
    free(ops);
    return 0;
}