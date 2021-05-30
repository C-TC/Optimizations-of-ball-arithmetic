#include "quad_double.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

quad_double quad_double_create_from_double(double d0, double d1, double d2, double d3) {
    quad_double ans;
    ans.d[0] = d0;
    ans.d[1] = d1;
    ans.d[2] = d2;
    ans.d[3] = d3;
    return ans;
}

void print_qd(quad_double qd, const char* msg) {
  printf("%s: ",msg);
  printf("%e + %e + %e + %e\n", qd.d[0], qd.d[1], qd.d[2], qd.d[3]);
}

void print_qd_arr(qd_arr qda, int num, const char* msg) {
  assert(num > 0);
  printf("%s: ",msg);
  int size = qda.size < num? qda.size : num;
  for (int i = 0; i < size; i++) {
    printf("%e + %e + %e + %e\n", qda.d0[i], qda.d1[i], qda.d2[i], qda.d3[i]);
  }
}

quad_double qd_add_qd(quad_double *a, quad_double *b) {
  
  double s0, s1, s2, s3;
  double t0, t1, t2, t3;
  
  s0 = two_sum(a->d[0], b->d[0], &t0);
  s1 = two_sum(a->d[1], b->d[1], &t1);
  s2 = two_sum(a->d[2], b->d[2], &t2);
  s3 = two_sum(a->d[3], b->d[3], &t3);

  s1 = two_sum(s1, t0, &t0);
  three_sum(&s2, &t0, &t1);
  three_sum2(&s3, &t0, &t2);
  t0 = t0 + t1 + t3;

  renorm5(&s0, &s1, &s2, &s3, &t0);
  return quad_double_create_from_double(s0, s1, s2, s3);
}

quad_double qd_sub_qd(quad_double *a, quad_double *b) {
  
  double s0, s1, s2, s3;
  double t0, t1, t2, t3;
  
  s0 = two_sum(a->d[0], -b->d[0], &t0);
  s1 = two_sum(a->d[1], -b->d[1], &t1);
  s2 = two_sum(a->d[2], -b->d[2], &t2);
  s3 = two_sum(a->d[3], -b->d[3], &t3);

  s1 = two_sum(s1, t0, &t0);
  three_sum(&s2, &t0, &t1);
  three_sum2(&s3, &t0, &t2);
  t0 = t0 + t1 + t3;

  renorm5(&s0, &s1, &s2, &s3, &t0);
  return quad_double_create_from_double(s0, s1, s2, s3);
}

quad_double qd_mul_d(quad_double *a, double b) {
  double p0, p1, p2, p3;
  double q0, q1, q2;
  double s0, s1, s2, s3, s4;

  p0 = two_prod(a->d[0], b, &q0);
  p1 = two_prod(a->d[1], b, &q1);
  p2 = two_prod(a->d[2], b, &q2);
  p3 = a->d[3] * b;

  s0 = p0;

  s1 = two_sum(q0, p1, &s2);

  three_sum(&s2, &q1, &p2);

  three_sum2(&q1, &q2, &p3);
  s3 = q1;

  s4 = q2 + p2;

  renorm5(&s0, &s1, &s2, &s3, &s4);
  return quad_double_create_from_double(s0, s1, s2, s3);
}

/* quad-double * quad-double */
/* a0 * b0                    0
        a0 * b1               1
        a1 * b0               2
             a0 * b2          3
             a1 * b1          4
             a2 * b0          5
                  a0 * b3     6
                  a1 * b2     7
                  a2 * b1     8
                  a3 * b0     9  */
