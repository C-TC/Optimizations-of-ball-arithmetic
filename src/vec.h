#ifndef VEC_H
#define VEC_H
#include "big_float.h"
#include "big_float_helper.h"
#include "big_integer.h"
#include "ball.h"
typedef struct Vec {
    Ball *data;
    int size;
} Vec;
Vec vec_add(Vec, Vec);
Vec vec_add_vec(Vec, Vec);
#endif
