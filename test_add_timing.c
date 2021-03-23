#include "base10.h"
#include "tsc_x86.h"

int main(){
    char str1[1000000];
    char str2[1000000];
    int num_runs = 1000;
    double cycles;
    myInt64 start;
    for(int i=5;i<20;i++){
        scanf("%s %s", str1, str2);
        BigInteger* p_biginteger1 = biginteger_from_string(str1, strlen(str1));
        BigInteger* p_biginteger2 = biginteger_from_string(str2, strlen(str2));
        BigInteger* p_biginteger3;
        start = start_tsc();
        for(int j=0;j<num_runs;j++){
            p_biginteger3 = biginteger_add(p_biginteger1, p_biginteger2, NULL);
            biginteger_delete(p_biginteger3);
        }
        cycles = (double)(stop_tsc(start)) / num_runs;
        printf("%d %lf\n", i, cycles);
        biginteger_delete(p_biginteger1);
        biginteger_delete(p_biginteger2);
    }
    return 0;
}
