#include <stdio.h>
#include <stdlib.h>
#include "tsc_x86.h"
#include "big_integer.h"

#define CYCLES_REQUIRED 1e8
#define MAX_FUNC 100
#define REP_COUNT 1

typedef BigInteger(*integer_add_f)(const BigInteger, const BigInteger);

integer_add_f* func;
char** func_name;
int func_cnt;

double rdtsc(integer_add_f f, const BigInteger left, const BigInteger right){
    int i, num_runs = 1;
    myInt64 cycles;
    myInt64 start;

    while(num_runs < (1 << 14)){
        start = start_tsc();
        for (i = 0; i < num_runs; ++i) {
            f(left, right);
        }
        cycles = stop_tsc(start);
        if(cycles >= CYCLES_REQUIRED) break;
        num_runs *= 2;
    }

    start = start_tsc();
    for (i = 0; i < num_runs; ++i) {
        f(left, right);
    }
    cycles = stop_tsc(start);
    return (double)cycles / num_runs;
}

void register_function(integer_add_f f, char* name){
    func[func_cnt] = f;
    func_name[func_cnt] = name;
    func_cnt++;
}

int main(){
    // init
    func = malloc(MAX_FUNC * sizeof(integer_add_f));
    func_name = malloc(MAX_FUNC * sizeof(char*));
    func_cnt = 0;

    // register functions
    register_function(big_integer_add, "func1");
    register_function(big_integer_add, "func2");
    register_function(big_integer_add, "func3");

    // testing
    for(int i=0;i<func_cnt;i++){
        // setup data
        BigInteger left;
        BigInteger right;
        left = big_integer_create(32423534632457656L);
        right = big_integer_create(34324556445654634L);
        double cycles = 0;
        for(int j=0;j<REP_COUNT;j++){
            cycles += rdtsc(func[i], left, right);
        }
        printf("%s: %.2lf cycles\n", func_name[i], cycles / REP_COUNT);
    }
    return 0;
}