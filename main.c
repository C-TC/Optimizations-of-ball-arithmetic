#include "base10.h"
// #include "base10_helper.h"

static void div_operand_gc(BigInteger *p1, BigInteger *p2, BigInteger *q, BigInteger *r) {
    biginteger_delete(p1);
    biginteger_delete(p2);
    biginteger_delete(q);
    biginteger_delete(r);
}

int main(){
    char str1[10000];
    char str2[10000];
    while(scanf("%s %s", str1, str2) == 2){
        BigInteger* p_biginteger1 = biginteger_from_string(str1, strlen(str1));
        BigInteger* p_biginteger2 = biginteger_from_string(str2, strlen(str2));
        BigInteger *q = malloc(sizeof(BigInteger));
        BigInteger *r = malloc(sizeof(BigInteger));
        int succ = biginteger_div(p_biginteger1, p_biginteger2, q, r);
        if (!succ) {
            printf("div error\n");
            div_operand_gc(p_biginteger1, p_biginteger2, q, r);
            continue;
        }
        biginteger_print(q);
        biginteger_print(r);
        div_operand_gc(p_biginteger1, p_biginteger2, q, r);
    }
    return 0;
}