quad_double qd_mul_qd(quad_double *a, quad_double *b) {
  double p0, p1, p2, p3, p4, p5;
  double q0, q1, q2, q3, q4, q5;
  double p6, p7, p8, p9;
  double q6, q7, q8, q9;
  double r0, r1;
  double t0, t1;
  double s0, s1, s2;

  p0 = two_prod(a->d[0], b->d[0], &q0);

  p1 = two_prod(a->d[0], b->d[1], &q1);
  p2 = two_prod(a->d[1], b->d[0], &q2);

  p3 = two_prod(a->d[0], b->d[2], &q3);
  p4 = two_prod(a->d[1], b->d[1], &q4);
  p5 = two_prod(a->d[2], b->d[0], &q5);

  /* Start Accumulation */
  three_sum(&p1, &p2, &q0);

  /* Six-Three Sum  of p2, q1, q2, p3, p4, p5. */
  three_sum(&p2, &q1, &q2);
  three_sum(&p3, &p4, &p5);
  /* compute (s0, s1, s2) = (p2, q1, q2) + (p3, p4, p5). */
  s0 = two_sum(p2, p3, &t0);
  s1 = two_sum(q1, p4, &t1);
  s2 = q2 + p5;
  s1 = two_sum(s1, t0, &t0);
  s2 += (t0 + t1);

  /* O(eps^3) order terms */
  p6 = two_prod(a->d[0], b->d[3], &q6);
  p7 = two_prod(a->d[1], b->d[2], &q7);
  p8 = two_prod(a->d[2], b->d[1], &q8);
  p9 = two_prod(a->d[3], b->d[0], &q9);

  /* Nine-Two-Sum of q0, s1, q3, q4, q5, p6, p7, p8, p9. */
  q0 = two_sum(q0, q3, &q3);
  q4 = two_sum(q4, q5, &q5);
  p6 = two_sum(p6, p7, &p7);
  p8 = two_sum(p8, p9, &p9);
  /* Compute (t0, t1) = (q0, q3) + (q4, q5). */
  t0 = two_sum(q0, q4, &t1);
  t1 += (q3 + q5);
  /* Compute (r0, r1) = (p6, p7) + (p8, p9). */
  r0 = two_sum(p6, p8, &r1);
  r1 += (p7 + p9);
  /* Compute (q3, q4) = (t0, t1) + (r0, r1). */
  q3 = two_sum(t0, r0, &q4);
  q4 += (t1 + r1);
  /* Compute (t0, t1) = (q3, q4) + s1. */
  t0 = two_sum(q3, s1, &t1);
  t1 += q4;

  /* O(eps^4) terms -- Nine-One-Sum */
  t1 += a->d[1] * b->d[3] + a->d[2] * b->d[2] + a->d[3] * b->d[1] + q6 + q7 + q8 + q9 + s2;

  renorm5(&p0, &p1, &s0, &t0, &t1);
  return quad_double_create_from_double(p0, p1, s0, t0);
}

quad_double qd_div_qd(quad_double *a, quad_double *b) {
  double q0, q1, q2, q3;

  quad_double r;
  quad_double tmp;

  q0 = a->d[0] / b->d[0];
  tmp = qd_mul_d(b, q0);
  r = qd_sub_qd(a, &tmp);

  q1 = r.d[0] / b->d[0];
  tmp = qd_mul_d(b, q1);
  r = qd_sub_qd(&r, &tmp);

  q2 = r.d[0] / b->d[0];
  tmp = qd_mul_d(b, q2);
  r = qd_sub_qd(&r, &tmp);

  q3 = r.d[0] / b->d[0];
  tmp = qd_mul_d(b, q3);
  r = qd_sub_qd(&r, &tmp);
  double q4 = r.d[0] / b->d[0];

  renorm5(&q0, &q1, &q2, &q3, &q4);

  return quad_double_create_from_double(q0, q1, q2, q3);
}

qd_arr qd_arr_create_from_double_arr(double *data, int size) {
  assert(size > 0);
  qd_arr ans;
  ans.size = size;
  ans.d0 = (double *)malloc(size * sizeof(double));
  ans.d1 = (double *)calloc(size, sizeof(double));
  ans.d2 = (double *)calloc(size, sizeof(double));
  ans.d3 = (double *)calloc(size, sizeof(double));

  for (int i = 0; i < size; i++) {
    ans.d0[i] = data[i];
  }

  return ans;
}

qd_arr qd_arr_create_random(int size, double min, double max) {
  assert(size > 0);
  qd_arr ans;
  ans.size = size;
  ans.d0 = (double *)malloc(size * sizeof(double));
  ans.d1 = (double *)malloc(size * sizeof(double));
  ans.d2 = (double *)malloc(size * sizeof(double));
  ans.d3 = (double *)malloc(size * sizeof(double));

  for (int i = 0; i < size; i++) {
    ans.d0[i] = (max - min) * ((double)rand() / (double)RAND_MAX) + min;
    ans.d1[i] = 1e-16 * (2.0 * ((double)rand() / (double)RAND_MAX) - 1);
    ans.d2[i] = 1e-32 * (2.0 * ((double)rand() / (double)RAND_MAX) - 1);
    ans.d3[i] = 1e-48 * (2.0 * ((double)rand() / (double)RAND_MAX) - 1);
  }

  return ans;
}

