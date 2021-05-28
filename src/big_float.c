#include "big_float.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include "big_float_helper.h"
#include <assert.h>
BigFloat big_float_multiply(BigFloat lo, BigFloat ro) {
    BigFloat ans;
    ans.power = lo.power + ro.power;
    ans.mantissa = big_integer_multiply(lo.mantissa, ro.mantissa);
    if (ans.mantissa.data.size != (lo.mantissa.data.size) + (ro.mantissa.data.size))  --ans.power; 
    if (0 == ans.mantissa.sign) ans.power = 0;
    return ans;
}

BigFloat big_float_mul_fixed_precision(BigFloat lo, BigFloat ro, const int precision) {
    assert(lo.mantissa.data.size >= precision && ro.mantissa.data.size >= precision);
    BigFloat ans;
    ans.power = lo.power + ro.power;
    int powerdiff = 0;
    ans.mantissa = big_integer_multiply_fixed_precision(lo.mantissa, ro.mantissa, precision, &powerdiff);
    ans.power += powerdiff;
    if (0 == ans.mantissa.sign) ans.power = 0;
    return ans;
}

void big_float_div_by_power_of_two_inplace_fixed_precision(BigFloat *bf, int power, const int precision){
    assert(power >= 0);
    assert(bf->mantissa.data.size >= precision);
    bf->power -= power / 32 ;
    power %= 32;
    if (power == 0) return;
    unsigned long threshold = 1ul << power;
    if (bf->mantissa.data.bits[bf->mantissa.data.size - 1] < threshold) bf->power--;
    big_integer_div_by_power_of_two_inplace_fixed_precision(&bf->mantissa, power, precision);

}

void big_float_mul_toplace_fixed_precision(BigFloat lo, BigFloat ro, BigFloat *res, const int precision) {
    assert(lo.mantissa.data.size >= precision && ro.mantissa.data.size >= precision);
    res->power = lo.power + ro.power;
    int powerdiff = 0;
    big_integer_multiply_toplace_fixed_precision(lo.mantissa, ro.mantissa, &res->mantissa, precision, &powerdiff);
    res->power += powerdiff;
    if (0 == res->mantissa.sign) res->power = 0;
}

