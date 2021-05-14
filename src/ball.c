#include "ball.h"
#include "ball_helper.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <string.h>
// #include "tsc_x86.h"
Ball ball_add(Ball lo, Ball ro) {
    Ball ans;
    ans.center = big_float_add(lo.center,ro.center);
    ans.radius = lo.radius + ro.radius;
    return ans;
}
Ball ball_sub(Ball lo, Ball ro) {
    Ball ans;
    ans.center = big_float_sub(lo.center,ro.center);
    ans.radius = lo.radius + ro.radius;
    return ans;
}
Ball ball_multiply(Ball lo, Ball ro) {
    Ball ans;
    ans.center = big_float_multiply(lo.center,ro.center);
    //ans.radius = dl*r + dr*l + l*r
    ans.radius = fabs(big_float_to_double(lo.center)) * ro.radius + fabs(big_float_to_double(ro.center)) * lo.radius + lo.radius * ro.radius;
    return ans;
}
Ball ball_div(Ball lo, Ball ro) {
    Ball ans;
    //TODO: what about ro = 0? how to define infinity in bigfloat?
    ans.center = big_float_div(lo.center,ro.center);
    double ans_center_abs_value = fabs(big_float_to_double(ans.center));
    double ro_abs_value = fabs(big_float_to_double(ro.center));
    // TODO: optimize this
    // ans.radius = l/r + l*dr/r^2 + dl/r + dl*dr/r^2
    ans.radius = ans_center_abs_value + ans_center_abs_value / fabs(big_float_to_double(ro.center)) * ro.radius + lo.radius / ro_abs_value + lo.radius * ro.radius / ro_abs_value / ro_abs_value;
    return ans;
}

Ball ball_multiply_quad_double(Ball lo, Ball ro){
    Ball ans;

    if(lo.center.mantissa.sign == 0 || ro.center.mantissa.sign == 0){
        // one of the input center is 0
        ans.center.power = 0;
        ans.center.mantissa.sign = 0;
        ans.center.mantissa.data.size = 1;
        ans.center.mantissa.data.capacity = 2;
        ans.center.mantissa.data.bits = calloc(2, 4);
    }else{
        unsigned int *tmp = calloc(16, 4);
        int offset_tmp;
        
        offset_tmp = lo.center.mantissa.data.size - 8;
        int offset_left = offset_tmp >= 0 ? offset_tmp : 0;
        offset_tmp = ro.center.mantissa.data.size - 8;
        int offset_right = offset_tmp >= 0 ? offset_tmp : 0;

        for (int j = offset_right; j < ro.center.mantissa.data.size; j++) {
            unsigned long long carry = 0;
            for (int i = offset_left; i < lo.center.mantissa.data.size; i++) {
                int idx = j - offset_right + i - offset_left;
                carry += (unsigned long long)lo.center.mantissa.data.bits[i] * ro.center.mantissa.data.bits[j] + tmp[idx];
                tmp[idx] = (unsigned int)carry;
                carry >>= 32;
            }
            int idx = j - offset_right + 8;
            carry += tmp[idx];
            tmp[idx] = (unsigned int)carry;
        }

        ans.center.power = lo.center.power + ro.center.power;
        ans.center.mantissa.sign = lo.center.mantissa.sign * ro.center.mantissa.sign;
        ans.center.mantissa.data.size = 8;
        ans.center.mantissa.data.capacity = 9;
        ans.center.mantissa.data.bits = calloc(9, 4);

        int offset = 8;
        for (int i = 15; i >= 8; i--) {
            if (tmp[i] != 0) {
                break;
            } else {
                offset--;
            }
        }        
        memmove(ans.center.mantissa.data.bits, tmp + offset, 32);
        for(int i=7;i>=0;i--){
            if(ans.center.mantissa.data.bits[i] == 0){
                ans.center.mantissa.data.size--;
            }else{
                break;
            }
        }

        if(lo.center.mantissa.data.size + ro.center.mantissa.data.size != ans.center.mantissa.data.size){
            ans.center.power--;
        }

        free(tmp); // release temporal variable
    }

    // ans.radius = dl*r + dr*l + l*r
    ans.radius = fabs(big_float_to_double(lo.center)) * ro.radius + fabs(big_float_to_double(ro.center)) * lo.radius + lo.radius * ro.radius;
    return ans;
}

typedef enum {BALL_ADD,BALL_SUB,BALL_MUL,BALL_DIV}test_type_ball;
void test_ball(double lo, double ro, test_type_ball t) {
    printf("\n\n----------------ball ");
    switch (t)
    {
    case BALL_ADD: printf("add"); break;
    case BALL_SUB: printf("sub"); break;
    case BALL_MUL: printf("mul"); break;
    case BALL_DIV: printf("div"); break;
    default: exit(-1);
    }
    printf(" routine----------------\n");
    Ball b1 = double_to_ball(lo);
    Ball b2 = double_to_ball(ro);
    printf("input 1: %e\n",lo);
    ball_print(b1);
    printf("input 2: %e\n",ro);
    ball_print(b2);
    Ball b3;
    switch (t)
    {
    case BALL_ADD: b3 = ball_add(b1,b2); break;
    case BALL_SUB: b3 = ball_sub(b1,b2); break;
    case BALL_MUL: b3 = ball_multiply(b1,b2); break;
    case BALL_DIV: b3 = ball_div(b1,b2); break;
    default: exit(-1);
    }
    printf("output: \n");
    ball_print(b3);
    printf("transformed back to double: %e\n",ball_to_double(b3));
    switch (t)
    {
    case BALL_ADD: printf("reference: %e\n",lo+ro); break;
    case BALL_SUB: printf("reference: %e\n",lo-ro); break;
    case BALL_MUL: printf("reference: %e\n",lo*ro); break;
    case BALL_DIV: printf("reference: %e\n",lo/ro); break;
    default: exit(-1);
    }
    ball_destory(&b1);
    ball_destory(&b2);
    ball_destory(&b3);
}

int main() {
    // double testdata = -1e300;
    // BigFloat test1 = double_to_big_float(testdata);
    // printf("input double: %e\n",testdata);
    // big_float_print(test1);
    // printf("transformed back to double: %e\n",big_float_to_double(test1));
    // big_float_destroy(&test1);

    // double testdata2 = -1e-300;
    // Ball test2 = double_to_ball(testdata2);
    // printf("input double: %e\n",testdata2);
    // ball_print(test2);
    // printf("transformed back to double: %e\n",big_float_to_double(test2.center));
    // ball_destory(&test2);

    // double testdata3 = -1e-300;
    // Ball test3 = double_to_ball(testdata3);
    // printf("input double: %e\n",testdata3);
    // ball_print(test3);
    // printf("transformed back to double: %e\n",ball_to_double(test3));
    // ball_destory(&test3);

    // test_ball(1.0e3,2.1e2,BALL_ADD);
    // test_ball(-1.0e3,1e3,BALL_ADD);
    // test_ball(1.0e300,2.1e301,BALL_ADD);

    // test_ball(1.0e3,2.1e2,BALL_MUL);
    // test_ball(-1.0e3,1e3,BALL_MUL);
    // test_ball(1.0e30,2.1e50,BALL_MUL);

    test_ball(1.0e3,2.1e2,BALL_DIV);
    test_ball(-1.0e3,1e3,BALL_DIV);
    test_ball(1.0e30,2.1e50,BALL_DIV);
}