void qd_destroy(qd_arr qda) {
  if (qda.d0) {free(qda.d0);}
  if (qda.d1) {free(qda.d1);}
  if (qda.d2) {free(qda.d2);}
  if (qda.d3) {free(qda.d3);}
}

qd_arr qd_arr_add(qd_arr lo, qd_arr ro) {
  assert(lo.size == ro.size);
  qd_arr ans;
  int size = lo.size;
  ans.size = size;
  ans.d0 = (double *)malloc(size * sizeof(double));
  ans.d1 = (double *)malloc(size * sizeof(double));
  ans.d2 = (double *)malloc(size * sizeof(double));
  ans.d3 = (double *)malloc(size * sizeof(double));

  double s0, s1, s2, s3;
  double t0, t1, t2, t3;

  for (int i = 0; i < size; i++) {
      
    s0 = two_sum(lo.d0[i], ro.d0[i], &t0);
    s1 = two_sum(lo.d1[i], ro.d1[i], &t1);
    s2 = two_sum(lo.d2[i], ro.d2[i], &t2);
    s3 = two_sum(lo.d3[i], ro.d3[i], &t3);

    s1 = two_sum(s1, t0, &t0);
    three_sum(&s2, &t0, &t1);
    three_sum2(&s3, &t0, &t2);
    t0 = t0 + t1 + t3;

    renorm5(&s0, &s1, &s2, &s3, &t0);
    ans.d0[i] = s0;
    ans.d1[i] = s1;
    ans.d2[i] = s2;
    ans.d3[i] = s3;
  }
  return ans;
}

qd_arr qd_arr_sub(qd_arr lo, qd_arr ro) {
  assert(lo.size == ro.size);
  qd_arr ans;
  int size = lo.size;
  ans.size = size;
  ans.d0 = (double *)malloc(size * sizeof(double));
  ans.d1 = (double *)malloc(size * sizeof(double));
  ans.d2 = (double *)malloc(size * sizeof(double));
  ans.d3 = (double *)malloc(size * sizeof(double));


  double s0, s1, s2, s3;
  double t0, t1, t2, t3;

  for (int i = 0; i < size; i++) {
      
    s0 = two_sum(lo.d0[i], -ro.d0[i], &t0);
    s1 = two_sum(lo.d1[i], -ro.d1[i], &t1);
    s2 = two_sum(lo.d2[i], -ro.d2[i], &t2);
    s3 = two_sum(lo.d3[i], -ro.d3[i], &t3);

    s1 = two_sum(s1, t0, &t0);
    three_sum(&s2, &t0, &t1);
    three_sum2(&s3, &t0, &t2);
    t0 = t0 + t1 + t3;

    renorm5(&s0, &s1, &s2, &s3, &t0);
    ans.d0[i] = s0;
    ans.d1[i] = s1;
    ans.d2[i] = s2;
    ans.d3[i] = s3;
  }
  return ans;
}