void big_float_mul_inplace_fixed_precision(BigFloat *lo, BigFloat ro, const int precision) {
    lo->power = lo->power + ro.power;
    int powerdiff = 0;
    big_integer_multiply_inplace_fixed_precision(&lo->mantissa, ro.mantissa, precision, &powerdiff);
    lo->power += powerdiff;
    if (0 == lo->mantissa.sign) lo->power = 0;
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

void big_float_add_inplace_fixed_precision(BigFloat *lo, BigFloat ro, const int precision) {
    int carried = 0;
    long long res_power = lo->power > ro.power? lo->power: ro.power;
    //need to align mantissa to use BigInt add
    int num_zeros_add_to_lo = lo->power - ro.power - lo->mantissa.data.size + ro.mantissa.data.size;
    BigInteger *lo_man_aligned, ro_man_aligned;
    if (num_zeros_add_to_lo > 0) {
        big_integer_add_trailing_zeros_inplace(&lo->mantissa, num_zeros_add_to_lo);
        lo_man_aligned = &lo->mantissa;
        ro_man_aligned = ro.mantissa;
        big_integer_add_inplace_fixed_precision(lo_man_aligned, ro_man_aligned, precision, &carried);
        lo->power = res_power + carried;
        if(lo->mantissa.sign ==0) lo->power = 0;
    } else if (num_zeros_add_to_lo < 0) {
        lo_man_aligned = &lo->mantissa;
        ro_man_aligned = big_integer_add_trailing_zeros(ro.mantissa, -num_zeros_add_to_lo);
        big_integer_add_inplace_fixed_precision(lo_man_aligned, ro_man_aligned, precision, &carried);
        lo->power = res_power + carried;
        if(lo->mantissa.sign ==0) lo->power = 0;
        free(ro_man_aligned.data.bits);
    } else {
        lo_man_aligned = &lo->mantissa;
        ro_man_aligned = ro.mantissa;
        big_integer_add_inplace_fixed_precision(lo_man_aligned, ro_man_aligned, precision, &carried);
        lo->power = res_power + carried;
        if(lo->mantissa.sign ==0) lo->power = 0;
    }
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

void big_float_sub_inplace_fixed_precision(BigFloat *lo, BigFloat ro, const int precision) {
    ro.mantissa.sign *= -1;
    big_float_add_inplace_fixed_precision(lo, ro, precision);
}

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
    BigFloat tmp_lo = big_float_deep_copy(lo);
    BigFloat tmp_ro = big_float_deep_copy(ro);
    BigFloat tmp_normalize;
    tmp_ro.power = 1;
    tmp_lo.power = 1;
    tmp_ro.mantissa.sign = 1;
    tmp_lo.mantissa.sign = 1;
    BigFloat tmp_point5;
    tmp_point5.mantissa = big_integer_create(2147483648u);
    tmp_point5.power = 0;
    unsigned int normalize_factor = (unsigned int)(log(ro.mantissa.data.bits[ro.mantissa.data.size - 1]) / log(2)) + 1;
    for (int i = 0; i < normalize_factor; ++i) {
        tmp_normalize = big_float_multiply(tmp_ro, tmp_point5);
        tmp_normalize.mantissa.data.capacity = tmp_normalize.mantissa.data.size;
        free(tmp_ro.mantissa.data.bits);
        tmp_ro = big_float_deep_copy(tmp_normalize);
        free(tmp_normalize.mantissa.data.bits);
    }
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
        tmp_x1.mantissa.data.capacity = tmp_x1.mantissa.data.size;
        tmp_x2 = big_float_sub(tmp_one, tmp_x1);
        tmp_x3 = big_float_multiply(tmp_x, tmp_x2);
        tmp_x3.mantissa.data.capacity = tmp_x3.mantissa.data.size;
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

    for (int i = 0; i < normalize_factor; ++i) {
        tmp_normalize = big_float_multiply(ans, tmp_point5);
        tmp_normalize.mantissa.data.capacity = tmp_normalize.mantissa.data.size;
        free(ans.mantissa.data.bits);
        ans.mantissa = big_float_deep_copy(tmp_normalize).mantissa;
        free(tmp_normalize.mantissa.data.bits);
    }

    int num_zeros_add_to_lo = lo.mantissa.data.size - ro.mantissa.data.size;
    BigInteger lo_man_aligned, ro_man_aligned;
    if (num_zeros_add_to_lo < 0) {
        lo_man_aligned = big_integer_add_trailing_zeros(lo.mantissa, -num_zeros_add_to_lo);
        ro_man_aligned = ro.mantissa;
    } else if (num_zeros_add_to_lo > 0) {
        lo_man_aligned = lo.mantissa;
        ro_man_aligned = big_integer_add_trailing_zeros(ro.mantissa, num_zeros_add_to_lo);
    } else {
        lo_man_aligned = lo.mantissa;
        ro_man_aligned = ro.mantissa;
    }
    ans.power = lo.power - ro.power + 1;
    if (big_integer_compare_data(&(lo_man_aligned.data), &(ro_man_aligned.data)) != 1) {--ans.power;}
    if (num_zeros_add_to_lo < 0) 
        {free(lo_man_aligned.data.bits);} 
    else if (num_zeros_add_to_lo > 0)
        {free(ro_man_aligned.data.bits);}
    free(tmp_lo.mantissa.data.bits);
    free(tmp_ro.mantissa.data.bits);
    free(tmp_x.mantissa.data.bits);
    free(tmp_one.mantissa.data.bits);
    free(tmp_point5.mantissa.data.bits);
    ans.mantissa.sign = lo.mantissa.sign * ro.mantissa.sign;
    return ans;
}

BigFloat big_float_sqrt_fix_precision(BigFloat lo, const int precision, const unsigned int x_0) {

}

BigFloat big_float_reciprocal_sqrt_fix_precision(BigFloat lo, const int precision, double x_0) {
    // output precision = working precision / 2
    int working_prec;
    assert(precision > 1);
    if (lo.mantissa.data.size >= 2 * precision) {working_prec = 2 * precision;}
    else {working_prec = lo.mantissa.data.size;}
    int output_precision = working_prec / 2;
    //TODO: How to terminate?
    int num_Iter = 10 * log(precision);
    int min_iter = log(precision);
    //unsigned long prev_last_bit = lo.mantissa.data.bits[working_prec / 2];
    // initial value x_0
    BigFloat x = double_to_big_float_fixed_precision(x_0, working_prec);
    //big_float_print_msg(x,"x_0");
    //printf("%e\n",big_float_to_double(x));
    // w = x_i ^ 2
    BigFloat w = big_float_mul_fixed_precision(x, x, working_prec);
    BigFloat one = big_float_create_from_uint_fixed_precision(1, precision);
    // w * lo
    big_float_mul_inplace_fixed_precision(&w, lo, working_prec);
    // d = w * lo - 1
    big_float_sub_inplace_fixed_precision(&w, one, working_prec);
    // d * x_i
    big_float_mul_inplace_fixed_precision(&w, x, working_prec);
    // d * x_i / 2
    big_float_div_by_power_of_two_inplace_fixed_precision(&w, 1, working_prec);
    // x_i+1 = x_i - d * x_i / 2
    big_float_sub_inplace_fixed_precision(&x, w, working_prec);
    //big_float_print_msg(x,"x_i+1 = x_i - d * x_i / 2");
    //printf("double: %e\n",big_float_to_double(x));
    unsigned int prev_value = x.mantissa.data.bits[x.mantissa.data.size - output_precision];
    // main loop
    for (int i = 0; i < num_Iter; i++) {
        big_float_mul_toplace_fixed_precision(x, x,&w, working_prec); 
        //big_float_print_msg(w,"w = x_i ^ 2");
        //printf("double: %e\n",big_float_to_double(w));
        big_float_mul_inplace_fixed_precision(&w, lo, working_prec);
        //big_float_print_msg(w,"w * lo");
        //printf("double: %e\n",big_float_to_double(w));
        big_float_sub_inplace_fixed_precision(&w, one, working_prec);
        //big_float_print_msg(w,"d = w * lo - 1");
        //printf("double: %e\n",big_float_to_double(w));
        big_float_mul_inplace_fixed_precision(&w, x, working_prec);
        //big_float_print_msg(w,"d * x_i");
        //printf("double: %e\n",big_float_to_double(w));
        big_float_div_by_power_of_two_inplace_fixed_precision(&w, 1, working_prec);
        //big_float_print_msg(w,"d * x_i / 2");
        //printf("double: %e\n",big_float_to_double(w));
        big_float_sub_inplace_fixed_precision(&x, w, working_prec);
        //big_float_print_msg(x,"x_i+1 = x_i - d * x_i / 2");
        //printf("double: %e\n",big_float_to_double(x));
        //printf("deciding value: %lu\n",prev_value);
        if (x.mantissa.data.bits[x.mantissa.data.size - output_precision] == prev_value && i > min_iter){
            printf("reciprocal sqrt took %d iterations to converge.\n",i+2);
            break;
        }
        else prev_value = x.mantissa.data.bits[x.mantissa.data.size - output_precision];
    }
    big_float_destroy(&w);
    big_float_destroy(&one);
    return x;
}


BigFloat big_float_guided_reciprocal_sqrt_fix_precision(BigFloat lo, const int precision) {
    // output precision = working precision / 2
    int working_prec;
    assert(precision > 1);
    if (lo.mantissa.data.size >= 2 * precision) {working_prec = 2 * precision;}
    else {working_prec = lo.mantissa.data.size;}
    int output_precision = working_prec / 2;
    
    //TODO: How to terminate?
    int num_Iter = 3 * log(precision);
    int min_iter = 1;
    // initial value x_0
    double guide = big_float_to_double(lo);
    assert(guide > 0);
    guide = 1 / sqrt(guide);
    BigFloat x = double_to_big_float_fixed_precision(guide, working_prec);
    // w = x_i ^ 2
    BigFloat w = big_float_mul_fixed_precision(x, x, working_prec);
    BigFloat one = big_float_create_from_uint_fixed_precision(1, working_prec);
    // w * lo
    big_float_mul_inplace_fixed_precision(&w, lo, working_prec);
    // d = w * lo - 1
    big_float_sub_inplace_fixed_precision(&w, one, working_prec);
    // d * x_i
    big_float_mul_inplace_fixed_precision(&w, x, working_prec);
    // d * x_i / 2
    big_float_div_by_power_of_two_inplace_fixed_precision(&w, 1, working_prec);
    // x_i+1 = x_i - d * x_i / 2
    big_float_sub_inplace_fixed_precision(&x, w, working_prec);

    unsigned int prev_value = x.mantissa.data.bits[x.mantissa.data.size - output_precision];

    // main loop
    for (int i = 0; i < num_Iter; i++) {
        big_float_mul_toplace_fixed_precision(x, x,&w, working_prec); 
        big_float_mul_inplace_fixed_precision(&w, lo, working_prec);
        big_float_sub_inplace_fixed_precision(&w, one, working_prec);
        big_float_mul_inplace_fixed_precision(&w, x, working_prec);
        big_float_div_by_power_of_two_inplace_fixed_precision(&w, 1, working_prec);
        big_float_sub_inplace_fixed_precision(&x, w, working_prec);
        big_float_print_msg(x,"x_i+1 = x_i - d * x_i / 2");
        printf(" %e\n",big_float_to_double(x));
        if (x.mantissa.data.bits[x.mantissa.data.size - output_precision] == prev_value && i > min_iter) {
            printf("guided reciprocal sqrt took %d iterations to converge.\n",i+2);
            break;
        }
        else prev_value = x.mantissa.data.bits[x.mantissa.data.size - output_precision];
    }
    big_float_destroy(&w);
    big_float_destroy(&one);
    return x;
    

}