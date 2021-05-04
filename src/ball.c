#include "ball.h"
#include "ball_helper.h"
#include <stdio.h>
#include <stdlib.h>
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
    ans.radius = big_float_to_double(lo.center) * ro.radius + big_float_to_double(ro.center) * lo.radius + lo.radius * ro.radius;
    return ans;
}
Ball ball_div(Ball lo, Ball ro) {
    Ball ans;
    //TODO: what about ro = 0? how to define infinity in bigfloat?
    ans.center = big_float_div(lo.center,ro.center);
    double anscentervalue = big_float_to_double(ans.center);
    double rovalue = big_float_to_double(ro.center);
    // TODO: optimize this
    // ans.radius = l/r + l*dr/r^2 + dl/r + dl*dr/r^2
    ans.radius = anscentervalue + anscentervalue / big_float_to_double(ro.center) * ro.radius + lo.radius / rovalue + lo.radius * ro.radius / rovalue / rovalue;
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
    BigFloat lo, ro;
    lo.power = ro.power = 1;
    lo.mantissa.sign = ro.mantissa.sign = 1;
    lo.mantissa.data.size = ro.mantissa.data.size = 1;
    lo.mantissa.data.capacity = ro.mantissa.data.capacity = 4;
    lo.mantissa.data.bits = (unsigned int*)calloc(4, sizeof(unsigned int));
    ro.mantissa.data.bits = (unsigned int*)calloc(4, sizeof(unsigned int));
    lo.mantissa.data.bits[0] = 2048u;
    // lo.mantissa.data.bits[1] = 3537031890u;
    // lo.mantissa.data.bits[2] = 2u;
    ro.mantissa.data.bits[0] = 2048u;
    // ro.mantissa.data.bits[1] = 3789677025u;
    // ro.mantissa.data.bits[2] = 1u;
    BigFloat ans = big_float_div(lo, ro);
    printf("%f\n", big_float_to_double(lo));
    printf("%f\n", big_float_to_double(ro));
    printf("%f\n", big_float_to_double(ans));
    // big_float_print(ans);
    // printf("The power of the ans is %lld\n", ans.power);
    free(lo.mantissa.data.bits);
    free(ro.mantissa.data.bits);
    free(ans.mantissa.data.bits);

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

    // test_ball(1.0e3,2.1e2,BALL_DIV);
    // test_ball(-1.0e3,1e3,BALL_DIV);
    // test_ball(1.0e30,2.1e50,BALL_DIV);
}