qd_arr qd_arr_mul(qd_arr lo, qd_arr ro) {
  assert(lo.size == ro.size);
  qd_arr ans;
  int size = lo.size;
  ans.size = size;
  ans.d0 = (double *)malloc(size * sizeof(double));
  ans.d1 = (double *)malloc(size * sizeof(double));
  ans.d2 = (double *)malloc(size * sizeof(double));
  ans.d3 = (double *)malloc(size * sizeof(double));


  double p0, p1, p2, p3, p4, p5;
  double q0, q1, q2, q3, q4, q5;
  double p6, p7, p8, p9;
  double q6, q7, q8, q9;
  double r0, r1;
  double t0, t1;
  double s0, s1, s2;

  for (int i = 0; i < size; i++) {
    p0 = two_prod(lo.d0[i], ro.d0[i], &q0);

    p1 = two_prod(lo.d0[i], ro.d1[i], &q1);
    p2 = two_prod(lo.d1[i], ro.d0[i], &q2);

    p3 = two_prod(lo.d0[i], ro.d2[i], &q3);
    p4 = two_prod(lo.d1[i], ro.d1[i], &q4);
    p5 = two_prod(lo.d2[i], ro.d0[i], &q5);

    /* Start Accumulation */
    three_sum(&p1, &p2, &q0);

    /* Six-Three Sum  of p2, q1, q2, p3, p4, p5. */
    three_sum(&p2, &q1, &q2);
    three_sum(&p3, &p4, &p5);
    /* compute (s0, s1, s2) = (p2, q1, q2) + (p3, p4, p5). */
    s0 = two_sum(p2, p3, &t0);
    s1 = two_sum(q1, p4, &t1);
    s2 = q2 + p5;
    s1 = two_sum(s1, t0, &t0);
    s2 += (t0 + t1);

    /* O(eps^3) order terms */
    p6 = two_prod(lo.d0[i], ro.d3[i], &q6);
    p7 = two_prod(lo.d1[i], ro.d2[i], &q7);
    p8 = two_prod(lo.d2[i], ro.d1[i], &q8);
    p9 = two_prod(lo.d3[i], ro.d0[i], &q9);

    /* Nine-Two-Sum of q0, s1, q3, q4, q5, p6, p7, p8, p9. */
    q0 = two_sum(q0, q3, &q3);
    q4 = two_sum(q4, q5, &q5);
    p6 = two_sum(p6, p7, &p7);
    p8 = two_sum(p8, p9, &p9);
    /* Compute (t0, t1) = (q0, q3) + (q4, q5). */
    t0 = two_sum(q0, q4, &t1);
    t1 += (q3 + q5);
    /* Compute (r0, r1) = (p6, p7) + (p8, p9). */
    r0 = two_sum(p6, p8, &r1);
    r1 += (p7 + p9);
    /* Compute (q3, q4) = (t0, t1) + (r0, r1). */
    q3 = two_sum(t0, r0, &q4);
    q4 += (t1 + r1);
    /* Compute (t0, t1) = (q3, q4) + s1. */
    t0 = two_sum(q3, s1, &t1);
    t1 += q4;

    /* O(eps^4) terms -- Nine-One-Sum */
    t1 += lo.d1[i] * ro.d3[i] + lo.d2[i] * ro.d2[i] + lo.d3[i] * ro.d1[i] + q6 + q7 + q8 + q9 + s2;

    renorm5(&p0, &p1, &s0, &t0, &t1);
    ans.d0[i] = p0;
    ans.d1[i] = p1;
    ans.d2[i] = s0;
    ans.d3[i] = t0;
  }
  return ans;
}

qd_arr qd_arr_div(qd_arr lo, qd_arr ro) {
  assert(lo.size == ro.size);
  qd_arr ans;
  int size = lo.size;
  ans.size = size;
  ans.d0 = (double *)malloc(size * sizeof(double));
  ans.d1 = (double *)malloc(size * sizeof(double));
  ans.d2 = (double *)malloc(size * sizeof(double));
  ans.d3 = (double *)malloc(size * sizeof(double));

  quad_double tmp1, tmp2, res;
  for (int i = 0; i < size; i++) {
    tmp1.d[0] = lo.d0[i];
    tmp1.d[1] = lo.d1[i];
    tmp1.d[2] = lo.d2[i];
    tmp1.d[3] = lo.d3[i];
    tmp2.d[0] = ro.d0[i];
    tmp2.d[1] = ro.d1[i];
    tmp2.d[2] = ro.d2[i];
    tmp2.d[3] = ro.d3[i];

    res = qd_div_qd(&tmp1, &tmp2);

    ans.d0[i] = res.d[0];
    ans.d1[i] = res.d[1];
    ans.d2[i] = res.d[2];
    ans.d3[i] = res.d[3];
  }
  return ans;
}

int main() {
/*   quad_double a=quad_double_create_from_double(1e30,2e10,3e-10,4e-30);
  quad_double b=quad_double_create_from_double(5e30,3e10,2e-10,1e-30);
  quad_double c=qd_sub_qd(&a,&b);
  print_qd(a,"debug");
  print_qd(b,"debug");
  print_qd(c,"debug"); */

  srand(11);
  qd_arr a=qd_arr_create_random(5,-1,1);
  qd_arr b=qd_arr_create_random(5,-1,1);
  print_qd_arr(a,3,"a");
  print_qd_arr(b,3,"b");
  qd_arr c=qd_arr_div(a,b);
  print_qd_arr(c,3,"c");

  return 0;
}