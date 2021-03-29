#include "ball_helper.h"
#include "bigfloat_helper.h"
void ball_delete(Ball *ball) {
    bigfloat_delete(ball->center);
    SAFE_FREE(ball);
}
