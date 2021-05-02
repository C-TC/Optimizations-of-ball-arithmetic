#include "big_float.h"
#include <stdlib.h>
#include <stdio.h>
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
        lo_man_aligned = lo.mantissa;
        ro_man_aligned = ro.mantissa;
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
        lo_man_aligned = lo.mantissa;
        ro_man_aligned = ro.mantissa;
        ans.mantissa = big_integer_subtract(lo_man_aligned,ro_man_aligned);
        ans.power =  lo.power + ans.mantissa.data.size - lo_man_aligned.data.size;
        if(ans.mantissa.sign ==0) ans.power = 0;
    }
    return ans;
}

//TODO:more robust, need to normalize the data to (0.5, 1), continuously mult 2
BigFloat big_float_div(BigFloat lo, BigFloat ro) {
    if (ro.mantissa.sign == 0) {
        printf("divide by zero error!\n");
        exit(-1);
    }
    BigFloat ans;
    if (lo.mantissa.sign == 0) {
        ans.power = 0;
        ans.mantissa = big_integer_create(0);
        return ans;
    }
    ans.power = lo.power - ro.power;
    BigFloat tmp_lo = big_float_deep_copy(lo);
    BigFloat tmp_ro = big_float_deep_copy(ro);
    tmp_ro.power = 0;
    BigFloat tmp_x, tmp_x1, tmp_x2, tmp_x3, tmp_x4;
    BigFloat tmp_c1, tmp_c2, tmp_c3;
    tmp_c1.power = tmp_c2.power = 1;
    tmp_c1.mantissa.sign = tmp_c2.mantissa.sign = 1;
    tmp_c1.mantissa.data.size = tmp_c2.mantissa.data.size = 3;
    tmp_c1.mantissa.data.capacity = tmp_c2.mantissa.data.capacity = 4;
    tmp_c1.mantissa.data.bits = (unsigned int*)calloc(4, sizeof(unsigned int));
    tmp_c2.mantissa.data.bits = (unsigned int*)calloc(4, sizeof(unsigned int));
    tmp_c1.mantissa.data.bits[0] = 3537035264u;
    tmp_c1.mantissa.data.bits[1] = 3537031890u;
    tmp_c1.mantissa.data.bits[2] = 2u;
    tmp_c2.mantissa.data.bits[0] = 3789676544u;
    tmp_c2.mantissa.data.bits[1] = 3789677025u;
    tmp_c2.mantissa.data.bits[2] = 1u;
    tmp_c3 = big_float_multiply(tmp_c2, tmp_ro);
    tmp_x = big_float_sub(tmp_c1, tmp_c3);
    free(tmp_c1.mantissa.data.bits);
    free(tmp_c2.mantissa.data.bits);
    free(tmp_c3.mantissa.data.bits);
    BigFloat tmp_one = big_float_create(big_integer_create(1), 1);
    for (int i = 0; i < DIV_STEP; ++i) {
        tmp_x1 = big_float_multiply(tmp_ro, tmp_x);
        tmp_x2 = big_float_sub(tmp_one, tmp_x1);
        tmp_x3 = big_float_multiply(tmp_x, tmp_x2);
        tmp_x4 = big_float_add(tmp_x, tmp_x3);
        free(tmp_x.mantissa.data.bits);
        tmp_x.power = tmp_x4.power;
        tmp_x.mantissa = big_integer_deepcopy(tmp_x4.mantissa);
        free(tmp_x1.mantissa.data.bits);
        free(tmp_x2.mantissa.data.bits);
        free(tmp_x3.mantissa.data.bits);
        free(tmp_x4.mantissa.data.bits);
    }
    ans.mantissa = (big_float_multiply(tmp_x, tmp_lo)).mantissa;

    int tmp_size = tmp_lo.mantissa.data.size > tmp_ro.mantissa.data.size? tmp_lo.mantissa.data.size: tmp_ro.mantissa.data.size;
    if (tmp_size != tmp_lo.mantissa.data.size) {
        BigFloat tmp_container = big_float_deep_copy(tmp_lo);
        free(tmp_lo.mantissa.data.bits);
        tmp_lo.mantissa.data.bits = (unsigned int *)calloc(tmp_size * 2, sizeof(unsigned int));
        tmp_lo.mantissa.data.size = tmp_size;
        tmp_lo.mantissa.data.capacity = tmp_size *2;
        for (int i = tmp_size; i > tmp_size - tmp_container.mantissa.data.size; --i) {
            tmp_lo.mantissa.data.bits[i - 1] = tmp_container.mantissa.data.bits[i - 1 - tmp_size + tmp_container.mantissa.data.size];
        }
        free(tmp_container.mantissa.data.bits);
    }
    if (tmp_size != tmp_ro.mantissa.data.size) {
        BigFloat tmp_container = big_float_deep_copy(tmp_ro);
        free(tmp_ro.mantissa.data.bits);
        tmp_ro.mantissa.data.bits = (unsigned int *)calloc(tmp_size * 2, sizeof(unsigned int));
        tmp_ro.mantissa.data.size = tmp_size;
        tmp_ro.mantissa.data.capacity = tmp_size *2;
        for (int i = tmp_size; i > tmp_size - tmp_container.mantissa.data.size; --i) {
            tmp_ro.mantissa.data.bits[i - 1] = tmp_container.mantissa.data.bits[i - 1 - tmp_size + tmp_container.mantissa.data.size];
        }
        free(tmp_container.mantissa.data.bits);
    }
    if (big_integer_compare_data(&tmp_lo.mantissa.data, &tmp_ro.mantissa.data) == -1) --ans.power;
    free(tmp_lo.mantissa.data.bits);
    free(tmp_ro.mantissa.data.bits);
    free(tmp_x.mantissa.data.bits);
    free(tmp_one.mantissa.data.bits);
    return ans;
}
