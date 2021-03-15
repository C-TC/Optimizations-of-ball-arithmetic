#include "base10.h"

int main(){
    char str1[10000];
    char str2[10000];
    while(scanf("%s %s", str1, str2) == 2){
        BigInteger* p_biginteger1 = biginteger_from_string(str1, strlen(str1));
        BigInteger* p_biginteger2 = biginteger_from_string(str2, strlen(str2));
        BigFloat *r3 = biginteger_div(p_biginteger1, p_biginteger2);
        r3 = malloc(sizeof(BigFloat));
        r3->is_zero = true;
        bigfloat_print(r3);
        biginteger_delete(p_biginteger1);
        biginteger_delete(p_biginteger2);
        bigfloat_delete(r3);
    }
    return 0;
}
