#include "big_float.h"
#include <stdlib.h>
#include "big_float_helper.h"
BigFloat big_float_multiply(BigFloat lo, BigFloat ro) {
    BigFloat ans;
    ans.power = lo.power + ro.power;
    ans.mantissa = big_integer_multiply(lo.mantissa, ro.mantissa);
    if (ans.mantissa.data.size != (lo.mantissa.data.size) + (ro.mantissa.data.size))  --ans.power; 
    if (0 == ans.mantissa.sign) ans.power = 0;
    return ans;
}
BigFloat big_float_add(BigFloat lo, BigFloat ro) {
    if (lo.mantissa.sign == 0) {
        BigFloat ans = big_float_deep_copy(ro);
        return ans;
    }
    if (ro.mantissa.sign == 0) {
        BigFloat ans = big_float_deep_copy(lo);
        return ans;
    }
    BigFloat ans;
    //assume lo.power > ro.power
    if (lo.power < ro.power) {
        BigFloat temp = lo;
        lo = ro;
        ro = temp;
    }
    //need to align mantissa to use BigInt add
    int num_zeros_add_to_lo = lo.power - ro.power - lo.mantissa.data.size + ro.mantissa.data.size;
    BigInteger lo_man_aligned, ro_man_aligned;
    if (num_zeros_add_to_lo > 0) {
        lo_man_aligned = big_integer_add_trailing_zeros(lo.mantissa, num_zeros_add_to_lo);
        ro_man_aligned = ro.mantissa;
        ans.mantissa = big_integer_add(lo_man_aligned,ro_man_aligned);
        ans.power =  lo.power + ans.mantissa.data.size - lo_man_aligned.data.size;
        if(ans.mantissa.sign ==0) ans.power = 0;
        free(lo_man_aligned.data.bits);
    } else if (num_zeros_add_to_lo < 0) {
        lo_man_aligned = lo.mantissa;
        ro_man_aligned = big_integer_add_trailing_zeros(ro.mantissa, -num_zeros_add_to_lo);
        ans.mantissa = big_integer_add(lo_man_aligned,ro_man_aligned);
        ans.power =  lo.power + ans.mantissa.data.size - lo_man_aligned.data.size;
        if(ans.mantissa.sign ==0) ans.power = 0;
        free(ro_man_aligned.data.bits);
    } else {
        ans.mantissa = big_integer_add(lo_man_aligned,ro_man_aligned);
        ans.power =  lo.power + ans.mantissa.data.size - lo_man_aligned.data.size;
        if(ans.mantissa.sign ==0) ans.power = 0;
    }
    return ans;
}

BigFloat big_float_sub(BigFloat lo, BigFloat ro) {
    if (lo.mantissa.sign == 0) {
        BigFloat ans = big_float_deep_copy(ro);
        ans.mantissa.sign = -ans.mantissa.sign;
        return ans;
    }
    if (ro.mantissa.sign == 0) {
        BigFloat ans = big_float_deep_copy(lo);
        return ans;
    }
    BigFloat ans;
    //assume lo.power > ro.power
    if (lo.power < ro.power) {
        BigFloat temp = lo;
        lo = ro;
        ro = temp;
    }
    //need to align mantissa to use BigInt add
    int num_zeros_add_to_lo = lo.power - ro.power - lo.mantissa.data.size + ro.mantissa.data.size;
    BigInteger lo_man_aligned, ro_man_aligned;
    if (num_zeros_add_to_lo > 0) {
        lo_man_aligned = big_integer_add_trailing_zeros(lo.mantissa, num_zeros_add_to_lo);
        ro_man_aligned = ro.mantissa;
        ans.mantissa = big_integer_subtract(lo_man_aligned,ro_man_aligned);
        ans.power =  lo.power + ans.mantissa.data.size - lo_man_aligned.data.size;
        if(ans.mantissa.sign ==0) ans.power = 0;
        free(lo_man_aligned.data.bits);
    } else if (num_zeros_add_to_lo < 0) {
        lo_man_aligned = lo.mantissa;
        ro_man_aligned = big_integer_add_trailing_zeros(ro.mantissa, -num_zeros_add_to_lo);
        ans.mantissa = big_integer_subtract(lo_man_aligned,ro_man_aligned);
        ans.power =  lo.power + ans.mantissa.data.size - lo_man_aligned.data.size;
        if(ans.mantissa.sign ==0) ans.power = 0;
        free(ro_man_aligned.data.bits);
    } else {
        ans.mantissa = big_integer_subtract(lo_man_aligned,ro_man_aligned);
        ans.power =  lo.power + ans.mantissa.data.size - lo_man_aligned.data.size;
        if(ans.mantissa.sign ==0) ans.power = 0;
    }
    return ans;
}


BigFloat big_float_div(BigFloat lo, BigFloat ro) {
    if (ro.mantissa.sign == 0) {
        exit(-1);
    }
    BigFloat ans;
    if (lo.mantissa.sign == 0) {
        ans.power = 0;
        ans.mantissa = big_integer_create(0);
        return ans;
    }
    ans.power = lo.power - ro.power;
    ro.power = -1;
    BigFloat x, x1, x2, x3, x4;
    // x initialization, e.g., how to represent 2.12345678, create function with array
    BigFloat fone = big_float_create(big_integer_create(1), 0);
    for (int i = 0; i < DIV_STEP; ++i) {
        x1 = big_float_multiply(ro, x);
        x2 = big_float_sub(fone, x1);
        x3 = big_float_multiply(x, x2);
        x4 = big_float_add(x, x3);
        big_float_destroy(&x);
        x = x4;
        x.mantissa = big_integer_deepcopy(x4.mantissa);
    }
    big_float_destroy(&x1);
    big_float_destroy(&x2);
    big_float_destroy(&x3);
    big_float_destroy(&x4);
    big_float_destroy(&fone);
    ans.mantissa = (big_float_multiply(x, lo)).mantissa;
    while(*lo.mantissa.data.bits == 0) {
        --lo.mantissa.data.size;
        ++lo.mantissa.data.bits;
    }
    while(*ro.mantissa.data.bits == 0) {
        --ro.mantissa.data.size;
        ++ro.mantissa.data.bits;
    };
    if (big_integer_compare_data(&lo.mantissa.data, &ro.mantissa.data) == -1) --ans.power;
    big_float_destroy(&x);
    return ans;
}
