#include "big_float.h"
#include <stdio.h>
BigFloat big_float_multiply(BigFloat lo, BigFloat ro) {
    BigFloat ans;
    ans.power = lo.power + ro.power;
    ans.mantissa = big_integer_multiply(lo.mantissa, ro.mantissa);
    if (ans.mantissa.data.size != (lo.mantissa.data.size) + (ro.mantissa.data.size))  --ans.power; 
    if (0 == ans.mantissa.sign) ans.power = 0;
    return ans;
}

BigFloat big_float_div(BigFloat lo, BigFloat ro) {
    BigFloat ans;
    ans.power = 0;
    return ans;
}
