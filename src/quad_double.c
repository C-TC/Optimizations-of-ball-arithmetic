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

qd_arr qd_arr_create_random_aligned(int size, double min, double max) {
  assert(size > 0);
  qd_arr ans;
  ans.size = size;
  ans.d0 = _mm_malloc(size * sizeof(double), ALIGNMENT);
  ans.d1 = _mm_malloc(size * sizeof(double), ALIGNMENT);
  ans.d2 = _mm_malloc(size * sizeof(double), ALIGNMENT);
  ans.d3 = _mm_malloc(size * sizeof(double), ALIGNMENT);

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

void qd_destroy_aligned(qd_arr qda) {
  if (qda.d0) {_mm_free(qda.d0);}
  if (qda.d1) {_mm_free(qda.d1);}
  if (qda.d2) {_mm_free(qda.d2);}
  if (qda.d3) {_mm_free(qda.d3);}
}

qd_arr qd_arr_add(qd_arr lo, qd_arr ro) {
  //assert(lo.size == ro.size);
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
  //assert(lo.size == ro.size);
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
  //assert(lo.size == ro.size);
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
  //assert(lo.size == ro.size);
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


void qd_arr_add_inplace(qd_arr lo, qd_arr ro) {
  //assert(lo.size == ro.size);
  int size = lo.size;

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
    lo.d0[i] = s0;
    lo.d1[i] = s1;
    lo.d2[i] = s2;
    lo.d3[i] = s3;
  }
}

void qd_arr_add_inplace_inline(qd_arr lo, qd_arr ro) {
  //assert(lo.size == ro.size);
  int size = lo.size;

  double s0, s1, s2, s3;
  double t0, t1, t2, t3;

  for (int i = 0; i < size; i++) {
      
    s0 = two_sum(lo.d0[i], ro.d0[i], &t0);
    s1 = two_sum(lo.d1[i], ro.d1[i], &t1);
    s2 = two_sum(lo.d2[i], ro.d2[i], &t2);
    s3 = two_sum(lo.d3[i], ro.d3[i], &t3);

    s1 = two_sum(s1, t0, &t0);
    three_sum_inline(&s2, &t0, &t1);
    three_sum2_inline(&s3, &t0, &t2);
    t0 = t0 + t1 + t3;

    renorm5(&s0, &s1, &s2, &s3, &t0);
    lo.d0[i] = s0;
    lo.d1[i] = s1;
    lo.d2[i] = s2;
    lo.d3[i] = s3;
  }
}

void qd_arr_add_inplace_vec(qd_arr lo, qd_arr ro) {
  //assert(lo.size == ro.size);
  int size = lo.size;

  double s0, s1, s2, s3;
  double t0, t1, t2, t3;
  __m256d vs0, vs1, vs2, vs3;
  __m256d vt0, vt1, vt2, vt3;
  __m256d ld0, ld1, ld2, ld3;
  __m256d rd0, rd1, rd2, rd3;
  double *tmp_vt0 = (double *)_mm_malloc(4 * sizeof(double), ALIGNMENT);
  int i;
  for (i = 0; i + 3 < size; i += 4) {
    ld0 = _mm256_load_pd(lo.d0 + i);
    ld1 = _mm256_load_pd(lo.d1 + i);
    ld2 = _mm256_load_pd(lo.d2 + i);
    ld3 = _mm256_load_pd(lo.d3 + i);
    rd0 = _mm256_load_pd(ro.d0 + i);
    rd1 = _mm256_load_pd(ro.d1 + i);
    rd2 = _mm256_load_pd(ro.d2 + i);
    rd3 = _mm256_load_pd(ro.d3 + i);

    vs0 = two_sum_vec(ld0, rd0, &vt0);
    vs1 = two_sum_vec(ld1, rd1, &vt1);
    vs2 = two_sum_vec(ld2, rd2, &vt2);
    vs3 = two_sum_vec(ld3, rd3, &vt3);

    vs1 = two_sum_vec(vs1, vt0, &vt0);
    three_sum_vec(&vs2, &vt0, &vt1);
    three_sum2_vec(&vs3, &vt0, &vt2);
    vt0 = _mm256_add_pd(_mm256_add_pd(vt0, vt1), vt3);

    //renorm5(&s0, &s1, &s2, &s3, &t0);
    _mm256_store_pd(lo.d0 + i, vs0);
    _mm256_store_pd(lo.d1 + i, vs1);
    _mm256_store_pd(lo.d2 + i, vs2);
    _mm256_store_pd(lo.d3 + i, vs3);
    _mm256_store_pd(tmp_vt0, vt0);
    renorm5(lo.d0 + i, lo.d1 + i, lo.d2 + i, lo.d3 + i, tmp_vt0);
    renorm5(lo.d0 + i + 1, lo.d1 + i + 1, lo.d2 + i + 1, lo.d3 + i + 1, tmp_vt0 + 1);
    renorm5(lo.d0 + i + 2, lo.d1 + i + 2, lo.d2 + i + 2, lo.d3 + i + 2, tmp_vt0 + 2);
    renorm5(lo.d0 + i + 3, lo.d1 + i + 3, lo.d2 + i + 3, lo.d3 + i + 3, tmp_vt0 + 3);
  }

  for (; i < size; i++) {
      
    s0 = two_sum(lo.d0[i], ro.d0[i], &t0);
    s1 = two_sum(lo.d1[i], ro.d1[i], &t1);
    s2 = two_sum(lo.d2[i], ro.d2[i], &t2);
    s3 = two_sum(lo.d3[i], ro.d3[i], &t3);

    s1 = two_sum(s1, t0, &t0);
    three_sum(&s2, &t0, &t1);
    three_sum2(&s3, &t0, &t2);
    t0 = t0 + t1 + t3;

    renorm5(&s0, &s1, &s2, &s3, &t0);
    lo.d0[i] = s0;
    lo.d1[i] = s1;
    lo.d2[i] = s2;
    lo.d3[i] = s3;
  }
  _mm_free(tmp_vt0);
}

void qd_arr_add_inplace_vec_inline(qd_arr lo, qd_arr ro) {
  //assert(lo.size == ro.size);
  int size = lo.size;

  double s0, s1, s2, s3;
  double t0, t1, t2, t3;
  __m256d vs0, vs1, vs2, vs3;
  __m256d vt0, vt1, vt2, vt3;
  __m256d ld0, ld1, ld2, ld3;
  __m256d rd0, rd1, rd2, rd3;
  __m256d tmp_s, tmp_t1, tmp_t2, tmp_t3, tmp_bb, tmp0, tmp1;
  double *tmp_vt0 = (double *)_mm_malloc(4 * sizeof(double), ALIGNMENT);
  int i;
  for (i = 0; i + 3 < size; i += 4) {
    ld0 = _mm256_load_pd(lo.d0 + i);
    ld1 = _mm256_load_pd(lo.d1 + i);
    ld2 = _mm256_load_pd(lo.d2 + i);
    ld3 = _mm256_load_pd(lo.d3 + i);
    rd0 = _mm256_load_pd(ro.d0 + i);
    rd1 = _mm256_load_pd(ro.d1 + i);
    rd2 = _mm256_load_pd(ro.d2 + i);
    rd3 = _mm256_load_pd(ro.d3 + i);

    //vs0 = two_sum_vec(ld0, rd0, &vt0);
    tmp_s = _mm256_add_pd(ld0, rd0);
    tmp_bb = _mm256_sub_pd(tmp_s, ld0);
    tmp0 = _mm256_sub_pd(tmp_s, tmp_bb);
    tmp1 = _mm256_sub_pd(rd0, tmp_bb);
    tmp0 = _mm256_sub_pd(ld0, tmp0);
    vt0 = _mm256_add_pd(tmp0, tmp1);
    vs0 = tmp_s;

    //vs1 = two_sum_vec(ld1, rd1, &vt1);
    tmp_s = _mm256_add_pd(ld1, rd1);
    tmp_bb = _mm256_sub_pd(tmp_s, ld1);
    tmp0 = _mm256_sub_pd(tmp_s, tmp_bb);
    tmp1 = _mm256_sub_pd(rd1, tmp_bb);
    tmp0 = _mm256_sub_pd(ld1, tmp0);
    vt1 = _mm256_add_pd(tmp0, tmp1);
    vs1 = tmp_s;

    //vs2 = two_sum_vec(ld2, rd2, &vt2);
    tmp_s = _mm256_add_pd(ld2, rd2);
    tmp_bb = _mm256_sub_pd(tmp_s, ld2);
    tmp0 = _mm256_sub_pd(tmp_s, tmp_bb);
    tmp1 = _mm256_sub_pd(rd2, tmp_bb);
    tmp0 = _mm256_sub_pd(ld2, tmp0);
    vt2 = _mm256_add_pd(tmp0, tmp1);
    vs2 = tmp_s;

    //vs3 = two_sum_vec(ld3, rd3, &vt3);
    tmp_s = _mm256_add_pd(ld3, rd3);
    tmp_bb = _mm256_sub_pd(tmp_s, ld3);
    tmp0 = _mm256_sub_pd(tmp_s, tmp_bb);
    tmp1 = _mm256_sub_pd(rd3, tmp_bb);
    tmp0 = _mm256_sub_pd(ld3, tmp0);
    vt3 = _mm256_add_pd(tmp0, tmp1);
    vs3 = tmp_s;

    //vs1 = two_sum_vec(vs1, vt0, &vt0);
    tmp_s = _mm256_add_pd(vs1, vt0);
    tmp_bb = _mm256_sub_pd(tmp_s, vs1);
    tmp0 = _mm256_sub_pd(tmp_s, tmp_bb);
    tmp1 = _mm256_sub_pd(vt0, tmp_bb);
    tmp0 = _mm256_sub_pd(vs1, tmp0);
    vt0 = _mm256_add_pd(tmp0, tmp1);
    vs1 = tmp_s;

    //three_sum_vec(&vs2, &vt0, &vt1);
    tmp_s = _mm256_add_pd(vs2, vt0);
    tmp_bb = _mm256_sub_pd(tmp_s, vs2);
    tmp0 = _mm256_sub_pd(tmp_s, tmp_bb);
    tmp1 = _mm256_sub_pd(vt0, tmp_bb);
    tmp0 = _mm256_sub_pd(vs2, tmp0);
    tmp_t2 = _mm256_add_pd(tmp0, tmp1);
    tmp_t1 = tmp_s;
    tmp_s = _mm256_add_pd(vt1, tmp_t1);
    tmp_bb = _mm256_sub_pd(tmp_s, vt1);
    tmp0 = _mm256_sub_pd(tmp_s, tmp_bb);
    tmp1 = _mm256_sub_pd(tmp_t1, tmp_bb);
    tmp0 = _mm256_sub_pd(vt1, tmp0);
    tmp_t3 = _mm256_add_pd(tmp0, tmp1);
    vs2 = tmp_s;
    tmp_s = _mm256_add_pd(tmp_t2, tmp_t3);
    tmp_bb = _mm256_sub_pd(tmp_s, tmp_t2);
    tmp0 = _mm256_sub_pd(tmp_s, tmp_bb);
    tmp1 = _mm256_sub_pd(tmp_t3, tmp_bb);
    tmp0 = _mm256_sub_pd(tmp_t2, tmp0);
    vt1 = _mm256_add_pd(tmp0, tmp1);
    vt0 = tmp_s;

    //three_sum2_vec(&vs3, &vt0, &vt2);
    tmp_s = _mm256_add_pd(vs3, vt0);
    tmp_bb = _mm256_sub_pd(tmp_s, vs3);
    tmp0 = _mm256_sub_pd(tmp_s, tmp_bb);
    tmp1 = _mm256_sub_pd(vt0, tmp_bb);
    tmp0 = _mm256_sub_pd(vs3, tmp0);
    tmp_t2 = _mm256_add_pd(tmp0, tmp1);
    tmp_t1 = tmp_s;
    tmp_s = _mm256_add_pd(vt2, tmp_t1);
    tmp_bb = _mm256_sub_pd(tmp_s, vt2);
    tmp0 = _mm256_sub_pd(tmp_s, tmp_bb);
    tmp1 = _mm256_sub_pd(tmp_t1, tmp_bb);
    tmp0 = _mm256_sub_pd(vt2, tmp0);
    tmp_t3 = _mm256_add_pd(tmp0, tmp1);
    vs3 = tmp_s;
    vt0 = _mm256_add_pd(tmp_t2, tmp_t3);

    //vt0 = _mm256_add_pd(_mm256_add_pd(vt0, vt1), vt3);
    tmp0 = _mm256_add_pd(vt0, vt1);
    vt0 = _mm256_add_pd(tmp0, vt3);

    //renorm5(&s0, &s1, &s2, &s3, &t0);
    _mm256_store_pd(lo.d0 + i, vs0);
    _mm256_store_pd(lo.d1 + i, vs1);
    _mm256_store_pd(lo.d2 + i, vs2);
    _mm256_store_pd(lo.d3 + i, vs3);
    _mm256_store_pd(tmp_vt0, vt0);
    renorm5_inline(lo.d0 + i, lo.d1 + i, lo.d2 + i, lo.d3 + i, tmp_vt0);
    renorm5_inline(lo.d0 + i + 1, lo.d1 + i + 1, lo.d2 + i + 1, lo.d3 + i + 1, tmp_vt0 + 1);
    renorm5_inline(lo.d0 + i + 2, lo.d1 + i + 2, lo.d2 + i + 2, lo.d3 + i + 2, tmp_vt0 + 2);
    renorm5_inline(lo.d0 + i + 3, lo.d1 + i + 3, lo.d2 + i + 3, lo.d3 + i + 3, tmp_vt0 + 3);
  }

  for (; i < size; i++) {
      
    s0 = two_sum(lo.d0[i], ro.d0[i], &t0);
    s1 = two_sum(lo.d1[i], ro.d1[i], &t1);
    s2 = two_sum(lo.d2[i], ro.d2[i], &t2);
    s3 = two_sum(lo.d3[i], ro.d3[i], &t3);

    s1 = two_sum(s1, t0, &t0);
    three_sum(&s2, &t0, &t1);
    three_sum2(&s3, &t0, &t2);
    t0 = t0 + t1 + t3;

    renorm5_inline(&s0, &s1, &s2, &s3, &t0);
    lo.d0[i] = s0;
    lo.d1[i] = s1;
    lo.d2[i] = s2;
    lo.d3[i] = s3;
  }
  _mm_free(tmp_vt0);
}

void qd_arr_add_inplace_vec_inline_reorder(qd_arr lo, qd_arr ro) {
  //assert(lo.size == ro.size);
  int size = lo.size;

  double s0, s1, s2, s3;
  double t0, t1, t2, t3;
  __m256d vs0, vs1, vs2, vs3;
  __m256d vt0, vt1, vt2, vt3;
  __m256d ld0, ld1, ld2, ld3;
  __m256d rd0, rd1, rd2, rd3;
  __m256d tmp_s, tmp_t1, tmp_t2, tmp_t3, tmp_bb, tmp0, tmp1;
  __m256d tmp_s_0, tmp_bb_0, tmp0_0, tmp1_0;
  __m256d tmp_s_1, tmp_bb_1, tmp0_1, tmp1_1;
  __m256d tmp_s_2, tmp_bb_2, tmp0_2, tmp1_2;
  __m256d tmp_s_3, tmp_bb_3, tmp0_3, tmp1_3;
  double *tmp_vt0 = (double *)_mm_malloc(4 * sizeof(double), ALIGNMENT);
  int i;
  for (i = 0; i + 3 < size; i += 4) {
    ld0 = _mm256_load_pd(lo.d0 + i);
    ld1 = _mm256_load_pd(lo.d1 + i);
    ld2 = _mm256_load_pd(lo.d2 + i);
    ld3 = _mm256_load_pd(lo.d3 + i);
    rd0 = _mm256_load_pd(ro.d0 + i);
    rd1 = _mm256_load_pd(ro.d1 + i);
    rd2 = _mm256_load_pd(ro.d2 + i);
    rd3 = _mm256_load_pd(ro.d3 + i);

    //vs0 = two_sum_vec(ld0, rd0, &vt0);
    //vs1 = two_sum_vec(ld1, rd1, &vt1);
    //vs2 = two_sum_vec(ld2, rd2, &vt2);
    //vs3 = two_sum_vec(ld3, rd3, &vt3);
    tmp_s_0 = _mm256_add_pd(ld0, rd0);
    tmp_s_1 = _mm256_add_pd(ld1, rd1);
    tmp_s_2 = _mm256_add_pd(ld2, rd2);
    tmp_s_3 = _mm256_add_pd(ld3, rd3);
    tmp_bb_0 = _mm256_sub_pd(tmp_s_0, ld0);
    tmp_bb_1 = _mm256_sub_pd(tmp_s_1, ld1);
    tmp_bb_2 = _mm256_sub_pd(tmp_s_2, ld2);
    tmp_bb_3 = _mm256_sub_pd(tmp_s_3, ld3);
    tmp0_0 = _mm256_sub_pd(tmp_s_0, tmp_bb_0);
    tmp0_1 = _mm256_sub_pd(tmp_s_1, tmp_bb_1);
    tmp0_2 = _mm256_sub_pd(tmp_s_2, tmp_bb_2);
    tmp0_3 = _mm256_sub_pd(tmp_s_3, tmp_bb_3);
    tmp1_0 = _mm256_sub_pd(rd0, tmp_bb_0);
    tmp1_1 = _mm256_sub_pd(rd1, tmp_bb_1);
    tmp1_2 = _mm256_sub_pd(rd2, tmp_bb_2);
    tmp1_3 = _mm256_sub_pd(rd3, tmp_bb_3);
    tmp0_0 = _mm256_sub_pd(ld0, tmp0_0);
    tmp0_1 = _mm256_sub_pd(ld1, tmp0_1);
    tmp0_2 = _mm256_sub_pd(ld2, tmp0_2);
    tmp0_3 = _mm256_sub_pd(ld3, tmp0_3);
    vt0 = _mm256_add_pd(tmp0_0, tmp1_0);
    vt1 = _mm256_add_pd(tmp0_1, tmp1_1);
    vt2 = _mm256_add_pd(tmp0_2, tmp1_2);
    vt3 = _mm256_add_pd(tmp0_3, tmp1_3);
    vs0 = tmp_s_0;
    vs1 = tmp_s_1;
    vs2 = tmp_s_2;
    vs3 = tmp_s_3;

    //vs1 = two_sum_vec(vs1, vt0, &vt0);
    tmp_s = _mm256_add_pd(vs1, vt0);
    tmp_bb = _mm256_sub_pd(tmp_s, vs1);
    tmp0 = _mm256_sub_pd(tmp_s, tmp_bb);
    tmp1 = _mm256_sub_pd(vt0, tmp_bb);
    tmp0 = _mm256_sub_pd(vs1, tmp0);
    vt0 = _mm256_add_pd(tmp0, tmp1);
    vs1 = tmp_s;

    //three_sum_vec(&vs2, &vt0, &vt1);
    tmp_s = _mm256_add_pd(vs2, vt0);
    tmp_bb = _mm256_sub_pd(tmp_s, vs2);
    tmp0 = _mm256_sub_pd(tmp_s, tmp_bb);
    tmp1 = _mm256_sub_pd(vt0, tmp_bb);
    tmp0 = _mm256_sub_pd(vs2, tmp0);
    tmp_t2 = _mm256_add_pd(tmp0, tmp1);
    tmp_t1 = tmp_s;
    tmp_s = _mm256_add_pd(vt1, tmp_t1);
    tmp_bb = _mm256_sub_pd(tmp_s, vt1);
    tmp0 = _mm256_sub_pd(tmp_s, tmp_bb);
    tmp1 = _mm256_sub_pd(tmp_t1, tmp_bb);
    tmp0 = _mm256_sub_pd(vt1, tmp0);
    tmp_t3 = _mm256_add_pd(tmp0, tmp1);
    vs2 = tmp_s;
    tmp_s = _mm256_add_pd(tmp_t2, tmp_t3);
    tmp_bb = _mm256_sub_pd(tmp_s, tmp_t2);
    tmp0 = _mm256_sub_pd(tmp_s, tmp_bb);
    tmp1 = _mm256_sub_pd(tmp_t3, tmp_bb);
    tmp0 = _mm256_sub_pd(tmp_t2, tmp0);
    vt1 = _mm256_add_pd(tmp0, tmp1);
    vt0 = tmp_s;

    //three_sum2_vec(&vs3, &vt0, &vt2);
    tmp_s = _mm256_add_pd(vs3, vt0);
    tmp_bb = _mm256_sub_pd(tmp_s, vs3);
    tmp0 = _mm256_sub_pd(tmp_s, tmp_bb);
    tmp1 = _mm256_sub_pd(vt0, tmp_bb);
    tmp0 = _mm256_sub_pd(vs3, tmp0);
    tmp_t2 = _mm256_add_pd(tmp0, tmp1);
    tmp_t1 = tmp_s;
    tmp_s = _mm256_add_pd(vt2, tmp_t1);
    tmp_bb = _mm256_sub_pd(tmp_s, vt2);
    tmp0 = _mm256_sub_pd(tmp_s, tmp_bb);
    tmp1 = _mm256_sub_pd(tmp_t1, tmp_bb);
    tmp0 = _mm256_sub_pd(vt2, tmp0);
    tmp_t3 = _mm256_add_pd(tmp0, tmp1);
    vs3 = tmp_s;
    vt0 = _mm256_add_pd(tmp_t2, tmp_t3);

    //vt0 = _mm256_add_pd(_mm256_add_pd(vt0, vt1), vt3);
    tmp0 = _mm256_add_pd(vt0, vt1);
    vt0 = _mm256_add_pd(tmp0, vt3);

    //renorm5(&s0, &s1, &s2, &s3, &t0);
    _mm256_store_pd(lo.d0 + i, vs0);
    _mm256_store_pd(lo.d1 + i, vs1);
    _mm256_store_pd(lo.d2 + i, vs2);
    _mm256_store_pd(lo.d3 + i, vs3);
    _mm256_store_pd(tmp_vt0, vt0);
    renorm5_inline(lo.d0 + i, lo.d1 + i, lo.d2 + i, lo.d3 + i, tmp_vt0);
    renorm5_inline(lo.d0 + i + 1, lo.d1 + i + 1, lo.d2 + i + 1, lo.d3 + i + 1, tmp_vt0 + 1);
    renorm5_inline(lo.d0 + i + 2, lo.d1 + i + 2, lo.d2 + i + 2, lo.d3 + i + 2, tmp_vt0 + 2);
    renorm5_inline(lo.d0 + i + 3, lo.d1 + i + 3, lo.d2 + i + 3, lo.d3 + i + 3, tmp_vt0 + 3);
  }

  for (; i < size; i++) {
      
    s0 = two_sum(lo.d0[i], ro.d0[i], &t0);
    s1 = two_sum(lo.d1[i], ro.d1[i], &t1);
    s2 = two_sum(lo.d2[i], ro.d2[i], &t2);
    s3 = two_sum(lo.d3[i], ro.d3[i], &t3);

    s1 = two_sum(s1, t0, &t0);
    three_sum(&s2, &t0, &t1);
    three_sum2(&s3, &t0, &t2);
    t0 = t0 + t1 + t3;

    renorm5_inline(&s0, &s1, &s2, &s3, &t0);
    lo.d0[i] = s0;
    lo.d1[i] = s1;
    lo.d2[i] = s2;
    lo.d3[i] = s3;
  }
  _mm_free(tmp_vt0);
}

void qd_arr_add_inplace_vec_inline_x2(qd_arr lo, qd_arr ro) {
  //assert(lo.size == ro.size);
  int size = lo.size;

  double s0, s1, s2, s3;
  double t0, t1, t2, t3;
  __m256d vs0, vs1, vs2, vs3;
  __m256d vs0_x0, vs1_x0, vs2_x0, vs3_x0;
  __m256d vs0_x1, vs1_x1, vs2_x1, vs3_x1;

  __m256d vt0, vt1, vt2, vt3;
  __m256d vt0_x0, vt1_x0, vt2_x0, vt3_x0;
  __m256d vt0_x1, vt1_x1, vt2_x1, vt3_x1;

  __m256d ld0, ld1, ld2, ld3;
  __m256d ld0_x0, ld1_x0, ld2_x0, ld3_x0;
  __m256d ld0_x1, ld1_x1, ld2_x1, ld3_x1;

  __m256d rd0, rd1, rd2, rd3;
  __m256d rd0_x0, rd1_x0, rd2_x0, rd3_x0;
  __m256d rd0_x1, rd1_x1, rd2_x1, rd3_x1;

  __m256d tmp_s, tmp_t1, tmp_t2, tmp_t3, tmp_bb, tmp0, tmp1;
  __m256d tmp_s_x0, tmp_t1_x0, tmp_t2_x0, tmp_t3_x0, tmp_bb_x0, tmp0_x0, tmp1_x0;
  __m256d tmp_s_x1, tmp_t1_x1, tmp_t2_x1, tmp_t3_x1, tmp_bb_x1, tmp0_x1, tmp1_x1;
  double *tmp_vt0 = (double *)_mm_malloc(4 * sizeof(double), ALIGNMENT);
  double *tmp_vt0_x0 = (double *)_mm_malloc(4 * sizeof(double), ALIGNMENT);
  double *tmp_vt0_x1 = (double *)_mm_malloc(4 * sizeof(double), ALIGNMENT);
  int i;
  for (i = 0; i + 7 < size; i += 8) {
    ld0_x0 = _mm256_load_pd(lo.d0 + i);
    ld0_x1 = _mm256_load_pd(lo.d0 + i + 4);
    ld1_x0 = _mm256_load_pd(lo.d1 + i);
    ld1_x1 = _mm256_load_pd(lo.d1 + i + 4);
    ld2_x0 = _mm256_load_pd(lo.d2 + i);
    ld2_x1 = _mm256_load_pd(lo.d2 + i + 4);
    ld3_x0 = _mm256_load_pd(lo.d3 + i);
    ld3_x1 = _mm256_load_pd(lo.d3 + i + 4);
    
    rd0_x0 = _mm256_load_pd(ro.d0 + i);
    rd0_x1 = _mm256_load_pd(ro.d0 + i + 4);
    rd1_x0 = _mm256_load_pd(ro.d1 + i);
    rd1_x1 = _mm256_load_pd(ro.d1 + i + 4);
    rd2_x0 = _mm256_load_pd(ro.d2 + i);
    rd2_x1 = _mm256_load_pd(ro.d2 + i + 4);
    rd3_x0 = _mm256_load_pd(ro.d3 + i);
    rd3_x1 = _mm256_load_pd(ro.d3 + i + 4);

    //vs0 = two_sum_vec(ld0, rd0, &vt0);
    tmp_s_x0 = _mm256_add_pd(ld0_x0, rd0_x0);
    tmp_s_x1 = _mm256_add_pd(ld0_x1, rd0_x1);

    tmp_bb_x0 = _mm256_sub_pd(tmp_s_x0, ld0_x0);
    tmp_bb_x1 = _mm256_sub_pd(tmp_s_x1, ld0_x1);

    tmp0_x0 = _mm256_sub_pd(tmp_s_x0, tmp_bb_x0);
    tmp0_x1 = _mm256_sub_pd(tmp_s_x1, tmp_bb_x1);
    
    tmp1_x0 = _mm256_sub_pd(rd0_x0, tmp_bb_x0);
    tmp1_x1 = _mm256_sub_pd(rd0_x1, tmp_bb_x1);
    
    tmp0_x0 = _mm256_sub_pd(ld0_x0, tmp0_x0);
    tmp0_x1 = _mm256_sub_pd(ld0_x1, tmp0_x1);

    vt0_x0 = _mm256_add_pd(tmp0_x0, tmp1_x0);
    vt0_x1 = _mm256_add_pd(tmp0_x1, tmp1_x1);

    vs0_x0 = tmp_s_x0;
    vs0_x1 = tmp_s_x1;

    //vs1 = two_sum_vec(ld1, rd1, &vt1);
    tmp_s_x0 = _mm256_add_pd(ld1_x0, rd1_x0);
    tmp_s_x1 = _mm256_add_pd(ld1_x1, rd1_x1);

    tmp_bb_x0 = _mm256_sub_pd(tmp_s_x0, ld1_x0);
    tmp_bb_x1 = _mm256_sub_pd(tmp_s_x1, ld1_x1);

    tmp0_x0 = _mm256_sub_pd(tmp_s_x0, tmp_bb_x0);
    tmp0_x1 = _mm256_sub_pd(tmp_s_x1, tmp_bb_x1);
    
    tmp1_x0 = _mm256_sub_pd(rd1_x0, tmp_bb_x0);
    tmp1_x1 = _mm256_sub_pd(rd1_x1, tmp_bb_x1);
    
    tmp0_x0 = _mm256_sub_pd(ld1_x0, tmp0_x0);
    tmp0_x1 = _mm256_sub_pd(ld1_x1, tmp0_x1);

    vt1_x0 = _mm256_add_pd(tmp0_x0, tmp1_x0);
    vt1_x1 = _mm256_add_pd(tmp0_x1, tmp1_x1);

    vs1_x0 = tmp_s_x0;
    vs1_x1 = tmp_s_x1;

    //vs2 = two_sum_vec(ld2, rd2, &vt2);
    tmp_s_x0 = _mm256_add_pd(ld2_x0, rd2_x0);
    tmp_s_x1 = _mm256_add_pd(ld2_x1, rd2_x1);

    tmp_bb_x0 = _mm256_sub_pd(tmp_s_x0, ld2_x0);
    tmp_bb_x1 = _mm256_sub_pd(tmp_s_x1, ld2_x1);

    tmp0_x0 = _mm256_sub_pd(tmp_s_x0, tmp_bb_x0);
    tmp0_x1 = _mm256_sub_pd(tmp_s_x1, tmp_bb_x1);
    
    tmp1_x0 = _mm256_sub_pd(rd2_x0, tmp_bb_x0);
    tmp1_x1 = _mm256_sub_pd(rd2_x1, tmp_bb_x1);
    
    tmp0_x0 = _mm256_sub_pd(ld2_x0, tmp0_x0);
    tmp0_x1 = _mm256_sub_pd(ld2_x1, tmp0_x1);

    vt2_x0 = _mm256_add_pd(tmp0_x0, tmp1_x0);
    vt2_x1 = _mm256_add_pd(tmp0_x1, tmp1_x1);

    vs2_x0 = tmp_s_x0;
    vs2_x1 = tmp_s_x1;

    //vs3 = two_sum_vec(ld3, rd3, &vt3);
    tmp_s_x0 = _mm256_add_pd(ld3_x0, rd3_x0);
    tmp_s_x1 = _mm256_add_pd(ld3_x1, rd3_x1);

    tmp_bb_x0 = _mm256_sub_pd(tmp_s_x0, ld3_x0);
    tmp_bb_x1 = _mm256_sub_pd(tmp_s_x1, ld3_x1);

    tmp0_x0 = _mm256_sub_pd(tmp_s_x0, tmp_bb_x0);
    tmp0_x1 = _mm256_sub_pd(tmp_s_x1, tmp_bb_x1);
    
    tmp1_x0 = _mm256_sub_pd(rd3_x0, tmp_bb_x0);
    tmp1_x1 = _mm256_sub_pd(rd3_x1, tmp_bb_x1);
    
    tmp0_x0 = _mm256_sub_pd(ld3_x0, tmp0_x0);
    tmp0_x1 = _mm256_sub_pd(ld3_x1, tmp0_x1);

    vt3_x0 = _mm256_add_pd(tmp0_x0, tmp1_x0);
    vt3_x1 = _mm256_add_pd(tmp0_x1, tmp1_x1);

    vs3_x0 = tmp_s_x0;
    vs3_x1 = tmp_s_x1;

    //vs1 = two_sum_vec(vs1, vt0, &vt0);
    tmp_s_x0 = _mm256_add_pd(vs1_x0, vt0_x0);
    tmp_s_x1 = _mm256_add_pd(vs1_x1, vt0_x1);

    tmp_bb_x0 = _mm256_sub_pd(tmp_s_x0, vs1_x0);
    tmp_bb_x1 = _mm256_sub_pd(tmp_s_x1, vs1_x1);

    tmp0_x0 = _mm256_sub_pd(tmp_s_x0, tmp_bb_x0);
    tmp0_x1 = _mm256_sub_pd(tmp_s_x1, tmp_bb_x1);
    
    tmp1_x0 = _mm256_sub_pd(vt0_x0, tmp_bb_x0);
    tmp1_x1 = _mm256_sub_pd(vt0_x1, tmp_bb_x1);
    
    tmp0_x0 = _mm256_sub_pd(vs1_x0, tmp0_x0);
    tmp0_x1 = _mm256_sub_pd(vs1_x1, tmp0_x1);

    vt0_x0 = _mm256_add_pd(tmp0_x0, tmp1_x0);
    vt0_x1 = _mm256_add_pd(tmp0_x1, tmp1_x1);

    vs1_x0 = tmp_s_x0;
    vs1_x1 = tmp_s_x1;

    //three_sum_vec(&vs2, &vt0, &vt1);

    /* tmp_s = _mm256_add_pd(vs2, vt0);
    tmp_bb = _mm256_sub_pd(tmp_s, vs2);
    tmp0 = _mm256_sub_pd(tmp_s, tmp_bb);
    tmp1 = _mm256_sub_pd(vt0, tmp_bb);
    tmp0 = _mm256_sub_pd(vs2, tmp0);
    tmp_t2 = _mm256_add_pd(tmp0, tmp1);
    tmp_t1 = tmp_s; */
    tmp_s_x0 = _mm256_add_pd(vs2_x0, vt0_x0);
    tmp_s_x1 = _mm256_add_pd(vs2_x1, vt0_x1);

    tmp_bb_x0 = _mm256_sub_pd(tmp_s_x0, vs2_x0);
    tmp_bb_x1 = _mm256_sub_pd(tmp_s_x1, vs2_x1);

    tmp0_x0 = _mm256_sub_pd(tmp_s_x0, tmp_bb_x0);
    tmp0_x1 = _mm256_sub_pd(tmp_s_x1, tmp_bb_x1);
    
    tmp1_x0 = _mm256_sub_pd(vt0_x0, tmp_bb_x0);
    tmp1_x1 = _mm256_sub_pd(vt0_x1, tmp_bb_x1);
    
    tmp0_x0 = _mm256_sub_pd(vs2_x0, tmp0_x0);
    tmp0_x1 = _mm256_sub_pd(vs2_x1, tmp0_x1);

    tmp_t2_x0 = _mm256_add_pd(tmp0_x0, tmp1_x0);
    tmp_t2_x1 = _mm256_add_pd(tmp0_x1, tmp1_x1);

    tmp_t1_x0 = tmp_s_x0;
    tmp_t1_x1 = tmp_s_x1;

    /* tmp_s = _mm256_add_pd(vt1, tmp_t1);
    tmp_bb = _mm256_sub_pd(tmp_s, vt1);
    tmp0 = _mm256_sub_pd(tmp_s, tmp_bb);
    tmp1 = _mm256_sub_pd(tmp_t1, tmp_bb);
    tmp0 = _mm256_sub_pd(vt1, tmp0);
    tmp_t3 = _mm256_add_pd(tmp0, tmp1);
    vs2 = tmp_s; */

    tmp_s_x0 = _mm256_add_pd(vt1_x0, tmp_t1_x0);
    tmp_s_x1 = _mm256_add_pd(vt1_x1, tmp_t1_x1);

    tmp_bb_x0 = _mm256_sub_pd(tmp_s_x0, vt1_x0);
    tmp_bb_x1 = _mm256_sub_pd(tmp_s_x1, vt1_x1);

    tmp0_x0 = _mm256_sub_pd(tmp_s_x0, tmp_bb_x0);
    tmp0_x1 = _mm256_sub_pd(tmp_s_x1, tmp_bb_x1);
    
    tmp1_x0 = _mm256_sub_pd(tmp_t1_x0, tmp_bb_x0);
    tmp1_x1 = _mm256_sub_pd(tmp_t1_x1, tmp_bb_x1);
    
    tmp0_x0 = _mm256_sub_pd(vt1_x0, tmp0_x0);
    tmp0_x1 = _mm256_sub_pd(vt1_x1, tmp0_x1);

    tmp_t3_x0 = _mm256_add_pd(tmp0_x0, tmp1_x0);
    tmp_t3_x1 = _mm256_add_pd(tmp0_x1, tmp1_x1);

    vs2_x0 = tmp_s_x0;
    vs2_x1 = tmp_s_x1;


    /* tmp_s = _mm256_add_pd(tmp_t2, tmp_t3);
    tmp_bb = _mm256_sub_pd(tmp_s, tmp_t2);
    tmp0 = _mm256_sub_pd(tmp_s, tmp_bb);
    tmp1 = _mm256_sub_pd(tmp_t3, tmp_bb);
    tmp0 = _mm256_sub_pd(tmp_t2, tmp0);
    vt1 = _mm256_add_pd(tmp0, tmp1);
    vt0 = tmp_s; */

    tmp_s_x0 = _mm256_add_pd(tmp_t2_x0, tmp_t3_x0);
    tmp_s_x1 = _mm256_add_pd(tmp_t2_x1, tmp_t3_x1);

    tmp_bb_x0 = _mm256_sub_pd(tmp_s_x0, tmp_t2_x0);
    tmp_bb_x1 = _mm256_sub_pd(tmp_s_x1, tmp_t2_x1);

    tmp0_x0 = _mm256_sub_pd(tmp_s_x0, tmp_bb_x0);
    tmp0_x1 = _mm256_sub_pd(tmp_s_x1, tmp_bb_x1);
    
    tmp1_x0 = _mm256_sub_pd(tmp_t3_x0, tmp_bb_x0);
    tmp1_x1 = _mm256_sub_pd(tmp_t3_x1, tmp_bb_x1);
    
    tmp0_x0 = _mm256_sub_pd(tmp_t2_x0, tmp0_x0);
    tmp0_x1 = _mm256_sub_pd(tmp_t2_x1, tmp0_x1);

    vt1_x0 = _mm256_add_pd(tmp0_x0, tmp1_x0);
    vt1_x1 = _mm256_add_pd(tmp0_x1, tmp1_x1);

    vt0_x0 = tmp_s_x0;
    vt0_x1 = tmp_s_x1;



    //three_sum2_vec(&vs3, &vt0, &vt2);

    /* tmp_s = _mm256_add_pd(vs3, vt0);
    tmp_bb = _mm256_sub_pd(tmp_s, vs3);
    tmp0 = _mm256_sub_pd(tmp_s, tmp_bb);
    tmp1 = _mm256_sub_pd(vt0, tmp_bb);
    tmp0 = _mm256_sub_pd(vs3, tmp0);
    tmp_t2 = _mm256_add_pd(tmp0, tmp1);
    tmp_t1 = tmp_s; */
    tmp_s_x0 = _mm256_add_pd(vs3_x0, vt0_x0);
    tmp_s_x1 = _mm256_add_pd(vs3_x1, vt0_x1);

    tmp_bb_x0 = _mm256_sub_pd(tmp_s_x0, vs3_x0);
    tmp_bb_x1 = _mm256_sub_pd(tmp_s_x1, vs3_x1);

    tmp0_x0 = _mm256_sub_pd(tmp_s_x0, tmp_bb_x0);
    tmp0_x1 = _mm256_sub_pd(tmp_s_x1, tmp_bb_x1);
    
    tmp1_x0 = _mm256_sub_pd(vt0_x0, tmp_bb_x0);
    tmp1_x1 = _mm256_sub_pd(vt0_x1, tmp_bb_x1);
    
    tmp0_x0 = _mm256_sub_pd(vs3_x0, tmp0_x0);
    tmp0_x1 = _mm256_sub_pd(vs3_x1, tmp0_x1);

    tmp_t2_x0 = _mm256_add_pd(tmp0_x0, tmp1_x0);
    tmp_t2_x1 = _mm256_add_pd(tmp0_x1, tmp1_x1);

    tmp_t1_x0 = tmp_s_x0;
    tmp_t1_x1 = tmp_s_x1;
    /* tmp_s = _mm256_add_pd(vt2, tmp_t1);
    tmp_bb = _mm256_sub_pd(tmp_s, vt2);
    tmp0 = _mm256_sub_pd(tmp_s, tmp_bb);
    tmp1 = _mm256_sub_pd(tmp_t1, tmp_bb);
    tmp0 = _mm256_sub_pd(vt2, tmp0);
    tmp_t3 = _mm256_add_pd(tmp0, tmp1);
    vs3 = tmp_s; */
    tmp_s_x0 = _mm256_add_pd(vt2_x0, tmp_t1_x0);
    tmp_s_x1 = _mm256_add_pd(vt2_x1, tmp_t1_x1);

    tmp_bb_x0 = _mm256_sub_pd(tmp_s_x0, vt2_x0);
    tmp_bb_x1 = _mm256_sub_pd(tmp_s_x1, vt2_x1);

    tmp0_x0 = _mm256_sub_pd(tmp_s_x0, tmp_bb_x0);
    tmp0_x1 = _mm256_sub_pd(tmp_s_x1, tmp_bb_x1);
    
    tmp1_x0 = _mm256_sub_pd(tmp_t1_x0, tmp_bb_x0);
    tmp1_x1 = _mm256_sub_pd(tmp_t1_x1, tmp_bb_x1);
    
    tmp0_x0 = _mm256_sub_pd(vt2_x0, tmp0_x0);
    tmp0_x1 = _mm256_sub_pd(vt2_x1, tmp0_x1);

    tmp_t3_x0 = _mm256_add_pd(tmp0_x0, tmp1_x0);
    tmp_t3_x1 = _mm256_add_pd(tmp0_x1, tmp1_x1);

    vs3_x0 = tmp_s_x0;
    vs3_x1 = tmp_s_x1;

    /* vt0 = _mm256_add_pd(tmp_t2, tmp_t3); */
    vt0_x0 = _mm256_add_pd(tmp_t2_x0, tmp_t3_x0);
    vt0_x1 = _mm256_add_pd(tmp_t2_x1, tmp_t3_x1);

    //vt0 = _mm256_add_pd(_mm256_add_pd(vt0, vt1), vt3);
    /* tmp0 = _mm256_add_pd(vt0, vt1); */
    tmp0_x0 = _mm256_add_pd(vt0_x0, vt1_x0);
    tmp0_x1 = _mm256_add_pd(vt0_x1, vt1_x1);

    /* vt0 = _mm256_add_pd(tmp0, vt3); */
    vt0_x0 = _mm256_add_pd(tmp0_x0, vt3_x0);
    vt0_x1 = _mm256_add_pd(tmp0_x1, vt3_x1);

    //renorm5(&s0, &s1, &s2, &s3, &t0);
    /* _mm256_store_pd(lo.d0 + i, vs0);
    _mm256_store_pd(lo.d1 + i, vs1);
    _mm256_store_pd(lo.d2 + i, vs2);
    _mm256_store_pd(lo.d3 + i, vs3);
    _mm256_store_pd(tmp_vt0, vt0); */
    _mm256_store_pd(lo.d0 + i, vs0_x0);
    _mm256_store_pd(lo.d0 + i + 4, vs0_x1);
    _mm256_store_pd(lo.d1 + i, vs1_x0);
    _mm256_store_pd(lo.d1 + i + 4, vs1_x1);
    _mm256_store_pd(lo.d2 + i, vs2_x0);
    _mm256_store_pd(lo.d2 + i + 4, vs2_x1);
    _mm256_store_pd(lo.d3 + i, vs3_x0);
    _mm256_store_pd(lo.d3 + i + 4, vs3_x1);
    _mm256_store_pd(tmp_vt0_x0, vt0_x0);
    _mm256_store_pd(tmp_vt0_x1, vt0_x1);
    /* renorm5_inline(lo.d0 + i, lo.d1 + i, lo.d2 + i, lo.d3 + i, tmp_vt0);
    renorm5_inline(lo.d0 + i + 1, lo.d1 + i + 1, lo.d2 + i + 1, lo.d3 + i + 1, tmp_vt0 + 1);
    renorm5_inline(lo.d0 + i + 2, lo.d1 + i + 2, lo.d2 + i + 2, lo.d3 + i + 2, tmp_vt0 + 2);
    renorm5_inline(lo.d0 + i + 3, lo.d1 + i + 3, lo.d2 + i + 3, lo.d3 + i + 3, tmp_vt0 + 3); */
    renorm5_inline(lo.d0 + i, lo.d1 + i, lo.d2 + i, lo.d3 + i, tmp_vt0_x0);
    renorm5_inline(lo.d0 + i + 1, lo.d1 + i + 1, lo.d2 + i + 1, lo.d3 + i + 1, tmp_vt0_x0 + 1);
    renorm5_inline(lo.d0 + i + 2, lo.d1 + i + 2, lo.d2 + i + 2, lo.d3 + i + 2, tmp_vt0_x0 + 2);
    renorm5_inline(lo.d0 + i + 3, lo.d1 + i + 3, lo.d2 + i + 3, lo.d3 + i + 3, tmp_vt0_x0 + 3);
    renorm5_inline(lo.d0 + i + 4, lo.d1 + i + 4, lo.d2 + i + 4, lo.d3 + i + 4, tmp_vt0_x1);
    renorm5_inline(lo.d0 + i + 5, lo.d1 + i + 5, lo.d2 + i + 5, lo.d3 + i + 5, tmp_vt0_x1 + 1);
    renorm5_inline(lo.d0 + i + 6, lo.d1 + i + 6, lo.d2 + i + 6, lo.d3 + i + 6, tmp_vt0_x1 + 2);
    renorm5_inline(lo.d0 + i + 7, lo.d1 + i + 7, lo.d2 + i + 7, lo.d3 + i + 7, tmp_vt0_x1 + 3);
  }

  for (; i + 3 < size; i += 4) {
    ld0 = _mm256_load_pd(lo.d0 + i);
    ld1 = _mm256_load_pd(lo.d1 + i);
    ld2 = _mm256_load_pd(lo.d2 + i);
    ld3 = _mm256_load_pd(lo.d3 + i);
    rd0 = _mm256_load_pd(ro.d0 + i);
    rd1 = _mm256_load_pd(ro.d1 + i);
    rd2 = _mm256_load_pd(ro.d2 + i);
    rd3 = _mm256_load_pd(ro.d3 + i);

    //vs0 = two_sum_vec(ld0, rd0, &vt0);
    tmp_s = _mm256_add_pd(ld0, rd0);
    tmp_bb = _mm256_sub_pd(tmp_s, ld0);
    tmp0 = _mm256_sub_pd(tmp_s, tmp_bb);
    tmp1 = _mm256_sub_pd(rd0, tmp_bb);
    tmp0 = _mm256_sub_pd(ld0, tmp0);
    vt0 = _mm256_add_pd(tmp0, tmp1);
    vs0 = tmp_s;

    //vs1 = two_sum_vec(ld1, rd1, &vt1);
    tmp_s = _mm256_add_pd(ld1, rd1);
    tmp_bb = _mm256_sub_pd(tmp_s, ld1);
    tmp0 = _mm256_sub_pd(tmp_s, tmp_bb);
    tmp1 = _mm256_sub_pd(rd1, tmp_bb);
    tmp0 = _mm256_sub_pd(ld1, tmp0);
    vt1 = _mm256_add_pd(tmp0, tmp1);
    vs1 = tmp_s;

    //vs2 = two_sum_vec(ld2, rd2, &vt2);
    tmp_s = _mm256_add_pd(ld2, rd2);
    tmp_bb = _mm256_sub_pd(tmp_s, ld2);
    tmp0 = _mm256_sub_pd(tmp_s, tmp_bb);
    tmp1 = _mm256_sub_pd(rd2, tmp_bb);
    tmp0 = _mm256_sub_pd(ld2, tmp0);
    vt2 = _mm256_add_pd(tmp0, tmp1);
    vs2 = tmp_s;

    //vs3 = two_sum_vec(ld3, rd3, &vt3);
    tmp_s = _mm256_add_pd(ld3, rd3);
    tmp_bb = _mm256_sub_pd(tmp_s, ld3);
    tmp0 = _mm256_sub_pd(tmp_s, tmp_bb);
    tmp1 = _mm256_sub_pd(rd3, tmp_bb);
    tmp0 = _mm256_sub_pd(ld3, tmp0);
    vt3 = _mm256_add_pd(tmp0, tmp1);
    vs3 = tmp_s;

    //vs1 = two_sum_vec(vs1, vt0, &vt0);
    tmp_s = _mm256_add_pd(vs1, vt0);
    tmp_bb = _mm256_sub_pd(tmp_s, vs1);
    tmp0 = _mm256_sub_pd(tmp_s, tmp_bb);
    tmp1 = _mm256_sub_pd(vt0, tmp_bb);
    tmp0 = _mm256_sub_pd(vs1, tmp0);
    vt0 = _mm256_add_pd(tmp0, tmp1);
    vs1 = tmp_s;

    //three_sum_vec(&vs2, &vt0, &vt1);
    tmp_s = _mm256_add_pd(vs2, vt0);
    tmp_bb = _mm256_sub_pd(tmp_s, vs2);
    tmp0 = _mm256_sub_pd(tmp_s, tmp_bb);
    tmp1 = _mm256_sub_pd(vt0, tmp_bb);
    tmp0 = _mm256_sub_pd(vs2, tmp0);
    tmp_t2 = _mm256_add_pd(tmp0, tmp1);
    tmp_t1 = tmp_s;
    tmp_s = _mm256_add_pd(vt1, tmp_t1);
    tmp_bb = _mm256_sub_pd(tmp_s, vt1);
    tmp0 = _mm256_sub_pd(tmp_s, tmp_bb);
    tmp1 = _mm256_sub_pd(tmp_t1, tmp_bb);
    tmp0 = _mm256_sub_pd(vt1, tmp0);
    tmp_t3 = _mm256_add_pd(tmp0, tmp1);
    vs2 = tmp_s;
    tmp_s = _mm256_add_pd(tmp_t2, tmp_t3);
    tmp_bb = _mm256_sub_pd(tmp_s, tmp_t2);
    tmp0 = _mm256_sub_pd(tmp_s, tmp_bb);
    tmp1 = _mm256_sub_pd(tmp_t3, tmp_bb);
    tmp0 = _mm256_sub_pd(tmp_t2, tmp0);
    vt1 = _mm256_add_pd(tmp0, tmp1);
    vt0 = tmp_s;

    //three_sum2_vec(&vs3, &vt0, &vt2);
    tmp_s = _mm256_add_pd(vs3, vt0);
    tmp_bb = _mm256_sub_pd(tmp_s, vs3);
    tmp0 = _mm256_sub_pd(tmp_s, tmp_bb);
    tmp1 = _mm256_sub_pd(vt0, tmp_bb);
    tmp0 = _mm256_sub_pd(vs3, tmp0);
    tmp_t2 = _mm256_add_pd(tmp0, tmp1);
    tmp_t1 = tmp_s;
    tmp_s = _mm256_add_pd(vt2, tmp_t1);
    tmp_bb = _mm256_sub_pd(tmp_s, vt2);
    tmp0 = _mm256_sub_pd(tmp_s, tmp_bb);
    tmp1 = _mm256_sub_pd(tmp_t1, tmp_bb);
    tmp0 = _mm256_sub_pd(vt2, tmp0);
    tmp_t3 = _mm256_add_pd(tmp0, tmp1);
    vs3 = tmp_s;
    vt0 = _mm256_add_pd(tmp_t2, tmp_t3);

    //vt0 = _mm256_add_pd(_mm256_add_pd(vt0, vt1), vt3);
    tmp0 = _mm256_add_pd(vt0, vt1);
    vt0 = _mm256_add_pd(tmp0, vt3);

    //renorm5(&s0, &s1, &s2, &s3, &t0);
    _mm256_store_pd(lo.d0 + i, vs0);
    _mm256_store_pd(lo.d1 + i, vs1);
    _mm256_store_pd(lo.d2 + i, vs2);
    _mm256_store_pd(lo.d3 + i, vs3);
    _mm256_store_pd(tmp_vt0, vt0);
    renorm5_inline(lo.d0 + i, lo.d1 + i, lo.d2 + i, lo.d3 + i, tmp_vt0);
    renorm5_inline(lo.d0 + i + 1, lo.d1 + i + 1, lo.d2 + i + 1, lo.d3 + i + 1, tmp_vt0 + 1);
    renorm5_inline(lo.d0 + i + 2, lo.d1 + i + 2, lo.d2 + i + 2, lo.d3 + i + 2, tmp_vt0 + 2);
    renorm5_inline(lo.d0 + i + 3, lo.d1 + i + 3, lo.d2 + i + 3, lo.d3 + i + 3, tmp_vt0 + 3);
  }

  for (; i < size; i++) {
      
    s0 = two_sum(lo.d0[i], ro.d0[i], &t0);
    s1 = two_sum(lo.d1[i], ro.d1[i], &t1);
    s2 = two_sum(lo.d2[i], ro.d2[i], &t2);
    s3 = two_sum(lo.d3[i], ro.d3[i], &t3);

    s1 = two_sum(s1, t0, &t0);
    three_sum(&s2, &t0, &t1);
    three_sum2(&s3, &t0, &t2);
    t0 = t0 + t1 + t3;

    renorm5_inline(&s0, &s1, &s2, &s3, &t0);
    lo.d0[i] = s0;
    lo.d1[i] = s1;
    lo.d2[i] = s2;
    lo.d3[i] = s3;
  }
  _mm_free(tmp_vt0);
  _mm_free(tmp_vt0_x0);
  _mm_free(tmp_vt0_x1);
}

void qd_arr_add_inplace_vec_inline_x3(qd_arr lo, qd_arr ro) {
  //assert(lo.size == ro.size);
  int size = lo.size;

  double s0, s1, s2, s3;
  double t0, t1, t2, t3;
  __m256d vs0, vs1, vs2, vs3;
  __m256d vs0_x0, vs1_x0, vs2_x0, vs3_x0;
  __m256d vs0_x1, vs1_x1, vs2_x1, vs3_x1;
  __m256d vs0_x2, vs1_x2, vs2_x2, vs3_x2;

  __m256d vt0, vt1, vt2, vt3;
  __m256d vt0_x0, vt1_x0, vt2_x0, vt3_x0;
  __m256d vt0_x1, vt1_x1, vt2_x1, vt3_x1;
  __m256d vt0_x2, vt1_x2, vt2_x2, vt3_x2;

  __m256d ld0, ld1, ld2, ld3;
  __m256d ld0_x0, ld1_x0, ld2_x0, ld3_x0;
  __m256d ld0_x1, ld1_x1, ld2_x1, ld3_x1;
  __m256d ld0_x2, ld1_x2, ld2_x2, ld3_x2;

  __m256d rd0, rd1, rd2, rd3;
  __m256d rd0_x0, rd1_x0, rd2_x0, rd3_x0;
  __m256d rd0_x1, rd1_x1, rd2_x1, rd3_x1;
  __m256d rd0_x2, rd1_x2, rd2_x2, rd3_x2;

  __m256d tmp_s, tmp_t1, tmp_t2, tmp_t3, tmp_bb, tmp0, tmp1;
  __m256d tmp_s_x0, tmp_t1_x0, tmp_t2_x0, tmp_t3_x0, tmp_bb_x0, tmp0_x0, tmp1_x0;
  __m256d tmp_s_x1, tmp_t1_x1, tmp_t2_x1, tmp_t3_x1, tmp_bb_x1, tmp0_x1, tmp1_x1;
  __m256d tmp_s_x2, tmp_t1_x2, tmp_t2_x2, tmp_t3_x2, tmp_bb_x2, tmp0_x2, tmp1_x2;
  double *tmp_vt0 = (double *)_mm_malloc(4 * sizeof(double), ALIGNMENT);
  double *tmp_vt0_x0 = (double *)_mm_malloc(4 * sizeof(double), ALIGNMENT);
  double *tmp_vt0_x1 = (double *)_mm_malloc(4 * sizeof(double), ALIGNMENT);
  double *tmp_vt0_x2 = (double *)_mm_malloc(4 * sizeof(double), ALIGNMENT);
  int i;
  for (i = 0; i + 11 < size; i += 12) {
    ld0_x0 = _mm256_load_pd(lo.d0 + i);
    ld0_x1 = _mm256_load_pd(lo.d0 + i + 4);
    ld0_x2 = _mm256_load_pd(lo.d0 + i + 8);
    ld1_x0 = _mm256_load_pd(lo.d1 + i);
    ld1_x1 = _mm256_load_pd(lo.d1 + i + 4);
    ld1_x2 = _mm256_load_pd(lo.d1 + i + 8);
    ld2_x0 = _mm256_load_pd(lo.d2 + i);
    ld2_x1 = _mm256_load_pd(lo.d2 + i + 4);
    ld2_x2 = _mm256_load_pd(lo.d2 + i + 8);
    ld3_x0 = _mm256_load_pd(lo.d3 + i);
    ld3_x1 = _mm256_load_pd(lo.d3 + i + 4);
    ld3_x2 = _mm256_load_pd(lo.d3 + i + 8);
    
    rd0_x0 = _mm256_load_pd(ro.d0 + i);
    rd0_x1 = _mm256_load_pd(ro.d0 + i + 4);
    rd0_x2 = _mm256_load_pd(ro.d0 + i + 8);
    rd1_x0 = _mm256_load_pd(ro.d1 + i);
    rd1_x1 = _mm256_load_pd(ro.d1 + i + 4);
    rd1_x2 = _mm256_load_pd(ro.d1 + i + 8);
    rd2_x0 = _mm256_load_pd(ro.d2 + i);
    rd2_x1 = _mm256_load_pd(ro.d2 + i + 4);
    rd2_x2 = _mm256_load_pd(ro.d2 + i + 8);
    rd3_x0 = _mm256_load_pd(ro.d3 + i);
    rd3_x1 = _mm256_load_pd(ro.d3 + i + 4);
    rd3_x2 = _mm256_load_pd(ro.d3 + i + 8);

    //vs0 = two_sum_vec(ld0, rd0, &vt0);
    tmp_s_x0 = _mm256_add_pd(ld0_x0, rd0_x0);
    tmp_s_x1 = _mm256_add_pd(ld0_x1, rd0_x1);
    tmp_s_x2 = _mm256_add_pd(ld0_x2, rd0_x2);

    tmp_bb_x0 = _mm256_sub_pd(tmp_s_x0, ld0_x0);
    tmp_bb_x1 = _mm256_sub_pd(tmp_s_x1, ld0_x1);
    tmp_bb_x2 = _mm256_sub_pd(tmp_s_x2, ld0_x2);

    tmp0_x0 = _mm256_sub_pd(tmp_s_x0, tmp_bb_x0);
    tmp0_x1 = _mm256_sub_pd(tmp_s_x1, tmp_bb_x1);
    tmp0_x2 = _mm256_sub_pd(tmp_s_x2, tmp_bb_x2);
    
    tmp1_x0 = _mm256_sub_pd(rd0_x0, tmp_bb_x0);
    tmp1_x1 = _mm256_sub_pd(rd0_x1, tmp_bb_x1);
    tmp1_x2 = _mm256_sub_pd(rd0_x2, tmp_bb_x2);
    
    tmp0_x0 = _mm256_sub_pd(ld0_x0, tmp0_x0);
    tmp0_x1 = _mm256_sub_pd(ld0_x1, tmp0_x1);
    tmp0_x2 = _mm256_sub_pd(ld0_x2, tmp0_x2);

    vt0_x0 = _mm256_add_pd(tmp0_x0, tmp1_x0);
    vt0_x1 = _mm256_add_pd(tmp0_x1, tmp1_x1);
    vt0_x2 = _mm256_add_pd(tmp0_x2, tmp1_x2);

    vs0_x0 = tmp_s_x0;
    vs0_x1 = tmp_s_x1;
    vs0_x2 = tmp_s_x2;

    //vs1 = two_sum_vec(ld1, rd1, &vt1);
    tmp_s_x0 = _mm256_add_pd(ld1_x0, rd1_x0);
    tmp_s_x1 = _mm256_add_pd(ld1_x1, rd1_x1);
    tmp_s_x2 = _mm256_add_pd(ld1_x2, rd1_x2);

    tmp_bb_x0 = _mm256_sub_pd(tmp_s_x0, ld1_x0);
    tmp_bb_x1 = _mm256_sub_pd(tmp_s_x1, ld1_x1);
    tmp_bb_x2 = _mm256_sub_pd(tmp_s_x2, ld1_x2);

    tmp0_x0 = _mm256_sub_pd(tmp_s_x0, tmp_bb_x0);
    tmp0_x1 = _mm256_sub_pd(tmp_s_x1, tmp_bb_x1);
    tmp0_x2 = _mm256_sub_pd(tmp_s_x2, tmp_bb_x2);
    
    tmp1_x0 = _mm256_sub_pd(rd1_x0, tmp_bb_x0);
    tmp1_x1 = _mm256_sub_pd(rd1_x1, tmp_bb_x1);
    tmp1_x2 = _mm256_sub_pd(rd1_x2, tmp_bb_x2);
    
    tmp0_x0 = _mm256_sub_pd(ld1_x0, tmp0_x0);
    tmp0_x1 = _mm256_sub_pd(ld1_x1, tmp0_x1);
    tmp0_x2 = _mm256_sub_pd(ld1_x2, tmp0_x2);

    vt1_x0 = _mm256_add_pd(tmp0_x0, tmp1_x0);
    vt1_x1 = _mm256_add_pd(tmp0_x1, tmp1_x1);
    vt1_x2 = _mm256_add_pd(tmp0_x2, tmp1_x2);

    vs1_x0 = tmp_s_x0;
    vs1_x1 = tmp_s_x1;
    vs1_x2 = tmp_s_x2;

    //vs2 = two_sum_vec(ld2, rd2, &vt2);
    tmp_s_x0 = _mm256_add_pd(ld2_x0, rd2_x0);
    tmp_s_x1 = _mm256_add_pd(ld2_x1, rd2_x1);
    tmp_s_x2 = _mm256_add_pd(ld2_x2, rd2_x2);

    tmp_bb_x0 = _mm256_sub_pd(tmp_s_x0, ld2_x0);
    tmp_bb_x1 = _mm256_sub_pd(tmp_s_x1, ld2_x1);
    tmp_bb_x2 = _mm256_sub_pd(tmp_s_x2, ld2_x2);

    tmp0_x0 = _mm256_sub_pd(tmp_s_x0, tmp_bb_x0);
    tmp0_x1 = _mm256_sub_pd(tmp_s_x1, tmp_bb_x1);
    tmp0_x2 = _mm256_sub_pd(tmp_s_x2, tmp_bb_x2);
    
    tmp1_x0 = _mm256_sub_pd(rd2_x0, tmp_bb_x0);
    tmp1_x1 = _mm256_sub_pd(rd2_x1, tmp_bb_x1);
    tmp1_x2 = _mm256_sub_pd(rd2_x2, tmp_bb_x2);
    
    tmp0_x0 = _mm256_sub_pd(ld2_x0, tmp0_x0);
    tmp0_x1 = _mm256_sub_pd(ld2_x1, tmp0_x1);
    tmp0_x2 = _mm256_sub_pd(ld2_x2, tmp0_x2);

    vt2_x0 = _mm256_add_pd(tmp0_x0, tmp1_x0);
    vt2_x1 = _mm256_add_pd(tmp0_x1, tmp1_x1);
    vt2_x2 = _mm256_add_pd(tmp0_x2, tmp1_x2);

    vs2_x0 = tmp_s_x0;
    vs2_x1 = tmp_s_x1;
    vs2_x2 = tmp_s_x2;

    //vs3 = two_sum_vec(ld3, rd3, &vt3);
    tmp_s_x0 = _mm256_add_pd(ld3_x0, rd3_x0);
    tmp_s_x1 = _mm256_add_pd(ld3_x1, rd3_x1);
    tmp_s_x2 = _mm256_add_pd(ld3_x2, rd3_x2);

    tmp_bb_x0 = _mm256_sub_pd(tmp_s_x0, ld3_x0);
    tmp_bb_x1 = _mm256_sub_pd(tmp_s_x1, ld3_x1);
    tmp_bb_x2 = _mm256_sub_pd(tmp_s_x2, ld3_x2);

    tmp0_x0 = _mm256_sub_pd(tmp_s_x0, tmp_bb_x0);
    tmp0_x1 = _mm256_sub_pd(tmp_s_x1, tmp_bb_x1);
    tmp0_x2 = _mm256_sub_pd(tmp_s_x2, tmp_bb_x2);
    
    tmp1_x0 = _mm256_sub_pd(rd3_x0, tmp_bb_x0);
    tmp1_x1 = _mm256_sub_pd(rd3_x1, tmp_bb_x1);
    tmp1_x2 = _mm256_sub_pd(rd3_x2, tmp_bb_x2);
    
    tmp0_x0 = _mm256_sub_pd(ld3_x0, tmp0_x0);
    tmp0_x1 = _mm256_sub_pd(ld3_x1, tmp0_x1);
    tmp0_x2 = _mm256_sub_pd(ld3_x2, tmp0_x2);

    vt3_x0 = _mm256_add_pd(tmp0_x0, tmp1_x0);
    vt3_x1 = _mm256_add_pd(tmp0_x1, tmp1_x1);
    vt3_x2 = _mm256_add_pd(tmp0_x2, tmp1_x2);

    vs3_x0 = tmp_s_x0;
    vs3_x1 = tmp_s_x1;
    vs3_x2 = tmp_s_x2;

    //vs1 = two_sum_vec(vs1, vt0, &vt0);
    tmp_s_x0 = _mm256_add_pd(vs1_x0, vt0_x0);
    tmp_s_x1 = _mm256_add_pd(vs1_x1, vt0_x1);
    tmp_s_x2 = _mm256_add_pd(vs1_x2, vt0_x2);

    tmp_bb_x0 = _mm256_sub_pd(tmp_s_x0, vs1_x0);
    tmp_bb_x1 = _mm256_sub_pd(tmp_s_x1, vs1_x1);
    tmp_bb_x2 = _mm256_sub_pd(tmp_s_x2, vs1_x2);

    tmp0_x0 = _mm256_sub_pd(tmp_s_x0, tmp_bb_x0);
    tmp0_x1 = _mm256_sub_pd(tmp_s_x1, tmp_bb_x1);
    tmp0_x2 = _mm256_sub_pd(tmp_s_x2, tmp_bb_x2);
    
    tmp1_x0 = _mm256_sub_pd(vt0_x0, tmp_bb_x0);
    tmp1_x1 = _mm256_sub_pd(vt0_x1, tmp_bb_x1);
    tmp1_x2 = _mm256_sub_pd(vt0_x2, tmp_bb_x2);
    
    tmp0_x0 = _mm256_sub_pd(vs1_x0, tmp0_x0);
    tmp0_x1 = _mm256_sub_pd(vs1_x1, tmp0_x1);
    tmp0_x2 = _mm256_sub_pd(vs1_x2, tmp0_x2);

    vt0_x0 = _mm256_add_pd(tmp0_x0, tmp1_x0);
    vt0_x1 = _mm256_add_pd(tmp0_x1, tmp1_x1);
    vt0_x2 = _mm256_add_pd(tmp0_x2, tmp1_x2);

    vs1_x0 = tmp_s_x0;
    vs1_x1 = tmp_s_x1;
    vs1_x2 = tmp_s_x2;

    //three_sum_vec(&vs2, &vt0, &vt1);

    /* tmp_s = _mm256_add_pd(vs2, vt0);
    tmp_bb = _mm256_sub_pd(tmp_s, vs2);
    tmp0 = _mm256_sub_pd(tmp_s, tmp_bb);
    tmp1 = _mm256_sub_pd(vt0, tmp_bb);
    tmp0 = _mm256_sub_pd(vs2, tmp0);
    tmp_t2 = _mm256_add_pd(tmp0, tmp1);
    tmp_t1 = tmp_s; */
    tmp_s_x0 = _mm256_add_pd(vs2_x0, vt0_x0);
    tmp_s_x1 = _mm256_add_pd(vs2_x1, vt0_x1);
    tmp_s_x2 = _mm256_add_pd(vs2_x2, vt0_x2);

    tmp_bb_x0 = _mm256_sub_pd(tmp_s_x0, vs2_x0);
    tmp_bb_x1 = _mm256_sub_pd(tmp_s_x1, vs2_x1);
    tmp_bb_x2 = _mm256_sub_pd(tmp_s_x2, vs2_x2);

    tmp0_x0 = _mm256_sub_pd(tmp_s_x0, tmp_bb_x0);
    tmp0_x1 = _mm256_sub_pd(tmp_s_x1, tmp_bb_x1);
    tmp0_x2 = _mm256_sub_pd(tmp_s_x2, tmp_bb_x2);
    
    tmp1_x0 = _mm256_sub_pd(vt0_x0, tmp_bb_x0);
    tmp1_x1 = _mm256_sub_pd(vt0_x1, tmp_bb_x1);
    tmp1_x2 = _mm256_sub_pd(vt0_x2, tmp_bb_x2);
    
    tmp0_x0 = _mm256_sub_pd(vs2_x0, tmp0_x0);
    tmp0_x1 = _mm256_sub_pd(vs2_x1, tmp0_x1);
    tmp0_x2 = _mm256_sub_pd(vs2_x2, tmp0_x2);

    tmp_t2_x0 = _mm256_add_pd(tmp0_x0, tmp1_x0);
    tmp_t2_x1 = _mm256_add_pd(tmp0_x1, tmp1_x1);
    tmp_t2_x2 = _mm256_add_pd(tmp0_x2, tmp1_x2);

    tmp_t1_x0 = tmp_s_x0;
    tmp_t1_x1 = tmp_s_x1;
    tmp_t1_x2 = tmp_s_x2;

    /* tmp_s = _mm256_add_pd(vt1, tmp_t1);
    tmp_bb = _mm256_sub_pd(tmp_s, vt1);
    tmp0 = _mm256_sub_pd(tmp_s, tmp_bb);
    tmp1 = _mm256_sub_pd(tmp_t1, tmp_bb);
    tmp0 = _mm256_sub_pd(vt1, tmp0);
    tmp_t3 = _mm256_add_pd(tmp0, tmp1);
    vs2 = tmp_s; */

    tmp_s_x0 = _mm256_add_pd(vt1_x0, tmp_t1_x0);
    tmp_s_x1 = _mm256_add_pd(vt1_x1, tmp_t1_x1);
    tmp_s_x2 = _mm256_add_pd(vt1_x2, tmp_t1_x2);

    tmp_bb_x0 = _mm256_sub_pd(tmp_s_x0, vt1_x0);
    tmp_bb_x1 = _mm256_sub_pd(tmp_s_x1, vt1_x1);
    tmp_bb_x2 = _mm256_sub_pd(tmp_s_x2, vt1_x2);

    tmp0_x0 = _mm256_sub_pd(tmp_s_x0, tmp_bb_x0);
    tmp0_x1 = _mm256_sub_pd(tmp_s_x1, tmp_bb_x1);
    tmp0_x2 = _mm256_sub_pd(tmp_s_x2, tmp_bb_x2);
    
    tmp1_x0 = _mm256_sub_pd(tmp_t1_x0, tmp_bb_x0);
    tmp1_x1 = _mm256_sub_pd(tmp_t1_x1, tmp_bb_x1);
    tmp1_x2 = _mm256_sub_pd(tmp_t1_x2, tmp_bb_x2);
    
    tmp0_x0 = _mm256_sub_pd(vt1_x0, tmp0_x0);
    tmp0_x1 = _mm256_sub_pd(vt1_x1, tmp0_x1);
    tmp0_x2 = _mm256_sub_pd(vt1_x2, tmp0_x2);

    tmp_t3_x0 = _mm256_add_pd(tmp0_x0, tmp1_x0);
    tmp_t3_x1 = _mm256_add_pd(tmp0_x1, tmp1_x1);
    tmp_t3_x2 = _mm256_add_pd(tmp0_x2, tmp1_x2);

    vs2_x0 = tmp_s_x0;
    vs2_x1 = tmp_s_x1;
    vs2_x2 = tmp_s_x2;


    /* tmp_s = _mm256_add_pd(tmp_t2, tmp_t3);
    tmp_bb = _mm256_sub_pd(tmp_s, tmp_t2);
    tmp0 = _mm256_sub_pd(tmp_s, tmp_bb);
    tmp1 = _mm256_sub_pd(tmp_t3, tmp_bb);
    tmp0 = _mm256_sub_pd(tmp_t2, tmp0);
    vt1 = _mm256_add_pd(tmp0, tmp1);
    vt0 = tmp_s; */

    tmp_s_x0 = _mm256_add_pd(tmp_t2_x0, tmp_t3_x0);
    tmp_s_x1 = _mm256_add_pd(tmp_t2_x1, tmp_t3_x1);
    tmp_s_x2 = _mm256_add_pd(tmp_t2_x2, tmp_t3_x2);

    tmp_bb_x0 = _mm256_sub_pd(tmp_s_x0, tmp_t2_x0);
    tmp_bb_x1 = _mm256_sub_pd(tmp_s_x1, tmp_t2_x1);
    tmp_bb_x2 = _mm256_sub_pd(tmp_s_x2, tmp_t2_x2);

    tmp0_x0 = _mm256_sub_pd(tmp_s_x0, tmp_bb_x0);
    tmp0_x1 = _mm256_sub_pd(tmp_s_x1, tmp_bb_x1);
    tmp0_x2 = _mm256_sub_pd(tmp_s_x2, tmp_bb_x2);
    
    tmp1_x0 = _mm256_sub_pd(tmp_t3_x0, tmp_bb_x0);
    tmp1_x1 = _mm256_sub_pd(tmp_t3_x1, tmp_bb_x1);
    tmp1_x2 = _mm256_sub_pd(tmp_t3_x2, tmp_bb_x2);
    
    tmp0_x0 = _mm256_sub_pd(tmp_t2_x0, tmp0_x0);
    tmp0_x1 = _mm256_sub_pd(tmp_t2_x1, tmp0_x1);
    tmp0_x2 = _mm256_sub_pd(tmp_t2_x2, tmp0_x2);

    vt1_x0 = _mm256_add_pd(tmp0_x0, tmp1_x0);
    vt1_x1 = _mm256_add_pd(tmp0_x1, tmp1_x1);
    vt1_x2 = _mm256_add_pd(tmp0_x2, tmp1_x2);

    vt0_x0 = tmp_s_x0;
    vt0_x1 = tmp_s_x1;
    vt0_x2 = tmp_s_x2;


    //three_sum2_vec(&vs3, &vt0, &vt2);

    /* tmp_s = _mm256_add_pd(vs3, vt0);
    tmp_bb = _mm256_sub_pd(tmp_s, vs3);
    tmp0 = _mm256_sub_pd(tmp_s, tmp_bb);
    tmp1 = _mm256_sub_pd(vt0, tmp_bb);
    tmp0 = _mm256_sub_pd(vs3, tmp0);
    tmp_t2 = _mm256_add_pd(tmp0, tmp1);
    tmp_t1 = tmp_s; */
    tmp_s_x0 = _mm256_add_pd(vs3_x0, vt0_x0);
    tmp_s_x1 = _mm256_add_pd(vs3_x1, vt0_x1);
    tmp_s_x2 = _mm256_add_pd(vs3_x2, vt0_x2);

    tmp_bb_x0 = _mm256_sub_pd(tmp_s_x0, vs3_x0);
    tmp_bb_x1 = _mm256_sub_pd(tmp_s_x1, vs3_x1);
    tmp_bb_x2 = _mm256_sub_pd(tmp_s_x2, vs3_x2);

    tmp0_x0 = _mm256_sub_pd(tmp_s_x0, tmp_bb_x0);
    tmp0_x1 = _mm256_sub_pd(tmp_s_x1, tmp_bb_x1);
    tmp0_x2 = _mm256_sub_pd(tmp_s_x2, tmp_bb_x2);
    
    tmp1_x0 = _mm256_sub_pd(vt0_x0, tmp_bb_x0);
    tmp1_x1 = _mm256_sub_pd(vt0_x1, tmp_bb_x1);
    tmp1_x2 = _mm256_sub_pd(vt0_x2, tmp_bb_x2);
    
    tmp0_x0 = _mm256_sub_pd(vs3_x0, tmp0_x0);
    tmp0_x1 = _mm256_sub_pd(vs3_x1, tmp0_x1);
    tmp0_x2 = _mm256_sub_pd(vs3_x2, tmp0_x2);

    tmp_t2_x0 = _mm256_add_pd(tmp0_x0, tmp1_x0);
    tmp_t2_x1 = _mm256_add_pd(tmp0_x1, tmp1_x1);
    tmp_t2_x2 = _mm256_add_pd(tmp0_x2, tmp1_x2);

    tmp_t1_x0 = tmp_s_x0;
    tmp_t1_x1 = tmp_s_x1;
    tmp_t1_x2 = tmp_s_x2;
    /* tmp_s = _mm256_add_pd(vt2, tmp_t1);
    tmp_bb = _mm256_sub_pd(tmp_s, vt2);
    tmp0 = _mm256_sub_pd(tmp_s, tmp_bb);
    tmp1 = _mm256_sub_pd(tmp_t1, tmp_bb);
    tmp0 = _mm256_sub_pd(vt2, tmp0);
    tmp_t3 = _mm256_add_pd(tmp0, tmp1);
    vs3 = tmp_s; */
    tmp_s_x0 = _mm256_add_pd(vt2_x0, tmp_t1_x0);
    tmp_s_x1 = _mm256_add_pd(vt2_x1, tmp_t1_x1);
    tmp_s_x2 = _mm256_add_pd(vt2_x2, tmp_t1_x2);

    tmp_bb_x0 = _mm256_sub_pd(tmp_s_x0, vt2_x0);
    tmp_bb_x1 = _mm256_sub_pd(tmp_s_x1, vt2_x1);
    tmp_bb_x2 = _mm256_sub_pd(tmp_s_x2, vt2_x2);

    tmp0_x0 = _mm256_sub_pd(tmp_s_x0, tmp_bb_x0);
    tmp0_x1 = _mm256_sub_pd(tmp_s_x1, tmp_bb_x1);
    tmp0_x2 = _mm256_sub_pd(tmp_s_x2, tmp_bb_x2);
    
    tmp1_x0 = _mm256_sub_pd(tmp_t1_x0, tmp_bb_x0);
    tmp1_x1 = _mm256_sub_pd(tmp_t1_x1, tmp_bb_x1);
    tmp1_x2 = _mm256_sub_pd(tmp_t1_x2, tmp_bb_x2);
    
    tmp0_x0 = _mm256_sub_pd(vt2_x0, tmp0_x0);
    tmp0_x1 = _mm256_sub_pd(vt2_x1, tmp0_x1);
    tmp0_x2 = _mm256_sub_pd(vt2_x2, tmp0_x2);

    tmp_t3_x0 = _mm256_add_pd(tmp0_x0, tmp1_x0);
    tmp_t3_x1 = _mm256_add_pd(tmp0_x1, tmp1_x1);
    tmp_t3_x2 = _mm256_add_pd(tmp0_x2, tmp1_x2);

    vs3_x0 = tmp_s_x0;
    vs3_x1 = tmp_s_x1;
    vs3_x2 = tmp_s_x2;

    /* vt0 = _mm256_add_pd(tmp_t2, tmp_t3); */
    vt0_x0 = _mm256_add_pd(tmp_t2_x0, tmp_t3_x0);
    vt0_x1 = _mm256_add_pd(tmp_t2_x1, tmp_t3_x1);
    vt0_x2 = _mm256_add_pd(tmp_t2_x2, tmp_t3_x2);

    //vt0 = _mm256_add_pd(_mm256_add_pd(vt0, vt1), vt3);
    /* tmp0 = _mm256_add_pd(vt0, vt1); */
    tmp0_x0 = _mm256_add_pd(vt0_x0, vt1_x0);
    tmp0_x1 = _mm256_add_pd(vt0_x1, vt1_x1);
    tmp0_x2 = _mm256_add_pd(vt0_x2, vt1_x2);

    /* vt0 = _mm256_add_pd(tmp0, vt3); */
    vt0_x0 = _mm256_add_pd(tmp0_x0, vt3_x0);
    vt0_x1 = _mm256_add_pd(tmp0_x1, vt3_x1);
    vt0_x2 = _mm256_add_pd(tmp0_x2, vt3_x2);

    //renorm5(&s0, &s1, &s2, &s3, &t0);
    /* _mm256_store_pd(lo.d0 + i, vs0);
    _mm256_store_pd(lo.d1 + i, vs1);
    _mm256_store_pd(lo.d2 + i, vs2);
    _mm256_store_pd(lo.d3 + i, vs3);
    _mm256_store_pd(tmp_vt0, vt0); */
    _mm256_store_pd(lo.d0 + i, vs0_x0);
    _mm256_store_pd(lo.d0 + i + 4, vs0_x1);
    _mm256_store_pd(lo.d0 + i + 8, vs0_x2);
    _mm256_store_pd(lo.d1 + i, vs1_x0);
    _mm256_store_pd(lo.d1 + i + 4, vs1_x1);
    _mm256_store_pd(lo.d1 + i + 8, vs1_x2);
    _mm256_store_pd(lo.d2 + i, vs2_x0);
    _mm256_store_pd(lo.d2 + i + 4, vs2_x1);
    _mm256_store_pd(lo.d2 + i + 8, vs2_x2);
    _mm256_store_pd(lo.d3 + i, vs3_x0);
    _mm256_store_pd(lo.d3 + i + 4, vs3_x1);
    _mm256_store_pd(lo.d3 + i + 8, vs3_x2);
    _mm256_store_pd(tmp_vt0_x0, vt0_x0);
    _mm256_store_pd(tmp_vt0_x1, vt0_x1);
    _mm256_store_pd(tmp_vt0_x2, vt0_x2);
    /* renorm5_inline(lo.d0 + i, lo.d1 + i, lo.d2 + i, lo.d3 + i, tmp_vt0);
    renorm5_inline(lo.d0 + i + 1, lo.d1 + i + 1, lo.d2 + i + 1, lo.d3 + i + 1, tmp_vt0 + 1);
    renorm5_inline(lo.d0 + i + 2, lo.d1 + i + 2, lo.d2 + i + 2, lo.d3 + i + 2, tmp_vt0 + 2);
    renorm5_inline(lo.d0 + i + 3, lo.d1 + i + 3, lo.d2 + i + 3, lo.d3 + i + 3, tmp_vt0 + 3); */
    renorm5_inline(lo.d0 + i, lo.d1 + i, lo.d2 + i, lo.d3 + i, tmp_vt0_x0);
    renorm5_inline(lo.d0 + i + 1, lo.d1 + i + 1, lo.d2 + i + 1, lo.d3 + i + 1, tmp_vt0_x0 + 1);
    renorm5_inline(lo.d0 + i + 2, lo.d1 + i + 2, lo.d2 + i + 2, lo.d3 + i + 2, tmp_vt0_x0 + 2);
    renorm5_inline(lo.d0 + i + 3, lo.d1 + i + 3, lo.d2 + i + 3, lo.d3 + i + 3, tmp_vt0_x0 + 3);
    renorm5_inline(lo.d0 + i + 4, lo.d1 + i + 4, lo.d2 + i + 4, lo.d3 + i + 4, tmp_vt0_x1);
    renorm5_inline(lo.d0 + i + 5, lo.d1 + i + 5, lo.d2 + i + 5, lo.d3 + i + 5, tmp_vt0_x1 + 1);
    renorm5_inline(lo.d0 + i + 6, lo.d1 + i + 6, lo.d2 + i + 6, lo.d3 + i + 6, tmp_vt0_x1 + 2);
    renorm5_inline(lo.d0 + i + 7, lo.d1 + i + 7, lo.d2 + i + 7, lo.d3 + i + 7, tmp_vt0_x1 + 3);
    renorm5_inline(lo.d0 + i + 8, lo.d1 + i + 8, lo.d2 + i + 8, lo.d3 + i + 8, tmp_vt0_x2);
    renorm5_inline(lo.d0 + i + 9, lo.d1 + i + 9, lo.d2 + i + 9, lo.d3 + i + 9, tmp_vt0_x2 + 1);
    renorm5_inline(lo.d0 + i + 10, lo.d1 + i + 10, lo.d2 + i + 10, lo.d3 + i + 10, tmp_vt0_x2 + 2);
    renorm5_inline(lo.d0 + i + 11, lo.d1 + i + 11, lo.d2 + i + 11, lo.d3 + i + 11, tmp_vt0_x2 + 3);
  }

  for (; i + 3 < size; i += 4) {
    ld0 = _mm256_load_pd(lo.d0 + i);
    ld1 = _mm256_load_pd(lo.d1 + i);
    ld2 = _mm256_load_pd(lo.d2 + i);
    ld3 = _mm256_load_pd(lo.d3 + i);
    rd0 = _mm256_load_pd(ro.d0 + i);
    rd1 = _mm256_load_pd(ro.d1 + i);
    rd2 = _mm256_load_pd(ro.d2 + i);
    rd3 = _mm256_load_pd(ro.d3 + i);

    //vs0 = two_sum_vec(ld0, rd0, &vt0);
    tmp_s = _mm256_add_pd(ld0, rd0);
    tmp_bb = _mm256_sub_pd(tmp_s, ld0);
    tmp0 = _mm256_sub_pd(tmp_s, tmp_bb);
    tmp1 = _mm256_sub_pd(rd0, tmp_bb);
    tmp0 = _mm256_sub_pd(ld0, tmp0);
    vt0 = _mm256_add_pd(tmp0, tmp1);
    vs0 = tmp_s;

    //vs1 = two_sum_vec(ld1, rd1, &vt1);
    tmp_s = _mm256_add_pd(ld1, rd1);
    tmp_bb = _mm256_sub_pd(tmp_s, ld1);
    tmp0 = _mm256_sub_pd(tmp_s, tmp_bb);
    tmp1 = _mm256_sub_pd(rd1, tmp_bb);
    tmp0 = _mm256_sub_pd(ld1, tmp0);
    vt1 = _mm256_add_pd(tmp0, tmp1);
    vs1 = tmp_s;

    //vs2 = two_sum_vec(ld2, rd2, &vt2);
    tmp_s = _mm256_add_pd(ld2, rd2);
    tmp_bb = _mm256_sub_pd(tmp_s, ld2);
    tmp0 = _mm256_sub_pd(tmp_s, tmp_bb);
    tmp1 = _mm256_sub_pd(rd2, tmp_bb);
    tmp0 = _mm256_sub_pd(ld2, tmp0);
    vt2 = _mm256_add_pd(tmp0, tmp1);
    vs2 = tmp_s;

    //vs3 = two_sum_vec(ld3, rd3, &vt3);
    tmp_s = _mm256_add_pd(ld3, rd3);
    tmp_bb = _mm256_sub_pd(tmp_s, ld3);
    tmp0 = _mm256_sub_pd(tmp_s, tmp_bb);
    tmp1 = _mm256_sub_pd(rd3, tmp_bb);
    tmp0 = _mm256_sub_pd(ld3, tmp0);
    vt3 = _mm256_add_pd(tmp0, tmp1);
    vs3 = tmp_s;

    //vs1 = two_sum_vec(vs1, vt0, &vt0);
    tmp_s = _mm256_add_pd(vs1, vt0);
    tmp_bb = _mm256_sub_pd(tmp_s, vs1);
    tmp0 = _mm256_sub_pd(tmp_s, tmp_bb);
    tmp1 = _mm256_sub_pd(vt0, tmp_bb);
    tmp0 = _mm256_sub_pd(vs1, tmp0);
    vt0 = _mm256_add_pd(tmp0, tmp1);
    vs1 = tmp_s;

    //three_sum_vec(&vs2, &vt0, &vt1);
    tmp_s = _mm256_add_pd(vs2, vt0);
    tmp_bb = _mm256_sub_pd(tmp_s, vs2);
    tmp0 = _mm256_sub_pd(tmp_s, tmp_bb);
    tmp1 = _mm256_sub_pd(vt0, tmp_bb);
    tmp0 = _mm256_sub_pd(vs2, tmp0);
    tmp_t2 = _mm256_add_pd(tmp0, tmp1);
    tmp_t1 = tmp_s;
    tmp_s = _mm256_add_pd(vt1, tmp_t1);
    tmp_bb = _mm256_sub_pd(tmp_s, vt1);
    tmp0 = _mm256_sub_pd(tmp_s, tmp_bb);
    tmp1 = _mm256_sub_pd(tmp_t1, tmp_bb);
    tmp0 = _mm256_sub_pd(vt1, tmp0);
    tmp_t3 = _mm256_add_pd(tmp0, tmp1);
    vs2 = tmp_s;
    tmp_s = _mm256_add_pd(tmp_t2, tmp_t3);
    tmp_bb = _mm256_sub_pd(tmp_s, tmp_t2);
    tmp0 = _mm256_sub_pd(tmp_s, tmp_bb);
    tmp1 = _mm256_sub_pd(tmp_t3, tmp_bb);
    tmp0 = _mm256_sub_pd(tmp_t2, tmp0);
    vt1 = _mm256_add_pd(tmp0, tmp1);
    vt0 = tmp_s;

    //three_sum2_vec(&vs3, &vt0, &vt2);
    tmp_s = _mm256_add_pd(vs3, vt0);
    tmp_bb = _mm256_sub_pd(tmp_s, vs3);
    tmp0 = _mm256_sub_pd(tmp_s, tmp_bb);
    tmp1 = _mm256_sub_pd(vt0, tmp_bb);
    tmp0 = _mm256_sub_pd(vs3, tmp0);
    tmp_t2 = _mm256_add_pd(tmp0, tmp1);
    tmp_t1 = tmp_s;
    tmp_s = _mm256_add_pd(vt2, tmp_t1);
    tmp_bb = _mm256_sub_pd(tmp_s, vt2);
    tmp0 = _mm256_sub_pd(tmp_s, tmp_bb);
    tmp1 = _mm256_sub_pd(tmp_t1, tmp_bb);
    tmp0 = _mm256_sub_pd(vt2, tmp0);
    tmp_t3 = _mm256_add_pd(tmp0, tmp1);
    vs3 = tmp_s;
    vt0 = _mm256_add_pd(tmp_t2, tmp_t3);

    //vt0 = _mm256_add_pd(_mm256_add_pd(vt0, vt1), vt3);
    tmp0 = _mm256_add_pd(vt0, vt1);
    vt0 = _mm256_add_pd(tmp0, vt3);

    //renorm5(&s0, &s1, &s2, &s3, &t0);
    _mm256_store_pd(lo.d0 + i, vs0);
    _mm256_store_pd(lo.d1 + i, vs1);
    _mm256_store_pd(lo.d2 + i, vs2);
    _mm256_store_pd(lo.d3 + i, vs3);
    _mm256_store_pd(tmp_vt0, vt0);
    renorm5_inline(lo.d0 + i, lo.d1 + i, lo.d2 + i, lo.d3 + i, tmp_vt0);
    renorm5_inline(lo.d0 + i + 1, lo.d1 + i + 1, lo.d2 + i + 1, lo.d3 + i + 1, tmp_vt0 + 1);
    renorm5_inline(lo.d0 + i + 2, lo.d1 + i + 2, lo.d2 + i + 2, lo.d3 + i + 2, tmp_vt0 + 2);
    renorm5_inline(lo.d0 + i + 3, lo.d1 + i + 3, lo.d2 + i + 3, lo.d3 + i + 3, tmp_vt0 + 3);
  }

  for (; i < size; i++) {
      
    s0 = two_sum(lo.d0[i], ro.d0[i], &t0);
    s1 = two_sum(lo.d1[i], ro.d1[i], &t1);
    s2 = two_sum(lo.d2[i], ro.d2[i], &t2);
    s3 = two_sum(lo.d3[i], ro.d3[i], &t3);

    s1 = two_sum(s1, t0, &t0);
    three_sum(&s2, &t0, &t1);
    three_sum2(&s3, &t0, &t2);
    t0 = t0 + t1 + t3;

    renorm5_inline(&s0, &s1, &s2, &s3, &t0);
    lo.d0[i] = s0;
    lo.d1[i] = s1;
    lo.d2[i] = s2;
    lo.d3[i] = s3;
  }
  _mm_free(tmp_vt0);
  _mm_free(tmp_vt0_x0);
  _mm_free(tmp_vt0_x1);
  _mm_free(tmp_vt0_x2);
}

void qd_arr_add_inplace_vec_inline_x4(qd_arr lo, qd_arr ro) {
  //assert(lo.size == ro.size);
  int size = lo.size;

  double s0, s1, s2, s3;
  double t0, t1, t2, t3;
  __m256d vs0, vs1, vs2, vs3;
  __m256d vs0_x0, vs1_x0, vs2_x0, vs3_x0;
  __m256d vs0_x1, vs1_x1, vs2_x1, vs3_x1;
  __m256d vs0_x2, vs1_x2, vs2_x2, vs3_x2;
  __m256d vs0_x3, vs1_x3, vs2_x3, vs3_x3;

  __m256d vt0, vt1, vt2, vt3;
  __m256d vt0_x0, vt1_x0, vt2_x0, vt3_x0;
  __m256d vt0_x1, vt1_x1, vt2_x1, vt3_x1;
  __m256d vt0_x2, vt1_x2, vt2_x2, vt3_x2;
  __m256d vt0_x3, vt1_x3, vt2_x3, vt3_x3;

  __m256d ld0, ld1, ld2, ld3;
  __m256d ld0_x0, ld1_x0, ld2_x0, ld3_x0;
  __m256d ld0_x1, ld1_x1, ld2_x1, ld3_x1;
  __m256d ld0_x2, ld1_x2, ld2_x2, ld3_x2;
  __m256d ld0_x3, ld1_x3, ld2_x3, ld3_x3;

  __m256d rd0, rd1, rd2, rd3;
  __m256d rd0_x0, rd1_x0, rd2_x0, rd3_x0;
  __m256d rd0_x1, rd1_x1, rd2_x1, rd3_x1;
  __m256d rd0_x2, rd1_x2, rd2_x2, rd3_x2;
  __m256d rd0_x3, rd1_x3, rd2_x3, rd3_x3;

  __m256d tmp_s, tmp_t1, tmp_t2, tmp_t3, tmp_bb, tmp0, tmp1;
  __m256d tmp_s_x0, tmp_t1_x0, tmp_t2_x0, tmp_t3_x0, tmp_bb_x0, tmp0_x0, tmp1_x0;
  __m256d tmp_s_x1, tmp_t1_x1, tmp_t2_x1, tmp_t3_x1, tmp_bb_x1, tmp0_x1, tmp1_x1;
  __m256d tmp_s_x2, tmp_t1_x2, tmp_t2_x2, tmp_t3_x2, tmp_bb_x2, tmp0_x2, tmp1_x2;
  __m256d tmp_s_x3, tmp_t1_x3, tmp_t2_x3, tmp_t3_x3, tmp_bb_x3, tmp0_x3, tmp1_x3;
  double *tmp_vt0 = (double *)_mm_malloc(4 * sizeof(double), ALIGNMENT);
  double *tmp_vt0_x0 = (double *)_mm_malloc(4 * sizeof(double), ALIGNMENT);
  double *tmp_vt0_x1 = (double *)_mm_malloc(4 * sizeof(double), ALIGNMENT);
  double *tmp_vt0_x2 = (double *)_mm_malloc(4 * sizeof(double), ALIGNMENT);
  double *tmp_vt0_x3 = (double *)_mm_malloc(4 * sizeof(double), ALIGNMENT);
  int i;
  for (i = 0; i + 15 < size; i += 16) {
    ld0_x0 = _mm256_load_pd(lo.d0 + i);
    ld0_x1 = _mm256_load_pd(lo.d0 + i + 4);
    ld0_x2 = _mm256_load_pd(lo.d0 + i + 8);
    ld0_x3 = _mm256_load_pd(lo.d0 + i + 12);
    ld1_x0 = _mm256_load_pd(lo.d1 + i);
    ld1_x1 = _mm256_load_pd(lo.d1 + i + 4);
    ld1_x2 = _mm256_load_pd(lo.d1 + i + 8);
    ld1_x3 = _mm256_load_pd(lo.d1 + i + 12);
    ld2_x0 = _mm256_load_pd(lo.d2 + i);
    ld2_x1 = _mm256_load_pd(lo.d2 + i + 4);
    ld2_x2 = _mm256_load_pd(lo.d2 + i + 8);
    ld2_x3 = _mm256_load_pd(lo.d2 + i + 12);
    ld3_x0 = _mm256_load_pd(lo.d3 + i);
    ld3_x1 = _mm256_load_pd(lo.d3 + i + 4);
    ld3_x2 = _mm256_load_pd(lo.d3 + i + 8);
    ld3_x3 = _mm256_load_pd(lo.d3 + i + 12);
    
    rd0_x0 = _mm256_load_pd(ro.d0 + i);
    rd0_x1 = _mm256_load_pd(ro.d0 + i + 4);
    rd0_x2 = _mm256_load_pd(ro.d0 + i + 8);
    rd0_x3 = _mm256_load_pd(ro.d0 + i + 12);
    rd1_x0 = _mm256_load_pd(ro.d1 + i);
    rd1_x1 = _mm256_load_pd(ro.d1 + i + 4);
    rd1_x2 = _mm256_load_pd(ro.d1 + i + 8);
    rd1_x3 = _mm256_load_pd(ro.d1 + i + 12);
    rd2_x0 = _mm256_load_pd(ro.d2 + i);
    rd2_x1 = _mm256_load_pd(ro.d2 + i + 4);
    rd2_x2 = _mm256_load_pd(ro.d2 + i + 8);
    rd2_x3 = _mm256_load_pd(ro.d2 + i + 12);
    rd3_x0 = _mm256_load_pd(ro.d3 + i);
    rd3_x1 = _mm256_load_pd(ro.d3 + i + 4);
    rd3_x2 = _mm256_load_pd(ro.d3 + i + 8);
    rd3_x3 = _mm256_load_pd(ro.d3 + i + 12);

    //vs0 = two_sum_vec(ld0, rd0, &vt0);
    tmp_s_x0 = _mm256_add_pd(ld0_x0, rd0_x0);
    tmp_s_x1 = _mm256_add_pd(ld0_x1, rd0_x1);
    tmp_s_x2 = _mm256_add_pd(ld0_x2, rd0_x2);
    tmp_s_x3 = _mm256_add_pd(ld0_x3, rd0_x3);

    tmp_bb_x0 = _mm256_sub_pd(tmp_s_x0, ld0_x0);
    tmp_bb_x1 = _mm256_sub_pd(tmp_s_x1, ld0_x1);
    tmp_bb_x2 = _mm256_sub_pd(tmp_s_x2, ld0_x2);
    tmp_bb_x3 = _mm256_sub_pd(tmp_s_x3, ld0_x3);

    tmp0_x0 = _mm256_sub_pd(tmp_s_x0, tmp_bb_x0);
    tmp0_x1 = _mm256_sub_pd(tmp_s_x1, tmp_bb_x1);
    tmp0_x2 = _mm256_sub_pd(tmp_s_x2, tmp_bb_x2);
    tmp0_x3 = _mm256_sub_pd(tmp_s_x3, tmp_bb_x3);
    
    tmp1_x0 = _mm256_sub_pd(rd0_x0, tmp_bb_x0);
    tmp1_x1 = _mm256_sub_pd(rd0_x1, tmp_bb_x1);
    tmp1_x2 = _mm256_sub_pd(rd0_x2, tmp_bb_x2);
    tmp1_x3 = _mm256_sub_pd(rd0_x3, tmp_bb_x3);
    
    tmp0_x0 = _mm256_sub_pd(ld0_x0, tmp0_x0);
    tmp0_x1 = _mm256_sub_pd(ld0_x1, tmp0_x1);
    tmp0_x2 = _mm256_sub_pd(ld0_x2, tmp0_x2);
    tmp0_x3 = _mm256_sub_pd(ld0_x3, tmp0_x3);

    vt0_x0 = _mm256_add_pd(tmp0_x0, tmp1_x0);
    vt0_x1 = _mm256_add_pd(tmp0_x1, tmp1_x1);
    vt0_x2 = _mm256_add_pd(tmp0_x2, tmp1_x2);
    vt0_x3 = _mm256_add_pd(tmp0_x3, tmp1_x3);

    vs0_x0 = tmp_s_x0;
    vs0_x1 = tmp_s_x1;
    vs0_x2 = tmp_s_x2;
    vs0_x3 = tmp_s_x3;

    //vs1 = two_sum_vec(ld1, rd1, &vt1);
    tmp_s_x0 = _mm256_add_pd(ld1_x0, rd1_x0);
    tmp_s_x1 = _mm256_add_pd(ld1_x1, rd1_x1);
    tmp_s_x2 = _mm256_add_pd(ld1_x2, rd1_x2);
    tmp_s_x3 = _mm256_add_pd(ld1_x3, rd1_x3);

    tmp_bb_x0 = _mm256_sub_pd(tmp_s_x0, ld1_x0);
    tmp_bb_x1 = _mm256_sub_pd(tmp_s_x1, ld1_x1);
    tmp_bb_x2 = _mm256_sub_pd(tmp_s_x2, ld1_x2);
    tmp_bb_x3 = _mm256_sub_pd(tmp_s_x3, ld1_x3);

    tmp0_x0 = _mm256_sub_pd(tmp_s_x0, tmp_bb_x0);
    tmp0_x1 = _mm256_sub_pd(tmp_s_x1, tmp_bb_x1);
    tmp0_x2 = _mm256_sub_pd(tmp_s_x2, tmp_bb_x2);
    tmp0_x3 = _mm256_sub_pd(tmp_s_x3, tmp_bb_x3);
    
    tmp1_x0 = _mm256_sub_pd(rd1_x0, tmp_bb_x0);
    tmp1_x1 = _mm256_sub_pd(rd1_x1, tmp_bb_x1);
    tmp1_x2 = _mm256_sub_pd(rd1_x2, tmp_bb_x2);
    tmp1_x3 = _mm256_sub_pd(rd1_x3, tmp_bb_x3);
    
    tmp0_x0 = _mm256_sub_pd(ld1_x0, tmp0_x0);
    tmp0_x1 = _mm256_sub_pd(ld1_x1, tmp0_x1);
    tmp0_x2 = _mm256_sub_pd(ld1_x2, tmp0_x2);
    tmp0_x3 = _mm256_sub_pd(ld1_x3, tmp0_x3);

    vt1_x0 = _mm256_add_pd(tmp0_x0, tmp1_x0);
    vt1_x1 = _mm256_add_pd(tmp0_x1, tmp1_x1);
    vt1_x2 = _mm256_add_pd(tmp0_x2, tmp1_x2);
    vt1_x3 = _mm256_add_pd(tmp0_x3, tmp1_x3);

    vs1_x0 = tmp_s_x0;
    vs1_x1 = tmp_s_x1;
    vs1_x2 = tmp_s_x2;
    vs1_x3 = tmp_s_x3;

    //vs2 = two_sum_vec(ld2, rd2, &vt2);
    tmp_s_x0 = _mm256_add_pd(ld2_x0, rd2_x0);
    tmp_s_x1 = _mm256_add_pd(ld2_x1, rd2_x1);
    tmp_s_x2 = _mm256_add_pd(ld2_x2, rd2_x2);
    tmp_s_x3 = _mm256_add_pd(ld2_x3, rd2_x3);

    tmp_bb_x0 = _mm256_sub_pd(tmp_s_x0, ld2_x0);
    tmp_bb_x1 = _mm256_sub_pd(tmp_s_x1, ld2_x1);
    tmp_bb_x2 = _mm256_sub_pd(tmp_s_x2, ld2_x2);
    tmp_bb_x3 = _mm256_sub_pd(tmp_s_x3, ld2_x3);

    tmp0_x0 = _mm256_sub_pd(tmp_s_x0, tmp_bb_x0);
    tmp0_x1 = _mm256_sub_pd(tmp_s_x1, tmp_bb_x1);
    tmp0_x2 = _mm256_sub_pd(tmp_s_x2, tmp_bb_x2);
    tmp0_x3 = _mm256_sub_pd(tmp_s_x3, tmp_bb_x3);
    
    tmp1_x0 = _mm256_sub_pd(rd2_x0, tmp_bb_x0);
    tmp1_x1 = _mm256_sub_pd(rd2_x1, tmp_bb_x1);
    tmp1_x2 = _mm256_sub_pd(rd2_x2, tmp_bb_x2);
    tmp1_x3 = _mm256_sub_pd(rd2_x3, tmp_bb_x3);
    
    tmp0_x0 = _mm256_sub_pd(ld2_x0, tmp0_x0);
    tmp0_x1 = _mm256_sub_pd(ld2_x1, tmp0_x1);
    tmp0_x2 = _mm256_sub_pd(ld2_x2, tmp0_x2);
    tmp0_x3 = _mm256_sub_pd(ld2_x3, tmp0_x3);

    vt2_x0 = _mm256_add_pd(tmp0_x0, tmp1_x0);
    vt2_x1 = _mm256_add_pd(tmp0_x1, tmp1_x1);
    vt2_x2 = _mm256_add_pd(tmp0_x2, tmp1_x2);
    vt2_x3 = _mm256_add_pd(tmp0_x3, tmp1_x3);

    vs2_x0 = tmp_s_x0;
    vs2_x1 = tmp_s_x1;
    vs2_x2 = tmp_s_x2;
    vs2_x3 = tmp_s_x3;

    //vs3 = two_sum_vec(ld3, rd3, &vt3);
    tmp_s_x0 = _mm256_add_pd(ld3_x0, rd3_x0);
    tmp_s_x1 = _mm256_add_pd(ld3_x1, rd3_x1);
    tmp_s_x2 = _mm256_add_pd(ld3_x2, rd3_x2);
    tmp_s_x3 = _mm256_add_pd(ld3_x3, rd3_x3);

    tmp_bb_x0 = _mm256_sub_pd(tmp_s_x0, ld3_x0);
    tmp_bb_x1 = _mm256_sub_pd(tmp_s_x1, ld3_x1);
    tmp_bb_x2 = _mm256_sub_pd(tmp_s_x2, ld3_x2);
    tmp_bb_x3 = _mm256_sub_pd(tmp_s_x3, ld3_x3);

    tmp0_x0 = _mm256_sub_pd(tmp_s_x0, tmp_bb_x0);
    tmp0_x1 = _mm256_sub_pd(tmp_s_x1, tmp_bb_x1);
    tmp0_x2 = _mm256_sub_pd(tmp_s_x2, tmp_bb_x2);
    tmp0_x3 = _mm256_sub_pd(tmp_s_x3, tmp_bb_x3);
    
    tmp1_x0 = _mm256_sub_pd(rd3_x0, tmp_bb_x0);
    tmp1_x1 = _mm256_sub_pd(rd3_x1, tmp_bb_x1);
    tmp1_x2 = _mm256_sub_pd(rd3_x2, tmp_bb_x2);
    tmp1_x3 = _mm256_sub_pd(rd3_x3, tmp_bb_x3);
    
    tmp0_x0 = _mm256_sub_pd(ld3_x0, tmp0_x0);
    tmp0_x1 = _mm256_sub_pd(ld3_x1, tmp0_x1);
    tmp0_x2 = _mm256_sub_pd(ld3_x2, tmp0_x2);
    tmp0_x3 = _mm256_sub_pd(ld3_x3, tmp0_x3);

    vt3_x0 = _mm256_add_pd(tmp0_x0, tmp1_x0);
    vt3_x1 = _mm256_add_pd(tmp0_x1, tmp1_x1);
    vt3_x2 = _mm256_add_pd(tmp0_x2, tmp1_x2);
    vt3_x3 = _mm256_add_pd(tmp0_x3, tmp1_x3);

    vs3_x0 = tmp_s_x0;
    vs3_x1 = tmp_s_x1;
    vs3_x2 = tmp_s_x2;
    vs3_x3 = tmp_s_x3;

    //vs1 = two_sum_vec(vs1, vt0, &vt0);
    tmp_s_x0 = _mm256_add_pd(vs1_x0, vt0_x0);
    tmp_s_x1 = _mm256_add_pd(vs1_x1, vt0_x1);
    tmp_s_x2 = _mm256_add_pd(vs1_x2, vt0_x2);
    tmp_s_x3 = _mm256_add_pd(vs1_x3, vt0_x3);

    tmp_bb_x0 = _mm256_sub_pd(tmp_s_x0, vs1_x0);
    tmp_bb_x1 = _mm256_sub_pd(tmp_s_x1, vs1_x1);
    tmp_bb_x2 = _mm256_sub_pd(tmp_s_x2, vs1_x2);
    tmp_bb_x3 = _mm256_sub_pd(tmp_s_x3, vs1_x3);

    tmp0_x0 = _mm256_sub_pd(tmp_s_x0, tmp_bb_x0);
    tmp0_x1 = _mm256_sub_pd(tmp_s_x1, tmp_bb_x1);
    tmp0_x2 = _mm256_sub_pd(tmp_s_x2, tmp_bb_x2);
    tmp0_x3 = _mm256_sub_pd(tmp_s_x3, tmp_bb_x3);
    
    tmp1_x0 = _mm256_sub_pd(vt0_x0, tmp_bb_x0);
    tmp1_x1 = _mm256_sub_pd(vt0_x1, tmp_bb_x1);
    tmp1_x2 = _mm256_sub_pd(vt0_x2, tmp_bb_x2);
    tmp1_x3 = _mm256_sub_pd(vt0_x3, tmp_bb_x3);
    
    tmp0_x0 = _mm256_sub_pd(vs1_x0, tmp0_x0);
    tmp0_x1 = _mm256_sub_pd(vs1_x1, tmp0_x1);
    tmp0_x2 = _mm256_sub_pd(vs1_x2, tmp0_x2);
    tmp0_x3 = _mm256_sub_pd(vs1_x3, tmp0_x3);

    vt0_x0 = _mm256_add_pd(tmp0_x0, tmp1_x0);
    vt0_x1 = _mm256_add_pd(tmp0_x1, tmp1_x1);
    vt0_x2 = _mm256_add_pd(tmp0_x2, tmp1_x2);
    vt0_x3 = _mm256_add_pd(tmp0_x3, tmp1_x3);

    vs1_x0 = tmp_s_x0;
    vs1_x1 = tmp_s_x1;
    vs1_x2 = tmp_s_x2;
    vs1_x3 = tmp_s_x3;

    //three_sum_vec(&vs2, &vt0, &vt1);

    /* tmp_s = _mm256_add_pd(vs2, vt0);
    tmp_bb = _mm256_sub_pd(tmp_s, vs2);
    tmp0 = _mm256_sub_pd(tmp_s, tmp_bb);
    tmp1 = _mm256_sub_pd(vt0, tmp_bb);
    tmp0 = _mm256_sub_pd(vs2, tmp0);
    tmp_t2 = _mm256_add_pd(tmp0, tmp1);
    tmp_t1 = tmp_s; */
    tmp_s_x0 = _mm256_add_pd(vs2_x0, vt0_x0);
    tmp_s_x1 = _mm256_add_pd(vs2_x1, vt0_x1);
    tmp_s_x2 = _mm256_add_pd(vs2_x2, vt0_x2);
    tmp_s_x3 = _mm256_add_pd(vs2_x3, vt0_x3);

    tmp_bb_x0 = _mm256_sub_pd(tmp_s_x0, vs2_x0);
    tmp_bb_x1 = _mm256_sub_pd(tmp_s_x1, vs2_x1);
    tmp_bb_x2 = _mm256_sub_pd(tmp_s_x2, vs2_x2);
    tmp_bb_x3 = _mm256_sub_pd(tmp_s_x3, vs2_x3);

    tmp0_x0 = _mm256_sub_pd(tmp_s_x0, tmp_bb_x0);
    tmp0_x1 = _mm256_sub_pd(tmp_s_x1, tmp_bb_x1);
    tmp0_x2 = _mm256_sub_pd(tmp_s_x2, tmp_bb_x2);
    tmp0_x3 = _mm256_sub_pd(tmp_s_x3, tmp_bb_x3);
    
    tmp1_x0 = _mm256_sub_pd(vt0_x0, tmp_bb_x0);
    tmp1_x1 = _mm256_sub_pd(vt0_x1, tmp_bb_x1);
    tmp1_x2 = _mm256_sub_pd(vt0_x2, tmp_bb_x2);
    tmp1_x3 = _mm256_sub_pd(vt0_x3, tmp_bb_x3);
    
    tmp0_x0 = _mm256_sub_pd(vs2_x0, tmp0_x0);
    tmp0_x1 = _mm256_sub_pd(vs2_x1, tmp0_x1);
    tmp0_x2 = _mm256_sub_pd(vs2_x2, tmp0_x2);
    tmp0_x3 = _mm256_sub_pd(vs2_x3, tmp0_x3);

    tmp_t2_x0 = _mm256_add_pd(tmp0_x0, tmp1_x0);
    tmp_t2_x1 = _mm256_add_pd(tmp0_x1, tmp1_x1);
    tmp_t2_x2 = _mm256_add_pd(tmp0_x2, tmp1_x2);
    tmp_t2_x3 = _mm256_add_pd(tmp0_x3, tmp1_x3);

    tmp_t1_x0 = tmp_s_x0;
    tmp_t1_x1 = tmp_s_x1;
    tmp_t1_x2 = tmp_s_x2;
    tmp_t1_x3 = tmp_s_x3;

    /* tmp_s = _mm256_add_pd(vt1, tmp_t1);
    tmp_bb = _mm256_sub_pd(tmp_s, vt1);
    tmp0 = _mm256_sub_pd(tmp_s, tmp_bb);
    tmp1 = _mm256_sub_pd(tmp_t1, tmp_bb);
    tmp0 = _mm256_sub_pd(vt1, tmp0);
    tmp_t3 = _mm256_add_pd(tmp0, tmp1);
    vs2 = tmp_s; */

    tmp_s_x0 = _mm256_add_pd(vt1_x0, tmp_t1_x0);
    tmp_s_x1 = _mm256_add_pd(vt1_x1, tmp_t1_x1);
    tmp_s_x2 = _mm256_add_pd(vt1_x2, tmp_t1_x2);
    tmp_s_x3 = _mm256_add_pd(vt1_x3, tmp_t1_x3);

    tmp_bb_x0 = _mm256_sub_pd(tmp_s_x0, vt1_x0);
    tmp_bb_x1 = _mm256_sub_pd(tmp_s_x1, vt1_x1);
    tmp_bb_x2 = _mm256_sub_pd(tmp_s_x2, vt1_x2);
    tmp_bb_x3 = _mm256_sub_pd(tmp_s_x3, vt1_x3);

    tmp0_x0 = _mm256_sub_pd(tmp_s_x0, tmp_bb_x0);
    tmp0_x1 = _mm256_sub_pd(tmp_s_x1, tmp_bb_x1);
    tmp0_x2 = _mm256_sub_pd(tmp_s_x2, tmp_bb_x2);
    tmp0_x3 = _mm256_sub_pd(tmp_s_x3, tmp_bb_x3);
    
    tmp1_x0 = _mm256_sub_pd(tmp_t1_x0, tmp_bb_x0);
    tmp1_x1 = _mm256_sub_pd(tmp_t1_x1, tmp_bb_x1);
    tmp1_x2 = _mm256_sub_pd(tmp_t1_x2, tmp_bb_x2);
    tmp1_x3 = _mm256_sub_pd(tmp_t1_x3, tmp_bb_x3);
    
    tmp0_x0 = _mm256_sub_pd(vt1_x0, tmp0_x0);
    tmp0_x1 = _mm256_sub_pd(vt1_x1, tmp0_x1);
    tmp0_x2 = _mm256_sub_pd(vt1_x2, tmp0_x2);
    tmp0_x3 = _mm256_sub_pd(vt1_x3, tmp0_x3);

    tmp_t3_x0 = _mm256_add_pd(tmp0_x0, tmp1_x0);
    tmp_t3_x1 = _mm256_add_pd(tmp0_x1, tmp1_x1);
    tmp_t3_x2 = _mm256_add_pd(tmp0_x2, tmp1_x2);
    tmp_t3_x3 = _mm256_add_pd(tmp0_x3, tmp1_x3);

    vs2_x0 = tmp_s_x0;
    vs2_x1 = tmp_s_x1;
    vs2_x2 = tmp_s_x2;
    vs2_x3 = tmp_s_x3;


    /* tmp_s = _mm256_add_pd(tmp_t2, tmp_t3);
    tmp_bb = _mm256_sub_pd(tmp_s, tmp_t2);
    tmp0 = _mm256_sub_pd(tmp_s, tmp_bb);
    tmp1 = _mm256_sub_pd(tmp_t3, tmp_bb);
    tmp0 = _mm256_sub_pd(tmp_t2, tmp0);
    vt1 = _mm256_add_pd(tmp0, tmp1);
    vt0 = tmp_s; */

    tmp_s_x0 = _mm256_add_pd(tmp_t2_x0, tmp_t3_x0);
    tmp_s_x1 = _mm256_add_pd(tmp_t2_x1, tmp_t3_x1);
    tmp_s_x2 = _mm256_add_pd(tmp_t2_x2, tmp_t3_x2);
    tmp_s_x3 = _mm256_add_pd(tmp_t2_x3, tmp_t3_x3);

    tmp_bb_x0 = _mm256_sub_pd(tmp_s_x0, tmp_t2_x0);
    tmp_bb_x1 = _mm256_sub_pd(tmp_s_x1, tmp_t2_x1);
    tmp_bb_x2 = _mm256_sub_pd(tmp_s_x2, tmp_t2_x2);
    tmp_bb_x3 = _mm256_sub_pd(tmp_s_x3, tmp_t2_x3);

    tmp0_x0 = _mm256_sub_pd(tmp_s_x0, tmp_bb_x0);
    tmp0_x1 = _mm256_sub_pd(tmp_s_x1, tmp_bb_x1);
    tmp0_x2 = _mm256_sub_pd(tmp_s_x2, tmp_bb_x2);
    tmp0_x3 = _mm256_sub_pd(tmp_s_x3, tmp_bb_x3);
    
    tmp1_x0 = _mm256_sub_pd(tmp_t3_x0, tmp_bb_x0);
    tmp1_x1 = _mm256_sub_pd(tmp_t3_x1, tmp_bb_x1);
    tmp1_x2 = _mm256_sub_pd(tmp_t3_x2, tmp_bb_x2);
    tmp1_x3 = _mm256_sub_pd(tmp_t3_x3, tmp_bb_x3);
    
    tmp0_x0 = _mm256_sub_pd(tmp_t2_x0, tmp0_x0);
    tmp0_x1 = _mm256_sub_pd(tmp_t2_x1, tmp0_x1);
    tmp0_x2 = _mm256_sub_pd(tmp_t2_x2, tmp0_x2);
    tmp0_x3 = _mm256_sub_pd(tmp_t2_x3, tmp0_x3);

    vt1_x0 = _mm256_add_pd(tmp0_x0, tmp1_x0);
    vt1_x1 = _mm256_add_pd(tmp0_x1, tmp1_x1);
    vt1_x2 = _mm256_add_pd(tmp0_x2, tmp1_x2);
    vt1_x3 = _mm256_add_pd(tmp0_x3, tmp1_x3);

    vt0_x0 = tmp_s_x0;
    vt0_x1 = tmp_s_x1;
    vt0_x2 = tmp_s_x2;
    vt0_x3 = tmp_s_x3;



    //three_sum2_vec(&vs3, &vt0, &vt2);

    /* tmp_s = _mm256_add_pd(vs3, vt0);
    tmp_bb = _mm256_sub_pd(tmp_s, vs3);
    tmp0 = _mm256_sub_pd(tmp_s, tmp_bb);
    tmp1 = _mm256_sub_pd(vt0, tmp_bb);
    tmp0 = _mm256_sub_pd(vs3, tmp0);
    tmp_t2 = _mm256_add_pd(tmp0, tmp1);
    tmp_t1 = tmp_s; */
    tmp_s_x0 = _mm256_add_pd(vs3_x0, vt0_x0);
    tmp_s_x1 = _mm256_add_pd(vs3_x1, vt0_x1);
    tmp_s_x2 = _mm256_add_pd(vs3_x2, vt0_x2);
    tmp_s_x3 = _mm256_add_pd(vs3_x3, vt0_x3);

    tmp_bb_x0 = _mm256_sub_pd(tmp_s_x0, vs3_x0);
    tmp_bb_x1 = _mm256_sub_pd(tmp_s_x1, vs3_x1);
    tmp_bb_x2 = _mm256_sub_pd(tmp_s_x2, vs3_x2);
    tmp_bb_x3 = _mm256_sub_pd(tmp_s_x3, vs3_x3);

    tmp0_x0 = _mm256_sub_pd(tmp_s_x0, tmp_bb_x0);
    tmp0_x1 = _mm256_sub_pd(tmp_s_x1, tmp_bb_x1);
    tmp0_x2 = _mm256_sub_pd(tmp_s_x2, tmp_bb_x2);
    tmp0_x3 = _mm256_sub_pd(tmp_s_x3, tmp_bb_x3);
    
    tmp1_x0 = _mm256_sub_pd(vt0_x0, tmp_bb_x0);
    tmp1_x1 = _mm256_sub_pd(vt0_x1, tmp_bb_x1);
    tmp1_x2 = _mm256_sub_pd(vt0_x2, tmp_bb_x2);
    tmp1_x3 = _mm256_sub_pd(vt0_x3, tmp_bb_x3);
    
    tmp0_x0 = _mm256_sub_pd(vs3_x0, tmp0_x0);
    tmp0_x1 = _mm256_sub_pd(vs3_x1, tmp0_x1);
    tmp0_x2 = _mm256_sub_pd(vs3_x2, tmp0_x2);
    tmp0_x3 = _mm256_sub_pd(vs3_x3, tmp0_x3);

    tmp_t2_x0 = _mm256_add_pd(tmp0_x0, tmp1_x0);
    tmp_t2_x1 = _mm256_add_pd(tmp0_x1, tmp1_x1);
    tmp_t2_x2 = _mm256_add_pd(tmp0_x2, tmp1_x2);
    tmp_t2_x3 = _mm256_add_pd(tmp0_x3, tmp1_x3);

    tmp_t1_x0 = tmp_s_x0;
    tmp_t1_x1 = tmp_s_x1;
    tmp_t1_x2 = tmp_s_x2;
    tmp_t1_x3 = tmp_s_x3;
    /* tmp_s = _mm256_add_pd(vt2, tmp_t1);
    tmp_bb = _mm256_sub_pd(tmp_s, vt2);
    tmp0 = _mm256_sub_pd(tmp_s, tmp_bb);
    tmp1 = _mm256_sub_pd(tmp_t1, tmp_bb);
    tmp0 = _mm256_sub_pd(vt2, tmp0);
    tmp_t3 = _mm256_add_pd(tmp0, tmp1);
    vs3 = tmp_s; */
    tmp_s_x0 = _mm256_add_pd(vt2_x0, tmp_t1_x0);
    tmp_s_x1 = _mm256_add_pd(vt2_x1, tmp_t1_x1);
    tmp_s_x2 = _mm256_add_pd(vt2_x2, tmp_t1_x2);
    tmp_s_x3 = _mm256_add_pd(vt2_x3, tmp_t1_x3);

    tmp_bb_x0 = _mm256_sub_pd(tmp_s_x0, vt2_x0);
    tmp_bb_x1 = _mm256_sub_pd(tmp_s_x1, vt2_x1);
    tmp_bb_x2 = _mm256_sub_pd(tmp_s_x2, vt2_x2);
    tmp_bb_x3 = _mm256_sub_pd(tmp_s_x3, vt2_x3);

    tmp0_x0 = _mm256_sub_pd(tmp_s_x0, tmp_bb_x0);
    tmp0_x1 = _mm256_sub_pd(tmp_s_x1, tmp_bb_x1);
    tmp0_x2 = _mm256_sub_pd(tmp_s_x2, tmp_bb_x2);
    tmp0_x3 = _mm256_sub_pd(tmp_s_x3, tmp_bb_x3);
    
    tmp1_x0 = _mm256_sub_pd(tmp_t1_x0, tmp_bb_x0);
    tmp1_x1 = _mm256_sub_pd(tmp_t1_x1, tmp_bb_x1);
    tmp1_x2 = _mm256_sub_pd(tmp_t1_x2, tmp_bb_x2);
    tmp1_x3 = _mm256_sub_pd(tmp_t1_x3, tmp_bb_x3);
    
    tmp0_x0 = _mm256_sub_pd(vt2_x0, tmp0_x0);
    tmp0_x1 = _mm256_sub_pd(vt2_x1, tmp0_x1);
    tmp0_x2 = _mm256_sub_pd(vt2_x2, tmp0_x2);
    tmp0_x3 = _mm256_sub_pd(vt2_x3, tmp0_x3);

    tmp_t3_x0 = _mm256_add_pd(tmp0_x0, tmp1_x0);
    tmp_t3_x1 = _mm256_add_pd(tmp0_x1, tmp1_x1);
    tmp_t3_x2 = _mm256_add_pd(tmp0_x2, tmp1_x2);
    tmp_t3_x3 = _mm256_add_pd(tmp0_x3, tmp1_x3);

    vs3_x0 = tmp_s_x0;
    vs3_x1 = tmp_s_x1;
    vs3_x2 = tmp_s_x2;
    vs3_x3 = tmp_s_x3;

    /* vt0 = _mm256_add_pd(tmp_t2, tmp_t3); */
    vt0_x0 = _mm256_add_pd(tmp_t2_x0, tmp_t3_x0);
    vt0_x1 = _mm256_add_pd(tmp_t2_x1, tmp_t3_x1);
    vt0_x2 = _mm256_add_pd(tmp_t2_x2, tmp_t3_x2);
    vt0_x3 = _mm256_add_pd(tmp_t2_x3, tmp_t3_x3);

    //vt0 = _mm256_add_pd(_mm256_add_pd(vt0, vt1), vt3);
    /* tmp0 = _mm256_add_pd(vt0, vt1); */
    tmp0_x0 = _mm256_add_pd(vt0_x0, vt1_x0);
    tmp0_x1 = _mm256_add_pd(vt0_x1, vt1_x1);
    tmp0_x2 = _mm256_add_pd(vt0_x2, vt1_x2);
    tmp0_x3 = _mm256_add_pd(vt0_x3, vt1_x3);

    /* vt0 = _mm256_add_pd(tmp0, vt3); */
    vt0_x0 = _mm256_add_pd(tmp0_x0, vt3_x0);
    vt0_x1 = _mm256_add_pd(tmp0_x1, vt3_x1);
    vt0_x2 = _mm256_add_pd(tmp0_x2, vt3_x2);
    vt0_x3 = _mm256_add_pd(tmp0_x3, vt3_x3);

    //renorm5(&s0, &s1, &s2, &s3, &t0);
    /* _mm256_store_pd(lo.d0 + i, vs0);
    _mm256_store_pd(lo.d1 + i, vs1);
    _mm256_store_pd(lo.d2 + i, vs2);
    _mm256_store_pd(lo.d3 + i, vs3);
    _mm256_store_pd(tmp_vt0, vt0); */
    _mm256_store_pd(lo.d0 + i, vs0_x0);
    _mm256_store_pd(lo.d0 + i + 4, vs0_x1);
    _mm256_store_pd(lo.d0 + i + 8, vs0_x2);
    _mm256_store_pd(lo.d0 + i + 12, vs0_x3);
    _mm256_store_pd(lo.d1 + i, vs1_x0);
    _mm256_store_pd(lo.d1 + i + 4, vs1_x1);
    _mm256_store_pd(lo.d1 + i + 8, vs1_x2);
    _mm256_store_pd(lo.d1 + i + 12, vs1_x3);
    _mm256_store_pd(lo.d2 + i, vs2_x0);
    _mm256_store_pd(lo.d2 + i + 4, vs2_x1);
    _mm256_store_pd(lo.d2 + i + 8, vs2_x2);
    _mm256_store_pd(lo.d2 + i + 12, vs2_x3);
    _mm256_store_pd(lo.d3 + i, vs3_x0);
    _mm256_store_pd(lo.d3 + i + 4, vs3_x1);
    _mm256_store_pd(lo.d3 + i + 8, vs3_x2);
    _mm256_store_pd(lo.d3 + i + 12, vs3_x3);
    _mm256_store_pd(tmp_vt0_x0, vt0_x0);
    _mm256_store_pd(tmp_vt0_x1, vt0_x1);
    _mm256_store_pd(tmp_vt0_x2, vt0_x2);
    _mm256_store_pd(tmp_vt0_x3, vt0_x3);
    /* renorm5_inline(lo.d0 + i, lo.d1 + i, lo.d2 + i, lo.d3 + i, tmp_vt0);
    renorm5_inline(lo.d0 + i + 1, lo.d1 + i + 1, lo.d2 + i + 1, lo.d3 + i + 1, tmp_vt0 + 1);
    renorm5_inline(lo.d0 + i + 2, lo.d1 + i + 2, lo.d2 + i + 2, lo.d3 + i + 2, tmp_vt0 + 2);
    renorm5_inline(lo.d0 + i + 3, lo.d1 + i + 3, lo.d2 + i + 3, lo.d3 + i + 3, tmp_vt0 + 3); */
    renorm5_inline(lo.d0 + i, lo.d1 + i, lo.d2 + i, lo.d3 + i, tmp_vt0_x0);
    renorm5_inline(lo.d0 + i + 1, lo.d1 + i + 1, lo.d2 + i + 1, lo.d3 + i + 1, tmp_vt0_x0 + 1);
    renorm5_inline(lo.d0 + i + 2, lo.d1 + i + 2, lo.d2 + i + 2, lo.d3 + i + 2, tmp_vt0_x0 + 2);
    renorm5_inline(lo.d0 + i + 3, lo.d1 + i + 3, lo.d2 + i + 3, lo.d3 + i + 3, tmp_vt0_x0 + 3);
    renorm5_inline(lo.d0 + i + 4, lo.d1 + i + 4, lo.d2 + i + 4, lo.d3 + i + 4, tmp_vt0_x1);
    renorm5_inline(lo.d0 + i + 5, lo.d1 + i + 5, lo.d2 + i + 5, lo.d3 + i + 5, tmp_vt0_x1 + 1);
    renorm5_inline(lo.d0 + i + 6, lo.d1 + i + 6, lo.d2 + i + 6, lo.d3 + i + 6, tmp_vt0_x1 + 2);
    renorm5_inline(lo.d0 + i + 7, lo.d1 + i + 7, lo.d2 + i + 7, lo.d3 + i + 7, tmp_vt0_x1 + 3);
    renorm5_inline(lo.d0 + i + 8, lo.d1 + i + 8, lo.d2 + i + 8, lo.d3 + i + 8, tmp_vt0_x2);
    renorm5_inline(lo.d0 + i + 9, lo.d1 + i + 9, lo.d2 + i + 9, lo.d3 + i + 9, tmp_vt0_x2 + 1);
    renorm5_inline(lo.d0 + i + 10, lo.d1 + i + 10, lo.d2 + i + 10, lo.d3 + i + 10, tmp_vt0_x2 + 2);
    renorm5_inline(lo.d0 + i + 11, lo.d1 + i + 11, lo.d2 + i + 11, lo.d3 + i + 11, tmp_vt0_x2 + 3);
    renorm5_inline(lo.d0 + i + 12, lo.d1 + i + 12, lo.d2 + i + 12, lo.d3 + i + 12, tmp_vt0_x3);
    renorm5_inline(lo.d0 + i + 13, lo.d1 + i + 13, lo.d2 + i + 13, lo.d3 + i + 13, tmp_vt0_x3 + 1);
    renorm5_inline(lo.d0 + i + 14, lo.d1 + i + 14, lo.d2 + i + 14, lo.d3 + i + 14, tmp_vt0_x3 + 2);
    renorm5_inline(lo.d0 + i + 15, lo.d1 + i + 15, lo.d2 + i + 15, lo.d3 + i + 15, tmp_vt0_x3 + 3);
  }

  for (; i + 3 < size; i += 4) {
    ld0 = _mm256_load_pd(lo.d0 + i);
    ld1 = _mm256_load_pd(lo.d1 + i);
    ld2 = _mm256_load_pd(lo.d2 + i);
    ld3 = _mm256_load_pd(lo.d3 + i);
    rd0 = _mm256_load_pd(ro.d0 + i);
    rd1 = _mm256_load_pd(ro.d1 + i);
    rd2 = _mm256_load_pd(ro.d2 + i);
    rd3 = _mm256_load_pd(ro.d3 + i);

    //vs0 = two_sum_vec(ld0, rd0, &vt0);
    tmp_s = _mm256_add_pd(ld0, rd0);
    tmp_bb = _mm256_sub_pd(tmp_s, ld0);
    tmp0 = _mm256_sub_pd(tmp_s, tmp_bb);
    tmp1 = _mm256_sub_pd(rd0, tmp_bb);
    tmp0 = _mm256_sub_pd(ld0, tmp0);
    vt0 = _mm256_add_pd(tmp0, tmp1);
    vs0 = tmp_s;

    //vs1 = two_sum_vec(ld1, rd1, &vt1);
    tmp_s = _mm256_add_pd(ld1, rd1);
    tmp_bb = _mm256_sub_pd(tmp_s, ld1);
    tmp0 = _mm256_sub_pd(tmp_s, tmp_bb);
    tmp1 = _mm256_sub_pd(rd1, tmp_bb);
    tmp0 = _mm256_sub_pd(ld1, tmp0);
    vt1 = _mm256_add_pd(tmp0, tmp1);
    vs1 = tmp_s;

    //vs2 = two_sum_vec(ld2, rd2, &vt2);
    tmp_s = _mm256_add_pd(ld2, rd2);
    tmp_bb = _mm256_sub_pd(tmp_s, ld2);
    tmp0 = _mm256_sub_pd(tmp_s, tmp_bb);
    tmp1 = _mm256_sub_pd(rd2, tmp_bb);
    tmp0 = _mm256_sub_pd(ld2, tmp0);
    vt2 = _mm256_add_pd(tmp0, tmp1);
    vs2 = tmp_s;

    //vs3 = two_sum_vec(ld3, rd3, &vt3);
    tmp_s = _mm256_add_pd(ld3, rd3);
    tmp_bb = _mm256_sub_pd(tmp_s, ld3);
    tmp0 = _mm256_sub_pd(tmp_s, tmp_bb);
    tmp1 = _mm256_sub_pd(rd3, tmp_bb);
    tmp0 = _mm256_sub_pd(ld3, tmp0);
    vt3 = _mm256_add_pd(tmp0, tmp1);
    vs3 = tmp_s;

    //vs1 = two_sum_vec(vs1, vt0, &vt0);
    tmp_s = _mm256_add_pd(vs1, vt0);
    tmp_bb = _mm256_sub_pd(tmp_s, vs1);
    tmp0 = _mm256_sub_pd(tmp_s, tmp_bb);
    tmp1 = _mm256_sub_pd(vt0, tmp_bb);
    tmp0 = _mm256_sub_pd(vs1, tmp0);
    vt0 = _mm256_add_pd(tmp0, tmp1);
    vs1 = tmp_s;

    //three_sum_vec(&vs2, &vt0, &vt1);
    tmp_s = _mm256_add_pd(vs2, vt0);
    tmp_bb = _mm256_sub_pd(tmp_s, vs2);
    tmp0 = _mm256_sub_pd(tmp_s, tmp_bb);
    tmp1 = _mm256_sub_pd(vt0, tmp_bb);
    tmp0 = _mm256_sub_pd(vs2, tmp0);
    tmp_t2 = _mm256_add_pd(tmp0, tmp1);
    tmp_t1 = tmp_s;
    tmp_s = _mm256_add_pd(vt1, tmp_t1);
    tmp_bb = _mm256_sub_pd(tmp_s, vt1);
    tmp0 = _mm256_sub_pd(tmp_s, tmp_bb);
    tmp1 = _mm256_sub_pd(tmp_t1, tmp_bb);
    tmp0 = _mm256_sub_pd(vt1, tmp0);
    tmp_t3 = _mm256_add_pd(tmp0, tmp1);
    vs2 = tmp_s;
    tmp_s = _mm256_add_pd(tmp_t2, tmp_t3);
    tmp_bb = _mm256_sub_pd(tmp_s, tmp_t2);
    tmp0 = _mm256_sub_pd(tmp_s, tmp_bb);
    tmp1 = _mm256_sub_pd(tmp_t3, tmp_bb);
    tmp0 = _mm256_sub_pd(tmp_t2, tmp0);
    vt1 = _mm256_add_pd(tmp0, tmp1);
    vt0 = tmp_s;

    //three_sum2_vec(&vs3, &vt0, &vt2);
    tmp_s = _mm256_add_pd(vs3, vt0);
    tmp_bb = _mm256_sub_pd(tmp_s, vs3);
    tmp0 = _mm256_sub_pd(tmp_s, tmp_bb);
    tmp1 = _mm256_sub_pd(vt0, tmp_bb);
    tmp0 = _mm256_sub_pd(vs3, tmp0);
    tmp_t2 = _mm256_add_pd(tmp0, tmp1);
    tmp_t1 = tmp_s;
    tmp_s = _mm256_add_pd(vt2, tmp_t1);
    tmp_bb = _mm256_sub_pd(tmp_s, vt2);
    tmp0 = _mm256_sub_pd(tmp_s, tmp_bb);
    tmp1 = _mm256_sub_pd(tmp_t1, tmp_bb);
    tmp0 = _mm256_sub_pd(vt2, tmp0);
    tmp_t3 = _mm256_add_pd(tmp0, tmp1);
    vs3 = tmp_s;
    vt0 = _mm256_add_pd(tmp_t2, tmp_t3);

    //vt0 = _mm256_add_pd(_mm256_add_pd(vt0, vt1), vt3);
    tmp0 = _mm256_add_pd(vt0, vt1);
    vt0 = _mm256_add_pd(tmp0, vt3);

    //renorm5(&s0, &s1, &s2, &s3, &t0);
    _mm256_store_pd(lo.d0 + i, vs0);
    _mm256_store_pd(lo.d1 + i, vs1);
    _mm256_store_pd(lo.d2 + i, vs2);
    _mm256_store_pd(lo.d3 + i, vs3);
    _mm256_store_pd(tmp_vt0, vt0);
    renorm5_inline(lo.d0 + i, lo.d1 + i, lo.d2 + i, lo.d3 + i, tmp_vt0);
    renorm5_inline(lo.d0 + i + 1, lo.d1 + i + 1, lo.d2 + i + 1, lo.d3 + i + 1, tmp_vt0 + 1);
    renorm5_inline(lo.d0 + i + 2, lo.d1 + i + 2, lo.d2 + i + 2, lo.d3 + i + 2, tmp_vt0 + 2);
    renorm5_inline(lo.d0 + i + 3, lo.d1 + i + 3, lo.d2 + i + 3, lo.d3 + i + 3, tmp_vt0 + 3);
  }

  for (; i < size; i++) {
      
    s0 = two_sum(lo.d0[i], ro.d0[i], &t0);
    s1 = two_sum(lo.d1[i], ro.d1[i], &t1);
    s2 = two_sum(lo.d2[i], ro.d2[i], &t2);
    s3 = two_sum(lo.d3[i], ro.d3[i], &t3);

    s1 = two_sum(s1, t0, &t0);
    three_sum(&s2, &t0, &t1);
    three_sum2(&s3, &t0, &t2);
    t0 = t0 + t1 + t3;

    renorm5_inline(&s0, &s1, &s2, &s3, &t0);
    lo.d0[i] = s0;
    lo.d1[i] = s1;
    lo.d2[i] = s2;
    lo.d3[i] = s3;
  }
  _mm_free(tmp_vt0);
  _mm_free(tmp_vt0_x0);
  _mm_free(tmp_vt0_x1);
  _mm_free(tmp_vt0_x2);
  _mm_free(tmp_vt0_x3);
}

void qd_arr_add_inplace_vec_inline_x6(qd_arr lo, qd_arr ro) {
  //assert(lo.size == ro.size);
  int size = lo.size;

  double s0, s1, s2, s3;
  double t0, t1, t2, t3;
  __m256d vs0, vs1, vs2, vs3;
  __m256d vs0_x0, vs1_x0, vs2_x0, vs3_x0;
  __m256d vs0_x1, vs1_x1, vs2_x1, vs3_x1;
  __m256d vs0_x2, vs1_x2, vs2_x2, vs3_x2;
  __m256d vs0_x3, vs1_x3, vs2_x3, vs3_x3;
  __m256d vs0_x4, vs1_x4, vs2_x4, vs3_x4;
  __m256d vs0_x5, vs1_x5, vs2_x5, vs3_x5;

  __m256d vt0, vt1, vt2, vt3;
  __m256d vt0_x0, vt1_x0, vt2_x0, vt3_x0;
  __m256d vt0_x1, vt1_x1, vt2_x1, vt3_x1;
  __m256d vt0_x2, vt1_x2, vt2_x2, vt3_x2;
  __m256d vt0_x3, vt1_x3, vt2_x3, vt3_x3;
  __m256d vt0_x4, vt1_x4, vt2_x4, vt3_x4;
  __m256d vt0_x5, vt1_x5, vt2_x5, vt3_x5;

  __m256d ld0, ld1, ld2, ld3;
  __m256d ld0_x0, ld1_x0, ld2_x0, ld3_x0;
  __m256d ld0_x1, ld1_x1, ld2_x1, ld3_x1;
  __m256d ld0_x2, ld1_x2, ld2_x2, ld3_x2;
  __m256d ld0_x3, ld1_x3, ld2_x3, ld3_x3;
  __m256d ld0_x4, ld1_x4, ld2_x4, ld3_x4;
  __m256d ld0_x5, ld1_x5, ld2_x5, ld3_x5;

  __m256d rd0, rd1, rd2, rd3;
  __m256d rd0_x0, rd1_x0, rd2_x0, rd3_x0;
  __m256d rd0_x1, rd1_x1, rd2_x1, rd3_x1;
  __m256d rd0_x2, rd1_x2, rd2_x2, rd3_x2;
  __m256d rd0_x3, rd1_x3, rd2_x3, rd3_x3;
  __m256d rd0_x4, rd1_x4, rd2_x4, rd3_x4;
  __m256d rd0_x5, rd1_x5, rd2_x5, rd3_x5;

  __m256d tmp_s, tmp_t1, tmp_t2, tmp_t3, tmp_bb, tmp0, tmp1;
  __m256d tmp_s_x0, tmp_t1_x0, tmp_t2_x0, tmp_t3_x0, tmp_bb_x0, tmp0_x0, tmp1_x0;
  __m256d tmp_s_x1, tmp_t1_x1, tmp_t2_x1, tmp_t3_x1, tmp_bb_x1, tmp0_x1, tmp1_x1;
  __m256d tmp_s_x2, tmp_t1_x2, tmp_t2_x2, tmp_t3_x2, tmp_bb_x2, tmp0_x2, tmp1_x2;
  __m256d tmp_s_x3, tmp_t1_x3, tmp_t2_x3, tmp_t3_x3, tmp_bb_x3, tmp0_x3, tmp1_x3;
  __m256d tmp_s_x4, tmp_t1_x4, tmp_t2_x4, tmp_t3_x4, tmp_bb_x4, tmp0_x4, tmp1_x4;
  __m256d tmp_s_x5, tmp_t1_x5, tmp_t2_x5, tmp_t3_x5, tmp_bb_x5, tmp0_x5, tmp1_x5;
  double *tmp_vt0 = (double *)_mm_malloc(4 * sizeof(double), ALIGNMENT);
  double *tmp_vt0_x0 = (double *)_mm_malloc(4 * sizeof(double), ALIGNMENT);
  double *tmp_vt0_x1 = (double *)_mm_malloc(4 * sizeof(double), ALIGNMENT);
  double *tmp_vt0_x2 = (double *)_mm_malloc(4 * sizeof(double), ALIGNMENT);
  double *tmp_vt0_x3 = (double *)_mm_malloc(4 * sizeof(double), ALIGNMENT);
  double *tmp_vt0_x4 = (double *)_mm_malloc(4 * sizeof(double), ALIGNMENT);
  double *tmp_vt0_x5 = (double *)_mm_malloc(4 * sizeof(double), ALIGNMENT);
  int i;
  for (i = 0; i + 31 < size; i += 32) {
    ld0_x0 = _mm256_load_pd(lo.d0 + i);
    ld0_x1 = _mm256_load_pd(lo.d0 + i + 4);
    ld0_x2 = _mm256_load_pd(lo.d0 + i + 8);
    ld0_x3 = _mm256_load_pd(lo.d0 + i + 12);
    ld0_x4 = _mm256_load_pd(lo.d0 + i + 16);
    ld0_x5 = _mm256_load_pd(lo.d0 + i + 20);
    ld1_x0 = _mm256_load_pd(lo.d1 + i);
    ld1_x1 = _mm256_load_pd(lo.d1 + i + 4);
    ld1_x2 = _mm256_load_pd(lo.d1 + i + 8);
    ld1_x3 = _mm256_load_pd(lo.d1 + i + 12);
    ld1_x4 = _mm256_load_pd(lo.d1 + i + 16);
    ld1_x5 = _mm256_load_pd(lo.d1 + i + 20);
    ld2_x0 = _mm256_load_pd(lo.d2 + i);
    ld2_x1 = _mm256_load_pd(lo.d2 + i + 4);
    ld2_x2 = _mm256_load_pd(lo.d2 + i + 8);
    ld2_x3 = _mm256_load_pd(lo.d2 + i + 12);
    ld2_x4 = _mm256_load_pd(lo.d2 + i + 16);
    ld2_x5 = _mm256_load_pd(lo.d2 + i + 20);
    ld3_x0 = _mm256_load_pd(lo.d3 + i);
    ld3_x1 = _mm256_load_pd(lo.d3 + i + 4);
    ld3_x2 = _mm256_load_pd(lo.d3 + i + 8);
    ld3_x3 = _mm256_load_pd(lo.d3 + i + 12);
    ld3_x4 = _mm256_load_pd(lo.d3 + i + 16);
    ld3_x5 = _mm256_load_pd(lo.d3 + i + 20);
    
    
    rd0_x0 = _mm256_load_pd(ro.d0 + i);
    rd0_x1 = _mm256_load_pd(ro.d0 + i + 4);
    rd0_x2 = _mm256_load_pd(ro.d0 + i + 8);
    rd0_x3 = _mm256_load_pd(ro.d0 + i + 12);
    rd0_x4 = _mm256_load_pd(ro.d0 + i + 16);
    rd0_x5 = _mm256_load_pd(ro.d0 + i + 20);
    rd1_x0 = _mm256_load_pd(ro.d1 + i);
    rd1_x1 = _mm256_load_pd(ro.d1 + i + 4);
    rd1_x2 = _mm256_load_pd(ro.d1 + i + 8);
    rd1_x3 = _mm256_load_pd(ro.d1 + i + 12);
    rd1_x4 = _mm256_load_pd(ro.d1 + i + 16);
    rd1_x5 = _mm256_load_pd(ro.d1 + i + 20);
    rd2_x0 = _mm256_load_pd(ro.d2 + i);
    rd2_x1 = _mm256_load_pd(ro.d2 + i + 4);
    rd2_x2 = _mm256_load_pd(ro.d2 + i + 8);
    rd2_x3 = _mm256_load_pd(ro.d2 + i + 12);
    rd2_x4 = _mm256_load_pd(ro.d2 + i + 16);
    rd2_x5 = _mm256_load_pd(ro.d2 + i + 20);
    rd3_x0 = _mm256_load_pd(ro.d3 + i);
    rd3_x1 = _mm256_load_pd(ro.d3 + i + 4);
    rd3_x2 = _mm256_load_pd(ro.d3 + i + 8);
    rd3_x3 = _mm256_load_pd(ro.d3 + i + 12);
    rd3_x4 = _mm256_load_pd(ro.d3 + i + 16);
    rd3_x5 = _mm256_load_pd(ro.d3 + i + 20);

    //vs0 = two_sum_vec(ld0, rd0, &vt0);
    tmp_s_x0 = _mm256_add_pd(ld0_x0, rd0_x0);
    tmp_s_x1 = _mm256_add_pd(ld0_x1, rd0_x1);
    tmp_s_x2 = _mm256_add_pd(ld0_x2, rd0_x2);
    tmp_s_x3 = _mm256_add_pd(ld0_x3, rd0_x3);
    tmp_s_x4 = _mm256_add_pd(ld0_x4, rd0_x4);
    tmp_s_x5 = _mm256_add_pd(ld0_x5, rd0_x5);

    tmp_bb_x0 = _mm256_sub_pd(tmp_s_x0, ld0_x0);
    tmp_bb_x1 = _mm256_sub_pd(tmp_s_x1, ld0_x1);
    tmp_bb_x2 = _mm256_sub_pd(tmp_s_x2, ld0_x2);
    tmp_bb_x3 = _mm256_sub_pd(tmp_s_x3, ld0_x3);
    tmp_bb_x4 = _mm256_sub_pd(tmp_s_x4, ld0_x4);
    tmp_bb_x5 = _mm256_sub_pd(tmp_s_x5, ld0_x5);

    tmp0_x0 = _mm256_sub_pd(tmp_s_x0, tmp_bb_x0);
    tmp0_x1 = _mm256_sub_pd(tmp_s_x1, tmp_bb_x1);
    tmp0_x2 = _mm256_sub_pd(tmp_s_x2, tmp_bb_x2);
    tmp0_x3 = _mm256_sub_pd(tmp_s_x3, tmp_bb_x3);
    tmp0_x4 = _mm256_sub_pd(tmp_s_x4, tmp_bb_x4);
    tmp0_x5 = _mm256_sub_pd(tmp_s_x5, tmp_bb_x5);
    
    tmp1_x0 = _mm256_sub_pd(rd0_x0, tmp_bb_x0);
    tmp1_x1 = _mm256_sub_pd(rd0_x1, tmp_bb_x1);
    tmp1_x2 = _mm256_sub_pd(rd0_x2, tmp_bb_x2);
    tmp1_x3 = _mm256_sub_pd(rd0_x3, tmp_bb_x3);
    tmp1_x4 = _mm256_sub_pd(rd0_x4, tmp_bb_x4);
    tmp1_x5 = _mm256_sub_pd(rd0_x5, tmp_bb_x5);
    
    tmp0_x0 = _mm256_sub_pd(ld0_x0, tmp0_x0);
    tmp0_x1 = _mm256_sub_pd(ld0_x1, tmp0_x1);
    tmp0_x2 = _mm256_sub_pd(ld0_x2, tmp0_x2);
    tmp0_x3 = _mm256_sub_pd(ld0_x3, tmp0_x3);
    tmp0_x4 = _mm256_sub_pd(ld0_x4, tmp0_x4);
    tmp0_x5 = _mm256_sub_pd(ld0_x5, tmp0_x5);

    vt0_x0 = _mm256_add_pd(tmp0_x0, tmp1_x0);
    vt0_x1 = _mm256_add_pd(tmp0_x1, tmp1_x1);
    vt0_x2 = _mm256_add_pd(tmp0_x2, tmp1_x2);
    vt0_x3 = _mm256_add_pd(tmp0_x3, tmp1_x3);
    vt0_x4 = _mm256_add_pd(tmp0_x4, tmp1_x4);
    vt0_x5 = _mm256_add_pd(tmp0_x5, tmp1_x5);

    vs0_x0 = tmp_s_x0;
    vs0_x1 = tmp_s_x1;
    vs0_x2 = tmp_s_x2;
    vs0_x3 = tmp_s_x3;
    vs0_x4 = tmp_s_x4;
    vs0_x5 = tmp_s_x5;

    //vs1 = two_sum_vec(ld1, rd1, &vt1);
    tmp_s_x0 = _mm256_add_pd(ld1_x0, rd1_x0);
    tmp_s_x1 = _mm256_add_pd(ld1_x1, rd1_x1);
    tmp_s_x2 = _mm256_add_pd(ld1_x2, rd1_x2);
    tmp_s_x3 = _mm256_add_pd(ld1_x3, rd1_x3);
    tmp_s_x4 = _mm256_add_pd(ld1_x4, rd1_x4);
    tmp_s_x5 = _mm256_add_pd(ld1_x5, rd1_x5);

    tmp_bb_x0 = _mm256_sub_pd(tmp_s_x0, ld1_x0);
    tmp_bb_x1 = _mm256_sub_pd(tmp_s_x1, ld1_x1);
    tmp_bb_x2 = _mm256_sub_pd(tmp_s_x2, ld1_x2);
    tmp_bb_x3 = _mm256_sub_pd(tmp_s_x3, ld1_x3);
    tmp_bb_x4 = _mm256_sub_pd(tmp_s_x4, ld1_x4);
    tmp_bb_x5 = _mm256_sub_pd(tmp_s_x5, ld1_x5);

    tmp0_x0 = _mm256_sub_pd(tmp_s_x0, tmp_bb_x0);
    tmp0_x1 = _mm256_sub_pd(tmp_s_x1, tmp_bb_x1);
    tmp0_x2 = _mm256_sub_pd(tmp_s_x2, tmp_bb_x2);
    tmp0_x3 = _mm256_sub_pd(tmp_s_x3, tmp_bb_x3);
    tmp0_x4 = _mm256_sub_pd(tmp_s_x4, tmp_bb_x4);
    tmp0_x5 = _mm256_sub_pd(tmp_s_x5, tmp_bb_x5);
    
    tmp1_x0 = _mm256_sub_pd(rd1_x0, tmp_bb_x0);
    tmp1_x1 = _mm256_sub_pd(rd1_x1, tmp_bb_x1);
    tmp1_x2 = _mm256_sub_pd(rd1_x2, tmp_bb_x2);
    tmp1_x3 = _mm256_sub_pd(rd1_x3, tmp_bb_x3);
    tmp1_x4 = _mm256_sub_pd(rd1_x4, tmp_bb_x4);
    tmp1_x5 = _mm256_sub_pd(rd1_x5, tmp_bb_x5);
    
    tmp0_x0 = _mm256_sub_pd(ld1_x0, tmp0_x0);
    tmp0_x1 = _mm256_sub_pd(ld1_x1, tmp0_x1);
    tmp0_x2 = _mm256_sub_pd(ld1_x2, tmp0_x2);
    tmp0_x3 = _mm256_sub_pd(ld1_x3, tmp0_x3);
    tmp0_x4 = _mm256_sub_pd(ld1_x4, tmp0_x4);
    tmp0_x5 = _mm256_sub_pd(ld1_x5, tmp0_x5);

    vt1_x0 = _mm256_add_pd(tmp0_x0, tmp1_x0);
    vt1_x1 = _mm256_add_pd(tmp0_x1, tmp1_x1);
    vt1_x2 = _mm256_add_pd(tmp0_x2, tmp1_x2);
    vt1_x3 = _mm256_add_pd(tmp0_x3, tmp1_x3);
    vt1_x4 = _mm256_add_pd(tmp0_x4, tmp1_x4);
    vt1_x5 = _mm256_add_pd(tmp0_x5, tmp1_x5);

    vs1_x0 = tmp_s_x0;
    vs1_x1 = tmp_s_x1;
    vs1_x2 = tmp_s_x2;
    vs1_x3 = tmp_s_x3;
    vs1_x4 = tmp_s_x4;
    vs1_x5 = tmp_s_x5;

    //vs2 = two_sum_vec(ld2, rd2, &vt2);
    tmp_s_x0 = _mm256_add_pd(ld2_x0, rd2_x0);
    tmp_s_x1 = _mm256_add_pd(ld2_x1, rd2_x1);
    tmp_s_x2 = _mm256_add_pd(ld2_x2, rd2_x2);
    tmp_s_x3 = _mm256_add_pd(ld2_x3, rd2_x3);
    tmp_s_x4 = _mm256_add_pd(ld2_x4, rd2_x4);
    tmp_s_x5 = _mm256_add_pd(ld2_x5, rd2_x5);

    tmp_bb_x0 = _mm256_sub_pd(tmp_s_x0, ld2_x0);
    tmp_bb_x1 = _mm256_sub_pd(tmp_s_x1, ld2_x1);
    tmp_bb_x2 = _mm256_sub_pd(tmp_s_x2, ld2_x2);
    tmp_bb_x3 = _mm256_sub_pd(tmp_s_x3, ld2_x3);
    tmp_bb_x4 = _mm256_sub_pd(tmp_s_x4, ld2_x4);
    tmp_bb_x5 = _mm256_sub_pd(tmp_s_x5, ld2_x5);

    tmp0_x0 = _mm256_sub_pd(tmp_s_x0, tmp_bb_x0);
    tmp0_x1 = _mm256_sub_pd(tmp_s_x1, tmp_bb_x1);
    tmp0_x2 = _mm256_sub_pd(tmp_s_x2, tmp_bb_x2);
    tmp0_x3 = _mm256_sub_pd(tmp_s_x3, tmp_bb_x3);
    tmp0_x4 = _mm256_sub_pd(tmp_s_x4, tmp_bb_x4);
    tmp0_x5 = _mm256_sub_pd(tmp_s_x5, tmp_bb_x5);
    
    tmp1_x0 = _mm256_sub_pd(rd2_x0, tmp_bb_x0);
    tmp1_x1 = _mm256_sub_pd(rd2_x1, tmp_bb_x1);
    tmp1_x2 = _mm256_sub_pd(rd2_x2, tmp_bb_x2);
    tmp1_x3 = _mm256_sub_pd(rd2_x3, tmp_bb_x3);
    tmp1_x4 = _mm256_sub_pd(rd2_x4, tmp_bb_x4);
    tmp1_x5 = _mm256_sub_pd(rd2_x5, tmp_bb_x5);
    
    tmp0_x0 = _mm256_sub_pd(ld2_x0, tmp0_x0);
    tmp0_x1 = _mm256_sub_pd(ld2_x1, tmp0_x1);
    tmp0_x2 = _mm256_sub_pd(ld2_x2, tmp0_x2);
    tmp0_x3 = _mm256_sub_pd(ld2_x3, tmp0_x3);
    tmp0_x4 = _mm256_sub_pd(ld2_x4, tmp0_x4);
    tmp0_x5 = _mm256_sub_pd(ld2_x5, tmp0_x5);

    vt2_x0 = _mm256_add_pd(tmp0_x0, tmp1_x0);
    vt2_x1 = _mm256_add_pd(tmp0_x1, tmp1_x1);
    vt2_x2 = _mm256_add_pd(tmp0_x2, tmp1_x2);
    vt2_x3 = _mm256_add_pd(tmp0_x3, tmp1_x3);
    vt2_x4 = _mm256_add_pd(tmp0_x4, tmp1_x4);
    vt2_x5 = _mm256_add_pd(tmp0_x5, tmp1_x5);

    vs2_x0 = tmp_s_x0;
    vs2_x1 = tmp_s_x1;
    vs2_x2 = tmp_s_x2;
    vs2_x3 = tmp_s_x3;
    vs2_x4 = tmp_s_x4;
    vs2_x5 = tmp_s_x5;

    //vs3 = two_sum_vec(ld3, rd3, &vt3);
    tmp_s_x0 = _mm256_add_pd(ld3_x0, rd3_x0);
    tmp_s_x1 = _mm256_add_pd(ld3_x1, rd3_x1);
    tmp_s_x2 = _mm256_add_pd(ld3_x2, rd3_x2);
    tmp_s_x3 = _mm256_add_pd(ld3_x3, rd3_x3);
    tmp_s_x4 = _mm256_add_pd(ld3_x4, rd3_x4);
    tmp_s_x5 = _mm256_add_pd(ld3_x5, rd3_x5);

    tmp_bb_x0 = _mm256_sub_pd(tmp_s_x0, ld3_x0);
    tmp_bb_x1 = _mm256_sub_pd(tmp_s_x1, ld3_x1);
    tmp_bb_x2 = _mm256_sub_pd(tmp_s_x2, ld3_x2);
    tmp_bb_x3 = _mm256_sub_pd(tmp_s_x3, ld3_x3);
    tmp_bb_x4 = _mm256_sub_pd(tmp_s_x4, ld3_x4);
    tmp_bb_x5 = _mm256_sub_pd(tmp_s_x5, ld3_x5);

    tmp0_x0 = _mm256_sub_pd(tmp_s_x0, tmp_bb_x0);
    tmp0_x1 = _mm256_sub_pd(tmp_s_x1, tmp_bb_x1);
    tmp0_x2 = _mm256_sub_pd(tmp_s_x2, tmp_bb_x2);
    tmp0_x3 = _mm256_sub_pd(tmp_s_x3, tmp_bb_x3);
    tmp0_x4 = _mm256_sub_pd(tmp_s_x4, tmp_bb_x4);
    tmp0_x5 = _mm256_sub_pd(tmp_s_x5, tmp_bb_x5);
    
    tmp1_x0 = _mm256_sub_pd(rd3_x0, tmp_bb_x0);
    tmp1_x1 = _mm256_sub_pd(rd3_x1, tmp_bb_x1);
    tmp1_x2 = _mm256_sub_pd(rd3_x2, tmp_bb_x2);
    tmp1_x3 = _mm256_sub_pd(rd3_x3, tmp_bb_x3);
    tmp1_x4 = _mm256_sub_pd(rd3_x4, tmp_bb_x4);
    tmp1_x5 = _mm256_sub_pd(rd3_x5, tmp_bb_x5);
    
    tmp0_x0 = _mm256_sub_pd(ld3_x0, tmp0_x0);
    tmp0_x1 = _mm256_sub_pd(ld3_x1, tmp0_x1);
    tmp0_x2 = _mm256_sub_pd(ld3_x2, tmp0_x2);
    tmp0_x3 = _mm256_sub_pd(ld3_x3, tmp0_x3);
    tmp0_x4 = _mm256_sub_pd(ld3_x4, tmp0_x4);
    tmp0_x5 = _mm256_sub_pd(ld3_x5, tmp0_x5);

    vt3_x0 = _mm256_add_pd(tmp0_x0, tmp1_x0);
    vt3_x1 = _mm256_add_pd(tmp0_x1, tmp1_x1);
    vt3_x2 = _mm256_add_pd(tmp0_x2, tmp1_x2);
    vt3_x3 = _mm256_add_pd(tmp0_x3, tmp1_x3);
    vt3_x4 = _mm256_add_pd(tmp0_x4, tmp1_x4);
    vt3_x5 = _mm256_add_pd(tmp0_x5, tmp1_x5);

    vs3_x0 = tmp_s_x0;
    vs3_x1 = tmp_s_x1;
    vs3_x2 = tmp_s_x2;
    vs3_x3 = tmp_s_x3;
    vs3_x4 = tmp_s_x4;
    vs3_x5 = tmp_s_x5;

    //vs1 = two_sum_vec(vs1, vt0, &vt0);
    tmp_s_x0 = _mm256_add_pd(vs1_x0, vt0_x0);
    tmp_s_x1 = _mm256_add_pd(vs1_x1, vt0_x1);
    tmp_s_x2 = _mm256_add_pd(vs1_x2, vt0_x2);
    tmp_s_x3 = _mm256_add_pd(vs1_x3, vt0_x3);
    tmp_s_x4 = _mm256_add_pd(vs1_x4, vt0_x4);
    tmp_s_x5 = _mm256_add_pd(vs1_x5, vt0_x5);

    tmp_bb_x0 = _mm256_sub_pd(tmp_s_x0, vs1_x0);
    tmp_bb_x1 = _mm256_sub_pd(tmp_s_x1, vs1_x1);
    tmp_bb_x2 = _mm256_sub_pd(tmp_s_x2, vs1_x2);
    tmp_bb_x3 = _mm256_sub_pd(tmp_s_x3, vs1_x3);
    tmp_bb_x4 = _mm256_sub_pd(tmp_s_x4, vs1_x4);
    tmp_bb_x5 = _mm256_sub_pd(tmp_s_x5, vs1_x5);

    tmp0_x0 = _mm256_sub_pd(tmp_s_x0, tmp_bb_x0);
    tmp0_x1 = _mm256_sub_pd(tmp_s_x1, tmp_bb_x1);
    tmp0_x2 = _mm256_sub_pd(tmp_s_x2, tmp_bb_x2);
    tmp0_x3 = _mm256_sub_pd(tmp_s_x3, tmp_bb_x3);
    tmp0_x4 = _mm256_sub_pd(tmp_s_x4, tmp_bb_x4);
    tmp0_x5 = _mm256_sub_pd(tmp_s_x5, tmp_bb_x5);
    
    tmp1_x0 = _mm256_sub_pd(vt0_x0, tmp_bb_x0);
    tmp1_x1 = _mm256_sub_pd(vt0_x1, tmp_bb_x1);
    tmp1_x2 = _mm256_sub_pd(vt0_x2, tmp_bb_x2);
    tmp1_x3 = _mm256_sub_pd(vt0_x3, tmp_bb_x3);
    tmp1_x4 = _mm256_sub_pd(vt0_x4, tmp_bb_x4);
    tmp1_x5 = _mm256_sub_pd(vt0_x5, tmp_bb_x5);
    
    tmp0_x0 = _mm256_sub_pd(vs1_x0, tmp0_x0);
    tmp0_x1 = _mm256_sub_pd(vs1_x1, tmp0_x1);
    tmp0_x2 = _mm256_sub_pd(vs1_x2, tmp0_x2);
    tmp0_x3 = _mm256_sub_pd(vs1_x3, tmp0_x3);
    tmp0_x4 = _mm256_sub_pd(vs1_x4, tmp0_x4);
    tmp0_x5 = _mm256_sub_pd(vs1_x5, tmp0_x5);

    vt0_x0 = _mm256_add_pd(tmp0_x0, tmp1_x0);
    vt0_x1 = _mm256_add_pd(tmp0_x1, tmp1_x1);
    vt0_x2 = _mm256_add_pd(tmp0_x2, tmp1_x2);
    vt0_x3 = _mm256_add_pd(tmp0_x3, tmp1_x3);
    vt0_x4 = _mm256_add_pd(tmp0_x4, tmp1_x4);
    vt0_x5 = _mm256_add_pd(tmp0_x5, tmp1_x5);

    vs1_x0 = tmp_s_x0;
    vs1_x1 = tmp_s_x1;
    vs1_x2 = tmp_s_x2;
    vs1_x3 = tmp_s_x3;
    vs1_x4 = tmp_s_x4;
    vs1_x5 = tmp_s_x5;

    //three_sum_vec(&vs2, &vt0, &vt1);

    /* tmp_s = _mm256_add_pd(vs2, vt0);
    tmp_bb = _mm256_sub_pd(tmp_s, vs2);
    tmp0 = _mm256_sub_pd(tmp_s, tmp_bb);
    tmp1 = _mm256_sub_pd(vt0, tmp_bb);
    tmp0 = _mm256_sub_pd(vs2, tmp0);
    tmp_t2 = _mm256_add_pd(tmp0, tmp1);
    tmp_t1 = tmp_s; */
    tmp_s_x0 = _mm256_add_pd(vs2_x0, vt0_x0);
    tmp_s_x1 = _mm256_add_pd(vs2_x1, vt0_x1);
    tmp_s_x2 = _mm256_add_pd(vs2_x2, vt0_x2);
    tmp_s_x3 = _mm256_add_pd(vs2_x3, vt0_x3);
    tmp_s_x4 = _mm256_add_pd(vs2_x4, vt0_x4);
    tmp_s_x5 = _mm256_add_pd(vs2_x5, vt0_x5);

    tmp_bb_x0 = _mm256_sub_pd(tmp_s_x0, vs2_x0);
    tmp_bb_x1 = _mm256_sub_pd(tmp_s_x1, vs2_x1);
    tmp_bb_x2 = _mm256_sub_pd(tmp_s_x2, vs2_x2);
    tmp_bb_x3 = _mm256_sub_pd(tmp_s_x3, vs2_x3);
    tmp_bb_x4 = _mm256_sub_pd(tmp_s_x4, vs2_x4);
    tmp_bb_x5 = _mm256_sub_pd(tmp_s_x5, vs2_x5);

    tmp0_x0 = _mm256_sub_pd(tmp_s_x0, tmp_bb_x0);
    tmp0_x1 = _mm256_sub_pd(tmp_s_x1, tmp_bb_x1);
    tmp0_x2 = _mm256_sub_pd(tmp_s_x2, tmp_bb_x2);
    tmp0_x3 = _mm256_sub_pd(tmp_s_x3, tmp_bb_x3);
    tmp0_x4 = _mm256_sub_pd(tmp_s_x4, tmp_bb_x4);
    tmp0_x5 = _mm256_sub_pd(tmp_s_x5, tmp_bb_x5);
    
    tmp1_x0 = _mm256_sub_pd(vt0_x0, tmp_bb_x0);
    tmp1_x1 = _mm256_sub_pd(vt0_x1, tmp_bb_x1);
    tmp1_x2 = _mm256_sub_pd(vt0_x2, tmp_bb_x2);
    tmp1_x3 = _mm256_sub_pd(vt0_x3, tmp_bb_x3);
    tmp1_x4 = _mm256_sub_pd(vt0_x4, tmp_bb_x4);
    tmp1_x5 = _mm256_sub_pd(vt0_x5, tmp_bb_x5);
    
    tmp0_x0 = _mm256_sub_pd(vs2_x0, tmp0_x0);
    tmp0_x1 = _mm256_sub_pd(vs2_x1, tmp0_x1);
    tmp0_x2 = _mm256_sub_pd(vs2_x2, tmp0_x2);
    tmp0_x3 = _mm256_sub_pd(vs2_x3, tmp0_x3);
    tmp0_x4 = _mm256_sub_pd(vs2_x4, tmp0_x4);
    tmp0_x5 = _mm256_sub_pd(vs2_x5, tmp0_x5);

    tmp_t2_x0 = _mm256_add_pd(tmp0_x0, tmp1_x0);
    tmp_t2_x1 = _mm256_add_pd(tmp0_x1, tmp1_x1);
    tmp_t2_x2 = _mm256_add_pd(tmp0_x2, tmp1_x2);
    tmp_t2_x3 = _mm256_add_pd(tmp0_x3, tmp1_x3);
    tmp_t2_x4 = _mm256_add_pd(tmp0_x4, tmp1_x4);
    tmp_t2_x5 = _mm256_add_pd(tmp0_x5, tmp1_x5);

    tmp_t1_x0 = tmp_s_x0;
    tmp_t1_x1 = tmp_s_x1;
    tmp_t1_x2 = tmp_s_x2;
    tmp_t1_x3 = tmp_s_x3;
    tmp_t1_x4 = tmp_s_x4;
    tmp_t1_x5 = tmp_s_x5;

    /* tmp_s = _mm256_add_pd(vt1, tmp_t1);
    tmp_bb = _mm256_sub_pd(tmp_s, vt1);
    tmp0 = _mm256_sub_pd(tmp_s, tmp_bb);
    tmp1 = _mm256_sub_pd(tmp_t1, tmp_bb);
    tmp0 = _mm256_sub_pd(vt1, tmp0);
    tmp_t3 = _mm256_add_pd(tmp0, tmp1);
    vs2 = tmp_s; */

    tmp_s_x0 = _mm256_add_pd(vt1_x0, tmp_t1_x0);
    tmp_s_x1 = _mm256_add_pd(vt1_x1, tmp_t1_x1);
    tmp_s_x2 = _mm256_add_pd(vt1_x2, tmp_t1_x2);
    tmp_s_x3 = _mm256_add_pd(vt1_x3, tmp_t1_x3);
    tmp_s_x4 = _mm256_add_pd(vt1_x4, tmp_t1_x4);
    tmp_s_x5 = _mm256_add_pd(vt1_x5, tmp_t1_x5);

    tmp_bb_x0 = _mm256_sub_pd(tmp_s_x0, vt1_x0);
    tmp_bb_x1 = _mm256_sub_pd(tmp_s_x1, vt1_x1);
    tmp_bb_x2 = _mm256_sub_pd(tmp_s_x2, vt1_x2);
    tmp_bb_x3 = _mm256_sub_pd(tmp_s_x3, vt1_x3);
    tmp_bb_x4 = _mm256_sub_pd(tmp_s_x4, vt1_x4);
    tmp_bb_x5 = _mm256_sub_pd(tmp_s_x5, vt1_x5);

    tmp0_x0 = _mm256_sub_pd(tmp_s_x0, tmp_bb_x0);
    tmp0_x1 = _mm256_sub_pd(tmp_s_x1, tmp_bb_x1);
    tmp0_x2 = _mm256_sub_pd(tmp_s_x2, tmp_bb_x2);
    tmp0_x3 = _mm256_sub_pd(tmp_s_x3, tmp_bb_x3);
    tmp0_x4 = _mm256_sub_pd(tmp_s_x4, tmp_bb_x4);
    tmp0_x5 = _mm256_sub_pd(tmp_s_x5, tmp_bb_x5);
    
    tmp1_x0 = _mm256_sub_pd(tmp_t1_x0, tmp_bb_x0);
    tmp1_x1 = _mm256_sub_pd(tmp_t1_x1, tmp_bb_x1);
    tmp1_x2 = _mm256_sub_pd(tmp_t1_x2, tmp_bb_x2);
    tmp1_x3 = _mm256_sub_pd(tmp_t1_x3, tmp_bb_x3);
    tmp1_x4 = _mm256_sub_pd(tmp_t1_x4, tmp_bb_x4);
    tmp1_x5 = _mm256_sub_pd(tmp_t1_x5, tmp_bb_x5);
    
    tmp0_x0 = _mm256_sub_pd(vt1_x0, tmp0_x0);
    tmp0_x1 = _mm256_sub_pd(vt1_x1, tmp0_x1);
    tmp0_x2 = _mm256_sub_pd(vt1_x2, tmp0_x2);
    tmp0_x3 = _mm256_sub_pd(vt1_x3, tmp0_x3);
    tmp0_x4 = _mm256_sub_pd(vt1_x4, tmp0_x4);
    tmp0_x5 = _mm256_sub_pd(vt1_x5, tmp0_x5);

    tmp_t3_x0 = _mm256_add_pd(tmp0_x0, tmp1_x0);
    tmp_t3_x1 = _mm256_add_pd(tmp0_x1, tmp1_x1);
    tmp_t3_x2 = _mm256_add_pd(tmp0_x2, tmp1_x2);
    tmp_t3_x3 = _mm256_add_pd(tmp0_x3, tmp1_x3);
    tmp_t3_x4 = _mm256_add_pd(tmp0_x4, tmp1_x4);
    tmp_t3_x5 = _mm256_add_pd(tmp0_x5, tmp1_x5);

    vs2_x0 = tmp_s_x0;
    vs2_x1 = tmp_s_x1;
    vs2_x2 = tmp_s_x2;
    vs2_x3 = tmp_s_x3;
    vs2_x4 = tmp_s_x4;
    vs2_x5 = tmp_s_x5;


    /* tmp_s = _mm256_add_pd(tmp_t2, tmp_t3);
    tmp_bb = _mm256_sub_pd(tmp_s, tmp_t2);
    tmp0 = _mm256_sub_pd(tmp_s, tmp_bb);
    tmp1 = _mm256_sub_pd(tmp_t3, tmp_bb);
    tmp0 = _mm256_sub_pd(tmp_t2, tmp0);
    vt1 = _mm256_add_pd(tmp0, tmp1);
    vt0 = tmp_s; */

    tmp_s_x0 = _mm256_add_pd(tmp_t2_x0, tmp_t3_x0);
    tmp_s_x1 = _mm256_add_pd(tmp_t2_x1, tmp_t3_x1);
    tmp_s_x2 = _mm256_add_pd(tmp_t2_x2, tmp_t3_x2);
    tmp_s_x3 = _mm256_add_pd(tmp_t2_x3, tmp_t3_x3);
    tmp_s_x4 = _mm256_add_pd(tmp_t2_x4, tmp_t3_x4);
    tmp_s_x5 = _mm256_add_pd(tmp_t2_x5, tmp_t3_x5);

    tmp_bb_x0 = _mm256_sub_pd(tmp_s_x0, tmp_t2_x0);
    tmp_bb_x1 = _mm256_sub_pd(tmp_s_x1, tmp_t2_x1);
    tmp_bb_x2 = _mm256_sub_pd(tmp_s_x2, tmp_t2_x2);
    tmp_bb_x3 = _mm256_sub_pd(tmp_s_x3, tmp_t2_x3);
    tmp_bb_x4 = _mm256_sub_pd(tmp_s_x4, tmp_t2_x4);
    tmp_bb_x5 = _mm256_sub_pd(tmp_s_x5, tmp_t2_x5);

    tmp0_x0 = _mm256_sub_pd(tmp_s_x0, tmp_bb_x0);
    tmp0_x1 = _mm256_sub_pd(tmp_s_x1, tmp_bb_x1);
    tmp0_x2 = _mm256_sub_pd(tmp_s_x2, tmp_bb_x2);
    tmp0_x3 = _mm256_sub_pd(tmp_s_x3, tmp_bb_x3);
    tmp0_x4 = _mm256_sub_pd(tmp_s_x4, tmp_bb_x4);
    tmp0_x5 = _mm256_sub_pd(tmp_s_x5, tmp_bb_x5);
    
    tmp1_x0 = _mm256_sub_pd(tmp_t3_x0, tmp_bb_x0);
    tmp1_x1 = _mm256_sub_pd(tmp_t3_x1, tmp_bb_x1);
    tmp1_x2 = _mm256_sub_pd(tmp_t3_x2, tmp_bb_x2);
    tmp1_x3 = _mm256_sub_pd(tmp_t3_x3, tmp_bb_x3);
    tmp1_x4 = _mm256_sub_pd(tmp_t3_x4, tmp_bb_x4);
    tmp1_x5 = _mm256_sub_pd(tmp_t3_x5, tmp_bb_x5);
    
    tmp0_x0 = _mm256_sub_pd(tmp_t2_x0, tmp0_x0);
    tmp0_x1 = _mm256_sub_pd(tmp_t2_x1, tmp0_x1);
    tmp0_x2 = _mm256_sub_pd(tmp_t2_x2, tmp0_x2);
    tmp0_x3 = _mm256_sub_pd(tmp_t2_x3, tmp0_x3);
    tmp0_x4 = _mm256_sub_pd(tmp_t2_x4, tmp0_x4);
    tmp0_x5 = _mm256_sub_pd(tmp_t2_x5, tmp0_x5);

    vt1_x0 = _mm256_add_pd(tmp0_x0, tmp1_x0);
    vt1_x1 = _mm256_add_pd(tmp0_x1, tmp1_x1);
    vt1_x2 = _mm256_add_pd(tmp0_x2, tmp1_x2);
    vt1_x3 = _mm256_add_pd(tmp0_x3, tmp1_x3);
    vt1_x4 = _mm256_add_pd(tmp0_x4, tmp1_x4);
    vt1_x5 = _mm256_add_pd(tmp0_x5, tmp1_x5);

    vt0_x0 = tmp_s_x0;
    vt0_x1 = tmp_s_x1;
    vt0_x2 = tmp_s_x2;
    vt0_x3 = tmp_s_x3;
    vt0_x4 = tmp_s_x4;
    vt0_x5 = tmp_s_x5;



    //three_sum2_vec(&vs3, &vt0, &vt2);

    /* tmp_s = _mm256_add_pd(vs3, vt0);
    tmp_bb = _mm256_sub_pd(tmp_s, vs3);
    tmp0 = _mm256_sub_pd(tmp_s, tmp_bb);
    tmp1 = _mm256_sub_pd(vt0, tmp_bb);
    tmp0 = _mm256_sub_pd(vs3, tmp0);
    tmp_t2 = _mm256_add_pd(tmp0, tmp1);
    tmp_t1 = tmp_s; */
    tmp_s_x0 = _mm256_add_pd(vs3_x0, vt0_x0);
    tmp_s_x1 = _mm256_add_pd(vs3_x1, vt0_x1);
    tmp_s_x2 = _mm256_add_pd(vs3_x2, vt0_x2);
    tmp_s_x3 = _mm256_add_pd(vs3_x3, vt0_x3);
    tmp_s_x4 = _mm256_add_pd(vs3_x4, vt0_x4);
    tmp_s_x5 = _mm256_add_pd(vs3_x5, vt0_x5);

    tmp_bb_x0 = _mm256_sub_pd(tmp_s_x0, vs3_x0);
    tmp_bb_x1 = _mm256_sub_pd(tmp_s_x1, vs3_x1);
    tmp_bb_x2 = _mm256_sub_pd(tmp_s_x2, vs3_x2);
    tmp_bb_x3 = _mm256_sub_pd(tmp_s_x3, vs3_x3);
    tmp_bb_x4 = _mm256_sub_pd(tmp_s_x4, vs3_x4);
    tmp_bb_x5 = _mm256_sub_pd(tmp_s_x5, vs3_x5);

    tmp0_x0 = _mm256_sub_pd(tmp_s_x0, tmp_bb_x0);
    tmp0_x1 = _mm256_sub_pd(tmp_s_x1, tmp_bb_x1);
    tmp0_x2 = _mm256_sub_pd(tmp_s_x2, tmp_bb_x2);
    tmp0_x3 = _mm256_sub_pd(tmp_s_x3, tmp_bb_x3);
    tmp0_x4 = _mm256_sub_pd(tmp_s_x4, tmp_bb_x4);
    tmp0_x5 = _mm256_sub_pd(tmp_s_x5, tmp_bb_x5);
    
    tmp1_x0 = _mm256_sub_pd(vt0_x0, tmp_bb_x0);
    tmp1_x1 = _mm256_sub_pd(vt0_x1, tmp_bb_x1);
    tmp1_x2 = _mm256_sub_pd(vt0_x2, tmp_bb_x2);
    tmp1_x3 = _mm256_sub_pd(vt0_x3, tmp_bb_x3);
    tmp1_x4 = _mm256_sub_pd(vt0_x4, tmp_bb_x4);
    tmp1_x5 = _mm256_sub_pd(vt0_x5, tmp_bb_x5);
    
    tmp0_x0 = _mm256_sub_pd(vs3_x0, tmp0_x0);
    tmp0_x1 = _mm256_sub_pd(vs3_x1, tmp0_x1);
    tmp0_x2 = _mm256_sub_pd(vs3_x2, tmp0_x2);
    tmp0_x3 = _mm256_sub_pd(vs3_x3, tmp0_x3);
    tmp0_x4 = _mm256_sub_pd(vs3_x4, tmp0_x4);
    tmp0_x5 = _mm256_sub_pd(vs3_x5, tmp0_x5);

    tmp_t2_x0 = _mm256_add_pd(tmp0_x0, tmp1_x0);
    tmp_t2_x1 = _mm256_add_pd(tmp0_x1, tmp1_x1);
    tmp_t2_x2 = _mm256_add_pd(tmp0_x2, tmp1_x2);
    tmp_t2_x3 = _mm256_add_pd(tmp0_x3, tmp1_x3);
    tmp_t2_x4 = _mm256_add_pd(tmp0_x4, tmp1_x4);
    tmp_t2_x5 = _mm256_add_pd(tmp0_x5, tmp1_x5);

    tmp_t1_x0 = tmp_s_x0;
    tmp_t1_x1 = tmp_s_x1;
    tmp_t1_x2 = tmp_s_x2;
    tmp_t1_x3 = tmp_s_x3;
    tmp_t1_x4 = tmp_s_x4;
    tmp_t1_x5 = tmp_s_x5;
    /* tmp_s = _mm256_add_pd(vt2, tmp_t1);
    tmp_bb = _mm256_sub_pd(tmp_s, vt2);
    tmp0 = _mm256_sub_pd(tmp_s, tmp_bb);
    tmp1 = _mm256_sub_pd(tmp_t1, tmp_bb);
    tmp0 = _mm256_sub_pd(vt2, tmp0);
    tmp_t3 = _mm256_add_pd(tmp0, tmp1);
    vs3 = tmp_s; */
    tmp_s_x0 = _mm256_add_pd(vt2_x0, tmp_t1_x0);
    tmp_s_x1 = _mm256_add_pd(vt2_x1, tmp_t1_x1);
    tmp_s_x2 = _mm256_add_pd(vt2_x2, tmp_t1_x2);
    tmp_s_x3 = _mm256_add_pd(vt2_x3, tmp_t1_x3);
    tmp_s_x4 = _mm256_add_pd(vt2_x4, tmp_t1_x4);
    tmp_s_x5 = _mm256_add_pd(vt2_x5, tmp_t1_x5);

    tmp_bb_x0 = _mm256_sub_pd(tmp_s_x0, vt2_x0);
    tmp_bb_x1 = _mm256_sub_pd(tmp_s_x1, vt2_x1);
    tmp_bb_x2 = _mm256_sub_pd(tmp_s_x2, vt2_x2);
    tmp_bb_x3 = _mm256_sub_pd(tmp_s_x3, vt2_x3);
    tmp_bb_x4 = _mm256_sub_pd(tmp_s_x4, vt2_x4);
    tmp_bb_x5 = _mm256_sub_pd(tmp_s_x5, vt2_x5);

    tmp0_x0 = _mm256_sub_pd(tmp_s_x0, tmp_bb_x0);
    tmp0_x1 = _mm256_sub_pd(tmp_s_x1, tmp_bb_x1);
    tmp0_x2 = _mm256_sub_pd(tmp_s_x2, tmp_bb_x2);
    tmp0_x3 = _mm256_sub_pd(tmp_s_x3, tmp_bb_x3);
    tmp0_x4 = _mm256_sub_pd(tmp_s_x4, tmp_bb_x4);
    tmp0_x5 = _mm256_sub_pd(tmp_s_x5, tmp_bb_x5);
    
    tmp1_x0 = _mm256_sub_pd(tmp_t1_x0, tmp_bb_x0);
    tmp1_x1 = _mm256_sub_pd(tmp_t1_x1, tmp_bb_x1);
    tmp1_x2 = _mm256_sub_pd(tmp_t1_x2, tmp_bb_x2);
    tmp1_x3 = _mm256_sub_pd(tmp_t1_x3, tmp_bb_x3);
    tmp1_x4 = _mm256_sub_pd(tmp_t1_x4, tmp_bb_x4);
    tmp1_x5 = _mm256_sub_pd(tmp_t1_x5, tmp_bb_x5);
    
    tmp0_x0 = _mm256_sub_pd(vt2_x0, tmp0_x0);
    tmp0_x1 = _mm256_sub_pd(vt2_x1, tmp0_x1);
    tmp0_x2 = _mm256_sub_pd(vt2_x2, tmp0_x2);
    tmp0_x3 = _mm256_sub_pd(vt2_x3, tmp0_x3);
    tmp0_x4 = _mm256_sub_pd(vt2_x4, tmp0_x4);
    tmp0_x5 = _mm256_sub_pd(vt2_x5, tmp0_x5);

    tmp_t3_x0 = _mm256_add_pd(tmp0_x0, tmp1_x0);
    tmp_t3_x1 = _mm256_add_pd(tmp0_x1, tmp1_x1);
    tmp_t3_x2 = _mm256_add_pd(tmp0_x2, tmp1_x2);
    tmp_t3_x3 = _mm256_add_pd(tmp0_x3, tmp1_x3);
    tmp_t3_x4 = _mm256_add_pd(tmp0_x4, tmp1_x4);
    tmp_t3_x5 = _mm256_add_pd(tmp0_x5, tmp1_x5);

    vs3_x0 = tmp_s_x0;
    vs3_x1 = tmp_s_x1;
    vs3_x2 = tmp_s_x2;
    vs3_x3 = tmp_s_x3;
    vs3_x4 = tmp_s_x4;
    vs3_x5 = tmp_s_x5;

    /* vt0 = _mm256_add_pd(tmp_t2, tmp_t3); */
    vt0_x0 = _mm256_add_pd(tmp_t2_x0, tmp_t3_x0);
    vt0_x1 = _mm256_add_pd(tmp_t2_x1, tmp_t3_x1);
    vt0_x2 = _mm256_add_pd(tmp_t2_x2, tmp_t3_x2);
    vt0_x3 = _mm256_add_pd(tmp_t2_x3, tmp_t3_x3);
    vt0_x4 = _mm256_add_pd(tmp_t2_x4, tmp_t3_x4);
    vt0_x5 = _mm256_add_pd(tmp_t2_x5, tmp_t3_x5);

    //vt0 = _mm256_add_pd(_mm256_add_pd(vt0, vt1), vt3);
    /* tmp0 = _mm256_add_pd(vt0, vt1); */
    tmp0_x0 = _mm256_add_pd(vt0_x0, vt1_x0);
    tmp0_x1 = _mm256_add_pd(vt0_x1, vt1_x1);
    tmp0_x2 = _mm256_add_pd(vt0_x2, vt1_x2);
    tmp0_x3 = _mm256_add_pd(vt0_x3, vt1_x3);
    tmp0_x4 = _mm256_add_pd(vt0_x4, vt1_x4);
    tmp0_x5 = _mm256_add_pd(vt0_x5, vt1_x5);

    /* vt0 = _mm256_add_pd(tmp0, vt3); */
    vt0_x0 = _mm256_add_pd(tmp0_x0, vt3_x0);
    vt0_x1 = _mm256_add_pd(tmp0_x1, vt3_x1);
    vt0_x2 = _mm256_add_pd(tmp0_x2, vt3_x2);
    vt0_x3 = _mm256_add_pd(tmp0_x3, vt3_x3);
    vt0_x4 = _mm256_add_pd(tmp0_x4, vt3_x4);
    vt0_x5 = _mm256_add_pd(tmp0_x5, vt3_x5);

    //renorm5(&s0, &s1, &s2, &s3, &t0);
    /* _mm256_store_pd(lo.d0 + i, vs0);
    _mm256_store_pd(lo.d1 + i, vs1);
    _mm256_store_pd(lo.d2 + i, vs2);
    _mm256_store_pd(lo.d3 + i, vs3);
    _mm256_store_pd(tmp_vt0, vt0); */
    _mm256_store_pd(lo.d0 + i, vs0_x0);
    _mm256_store_pd(lo.d0 + i + 4, vs0_x1);
    _mm256_store_pd(lo.d0 + i + 8, vs0_x2);
    _mm256_store_pd(lo.d0 + i + 12, vs0_x3);
    _mm256_store_pd(lo.d0 + i + 16, vs0_x4);
    _mm256_store_pd(lo.d0 + i + 20, vs0_x5);
    _mm256_store_pd(lo.d1 + i, vs1_x0);
    _mm256_store_pd(lo.d1 + i + 4, vs1_x1);
    _mm256_store_pd(lo.d1 + i + 8, vs1_x2);
    _mm256_store_pd(lo.d1 + i + 12, vs1_x3);
    _mm256_store_pd(lo.d1 + i + 16, vs1_x4);
    _mm256_store_pd(lo.d1 + i + 20, vs1_x5);
    _mm256_store_pd(lo.d2 + i, vs2_x0);
    _mm256_store_pd(lo.d2 + i + 4, vs2_x1);
    _mm256_store_pd(lo.d2 + i + 8, vs2_x2);
    _mm256_store_pd(lo.d2 + i + 12, vs2_x3);
    _mm256_store_pd(lo.d2 + i + 16, vs2_x4);
    _mm256_store_pd(lo.d2 + i + 20, vs2_x5);
    _mm256_store_pd(lo.d3 + i, vs3_x0);
    _mm256_store_pd(lo.d3 + i + 4, vs3_x1);
    _mm256_store_pd(lo.d3 + i + 8, vs3_x2);
    _mm256_store_pd(lo.d3 + i + 12, vs3_x3);
    _mm256_store_pd(lo.d3 + i + 16, vs3_x4);
    _mm256_store_pd(lo.d3 + i + 20, vs3_x5);
    _mm256_store_pd(tmp_vt0_x0, vt0_x0);
    _mm256_store_pd(tmp_vt0_x1, vt0_x1);
    _mm256_store_pd(tmp_vt0_x2, vt0_x2);
    _mm256_store_pd(tmp_vt0_x3, vt0_x3);
    _mm256_store_pd(tmp_vt0_x4, vt0_x4);
    _mm256_store_pd(tmp_vt0_x5, vt0_x5);
    /* renorm5_inline(lo.d0 + i, lo.d1 + i, lo.d2 + i, lo.d3 + i, tmp_vt0);
    renorm5_inline(lo.d0 + i + 1, lo.d1 + i + 1, lo.d2 + i + 1, lo.d3 + i + 1, tmp_vt0 + 1);
    renorm5_inline(lo.d0 + i + 2, lo.d1 + i + 2, lo.d2 + i + 2, lo.d3 + i + 2, tmp_vt0 + 2);
    renorm5_inline(lo.d0 + i + 3, lo.d1 + i + 3, lo.d2 + i + 3, lo.d3 + i + 3, tmp_vt0 + 3); */
    renorm5_inline(lo.d0 + i, lo.d1 + i, lo.d2 + i, lo.d3 + i, tmp_vt0_x0);
    renorm5_inline(lo.d0 + i + 1, lo.d1 + i + 1, lo.d2 + i + 1, lo.d3 + i + 1, tmp_vt0_x0 + 1);
    renorm5_inline(lo.d0 + i + 2, lo.d1 + i + 2, lo.d2 + i + 2, lo.d3 + i + 2, tmp_vt0_x0 + 2);
    renorm5_inline(lo.d0 + i + 3, lo.d1 + i + 3, lo.d2 + i + 3, lo.d3 + i + 3, tmp_vt0_x0 + 3);
    renorm5_inline(lo.d0 + i + 4, lo.d1 + i + 4, lo.d2 + i + 4, lo.d3 + i + 4, tmp_vt0_x1);
    renorm5_inline(lo.d0 + i + 5, lo.d1 + i + 5, lo.d2 + i + 5, lo.d3 + i + 5, tmp_vt0_x1 + 1);
    renorm5_inline(lo.d0 + i + 6, lo.d1 + i + 6, lo.d2 + i + 6, lo.d3 + i + 6, tmp_vt0_x1 + 2);
    renorm5_inline(lo.d0 + i + 7, lo.d1 + i + 7, lo.d2 + i + 7, lo.d3 + i + 7, tmp_vt0_x1 + 3);
    renorm5_inline(lo.d0 + i + 8, lo.d1 + i + 8, lo.d2 + i + 8, lo.d3 + i + 8, tmp_vt0_x2);
    renorm5_inline(lo.d0 + i + 9, lo.d1 + i + 9, lo.d2 + i + 9, lo.d3 + i + 9, tmp_vt0_x2 + 1);
    renorm5_inline(lo.d0 + i + 10, lo.d1 + i + 10, lo.d2 + i + 10, lo.d3 + i + 10, tmp_vt0_x2 + 2);
    renorm5_inline(lo.d0 + i + 11, lo.d1 + i + 11, lo.d2 + i + 11, lo.d3 + i + 11, tmp_vt0_x2 + 3);
    renorm5_inline(lo.d0 + i + 12, lo.d1 + i + 12, lo.d2 + i + 12, lo.d3 + i + 12, tmp_vt0_x3);
    renorm5_inline(lo.d0 + i + 13, lo.d1 + i + 13, lo.d2 + i + 13, lo.d3 + i + 13, tmp_vt0_x3 + 1);
    renorm5_inline(lo.d0 + i + 14, lo.d1 + i + 14, lo.d2 + i + 14, lo.d3 + i + 14, tmp_vt0_x3 + 2);
    renorm5_inline(lo.d0 + i + 15, lo.d1 + i + 15, lo.d2 + i + 15, lo.d3 + i + 15, tmp_vt0_x3 + 3);
    renorm5_inline(lo.d0 + i + 16, lo.d1 + i + 16, lo.d2 + i + 16, lo.d3 + i + 16, tmp_vt0_x4);
    renorm5_inline(lo.d0 + i + 17, lo.d1 + i + 17, lo.d2 + i + 17, lo.d3 + i + 17, tmp_vt0_x4 + 1);
    renorm5_inline(lo.d0 + i + 18, lo.d1 + i + 18, lo.d2 + i + 18, lo.d3 + i + 18, tmp_vt0_x4 + 2);
    renorm5_inline(lo.d0 + i + 19, lo.d1 + i + 19, lo.d2 + i + 19, lo.d3 + i + 19, tmp_vt0_x4 + 3);
    renorm5_inline(lo.d0 + i + 20, lo.d1 + i + 20, lo.d2 + i + 20, lo.d3 + i + 20, tmp_vt0_x5);
    renorm5_inline(lo.d0 + i + 21, lo.d1 + i + 21, lo.d2 + i + 21, lo.d3 + i + 21, tmp_vt0_x5 + 1);
    renorm5_inline(lo.d0 + i + 22, lo.d1 + i + 22, lo.d2 + i + 22, lo.d3 + i + 22, tmp_vt0_x5 + 2);
    renorm5_inline(lo.d0 + i + 23, lo.d1 + i + 23, lo.d2 + i + 23, lo.d3 + i + 23, tmp_vt0_x5 + 3);
  }

  for (; i + 3 < size; i += 4) {
    ld0 = _mm256_load_pd(lo.d0 + i);
    ld1 = _mm256_load_pd(lo.d1 + i);
    ld2 = _mm256_load_pd(lo.d2 + i);
    ld3 = _mm256_load_pd(lo.d3 + i);
    rd0 = _mm256_load_pd(ro.d0 + i);
    rd1 = _mm256_load_pd(ro.d1 + i);
    rd2 = _mm256_load_pd(ro.d2 + i);
    rd3 = _mm256_load_pd(ro.d3 + i);

    //vs0 = two_sum_vec(ld0, rd0, &vt0);
    tmp_s = _mm256_add_pd(ld0, rd0);
    tmp_bb = _mm256_sub_pd(tmp_s, ld0);
    tmp0 = _mm256_sub_pd(tmp_s, tmp_bb);
    tmp1 = _mm256_sub_pd(rd0, tmp_bb);
    tmp0 = _mm256_sub_pd(ld0, tmp0);
    vt0 = _mm256_add_pd(tmp0, tmp1);
    vs0 = tmp_s;

    //vs1 = two_sum_vec(ld1, rd1, &vt1);
    tmp_s = _mm256_add_pd(ld1, rd1);
    tmp_bb = _mm256_sub_pd(tmp_s, ld1);
    tmp0 = _mm256_sub_pd(tmp_s, tmp_bb);
    tmp1 = _mm256_sub_pd(rd1, tmp_bb);
    tmp0 = _mm256_sub_pd(ld1, tmp0);
    vt1 = _mm256_add_pd(tmp0, tmp1);
    vs1 = tmp_s;

    //vs2 = two_sum_vec(ld2, rd2, &vt2);
    tmp_s = _mm256_add_pd(ld2, rd2);
    tmp_bb = _mm256_sub_pd(tmp_s, ld2);
    tmp0 = _mm256_sub_pd(tmp_s, tmp_bb);
    tmp1 = _mm256_sub_pd(rd2, tmp_bb);
    tmp0 = _mm256_sub_pd(ld2, tmp0);
    vt2 = _mm256_add_pd(tmp0, tmp1);
    vs2 = tmp_s;

    //vs3 = two_sum_vec(ld3, rd3, &vt3);
    tmp_s = _mm256_add_pd(ld3, rd3);
    tmp_bb = _mm256_sub_pd(tmp_s, ld3);
    tmp0 = _mm256_sub_pd(tmp_s, tmp_bb);
    tmp1 = _mm256_sub_pd(rd3, tmp_bb);
    tmp0 = _mm256_sub_pd(ld3, tmp0);
    vt3 = _mm256_add_pd(tmp0, tmp1);
    vs3 = tmp_s;

    //vs1 = two_sum_vec(vs1, vt0, &vt0);
    tmp_s = _mm256_add_pd(vs1, vt0);
    tmp_bb = _mm256_sub_pd(tmp_s, vs1);
    tmp0 = _mm256_sub_pd(tmp_s, tmp_bb);
    tmp1 = _mm256_sub_pd(vt0, tmp_bb);
    tmp0 = _mm256_sub_pd(vs1, tmp0);
    vt0 = _mm256_add_pd(tmp0, tmp1);
    vs1 = tmp_s;

    //three_sum_vec(&vs2, &vt0, &vt1);
    tmp_s = _mm256_add_pd(vs2, vt0);
    tmp_bb = _mm256_sub_pd(tmp_s, vs2);
    tmp0 = _mm256_sub_pd(tmp_s, tmp_bb);
    tmp1 = _mm256_sub_pd(vt0, tmp_bb);
    tmp0 = _mm256_sub_pd(vs2, tmp0);
    tmp_t2 = _mm256_add_pd(tmp0, tmp1);
    tmp_t1 = tmp_s;
    tmp_s = _mm256_add_pd(vt1, tmp_t1);
    tmp_bb = _mm256_sub_pd(tmp_s, vt1);
    tmp0 = _mm256_sub_pd(tmp_s, tmp_bb);
    tmp1 = _mm256_sub_pd(tmp_t1, tmp_bb);
    tmp0 = _mm256_sub_pd(vt1, tmp0);
    tmp_t3 = _mm256_add_pd(tmp0, tmp1);
    vs2 = tmp_s;
    tmp_s = _mm256_add_pd(tmp_t2, tmp_t3);
    tmp_bb = _mm256_sub_pd(tmp_s, tmp_t2);
    tmp0 = _mm256_sub_pd(tmp_s, tmp_bb);
    tmp1 = _mm256_sub_pd(tmp_t3, tmp_bb);
    tmp0 = _mm256_sub_pd(tmp_t2, tmp0);
    vt1 = _mm256_add_pd(tmp0, tmp1);
    vt0 = tmp_s;

    //three_sum2_vec(&vs3, &vt0, &vt2);
    tmp_s = _mm256_add_pd(vs3, vt0);
    tmp_bb = _mm256_sub_pd(tmp_s, vs3);
    tmp0 = _mm256_sub_pd(tmp_s, tmp_bb);
    tmp1 = _mm256_sub_pd(vt0, tmp_bb);
    tmp0 = _mm256_sub_pd(vs3, tmp0);
    tmp_t2 = _mm256_add_pd(tmp0, tmp1);
    tmp_t1 = tmp_s;
    tmp_s = _mm256_add_pd(vt2, tmp_t1);
    tmp_bb = _mm256_sub_pd(tmp_s, vt2);
    tmp0 = _mm256_sub_pd(tmp_s, tmp_bb);
    tmp1 = _mm256_sub_pd(tmp_t1, tmp_bb);
    tmp0 = _mm256_sub_pd(vt2, tmp0);
    tmp_t3 = _mm256_add_pd(tmp0, tmp1);
    vs3 = tmp_s;
    vt0 = _mm256_add_pd(tmp_t2, tmp_t3);

    //vt0 = _mm256_add_pd(_mm256_add_pd(vt0, vt1), vt3);
    tmp0 = _mm256_add_pd(vt0, vt1);
    vt0 = _mm256_add_pd(tmp0, vt3);

    //renorm5(&s0, &s1, &s2, &s3, &t0);
    _mm256_store_pd(lo.d0 + i, vs0);
    _mm256_store_pd(lo.d1 + i, vs1);
    _mm256_store_pd(lo.d2 + i, vs2);
    _mm256_store_pd(lo.d3 + i, vs3);
    _mm256_store_pd(tmp_vt0, vt0);
    renorm5_inline(lo.d0 + i, lo.d1 + i, lo.d2 + i, lo.d3 + i, tmp_vt0);
    renorm5_inline(lo.d0 + i + 1, lo.d1 + i + 1, lo.d2 + i + 1, lo.d3 + i + 1, tmp_vt0 + 1);
    renorm5_inline(lo.d0 + i + 2, lo.d1 + i + 2, lo.d2 + i + 2, lo.d3 + i + 2, tmp_vt0 + 2);
    renorm5_inline(lo.d0 + i + 3, lo.d1 + i + 3, lo.d2 + i + 3, lo.d3 + i + 3, tmp_vt0 + 3);
  }

  for (; i < size; i++) {
      
    s0 = two_sum(lo.d0[i], ro.d0[i], &t0);
    s1 = two_sum(lo.d1[i], ro.d1[i], &t1);
    s2 = two_sum(lo.d2[i], ro.d2[i], &t2);
    s3 = two_sum(lo.d3[i], ro.d3[i], &t3);

    s1 = two_sum(s1, t0, &t0);
    three_sum(&s2, &t0, &t1);
    three_sum2(&s3, &t0, &t2);
    t0 = t0 + t1 + t3;

    renorm5_inline(&s0, &s1, &s2, &s3, &t0);
    lo.d0[i] = s0;
    lo.d1[i] = s1;
    lo.d2[i] = s2;
    lo.d3[i] = s3;
  }
  _mm_free(tmp_vt0);
  _mm_free(tmp_vt0_x0);
  _mm_free(tmp_vt0_x1);
  _mm_free(tmp_vt0_x2);
  _mm_free(tmp_vt0_x3);
  _mm_free(tmp_vt0_x4);
  _mm_free(tmp_vt0_x5);
}

void qd_arr_add_inplace_vec_inline_x8(qd_arr lo, qd_arr ro) {
  //assert(lo.size == ro.size);
  int size = lo.size;

  double s0, s1, s2, s3;
  double t0, t1, t2, t3;
  __m256d vs0, vs1, vs2, vs3;
  __m256d vs0_x0, vs1_x0, vs2_x0, vs3_x0;
  __m256d vs0_x1, vs1_x1, vs2_x1, vs3_x1;
  __m256d vs0_x2, vs1_x2, vs2_x2, vs3_x2;
  __m256d vs0_x3, vs1_x3, vs2_x3, vs3_x3;
  __m256d vs0_x4, vs1_x4, vs2_x4, vs3_x4;
  __m256d vs0_x5, vs1_x5, vs2_x5, vs3_x5;
  __m256d vs0_x6, vs1_x6, vs2_x6, vs3_x6;
  __m256d vs0_x7, vs1_x7, vs2_x7, vs3_x7;

  __m256d vt0, vt1, vt2, vt3;
  __m256d vt0_x0, vt1_x0, vt2_x0, vt3_x0;
  __m256d vt0_x1, vt1_x1, vt2_x1, vt3_x1;
  __m256d vt0_x2, vt1_x2, vt2_x2, vt3_x2;
  __m256d vt0_x3, vt1_x3, vt2_x3, vt3_x3;
  __m256d vt0_x4, vt1_x4, vt2_x4, vt3_x4;
  __m256d vt0_x5, vt1_x5, vt2_x5, vt3_x5;
  __m256d vt0_x6, vt1_x6, vt2_x6, vt3_x6;
  __m256d vt0_x7, vt1_x7, vt2_x7, vt3_x7;

  __m256d ld0, ld1, ld2, ld3;
  __m256d ld0_x0, ld1_x0, ld2_x0, ld3_x0;
  __m256d ld0_x1, ld1_x1, ld2_x1, ld3_x1;
  __m256d ld0_x2, ld1_x2, ld2_x2, ld3_x2;
  __m256d ld0_x3, ld1_x3, ld2_x3, ld3_x3;
  __m256d ld0_x4, ld1_x4, ld2_x4, ld3_x4;
  __m256d ld0_x5, ld1_x5, ld2_x5, ld3_x5;
  __m256d ld0_x6, ld1_x6, ld2_x6, ld3_x6;
  __m256d ld0_x7, ld1_x7, ld2_x7, ld3_x7;

  __m256d rd0, rd1, rd2, rd3;
  __m256d rd0_x0, rd1_x0, rd2_x0, rd3_x0;
  __m256d rd0_x1, rd1_x1, rd2_x1, rd3_x1;
  __m256d rd0_x2, rd1_x2, rd2_x2, rd3_x2;
  __m256d rd0_x3, rd1_x3, rd2_x3, rd3_x3;
  __m256d rd0_x4, rd1_x4, rd2_x4, rd3_x4;
  __m256d rd0_x5, rd1_x5, rd2_x5, rd3_x5;
  __m256d rd0_x6, rd1_x6, rd2_x6, rd3_x6;
  __m256d rd0_x7, rd1_x7, rd2_x7, rd3_x7;

  __m256d tmp_s, tmp_t1, tmp_t2, tmp_t3, tmp_bb, tmp0, tmp1;
  __m256d tmp_s_x0, tmp_t1_x0, tmp_t2_x0, tmp_t3_x0, tmp_bb_x0, tmp0_x0, tmp1_x0;
  __m256d tmp_s_x1, tmp_t1_x1, tmp_t2_x1, tmp_t3_x1, tmp_bb_x1, tmp0_x1, tmp1_x1;
  __m256d tmp_s_x2, tmp_t1_x2, tmp_t2_x2, tmp_t3_x2, tmp_bb_x2, tmp0_x2, tmp1_x2;
  __m256d tmp_s_x3, tmp_t1_x3, tmp_t2_x3, tmp_t3_x3, tmp_bb_x3, tmp0_x3, tmp1_x3;
  __m256d tmp_s_x4, tmp_t1_x4, tmp_t2_x4, tmp_t3_x4, tmp_bb_x4, tmp0_x4, tmp1_x4;
  __m256d tmp_s_x5, tmp_t1_x5, tmp_t2_x5, tmp_t3_x5, tmp_bb_x5, tmp0_x5, tmp1_x5;
  __m256d tmp_s_x6, tmp_t1_x6, tmp_t2_x6, tmp_t3_x6, tmp_bb_x6, tmp0_x6, tmp1_x6;
  __m256d tmp_s_x7, tmp_t1_x7, tmp_t2_x7, tmp_t3_x7, tmp_bb_x7, tmp0_x7, tmp1_x7;
  double *tmp_vt0 = (double *)_mm_malloc(4 * sizeof(double), ALIGNMENT);
  double *tmp_vt0_x0 = (double *)_mm_malloc(4 * sizeof(double), ALIGNMENT);
  double *tmp_vt0_x1 = (double *)_mm_malloc(4 * sizeof(double), ALIGNMENT);
  double *tmp_vt0_x2 = (double *)_mm_malloc(4 * sizeof(double), ALIGNMENT);
  double *tmp_vt0_x3 = (double *)_mm_malloc(4 * sizeof(double), ALIGNMENT);
  double *tmp_vt0_x4 = (double *)_mm_malloc(4 * sizeof(double), ALIGNMENT);
  double *tmp_vt0_x5 = (double *)_mm_malloc(4 * sizeof(double), ALIGNMENT);
  double *tmp_vt0_x6 = (double *)_mm_malloc(4 * sizeof(double), ALIGNMENT);
  double *tmp_vt0_x7 = (double *)_mm_malloc(4 * sizeof(double), ALIGNMENT);
  int i;
  for (i = 0; i + 31 < size; i += 32) {
    ld0_x0 = _mm256_load_pd(lo.d0 + i);
    ld0_x1 = _mm256_load_pd(lo.d0 + i + 4);
    ld0_x2 = _mm256_load_pd(lo.d0 + i + 8);
    ld0_x3 = _mm256_load_pd(lo.d0 + i + 12);
    ld0_x4 = _mm256_load_pd(lo.d0 + i + 16);
    ld0_x5 = _mm256_load_pd(lo.d0 + i + 20);
    ld0_x6 = _mm256_load_pd(lo.d0 + i + 24);
    ld0_x7 = _mm256_load_pd(lo.d0 + i + 28);
    ld1_x0 = _mm256_load_pd(lo.d1 + i);
    ld1_x1 = _mm256_load_pd(lo.d1 + i + 4);
    ld1_x2 = _mm256_load_pd(lo.d1 + i + 8);
    ld1_x3 = _mm256_load_pd(lo.d1 + i + 12);
    ld1_x4 = _mm256_load_pd(lo.d1 + i + 16);
    ld1_x5 = _mm256_load_pd(lo.d1 + i + 20);
    ld1_x6 = _mm256_load_pd(lo.d1 + i + 24);
    ld1_x7 = _mm256_load_pd(lo.d1 + i + 28);
    ld2_x0 = _mm256_load_pd(lo.d2 + i);
    ld2_x1 = _mm256_load_pd(lo.d2 + i + 4);
    ld2_x2 = _mm256_load_pd(lo.d2 + i + 8);
    ld2_x3 = _mm256_load_pd(lo.d2 + i + 12);
    ld2_x4 = _mm256_load_pd(lo.d2 + i + 16);
    ld2_x5 = _mm256_load_pd(lo.d2 + i + 20);
    ld2_x6 = _mm256_load_pd(lo.d2 + i + 24);
    ld2_x7 = _mm256_load_pd(lo.d2 + i + 28);
    ld3_x0 = _mm256_load_pd(lo.d3 + i);
    ld3_x1 = _mm256_load_pd(lo.d3 + i + 4);
    ld3_x2 = _mm256_load_pd(lo.d3 + i + 8);
    ld3_x3 = _mm256_load_pd(lo.d3 + i + 12);
    ld3_x4 = _mm256_load_pd(lo.d3 + i + 16);
    ld3_x5 = _mm256_load_pd(lo.d3 + i + 20);
    ld3_x6 = _mm256_load_pd(lo.d3 + i + 24);
    ld3_x7 = _mm256_load_pd(lo.d3 + i + 28);
    
    
    rd0_x0 = _mm256_load_pd(ro.d0 + i);
    rd0_x1 = _mm256_load_pd(ro.d0 + i + 4);
    rd0_x2 = _mm256_load_pd(ro.d0 + i + 8);
    rd0_x3 = _mm256_load_pd(ro.d0 + i + 12);
    rd0_x4 = _mm256_load_pd(ro.d0 + i + 16);
    rd0_x5 = _mm256_load_pd(ro.d0 + i + 20);
    rd0_x6 = _mm256_load_pd(ro.d0 + i + 24);
    rd0_x7 = _mm256_load_pd(ro.d0 + i + 28);
    rd1_x0 = _mm256_load_pd(ro.d1 + i);
    rd1_x1 = _mm256_load_pd(ro.d1 + i + 4);
    rd1_x2 = _mm256_load_pd(ro.d1 + i + 8);
    rd1_x3 = _mm256_load_pd(ro.d1 + i + 12);
    rd1_x4 = _mm256_load_pd(ro.d1 + i + 16);
    rd1_x5 = _mm256_load_pd(ro.d1 + i + 20);
    rd1_x6 = _mm256_load_pd(ro.d1 + i + 24);
    rd1_x7 = _mm256_load_pd(ro.d1 + i + 28);
    rd2_x0 = _mm256_load_pd(ro.d2 + i);
    rd2_x1 = _mm256_load_pd(ro.d2 + i + 4);
    rd2_x2 = _mm256_load_pd(ro.d2 + i + 8);
    rd2_x3 = _mm256_load_pd(ro.d2 + i + 12);
    rd2_x4 = _mm256_load_pd(ro.d2 + i + 16);
    rd2_x5 = _mm256_load_pd(ro.d2 + i + 20);
    rd2_x6 = _mm256_load_pd(ro.d2 + i + 24);
    rd2_x7 = _mm256_load_pd(ro.d2 + i + 28);
    rd3_x0 = _mm256_load_pd(ro.d3 + i);
    rd3_x1 = _mm256_load_pd(ro.d3 + i + 4);
    rd3_x2 = _mm256_load_pd(ro.d3 + i + 8);
    rd3_x3 = _mm256_load_pd(ro.d3 + i + 12);
    rd3_x4 = _mm256_load_pd(ro.d3 + i + 16);
    rd3_x5 = _mm256_load_pd(ro.d3 + i + 20);
    rd3_x6 = _mm256_load_pd(ro.d3 + i + 24);
    rd3_x7 = _mm256_load_pd(ro.d3 + i + 28);

    //vs0 = two_sum_vec(ld0, rd0, &vt0);
    tmp_s_x0 = _mm256_add_pd(ld0_x0, rd0_x0);
    tmp_s_x1 = _mm256_add_pd(ld0_x1, rd0_x1);
    tmp_s_x2 = _mm256_add_pd(ld0_x2, rd0_x2);
    tmp_s_x3 = _mm256_add_pd(ld0_x3, rd0_x3);
    tmp_s_x4 = _mm256_add_pd(ld0_x4, rd0_x4);
    tmp_s_x5 = _mm256_add_pd(ld0_x5, rd0_x5);
    tmp_s_x6 = _mm256_add_pd(ld0_x6, rd0_x6);
    tmp_s_x7 = _mm256_add_pd(ld0_x7, rd0_x7);

    tmp_bb_x0 = _mm256_sub_pd(tmp_s_x0, ld0_x0);
    tmp_bb_x1 = _mm256_sub_pd(tmp_s_x1, ld0_x1);
    tmp_bb_x2 = _mm256_sub_pd(tmp_s_x2, ld0_x2);
    tmp_bb_x3 = _mm256_sub_pd(tmp_s_x3, ld0_x3);
    tmp_bb_x4 = _mm256_sub_pd(tmp_s_x4, ld0_x4);
    tmp_bb_x5 = _mm256_sub_pd(tmp_s_x5, ld0_x5);
    tmp_bb_x6 = _mm256_sub_pd(tmp_s_x6, ld0_x6);
    tmp_bb_x7 = _mm256_sub_pd(tmp_s_x7, ld0_x7);

    tmp0_x0 = _mm256_sub_pd(tmp_s_x0, tmp_bb_x0);
    tmp0_x1 = _mm256_sub_pd(tmp_s_x1, tmp_bb_x1);
    tmp0_x2 = _mm256_sub_pd(tmp_s_x2, tmp_bb_x2);
    tmp0_x3 = _mm256_sub_pd(tmp_s_x3, tmp_bb_x3);
    tmp0_x4 = _mm256_sub_pd(tmp_s_x4, tmp_bb_x4);
    tmp0_x5 = _mm256_sub_pd(tmp_s_x5, tmp_bb_x5);
    tmp0_x6 = _mm256_sub_pd(tmp_s_x6, tmp_bb_x6);
    tmp0_x7 = _mm256_sub_pd(tmp_s_x7, tmp_bb_x7);
    
    tmp1_x0 = _mm256_sub_pd(rd0_x0, tmp_bb_x0);
    tmp1_x1 = _mm256_sub_pd(rd0_x1, tmp_bb_x1);
    tmp1_x2 = _mm256_sub_pd(rd0_x2, tmp_bb_x2);
    tmp1_x3 = _mm256_sub_pd(rd0_x3, tmp_bb_x3);
    tmp1_x4 = _mm256_sub_pd(rd0_x4, tmp_bb_x4);
    tmp1_x5 = _mm256_sub_pd(rd0_x5, tmp_bb_x5);
    tmp1_x6 = _mm256_sub_pd(rd0_x6, tmp_bb_x6);
    tmp1_x7 = _mm256_sub_pd(rd0_x7, tmp_bb_x7);
    
    tmp0_x0 = _mm256_sub_pd(ld0_x0, tmp0_x0);
    tmp0_x1 = _mm256_sub_pd(ld0_x1, tmp0_x1);
    tmp0_x2 = _mm256_sub_pd(ld0_x2, tmp0_x2);
    tmp0_x3 = _mm256_sub_pd(ld0_x3, tmp0_x3);
    tmp0_x4 = _mm256_sub_pd(ld0_x4, tmp0_x4);
    tmp0_x5 = _mm256_sub_pd(ld0_x5, tmp0_x5);
    tmp0_x6 = _mm256_sub_pd(ld0_x6, tmp0_x6);
    tmp0_x7 = _mm256_sub_pd(ld0_x7, tmp0_x7);

    vt0_x0 = _mm256_add_pd(tmp0_x0, tmp1_x0);
    vt0_x1 = _mm256_add_pd(tmp0_x1, tmp1_x1);
    vt0_x2 = _mm256_add_pd(tmp0_x2, tmp1_x2);
    vt0_x3 = _mm256_add_pd(tmp0_x3, tmp1_x3);
    vt0_x4 = _mm256_add_pd(tmp0_x4, tmp1_x4);
    vt0_x5 = _mm256_add_pd(tmp0_x5, tmp1_x5);
    vt0_x6 = _mm256_add_pd(tmp0_x6, tmp1_x6);
    vt0_x7 = _mm256_add_pd(tmp0_x7, tmp1_x7);

    vs0_x0 = tmp_s_x0;
    vs0_x1 = tmp_s_x1;
    vs0_x2 = tmp_s_x2;
    vs0_x3 = tmp_s_x3;
    vs0_x4 = tmp_s_x4;
    vs0_x5 = tmp_s_x5;
    vs0_x6 = tmp_s_x6;
    vs0_x7 = tmp_s_x7;

    //vs1 = two_sum_vec(ld1, rd1, &vt1);
    tmp_s_x0 = _mm256_add_pd(ld1_x0, rd1_x0);
    tmp_s_x1 = _mm256_add_pd(ld1_x1, rd1_x1);
    tmp_s_x2 = _mm256_add_pd(ld1_x2, rd1_x2);
    tmp_s_x3 = _mm256_add_pd(ld1_x3, rd1_x3);
    tmp_s_x4 = _mm256_add_pd(ld1_x4, rd1_x4);
    tmp_s_x5 = _mm256_add_pd(ld1_x5, rd1_x5);
    tmp_s_x6 = _mm256_add_pd(ld1_x6, rd1_x6);
    tmp_s_x7 = _mm256_add_pd(ld1_x7, rd1_x7);

    tmp_bb_x0 = _mm256_sub_pd(tmp_s_x0, ld1_x0);
    tmp_bb_x1 = _mm256_sub_pd(tmp_s_x1, ld1_x1);
    tmp_bb_x2 = _mm256_sub_pd(tmp_s_x2, ld1_x2);
    tmp_bb_x3 = _mm256_sub_pd(tmp_s_x3, ld1_x3);
    tmp_bb_x4 = _mm256_sub_pd(tmp_s_x4, ld1_x4);
    tmp_bb_x5 = _mm256_sub_pd(tmp_s_x5, ld1_x5);
    tmp_bb_x6 = _mm256_sub_pd(tmp_s_x6, ld1_x6);
    tmp_bb_x7 = _mm256_sub_pd(tmp_s_x7, ld1_x7);

    tmp0_x0 = _mm256_sub_pd(tmp_s_x0, tmp_bb_x0);
    tmp0_x1 = _mm256_sub_pd(tmp_s_x1, tmp_bb_x1);
    tmp0_x2 = _mm256_sub_pd(tmp_s_x2, tmp_bb_x2);
    tmp0_x3 = _mm256_sub_pd(tmp_s_x3, tmp_bb_x3);
    tmp0_x4 = _mm256_sub_pd(tmp_s_x4, tmp_bb_x4);
    tmp0_x5 = _mm256_sub_pd(tmp_s_x5, tmp_bb_x5);
    tmp0_x6 = _mm256_sub_pd(tmp_s_x6, tmp_bb_x6);
    tmp0_x7 = _mm256_sub_pd(tmp_s_x7, tmp_bb_x7);
    
    tmp1_x0 = _mm256_sub_pd(rd1_x0, tmp_bb_x0);
    tmp1_x1 = _mm256_sub_pd(rd1_x1, tmp_bb_x1);
    tmp1_x2 = _mm256_sub_pd(rd1_x2, tmp_bb_x2);
    tmp1_x3 = _mm256_sub_pd(rd1_x3, tmp_bb_x3);
    tmp1_x4 = _mm256_sub_pd(rd1_x4, tmp_bb_x4);
    tmp1_x5 = _mm256_sub_pd(rd1_x5, tmp_bb_x5);
    tmp1_x6 = _mm256_sub_pd(rd1_x6, tmp_bb_x6);
    tmp1_x7 = _mm256_sub_pd(rd1_x7, tmp_bb_x7);
    
    tmp0_x0 = _mm256_sub_pd(ld1_x0, tmp0_x0);
    tmp0_x1 = _mm256_sub_pd(ld1_x1, tmp0_x1);
    tmp0_x2 = _mm256_sub_pd(ld1_x2, tmp0_x2);
    tmp0_x3 = _mm256_sub_pd(ld1_x3, tmp0_x3);
    tmp0_x4 = _mm256_sub_pd(ld1_x4, tmp0_x4);
    tmp0_x5 = _mm256_sub_pd(ld1_x5, tmp0_x5);
    tmp0_x6 = _mm256_sub_pd(ld1_x6, tmp0_x6);
    tmp0_x7 = _mm256_sub_pd(ld1_x7, tmp0_x7);

    vt1_x0 = _mm256_add_pd(tmp0_x0, tmp1_x0);
    vt1_x1 = _mm256_add_pd(tmp0_x1, tmp1_x1);
    vt1_x2 = _mm256_add_pd(tmp0_x2, tmp1_x2);
    vt1_x3 = _mm256_add_pd(tmp0_x3, tmp1_x3);
    vt1_x4 = _mm256_add_pd(tmp0_x4, tmp1_x4);
    vt1_x5 = _mm256_add_pd(tmp0_x5, tmp1_x5);
    vt1_x6 = _mm256_add_pd(tmp0_x6, tmp1_x6);
    vt1_x7 = _mm256_add_pd(tmp0_x7, tmp1_x7);

    vs1_x0 = tmp_s_x0;
    vs1_x1 = tmp_s_x1;
    vs1_x2 = tmp_s_x2;
    vs1_x3 = tmp_s_x3;
    vs1_x4 = tmp_s_x4;
    vs1_x5 = tmp_s_x5;
    vs1_x6 = tmp_s_x6;
    vs1_x7 = tmp_s_x7;

    //vs2 = two_sum_vec(ld2, rd2, &vt2);
    tmp_s_x0 = _mm256_add_pd(ld2_x0, rd2_x0);
    tmp_s_x1 = _mm256_add_pd(ld2_x1, rd2_x1);
    tmp_s_x2 = _mm256_add_pd(ld2_x2, rd2_x2);
    tmp_s_x3 = _mm256_add_pd(ld2_x3, rd2_x3);
    tmp_s_x4 = _mm256_add_pd(ld2_x4, rd2_x4);
    tmp_s_x5 = _mm256_add_pd(ld2_x5, rd2_x5);
    tmp_s_x6 = _mm256_add_pd(ld2_x6, rd2_x6);
    tmp_s_x7 = _mm256_add_pd(ld2_x7, rd2_x7);

    tmp_bb_x0 = _mm256_sub_pd(tmp_s_x0, ld2_x0);
    tmp_bb_x1 = _mm256_sub_pd(tmp_s_x1, ld2_x1);
    tmp_bb_x2 = _mm256_sub_pd(tmp_s_x2, ld2_x2);
    tmp_bb_x3 = _mm256_sub_pd(tmp_s_x3, ld2_x3);
    tmp_bb_x4 = _mm256_sub_pd(tmp_s_x4, ld2_x4);
    tmp_bb_x5 = _mm256_sub_pd(tmp_s_x5, ld2_x5);
    tmp_bb_x6 = _mm256_sub_pd(tmp_s_x6, ld2_x6);
    tmp_bb_x7 = _mm256_sub_pd(tmp_s_x7, ld2_x7);

    tmp0_x0 = _mm256_sub_pd(tmp_s_x0, tmp_bb_x0);
    tmp0_x1 = _mm256_sub_pd(tmp_s_x1, tmp_bb_x1);
    tmp0_x2 = _mm256_sub_pd(tmp_s_x2, tmp_bb_x2);
    tmp0_x3 = _mm256_sub_pd(tmp_s_x3, tmp_bb_x3);
    tmp0_x4 = _mm256_sub_pd(tmp_s_x4, tmp_bb_x4);
    tmp0_x5 = _mm256_sub_pd(tmp_s_x5, tmp_bb_x5);
    tmp0_x6 = _mm256_sub_pd(tmp_s_x6, tmp_bb_x6);
    tmp0_x7 = _mm256_sub_pd(tmp_s_x7, tmp_bb_x7);
    
    tmp1_x0 = _mm256_sub_pd(rd2_x0, tmp_bb_x0);
    tmp1_x1 = _mm256_sub_pd(rd2_x1, tmp_bb_x1);
    tmp1_x2 = _mm256_sub_pd(rd2_x2, tmp_bb_x2);
    tmp1_x3 = _mm256_sub_pd(rd2_x3, tmp_bb_x3);
    tmp1_x4 = _mm256_sub_pd(rd2_x4, tmp_bb_x4);
    tmp1_x5 = _mm256_sub_pd(rd2_x5, tmp_bb_x5);
    tmp1_x6 = _mm256_sub_pd(rd2_x6, tmp_bb_x6);
    tmp1_x7 = _mm256_sub_pd(rd2_x7, tmp_bb_x7);
    
    tmp0_x0 = _mm256_sub_pd(ld2_x0, tmp0_x0);
    tmp0_x1 = _mm256_sub_pd(ld2_x1, tmp0_x1);
    tmp0_x2 = _mm256_sub_pd(ld2_x2, tmp0_x2);
    tmp0_x3 = _mm256_sub_pd(ld2_x3, tmp0_x3);
    tmp0_x4 = _mm256_sub_pd(ld2_x4, tmp0_x4);
    tmp0_x5 = _mm256_sub_pd(ld2_x5, tmp0_x5);
    tmp0_x6 = _mm256_sub_pd(ld2_x6, tmp0_x6);
    tmp0_x7 = _mm256_sub_pd(ld2_x7, tmp0_x7);

    vt2_x0 = _mm256_add_pd(tmp0_x0, tmp1_x0);
    vt2_x1 = _mm256_add_pd(tmp0_x1, tmp1_x1);
    vt2_x2 = _mm256_add_pd(tmp0_x2, tmp1_x2);
    vt2_x3 = _mm256_add_pd(tmp0_x3, tmp1_x3);
    vt2_x4 = _mm256_add_pd(tmp0_x4, tmp1_x4);
    vt2_x5 = _mm256_add_pd(tmp0_x5, tmp1_x5);
    vt2_x6 = _mm256_add_pd(tmp0_x6, tmp1_x6);
    vt2_x7 = _mm256_add_pd(tmp0_x7, tmp1_x7);

    vs2_x0 = tmp_s_x0;
    vs2_x1 = tmp_s_x1;
    vs2_x2 = tmp_s_x2;
    vs2_x3 = tmp_s_x3;
    vs2_x4 = tmp_s_x4;
    vs2_x5 = tmp_s_x5;
    vs2_x6 = tmp_s_x6;
    vs2_x7 = tmp_s_x7;

    //vs3 = two_sum_vec(ld3, rd3, &vt3);
    tmp_s_x0 = _mm256_add_pd(ld3_x0, rd3_x0);
    tmp_s_x1 = _mm256_add_pd(ld3_x1, rd3_x1);
    tmp_s_x2 = _mm256_add_pd(ld3_x2, rd3_x2);
    tmp_s_x3 = _mm256_add_pd(ld3_x3, rd3_x3);
    tmp_s_x4 = _mm256_add_pd(ld3_x4, rd3_x4);
    tmp_s_x5 = _mm256_add_pd(ld3_x5, rd3_x5);
    tmp_s_x6 = _mm256_add_pd(ld3_x6, rd3_x6);
    tmp_s_x7 = _mm256_add_pd(ld3_x7, rd3_x7);

    tmp_bb_x0 = _mm256_sub_pd(tmp_s_x0, ld3_x0);
    tmp_bb_x1 = _mm256_sub_pd(tmp_s_x1, ld3_x1);
    tmp_bb_x2 = _mm256_sub_pd(tmp_s_x2, ld3_x2);
    tmp_bb_x3 = _mm256_sub_pd(tmp_s_x3, ld3_x3);
    tmp_bb_x4 = _mm256_sub_pd(tmp_s_x4, ld3_x4);
    tmp_bb_x5 = _mm256_sub_pd(tmp_s_x5, ld3_x5);
    tmp_bb_x6 = _mm256_sub_pd(tmp_s_x6, ld3_x6);
    tmp_bb_x7 = _mm256_sub_pd(tmp_s_x7, ld3_x7);

    tmp0_x0 = _mm256_sub_pd(tmp_s_x0, tmp_bb_x0);
    tmp0_x1 = _mm256_sub_pd(tmp_s_x1, tmp_bb_x1);
    tmp0_x2 = _mm256_sub_pd(tmp_s_x2, tmp_bb_x2);
    tmp0_x3 = _mm256_sub_pd(tmp_s_x3, tmp_bb_x3);
    tmp0_x4 = _mm256_sub_pd(tmp_s_x4, tmp_bb_x4);
    tmp0_x5 = _mm256_sub_pd(tmp_s_x5, tmp_bb_x5);
    tmp0_x6 = _mm256_sub_pd(tmp_s_x6, tmp_bb_x6);
    tmp0_x7 = _mm256_sub_pd(tmp_s_x7, tmp_bb_x7);
    
    tmp1_x0 = _mm256_sub_pd(rd3_x0, tmp_bb_x0);
    tmp1_x1 = _mm256_sub_pd(rd3_x1, tmp_bb_x1);
    tmp1_x2 = _mm256_sub_pd(rd3_x2, tmp_bb_x2);
    tmp1_x3 = _mm256_sub_pd(rd3_x3, tmp_bb_x3);
    tmp1_x4 = _mm256_sub_pd(rd3_x4, tmp_bb_x4);
    tmp1_x5 = _mm256_sub_pd(rd3_x5, tmp_bb_x5);
    tmp1_x6 = _mm256_sub_pd(rd3_x6, tmp_bb_x6);
    tmp1_x7 = _mm256_sub_pd(rd3_x7, tmp_bb_x7);
    
    tmp0_x0 = _mm256_sub_pd(ld3_x0, tmp0_x0);
    tmp0_x1 = _mm256_sub_pd(ld3_x1, tmp0_x1);
    tmp0_x2 = _mm256_sub_pd(ld3_x2, tmp0_x2);
    tmp0_x3 = _mm256_sub_pd(ld3_x3, tmp0_x3);
    tmp0_x4 = _mm256_sub_pd(ld3_x4, tmp0_x4);
    tmp0_x5 = _mm256_sub_pd(ld3_x5, tmp0_x5);
    tmp0_x6 = _mm256_sub_pd(ld3_x6, tmp0_x6);
    tmp0_x7 = _mm256_sub_pd(ld3_x7, tmp0_x7);

    vt3_x0 = _mm256_add_pd(tmp0_x0, tmp1_x0);
    vt3_x1 = _mm256_add_pd(tmp0_x1, tmp1_x1);
    vt3_x2 = _mm256_add_pd(tmp0_x2, tmp1_x2);
    vt3_x3 = _mm256_add_pd(tmp0_x3, tmp1_x3);
    vt3_x4 = _mm256_add_pd(tmp0_x4, tmp1_x4);
    vt3_x5 = _mm256_add_pd(tmp0_x5, tmp1_x5);
    vt3_x6 = _mm256_add_pd(tmp0_x6, tmp1_x6);
    vt3_x7 = _mm256_add_pd(tmp0_x7, tmp1_x7);

    vs3_x0 = tmp_s_x0;
    vs3_x1 = tmp_s_x1;
    vs3_x2 = tmp_s_x2;
    vs3_x3 = tmp_s_x3;
    vs3_x4 = tmp_s_x4;
    vs3_x5 = tmp_s_x5;
    vs3_x6 = tmp_s_x6;
    vs3_x7 = tmp_s_x7;

    //vs1 = two_sum_vec(vs1, vt0, &vt0);
    tmp_s_x0 = _mm256_add_pd(vs1_x0, vt0_x0);
    tmp_s_x1 = _mm256_add_pd(vs1_x1, vt0_x1);
    tmp_s_x2 = _mm256_add_pd(vs1_x2, vt0_x2);
    tmp_s_x3 = _mm256_add_pd(vs1_x3, vt0_x3);
    tmp_s_x4 = _mm256_add_pd(vs1_x4, vt0_x4);
    tmp_s_x5 = _mm256_add_pd(vs1_x5, vt0_x5);
    tmp_s_x6 = _mm256_add_pd(vs1_x6, vt0_x6);
    tmp_s_x7 = _mm256_add_pd(vs1_x7, vt0_x7);

    tmp_bb_x0 = _mm256_sub_pd(tmp_s_x0, vs1_x0);
    tmp_bb_x1 = _mm256_sub_pd(tmp_s_x1, vs1_x1);
    tmp_bb_x2 = _mm256_sub_pd(tmp_s_x2, vs1_x2);
    tmp_bb_x3 = _mm256_sub_pd(tmp_s_x3, vs1_x3);
    tmp_bb_x4 = _mm256_sub_pd(tmp_s_x4, vs1_x4);
    tmp_bb_x5 = _mm256_sub_pd(tmp_s_x5, vs1_x5);
    tmp_bb_x6 = _mm256_sub_pd(tmp_s_x6, vs1_x6);
    tmp_bb_x7 = _mm256_sub_pd(tmp_s_x7, vs1_x7);

    tmp0_x0 = _mm256_sub_pd(tmp_s_x0, tmp_bb_x0);
    tmp0_x1 = _mm256_sub_pd(tmp_s_x1, tmp_bb_x1);
    tmp0_x2 = _mm256_sub_pd(tmp_s_x2, tmp_bb_x2);
    tmp0_x3 = _mm256_sub_pd(tmp_s_x3, tmp_bb_x3);
    tmp0_x4 = _mm256_sub_pd(tmp_s_x4, tmp_bb_x4);
    tmp0_x5 = _mm256_sub_pd(tmp_s_x5, tmp_bb_x5);
    tmp0_x6 = _mm256_sub_pd(tmp_s_x6, tmp_bb_x6);
    tmp0_x7 = _mm256_sub_pd(tmp_s_x7, tmp_bb_x7);
    
    tmp1_x0 = _mm256_sub_pd(vt0_x0, tmp_bb_x0);
    tmp1_x1 = _mm256_sub_pd(vt0_x1, tmp_bb_x1);
    tmp1_x2 = _mm256_sub_pd(vt0_x2, tmp_bb_x2);
    tmp1_x3 = _mm256_sub_pd(vt0_x3, tmp_bb_x3);
    tmp1_x4 = _mm256_sub_pd(vt0_x4, tmp_bb_x4);
    tmp1_x5 = _mm256_sub_pd(vt0_x5, tmp_bb_x5);
    tmp1_x6 = _mm256_sub_pd(vt0_x6, tmp_bb_x6);
    tmp1_x7 = _mm256_sub_pd(vt0_x7, tmp_bb_x7);
    
    tmp0_x0 = _mm256_sub_pd(vs1_x0, tmp0_x0);
    tmp0_x1 = _mm256_sub_pd(vs1_x1, tmp0_x1);
    tmp0_x2 = _mm256_sub_pd(vs1_x2, tmp0_x2);
    tmp0_x3 = _mm256_sub_pd(vs1_x3, tmp0_x3);
    tmp0_x4 = _mm256_sub_pd(vs1_x4, tmp0_x4);
    tmp0_x5 = _mm256_sub_pd(vs1_x5, tmp0_x5);
    tmp0_x6 = _mm256_sub_pd(vs1_x6, tmp0_x6);
    tmp0_x7 = _mm256_sub_pd(vs1_x7, tmp0_x7);

    vt0_x0 = _mm256_add_pd(tmp0_x0, tmp1_x0);
    vt0_x1 = _mm256_add_pd(tmp0_x1, tmp1_x1);
    vt0_x2 = _mm256_add_pd(tmp0_x2, tmp1_x2);
    vt0_x3 = _mm256_add_pd(tmp0_x3, tmp1_x3);
    vt0_x4 = _mm256_add_pd(tmp0_x4, tmp1_x4);
    vt0_x5 = _mm256_add_pd(tmp0_x5, tmp1_x5);
    vt0_x6 = _mm256_add_pd(tmp0_x6, tmp1_x6);
    vt0_x7 = _mm256_add_pd(tmp0_x7, tmp1_x7);

    vs1_x0 = tmp_s_x0;
    vs1_x1 = tmp_s_x1;
    vs1_x2 = tmp_s_x2;
    vs1_x3 = tmp_s_x3;
    vs1_x4 = tmp_s_x4;
    vs1_x5 = tmp_s_x5;
    vs1_x6 = tmp_s_x6;
    vs1_x7 = tmp_s_x7;

    //three_sum_vec(&vs2, &vt0, &vt1);

    /* tmp_s = _mm256_add_pd(vs2, vt0);
    tmp_bb = _mm256_sub_pd(tmp_s, vs2);
    tmp0 = _mm256_sub_pd(tmp_s, tmp_bb);
    tmp1 = _mm256_sub_pd(vt0, tmp_bb);
    tmp0 = _mm256_sub_pd(vs2, tmp0);
    tmp_t2 = _mm256_add_pd(tmp0, tmp1);
    tmp_t1 = tmp_s; */
    tmp_s_x0 = _mm256_add_pd(vs2_x0, vt0_x0);
    tmp_s_x1 = _mm256_add_pd(vs2_x1, vt0_x1);
    tmp_s_x2 = _mm256_add_pd(vs2_x2, vt0_x2);
    tmp_s_x3 = _mm256_add_pd(vs2_x3, vt0_x3);
    tmp_s_x4 = _mm256_add_pd(vs2_x4, vt0_x4);
    tmp_s_x5 = _mm256_add_pd(vs2_x5, vt0_x5);
    tmp_s_x6 = _mm256_add_pd(vs2_x6, vt0_x6);
    tmp_s_x7 = _mm256_add_pd(vs2_x7, vt0_x7);

    tmp_bb_x0 = _mm256_sub_pd(tmp_s_x0, vs2_x0);
    tmp_bb_x1 = _mm256_sub_pd(tmp_s_x1, vs2_x1);
    tmp_bb_x2 = _mm256_sub_pd(tmp_s_x2, vs2_x2);
    tmp_bb_x3 = _mm256_sub_pd(tmp_s_x3, vs2_x3);
    tmp_bb_x4 = _mm256_sub_pd(tmp_s_x4, vs2_x4);
    tmp_bb_x5 = _mm256_sub_pd(tmp_s_x5, vs2_x5);
    tmp_bb_x6 = _mm256_sub_pd(tmp_s_x6, vs2_x6);
    tmp_bb_x7 = _mm256_sub_pd(tmp_s_x7, vs2_x7);

    tmp0_x0 = _mm256_sub_pd(tmp_s_x0, tmp_bb_x0);
    tmp0_x1 = _mm256_sub_pd(tmp_s_x1, tmp_bb_x1);
    tmp0_x2 = _mm256_sub_pd(tmp_s_x2, tmp_bb_x2);
    tmp0_x3 = _mm256_sub_pd(tmp_s_x3, tmp_bb_x3);
    tmp0_x4 = _mm256_sub_pd(tmp_s_x4, tmp_bb_x4);
    tmp0_x5 = _mm256_sub_pd(tmp_s_x5, tmp_bb_x5);
    tmp0_x6 = _mm256_sub_pd(tmp_s_x6, tmp_bb_x6);
    tmp0_x7 = _mm256_sub_pd(tmp_s_x7, tmp_bb_x7);
    
    tmp1_x0 = _mm256_sub_pd(vt0_x0, tmp_bb_x0);
    tmp1_x1 = _mm256_sub_pd(vt0_x1, tmp_bb_x1);
    tmp1_x2 = _mm256_sub_pd(vt0_x2, tmp_bb_x2);
    tmp1_x3 = _mm256_sub_pd(vt0_x3, tmp_bb_x3);
    tmp1_x4 = _mm256_sub_pd(vt0_x4, tmp_bb_x4);
    tmp1_x5 = _mm256_sub_pd(vt0_x5, tmp_bb_x5);
    tmp1_x6 = _mm256_sub_pd(vt0_x6, tmp_bb_x6);
    tmp1_x7 = _mm256_sub_pd(vt0_x7, tmp_bb_x7);
    
    tmp0_x0 = _mm256_sub_pd(vs2_x0, tmp0_x0);
    tmp0_x1 = _mm256_sub_pd(vs2_x1, tmp0_x1);
    tmp0_x2 = _mm256_sub_pd(vs2_x2, tmp0_x2);
    tmp0_x3 = _mm256_sub_pd(vs2_x3, tmp0_x3);
    tmp0_x4 = _mm256_sub_pd(vs2_x4, tmp0_x4);
    tmp0_x5 = _mm256_sub_pd(vs2_x5, tmp0_x5);
    tmp0_x6 = _mm256_sub_pd(vs2_x6, tmp0_x6);
    tmp0_x7 = _mm256_sub_pd(vs2_x7, tmp0_x7);

    tmp_t2_x0 = _mm256_add_pd(tmp0_x0, tmp1_x0);
    tmp_t2_x1 = _mm256_add_pd(tmp0_x1, tmp1_x1);
    tmp_t2_x2 = _mm256_add_pd(tmp0_x2, tmp1_x2);
    tmp_t2_x3 = _mm256_add_pd(tmp0_x3, tmp1_x3);
    tmp_t2_x4 = _mm256_add_pd(tmp0_x4, tmp1_x4);
    tmp_t2_x5 = _mm256_add_pd(tmp0_x5, tmp1_x5);
    tmp_t2_x6 = _mm256_add_pd(tmp0_x6, tmp1_x6);
    tmp_t2_x7 = _mm256_add_pd(tmp0_x7, tmp1_x7);

    tmp_t1_x0 = tmp_s_x0;
    tmp_t1_x1 = tmp_s_x1;
    tmp_t1_x2 = tmp_s_x2;
    tmp_t1_x3 = tmp_s_x3;
    tmp_t1_x4 = tmp_s_x4;
    tmp_t1_x5 = tmp_s_x5;
    tmp_t1_x6 = tmp_s_x6;
    tmp_t1_x7 = tmp_s_x7;

    /* tmp_s = _mm256_add_pd(vt1, tmp_t1);
    tmp_bb = _mm256_sub_pd(tmp_s, vt1);
    tmp0 = _mm256_sub_pd(tmp_s, tmp_bb);
    tmp1 = _mm256_sub_pd(tmp_t1, tmp_bb);
    tmp0 = _mm256_sub_pd(vt1, tmp0);
    tmp_t3 = _mm256_add_pd(tmp0, tmp1);
    vs2 = tmp_s; */

    tmp_s_x0 = _mm256_add_pd(vt1_x0, tmp_t1_x0);
    tmp_s_x1 = _mm256_add_pd(vt1_x1, tmp_t1_x1);
    tmp_s_x2 = _mm256_add_pd(vt1_x2, tmp_t1_x2);
    tmp_s_x3 = _mm256_add_pd(vt1_x3, tmp_t1_x3);
    tmp_s_x4 = _mm256_add_pd(vt1_x4, tmp_t1_x4);
    tmp_s_x5 = _mm256_add_pd(vt1_x5, tmp_t1_x5);
    tmp_s_x6 = _mm256_add_pd(vt1_x6, tmp_t1_x6);
    tmp_s_x7 = _mm256_add_pd(vt1_x7, tmp_t1_x7);

    tmp_bb_x0 = _mm256_sub_pd(tmp_s_x0, vt1_x0);
    tmp_bb_x1 = _mm256_sub_pd(tmp_s_x1, vt1_x1);
    tmp_bb_x2 = _mm256_sub_pd(tmp_s_x2, vt1_x2);
    tmp_bb_x3 = _mm256_sub_pd(tmp_s_x3, vt1_x3);
    tmp_bb_x4 = _mm256_sub_pd(tmp_s_x4, vt1_x4);
    tmp_bb_x5 = _mm256_sub_pd(tmp_s_x5, vt1_x5);
    tmp_bb_x6 = _mm256_sub_pd(tmp_s_x6, vt1_x6);
    tmp_bb_x7 = _mm256_sub_pd(tmp_s_x7, vt1_x7);

    tmp0_x0 = _mm256_sub_pd(tmp_s_x0, tmp_bb_x0);
    tmp0_x1 = _mm256_sub_pd(tmp_s_x1, tmp_bb_x1);
    tmp0_x2 = _mm256_sub_pd(tmp_s_x2, tmp_bb_x2);
    tmp0_x3 = _mm256_sub_pd(tmp_s_x3, tmp_bb_x3);
    tmp0_x4 = _mm256_sub_pd(tmp_s_x4, tmp_bb_x4);
    tmp0_x5 = _mm256_sub_pd(tmp_s_x5, tmp_bb_x5);
    tmp0_x6 = _mm256_sub_pd(tmp_s_x6, tmp_bb_x6);
    tmp0_x7 = _mm256_sub_pd(tmp_s_x7, tmp_bb_x7);
    
    tmp1_x0 = _mm256_sub_pd(tmp_t1_x0, tmp_bb_x0);
    tmp1_x1 = _mm256_sub_pd(tmp_t1_x1, tmp_bb_x1);
    tmp1_x2 = _mm256_sub_pd(tmp_t1_x2, tmp_bb_x2);
    tmp1_x3 = _mm256_sub_pd(tmp_t1_x3, tmp_bb_x3);
    tmp1_x4 = _mm256_sub_pd(tmp_t1_x4, tmp_bb_x4);
    tmp1_x5 = _mm256_sub_pd(tmp_t1_x5, tmp_bb_x5);
    tmp1_x6 = _mm256_sub_pd(tmp_t1_x6, tmp_bb_x6);
    tmp1_x7 = _mm256_sub_pd(tmp_t1_x7, tmp_bb_x7);
    
    tmp0_x0 = _mm256_sub_pd(vt1_x0, tmp0_x0);
    tmp0_x1 = _mm256_sub_pd(vt1_x1, tmp0_x1);
    tmp0_x2 = _mm256_sub_pd(vt1_x2, tmp0_x2);
    tmp0_x3 = _mm256_sub_pd(vt1_x3, tmp0_x3);
    tmp0_x4 = _mm256_sub_pd(vt1_x4, tmp0_x4);
    tmp0_x5 = _mm256_sub_pd(vt1_x5, tmp0_x5);
    tmp0_x6 = _mm256_sub_pd(vt1_x6, tmp0_x6);
    tmp0_x7 = _mm256_sub_pd(vt1_x7, tmp0_x7);

    tmp_t3_x0 = _mm256_add_pd(tmp0_x0, tmp1_x0);
    tmp_t3_x1 = _mm256_add_pd(tmp0_x1, tmp1_x1);
    tmp_t3_x2 = _mm256_add_pd(tmp0_x2, tmp1_x2);
    tmp_t3_x3 = _mm256_add_pd(tmp0_x3, tmp1_x3);
    tmp_t3_x4 = _mm256_add_pd(tmp0_x4, tmp1_x4);
    tmp_t3_x5 = _mm256_add_pd(tmp0_x5, tmp1_x5);
    tmp_t3_x6 = _mm256_add_pd(tmp0_x6, tmp1_x6);
    tmp_t3_x7 = _mm256_add_pd(tmp0_x7, tmp1_x7);

    vs2_x0 = tmp_s_x0;
    vs2_x1 = tmp_s_x1;
    vs2_x2 = tmp_s_x2;
    vs2_x3 = tmp_s_x3;
    vs2_x4 = tmp_s_x4;
    vs2_x5 = tmp_s_x5;
    vs2_x6 = tmp_s_x6;
    vs2_x7 = tmp_s_x7;


    /* tmp_s = _mm256_add_pd(tmp_t2, tmp_t3);
    tmp_bb = _mm256_sub_pd(tmp_s, tmp_t2);
    tmp0 = _mm256_sub_pd(tmp_s, tmp_bb);
    tmp1 = _mm256_sub_pd(tmp_t3, tmp_bb);
    tmp0 = _mm256_sub_pd(tmp_t2, tmp0);
    vt1 = _mm256_add_pd(tmp0, tmp1);
    vt0 = tmp_s; */

    tmp_s_x0 = _mm256_add_pd(tmp_t2_x0, tmp_t3_x0);
    tmp_s_x1 = _mm256_add_pd(tmp_t2_x1, tmp_t3_x1);
    tmp_s_x2 = _mm256_add_pd(tmp_t2_x2, tmp_t3_x2);
    tmp_s_x3 = _mm256_add_pd(tmp_t2_x3, tmp_t3_x3);
    tmp_s_x4 = _mm256_add_pd(tmp_t2_x4, tmp_t3_x4);
    tmp_s_x5 = _mm256_add_pd(tmp_t2_x5, tmp_t3_x5);
    tmp_s_x6 = _mm256_add_pd(tmp_t2_x6, tmp_t3_x6);
    tmp_s_x7 = _mm256_add_pd(tmp_t2_x7, tmp_t3_x7);

    tmp_bb_x0 = _mm256_sub_pd(tmp_s_x0, tmp_t2_x0);
    tmp_bb_x1 = _mm256_sub_pd(tmp_s_x1, tmp_t2_x1);
    tmp_bb_x2 = _mm256_sub_pd(tmp_s_x2, tmp_t2_x2);
    tmp_bb_x3 = _mm256_sub_pd(tmp_s_x3, tmp_t2_x3);
    tmp_bb_x4 = _mm256_sub_pd(tmp_s_x4, tmp_t2_x4);
    tmp_bb_x5 = _mm256_sub_pd(tmp_s_x5, tmp_t2_x5);
    tmp_bb_x6 = _mm256_sub_pd(tmp_s_x6, tmp_t2_x6);
    tmp_bb_x7 = _mm256_sub_pd(tmp_s_x7, tmp_t2_x7);

    tmp0_x0 = _mm256_sub_pd(tmp_s_x0, tmp_bb_x0);
    tmp0_x1 = _mm256_sub_pd(tmp_s_x1, tmp_bb_x1);
    tmp0_x2 = _mm256_sub_pd(tmp_s_x2, tmp_bb_x2);
    tmp0_x3 = _mm256_sub_pd(tmp_s_x3, tmp_bb_x3);
    tmp0_x4 = _mm256_sub_pd(tmp_s_x4, tmp_bb_x4);
    tmp0_x5 = _mm256_sub_pd(tmp_s_x5, tmp_bb_x5);
    tmp0_x6 = _mm256_sub_pd(tmp_s_x6, tmp_bb_x6);
    tmp0_x7 = _mm256_sub_pd(tmp_s_x7, tmp_bb_x7);
    
    tmp1_x0 = _mm256_sub_pd(tmp_t3_x0, tmp_bb_x0);
    tmp1_x1 = _mm256_sub_pd(tmp_t3_x1, tmp_bb_x1);
    tmp1_x2 = _mm256_sub_pd(tmp_t3_x2, tmp_bb_x2);
    tmp1_x3 = _mm256_sub_pd(tmp_t3_x3, tmp_bb_x3);
    tmp1_x4 = _mm256_sub_pd(tmp_t3_x4, tmp_bb_x4);
    tmp1_x5 = _mm256_sub_pd(tmp_t3_x5, tmp_bb_x5);
    tmp1_x6 = _mm256_sub_pd(tmp_t3_x6, tmp_bb_x6);
    tmp1_x7 = _mm256_sub_pd(tmp_t3_x7, tmp_bb_x7);
    
    tmp0_x0 = _mm256_sub_pd(tmp_t2_x0, tmp0_x0);
    tmp0_x1 = _mm256_sub_pd(tmp_t2_x1, tmp0_x1);
    tmp0_x2 = _mm256_sub_pd(tmp_t2_x2, tmp0_x2);
    tmp0_x3 = _mm256_sub_pd(tmp_t2_x3, tmp0_x3);
    tmp0_x4 = _mm256_sub_pd(tmp_t2_x4, tmp0_x4);
    tmp0_x5 = _mm256_sub_pd(tmp_t2_x5, tmp0_x5);
    tmp0_x6 = _mm256_sub_pd(tmp_t2_x6, tmp0_x6);
    tmp0_x7 = _mm256_sub_pd(tmp_t2_x7, tmp0_x7);

    vt1_x0 = _mm256_add_pd(tmp0_x0, tmp1_x0);
    vt1_x1 = _mm256_add_pd(tmp0_x1, tmp1_x1);
    vt1_x2 = _mm256_add_pd(tmp0_x2, tmp1_x2);
    vt1_x3 = _mm256_add_pd(tmp0_x3, tmp1_x3);
    vt1_x4 = _mm256_add_pd(tmp0_x4, tmp1_x4);
    vt1_x5 = _mm256_add_pd(tmp0_x5, tmp1_x5);
    vt1_x6 = _mm256_add_pd(tmp0_x6, tmp1_x6);
    vt1_x7 = _mm256_add_pd(tmp0_x7, tmp1_x7);

    vt0_x0 = tmp_s_x0;
    vt0_x1 = tmp_s_x1;
    vt0_x2 = tmp_s_x2;
    vt0_x3 = tmp_s_x3;
    vt0_x4 = tmp_s_x4;
    vt0_x5 = tmp_s_x5;
    vt0_x6 = tmp_s_x6;
    vt0_x7 = tmp_s_x7;



    //three_sum2_vec(&vs3, &vt0, &vt2);

    /* tmp_s = _mm256_add_pd(vs3, vt0);
    tmp_bb = _mm256_sub_pd(tmp_s, vs3);
    tmp0 = _mm256_sub_pd(tmp_s, tmp_bb);
    tmp1 = _mm256_sub_pd(vt0, tmp_bb);
    tmp0 = _mm256_sub_pd(vs3, tmp0);
    tmp_t2 = _mm256_add_pd(tmp0, tmp1);
    tmp_t1 = tmp_s; */
    tmp_s_x0 = _mm256_add_pd(vs3_x0, vt0_x0);
    tmp_s_x1 = _mm256_add_pd(vs3_x1, vt0_x1);
    tmp_s_x2 = _mm256_add_pd(vs3_x2, vt0_x2);
    tmp_s_x3 = _mm256_add_pd(vs3_x3, vt0_x3);
    tmp_s_x4 = _mm256_add_pd(vs3_x4, vt0_x4);
    tmp_s_x5 = _mm256_add_pd(vs3_x5, vt0_x5);
    tmp_s_x6 = _mm256_add_pd(vs3_x6, vt0_x6);
    tmp_s_x7 = _mm256_add_pd(vs3_x7, vt0_x7);

    tmp_bb_x0 = _mm256_sub_pd(tmp_s_x0, vs3_x0);
    tmp_bb_x1 = _mm256_sub_pd(tmp_s_x1, vs3_x1);
    tmp_bb_x2 = _mm256_sub_pd(tmp_s_x2, vs3_x2);
    tmp_bb_x3 = _mm256_sub_pd(tmp_s_x3, vs3_x3);
    tmp_bb_x4 = _mm256_sub_pd(tmp_s_x4, vs3_x4);
    tmp_bb_x5 = _mm256_sub_pd(tmp_s_x5, vs3_x5);
    tmp_bb_x6 = _mm256_sub_pd(tmp_s_x6, vs3_x6);
    tmp_bb_x7 = _mm256_sub_pd(tmp_s_x7, vs3_x7);

    tmp0_x0 = _mm256_sub_pd(tmp_s_x0, tmp_bb_x0);
    tmp0_x1 = _mm256_sub_pd(tmp_s_x1, tmp_bb_x1);
    tmp0_x2 = _mm256_sub_pd(tmp_s_x2, tmp_bb_x2);
    tmp0_x3 = _mm256_sub_pd(tmp_s_x3, tmp_bb_x3);
    tmp0_x4 = _mm256_sub_pd(tmp_s_x4, tmp_bb_x4);
    tmp0_x5 = _mm256_sub_pd(tmp_s_x5, tmp_bb_x5);
    tmp0_x6 = _mm256_sub_pd(tmp_s_x6, tmp_bb_x6);
    tmp0_x7 = _mm256_sub_pd(tmp_s_x7, tmp_bb_x7);
    
    tmp1_x0 = _mm256_sub_pd(vt0_x0, tmp_bb_x0);
    tmp1_x1 = _mm256_sub_pd(vt0_x1, tmp_bb_x1);
    tmp1_x2 = _mm256_sub_pd(vt0_x2, tmp_bb_x2);
    tmp1_x3 = _mm256_sub_pd(vt0_x3, tmp_bb_x3);
    tmp1_x4 = _mm256_sub_pd(vt0_x4, tmp_bb_x4);
    tmp1_x5 = _mm256_sub_pd(vt0_x5, tmp_bb_x5);
    tmp1_x6 = _mm256_sub_pd(vt0_x6, tmp_bb_x6);
    tmp1_x7 = _mm256_sub_pd(vt0_x7, tmp_bb_x7);
    
    tmp0_x0 = _mm256_sub_pd(vs3_x0, tmp0_x0);
    tmp0_x1 = _mm256_sub_pd(vs3_x1, tmp0_x1);
    tmp0_x2 = _mm256_sub_pd(vs3_x2, tmp0_x2);
    tmp0_x3 = _mm256_sub_pd(vs3_x3, tmp0_x3);
    tmp0_x4 = _mm256_sub_pd(vs3_x4, tmp0_x4);
    tmp0_x5 = _mm256_sub_pd(vs3_x5, tmp0_x5);
    tmp0_x6 = _mm256_sub_pd(vs3_x6, tmp0_x6);
    tmp0_x7 = _mm256_sub_pd(vs3_x7, tmp0_x7);

    tmp_t2_x0 = _mm256_add_pd(tmp0_x0, tmp1_x0);
    tmp_t2_x1 = _mm256_add_pd(tmp0_x1, tmp1_x1);
    tmp_t2_x2 = _mm256_add_pd(tmp0_x2, tmp1_x2);
    tmp_t2_x3 = _mm256_add_pd(tmp0_x3, tmp1_x3);
    tmp_t2_x4 = _mm256_add_pd(tmp0_x4, tmp1_x4);
    tmp_t2_x5 = _mm256_add_pd(tmp0_x5, tmp1_x5);
    tmp_t2_x6 = _mm256_add_pd(tmp0_x6, tmp1_x6);
    tmp_t2_x7 = _mm256_add_pd(tmp0_x7, tmp1_x7);

    tmp_t1_x0 = tmp_s_x0;
    tmp_t1_x1 = tmp_s_x1;
    tmp_t1_x2 = tmp_s_x2;
    tmp_t1_x3 = tmp_s_x3;
    tmp_t1_x4 = tmp_s_x4;
    tmp_t1_x5 = tmp_s_x5;
    tmp_t1_x6 = tmp_s_x6;
    tmp_t1_x7 = tmp_s_x7;
    /* tmp_s = _mm256_add_pd(vt2, tmp_t1);
    tmp_bb = _mm256_sub_pd(tmp_s, vt2);
    tmp0 = _mm256_sub_pd(tmp_s, tmp_bb);
    tmp1 = _mm256_sub_pd(tmp_t1, tmp_bb);
    tmp0 = _mm256_sub_pd(vt2, tmp0);
    tmp_t3 = _mm256_add_pd(tmp0, tmp1);
    vs3 = tmp_s; */
    tmp_s_x0 = _mm256_add_pd(vt2_x0, tmp_t1_x0);
    tmp_s_x1 = _mm256_add_pd(vt2_x1, tmp_t1_x1);
    tmp_s_x2 = _mm256_add_pd(vt2_x2, tmp_t1_x2);
    tmp_s_x3 = _mm256_add_pd(vt2_x3, tmp_t1_x3);
    tmp_s_x4 = _mm256_add_pd(vt2_x4, tmp_t1_x4);
    tmp_s_x5 = _mm256_add_pd(vt2_x5, tmp_t1_x5);
    tmp_s_x6 = _mm256_add_pd(vt2_x6, tmp_t1_x6);
    tmp_s_x7 = _mm256_add_pd(vt2_x7, tmp_t1_x7);

    tmp_bb_x0 = _mm256_sub_pd(tmp_s_x0, vt2_x0);
    tmp_bb_x1 = _mm256_sub_pd(tmp_s_x1, vt2_x1);
    tmp_bb_x2 = _mm256_sub_pd(tmp_s_x2, vt2_x2);
    tmp_bb_x3 = _mm256_sub_pd(tmp_s_x3, vt2_x3);
    tmp_bb_x4 = _mm256_sub_pd(tmp_s_x4, vt2_x4);
    tmp_bb_x5 = _mm256_sub_pd(tmp_s_x5, vt2_x5);
    tmp_bb_x6 = _mm256_sub_pd(tmp_s_x6, vt2_x6);
    tmp_bb_x7 = _mm256_sub_pd(tmp_s_x7, vt2_x7);

    tmp0_x0 = _mm256_sub_pd(tmp_s_x0, tmp_bb_x0);
    tmp0_x1 = _mm256_sub_pd(tmp_s_x1, tmp_bb_x1);
    tmp0_x2 = _mm256_sub_pd(tmp_s_x2, tmp_bb_x2);
    tmp0_x3 = _mm256_sub_pd(tmp_s_x3, tmp_bb_x3);
    tmp0_x4 = _mm256_sub_pd(tmp_s_x4, tmp_bb_x4);
    tmp0_x5 = _mm256_sub_pd(tmp_s_x5, tmp_bb_x5);
    tmp0_x6 = _mm256_sub_pd(tmp_s_x6, tmp_bb_x6);
    tmp0_x7 = _mm256_sub_pd(tmp_s_x7, tmp_bb_x7);
    
    tmp1_x0 = _mm256_sub_pd(tmp_t1_x0, tmp_bb_x0);
    tmp1_x1 = _mm256_sub_pd(tmp_t1_x1, tmp_bb_x1);
    tmp1_x2 = _mm256_sub_pd(tmp_t1_x2, tmp_bb_x2);
    tmp1_x3 = _mm256_sub_pd(tmp_t1_x3, tmp_bb_x3);
    tmp1_x4 = _mm256_sub_pd(tmp_t1_x4, tmp_bb_x4);
    tmp1_x5 = _mm256_sub_pd(tmp_t1_x5, tmp_bb_x5);
    tmp1_x6 = _mm256_sub_pd(tmp_t1_x6, tmp_bb_x6);
    tmp1_x7 = _mm256_sub_pd(tmp_t1_x7, tmp_bb_x7);
    
    tmp0_x0 = _mm256_sub_pd(vt2_x0, tmp0_x0);
    tmp0_x1 = _mm256_sub_pd(vt2_x1, tmp0_x1);
    tmp0_x2 = _mm256_sub_pd(vt2_x2, tmp0_x2);
    tmp0_x3 = _mm256_sub_pd(vt2_x3, tmp0_x3);
    tmp0_x4 = _mm256_sub_pd(vt2_x4, tmp0_x4);
    tmp0_x5 = _mm256_sub_pd(vt2_x5, tmp0_x5);
    tmp0_x6 = _mm256_sub_pd(vt2_x6, tmp0_x6);
    tmp0_x7 = _mm256_sub_pd(vt2_x7, tmp0_x7);

    tmp_t3_x0 = _mm256_add_pd(tmp0_x0, tmp1_x0);
    tmp_t3_x1 = _mm256_add_pd(tmp0_x1, tmp1_x1);
    tmp_t3_x2 = _mm256_add_pd(tmp0_x2, tmp1_x2);
    tmp_t3_x3 = _mm256_add_pd(tmp0_x3, tmp1_x3);
    tmp_t3_x4 = _mm256_add_pd(tmp0_x4, tmp1_x4);
    tmp_t3_x5 = _mm256_add_pd(tmp0_x5, tmp1_x5);
    tmp_t3_x6 = _mm256_add_pd(tmp0_x6, tmp1_x6);
    tmp_t3_x7 = _mm256_add_pd(tmp0_x7, tmp1_x7);

    vs3_x0 = tmp_s_x0;
    vs3_x1 = tmp_s_x1;
    vs3_x2 = tmp_s_x2;
    vs3_x3 = tmp_s_x3;
    vs3_x4 = tmp_s_x4;
    vs3_x5 = tmp_s_x5;
    vs3_x6 = tmp_s_x6;
    vs3_x7 = tmp_s_x7;

    /* vt0 = _mm256_add_pd(tmp_t2, tmp_t3); */
    vt0_x0 = _mm256_add_pd(tmp_t2_x0, tmp_t3_x0);
    vt0_x1 = _mm256_add_pd(tmp_t2_x1, tmp_t3_x1);
    vt0_x2 = _mm256_add_pd(tmp_t2_x2, tmp_t3_x2);
    vt0_x3 = _mm256_add_pd(tmp_t2_x3, tmp_t3_x3);
    vt0_x4 = _mm256_add_pd(tmp_t2_x4, tmp_t3_x4);
    vt0_x5 = _mm256_add_pd(tmp_t2_x5, tmp_t3_x5);
    vt0_x6 = _mm256_add_pd(tmp_t2_x6, tmp_t3_x6);
    vt0_x7 = _mm256_add_pd(tmp_t2_x7, tmp_t3_x7);

    //vt0 = _mm256_add_pd(_mm256_add_pd(vt0, vt1), vt3);
    /* tmp0 = _mm256_add_pd(vt0, vt1); */
    tmp0_x0 = _mm256_add_pd(vt0_x0, vt1_x0);
    tmp0_x1 = _mm256_add_pd(vt0_x1, vt1_x1);
    tmp0_x2 = _mm256_add_pd(vt0_x2, vt1_x2);
    tmp0_x3 = _mm256_add_pd(vt0_x3, vt1_x3);
    tmp0_x4 = _mm256_add_pd(vt0_x4, vt1_x4);
    tmp0_x5 = _mm256_add_pd(vt0_x5, vt1_x5);
    tmp0_x6 = _mm256_add_pd(vt0_x6, vt1_x6);
    tmp0_x7 = _mm256_add_pd(vt0_x7, vt1_x7);

    /* vt0 = _mm256_add_pd(tmp0, vt3); */
    vt0_x0 = _mm256_add_pd(tmp0_x0, vt3_x0);
    vt0_x1 = _mm256_add_pd(tmp0_x1, vt3_x1);
    vt0_x2 = _mm256_add_pd(tmp0_x2, vt3_x2);
    vt0_x3 = _mm256_add_pd(tmp0_x3, vt3_x3);
    vt0_x4 = _mm256_add_pd(tmp0_x4, vt3_x4);
    vt0_x5 = _mm256_add_pd(tmp0_x5, vt3_x5);
    vt0_x6 = _mm256_add_pd(tmp0_x6, vt3_x6);
    vt0_x7 = _mm256_add_pd(tmp0_x7, vt3_x7);

    //renorm5(&s0, &s1, &s2, &s3, &t0);
    /* _mm256_store_pd(lo.d0 + i, vs0);
    _mm256_store_pd(lo.d1 + i, vs1);
    _mm256_store_pd(lo.d2 + i, vs2);
    _mm256_store_pd(lo.d3 + i, vs3);
    _mm256_store_pd(tmp_vt0, vt0); */
    _mm256_store_pd(lo.d0 + i, vs0_x0);
    _mm256_store_pd(lo.d0 + i + 4, vs0_x1);
    _mm256_store_pd(lo.d0 + i + 8, vs0_x2);
    _mm256_store_pd(lo.d0 + i + 12, vs0_x3);
    _mm256_store_pd(lo.d0 + i + 16, vs0_x4);
    _mm256_store_pd(lo.d0 + i + 20, vs0_x5);
    _mm256_store_pd(lo.d0 + i + 24, vs0_x6);
    _mm256_store_pd(lo.d0 + i + 28, vs0_x7);
    _mm256_store_pd(lo.d1 + i, vs1_x0);
    _mm256_store_pd(lo.d1 + i + 4, vs1_x1);
    _mm256_store_pd(lo.d1 + i + 8, vs1_x2);
    _mm256_store_pd(lo.d1 + i + 12, vs1_x3);
    _mm256_store_pd(lo.d1 + i + 16, vs1_x4);
    _mm256_store_pd(lo.d1 + i + 20, vs1_x5);
    _mm256_store_pd(lo.d1 + i + 24, vs1_x6);
    _mm256_store_pd(lo.d1 + i + 28, vs1_x7);
    _mm256_store_pd(lo.d2 + i, vs2_x0);
    _mm256_store_pd(lo.d2 + i + 4, vs2_x1);
    _mm256_store_pd(lo.d2 + i + 8, vs2_x2);
    _mm256_store_pd(lo.d2 + i + 12, vs2_x3);
    _mm256_store_pd(lo.d2 + i + 16, vs2_x4);
    _mm256_store_pd(lo.d2 + i + 20, vs2_x5);
    _mm256_store_pd(lo.d2 + i + 24, vs2_x6);
    _mm256_store_pd(lo.d2 + i + 28, vs2_x7);
    _mm256_store_pd(lo.d3 + i, vs3_x0);
    _mm256_store_pd(lo.d3 + i + 4, vs3_x1);
    _mm256_store_pd(lo.d3 + i + 8, vs3_x2);
    _mm256_store_pd(lo.d3 + i + 12, vs3_x3);
    _mm256_store_pd(lo.d3 + i + 16, vs3_x4);
    _mm256_store_pd(lo.d3 + i + 20, vs3_x5);
    _mm256_store_pd(lo.d3 + i + 24, vs3_x6);
    _mm256_store_pd(lo.d3 + i + 28, vs3_x7);
    _mm256_store_pd(tmp_vt0_x0, vt0_x0);
    _mm256_store_pd(tmp_vt0_x1, vt0_x1);
    _mm256_store_pd(tmp_vt0_x2, vt0_x2);
    _mm256_store_pd(tmp_vt0_x3, vt0_x3);
    _mm256_store_pd(tmp_vt0_x4, vt0_x4);
    _mm256_store_pd(tmp_vt0_x5, vt0_x5);
    _mm256_store_pd(tmp_vt0_x6, vt0_x6);
    _mm256_store_pd(tmp_vt0_x7, vt0_x7);
    /* renorm5_inline(lo.d0 + i, lo.d1 + i, lo.d2 + i, lo.d3 + i, tmp_vt0);
    renorm5_inline(lo.d0 + i + 1, lo.d1 + i + 1, lo.d2 + i + 1, lo.d3 + i + 1, tmp_vt0 + 1);
    renorm5_inline(lo.d0 + i + 2, lo.d1 + i + 2, lo.d2 + i + 2, lo.d3 + i + 2, tmp_vt0 + 2);
    renorm5_inline(lo.d0 + i + 3, lo.d1 + i + 3, lo.d2 + i + 3, lo.d3 + i + 3, tmp_vt0 + 3); */
    renorm5_inline(lo.d0 + i, lo.d1 + i, lo.d2 + i, lo.d3 + i, tmp_vt0_x0);
    renorm5_inline(lo.d0 + i + 1, lo.d1 + i + 1, lo.d2 + i + 1, lo.d3 + i + 1, tmp_vt0_x0 + 1);
    renorm5_inline(lo.d0 + i + 2, lo.d1 + i + 2, lo.d2 + i + 2, lo.d3 + i + 2, tmp_vt0_x0 + 2);
    renorm5_inline(lo.d0 + i + 3, lo.d1 + i + 3, lo.d2 + i + 3, lo.d3 + i + 3, tmp_vt0_x0 + 3);
    renorm5_inline(lo.d0 + i + 4, lo.d1 + i + 4, lo.d2 + i + 4, lo.d3 + i + 4, tmp_vt0_x1);
    renorm5_inline(lo.d0 + i + 5, lo.d1 + i + 5, lo.d2 + i + 5, lo.d3 + i + 5, tmp_vt0_x1 + 1);
    renorm5_inline(lo.d0 + i + 6, lo.d1 + i + 6, lo.d2 + i + 6, lo.d3 + i + 6, tmp_vt0_x1 + 2);
    renorm5_inline(lo.d0 + i + 7, lo.d1 + i + 7, lo.d2 + i + 7, lo.d3 + i + 7, tmp_vt0_x1 + 3);
    renorm5_inline(lo.d0 + i + 8, lo.d1 + i + 8, lo.d2 + i + 8, lo.d3 + i + 8, tmp_vt0_x2);
    renorm5_inline(lo.d0 + i + 9, lo.d1 + i + 9, lo.d2 + i + 9, lo.d3 + i + 9, tmp_vt0_x2 + 1);
    renorm5_inline(lo.d0 + i + 10, lo.d1 + i + 10, lo.d2 + i + 10, lo.d3 + i + 10, tmp_vt0_x2 + 2);
    renorm5_inline(lo.d0 + i + 11, lo.d1 + i + 11, lo.d2 + i + 11, lo.d3 + i + 11, tmp_vt0_x2 + 3);
    renorm5_inline(lo.d0 + i + 12, lo.d1 + i + 12, lo.d2 + i + 12, lo.d3 + i + 12, tmp_vt0_x3);
    renorm5_inline(lo.d0 + i + 13, lo.d1 + i + 13, lo.d2 + i + 13, lo.d3 + i + 13, tmp_vt0_x3 + 1);
    renorm5_inline(lo.d0 + i + 14, lo.d1 + i + 14, lo.d2 + i + 14, lo.d3 + i + 14, tmp_vt0_x3 + 2);
    renorm5_inline(lo.d0 + i + 15, lo.d1 + i + 15, lo.d2 + i + 15, lo.d3 + i + 15, tmp_vt0_x3 + 3);
    renorm5_inline(lo.d0 + i + 16, lo.d1 + i + 16, lo.d2 + i + 16, lo.d3 + i + 16, tmp_vt0_x4);
    renorm5_inline(lo.d0 + i + 17, lo.d1 + i + 17, lo.d2 + i + 17, lo.d3 + i + 17, tmp_vt0_x4 + 1);
    renorm5_inline(lo.d0 + i + 18, lo.d1 + i + 18, lo.d2 + i + 18, lo.d3 + i + 18, tmp_vt0_x4 + 2);
    renorm5_inline(lo.d0 + i + 19, lo.d1 + i + 19, lo.d2 + i + 19, lo.d3 + i + 19, tmp_vt0_x4 + 3);
    renorm5_inline(lo.d0 + i + 20, lo.d1 + i + 20, lo.d2 + i + 20, lo.d3 + i + 20, tmp_vt0_x5);
    renorm5_inline(lo.d0 + i + 21, lo.d1 + i + 21, lo.d2 + i + 21, lo.d3 + i + 21, tmp_vt0_x5 + 1);
    renorm5_inline(lo.d0 + i + 22, lo.d1 + i + 22, lo.d2 + i + 22, lo.d3 + i + 22, tmp_vt0_x5 + 2);
    renorm5_inline(lo.d0 + i + 23, lo.d1 + i + 23, lo.d2 + i + 23, lo.d3 + i + 23, tmp_vt0_x5 + 3);
    renorm5_inline(lo.d0 + i + 24, lo.d1 + i + 24, lo.d2 + i + 24, lo.d3 + i + 24, tmp_vt0_x6);
    renorm5_inline(lo.d0 + i + 25, lo.d1 + i + 25, lo.d2 + i + 25, lo.d3 + i + 25, tmp_vt0_x6 + 1);
    renorm5_inline(lo.d0 + i + 26, lo.d1 + i + 26, lo.d2 + i + 26, lo.d3 + i + 26, tmp_vt0_x6 + 2);
    renorm5_inline(lo.d0 + i + 27, lo.d1 + i + 27, lo.d2 + i + 27, lo.d3 + i + 27, tmp_vt0_x6 + 3);
    renorm5_inline(lo.d0 + i + 28, lo.d1 + i + 28, lo.d2 + i + 28, lo.d3 + i + 28, tmp_vt0_x7);
    renorm5_inline(lo.d0 + i + 29, lo.d1 + i + 29, lo.d2 + i + 29, lo.d3 + i + 29, tmp_vt0_x7 + 1);
    renorm5_inline(lo.d0 + i + 30, lo.d1 + i + 30, lo.d2 + i + 30, lo.d3 + i + 30, tmp_vt0_x7 + 2);
    renorm5_inline(lo.d0 + i + 31, lo.d1 + i + 31, lo.d2 + i + 31, lo.d3 + i + 31, tmp_vt0_x7 + 3);
  }

  for (; i + 3 < size; i += 4) {
    ld0 = _mm256_load_pd(lo.d0 + i);
    ld1 = _mm256_load_pd(lo.d1 + i);
    ld2 = _mm256_load_pd(lo.d2 + i);
    ld3 = _mm256_load_pd(lo.d3 + i);
    rd0 = _mm256_load_pd(ro.d0 + i);
    rd1 = _mm256_load_pd(ro.d1 + i);
    rd2 = _mm256_load_pd(ro.d2 + i);
    rd3 = _mm256_load_pd(ro.d3 + i);

    //vs0 = two_sum_vec(ld0, rd0, &vt0);
    tmp_s = _mm256_add_pd(ld0, rd0);
    tmp_bb = _mm256_sub_pd(tmp_s, ld0);
    tmp0 = _mm256_sub_pd(tmp_s, tmp_bb);
    tmp1 = _mm256_sub_pd(rd0, tmp_bb);
    tmp0 = _mm256_sub_pd(ld0, tmp0);
    vt0 = _mm256_add_pd(tmp0, tmp1);
    vs0 = tmp_s;

    //vs1 = two_sum_vec(ld1, rd1, &vt1);
    tmp_s = _mm256_add_pd(ld1, rd1);
    tmp_bb = _mm256_sub_pd(tmp_s, ld1);
    tmp0 = _mm256_sub_pd(tmp_s, tmp_bb);
    tmp1 = _mm256_sub_pd(rd1, tmp_bb);
    tmp0 = _mm256_sub_pd(ld1, tmp0);
    vt1 = _mm256_add_pd(tmp0, tmp1);
    vs1 = tmp_s;

    //vs2 = two_sum_vec(ld2, rd2, &vt2);
    tmp_s = _mm256_add_pd(ld2, rd2);
    tmp_bb = _mm256_sub_pd(tmp_s, ld2);
    tmp0 = _mm256_sub_pd(tmp_s, tmp_bb);
    tmp1 = _mm256_sub_pd(rd2, tmp_bb);
    tmp0 = _mm256_sub_pd(ld2, tmp0);
    vt2 = _mm256_add_pd(tmp0, tmp1);
    vs2 = tmp_s;

    //vs3 = two_sum_vec(ld3, rd3, &vt3);
    tmp_s = _mm256_add_pd(ld3, rd3);
    tmp_bb = _mm256_sub_pd(tmp_s, ld3);
    tmp0 = _mm256_sub_pd(tmp_s, tmp_bb);
    tmp1 = _mm256_sub_pd(rd3, tmp_bb);
    tmp0 = _mm256_sub_pd(ld3, tmp0);
    vt3 = _mm256_add_pd(tmp0, tmp1);
    vs3 = tmp_s;

    //vs1 = two_sum_vec(vs1, vt0, &vt0);
    tmp_s = _mm256_add_pd(vs1, vt0);
    tmp_bb = _mm256_sub_pd(tmp_s, vs1);
    tmp0 = _mm256_sub_pd(tmp_s, tmp_bb);
    tmp1 = _mm256_sub_pd(vt0, tmp_bb);
    tmp0 = _mm256_sub_pd(vs1, tmp0);
    vt0 = _mm256_add_pd(tmp0, tmp1);
    vs1 = tmp_s;

    //three_sum_vec(&vs2, &vt0, &vt1);
    tmp_s = _mm256_add_pd(vs2, vt0);
    tmp_bb = _mm256_sub_pd(tmp_s, vs2);
    tmp0 = _mm256_sub_pd(tmp_s, tmp_bb);
    tmp1 = _mm256_sub_pd(vt0, tmp_bb);
    tmp0 = _mm256_sub_pd(vs2, tmp0);
    tmp_t2 = _mm256_add_pd(tmp0, tmp1);
    tmp_t1 = tmp_s;
    tmp_s = _mm256_add_pd(vt1, tmp_t1);
    tmp_bb = _mm256_sub_pd(tmp_s, vt1);
    tmp0 = _mm256_sub_pd(tmp_s, tmp_bb);
    tmp1 = _mm256_sub_pd(tmp_t1, tmp_bb);
    tmp0 = _mm256_sub_pd(vt1, tmp0);
    tmp_t3 = _mm256_add_pd(tmp0, tmp1);
    vs2 = tmp_s;
    tmp_s = _mm256_add_pd(tmp_t2, tmp_t3);
    tmp_bb = _mm256_sub_pd(tmp_s, tmp_t2);
    tmp0 = _mm256_sub_pd(tmp_s, tmp_bb);
    tmp1 = _mm256_sub_pd(tmp_t3, tmp_bb);
    tmp0 = _mm256_sub_pd(tmp_t2, tmp0);
    vt1 = _mm256_add_pd(tmp0, tmp1);
    vt0 = tmp_s;

    //three_sum2_vec(&vs3, &vt0, &vt2);
    tmp_s = _mm256_add_pd(vs3, vt0);
    tmp_bb = _mm256_sub_pd(tmp_s, vs3);
    tmp0 = _mm256_sub_pd(tmp_s, tmp_bb);
    tmp1 = _mm256_sub_pd(vt0, tmp_bb);
    tmp0 = _mm256_sub_pd(vs3, tmp0);
    tmp_t2 = _mm256_add_pd(tmp0, tmp1);
    tmp_t1 = tmp_s;
    tmp_s = _mm256_add_pd(vt2, tmp_t1);
    tmp_bb = _mm256_sub_pd(tmp_s, vt2);
    tmp0 = _mm256_sub_pd(tmp_s, tmp_bb);
    tmp1 = _mm256_sub_pd(tmp_t1, tmp_bb);
    tmp0 = _mm256_sub_pd(vt2, tmp0);
    tmp_t3 = _mm256_add_pd(tmp0, tmp1);
    vs3 = tmp_s;
    vt0 = _mm256_add_pd(tmp_t2, tmp_t3);

    //vt0 = _mm256_add_pd(_mm256_add_pd(vt0, vt1), vt3);
    tmp0 = _mm256_add_pd(vt0, vt1);
    vt0 = _mm256_add_pd(tmp0, vt3);

    //renorm5(&s0, &s1, &s2, &s3, &t0);
    _mm256_store_pd(lo.d0 + i, vs0);
    _mm256_store_pd(lo.d1 + i, vs1);
    _mm256_store_pd(lo.d2 + i, vs2);
    _mm256_store_pd(lo.d3 + i, vs3);
    _mm256_store_pd(tmp_vt0, vt0);
    renorm5_inline(lo.d0 + i, lo.d1 + i, lo.d2 + i, lo.d3 + i, tmp_vt0);
    renorm5_inline(lo.d0 + i + 1, lo.d1 + i + 1, lo.d2 + i + 1, lo.d3 + i + 1, tmp_vt0 + 1);
    renorm5_inline(lo.d0 + i + 2, lo.d1 + i + 2, lo.d2 + i + 2, lo.d3 + i + 2, tmp_vt0 + 2);
    renorm5_inline(lo.d0 + i + 3, lo.d1 + i + 3, lo.d2 + i + 3, lo.d3 + i + 3, tmp_vt0 + 3);
  }

  for (; i < size; i++) {
      
    s0 = two_sum(lo.d0[i], ro.d0[i], &t0);
    s1 = two_sum(lo.d1[i], ro.d1[i], &t1);
    s2 = two_sum(lo.d2[i], ro.d2[i], &t2);
    s3 = two_sum(lo.d3[i], ro.d3[i], &t3);

    s1 = two_sum(s1, t0, &t0);
    three_sum(&s2, &t0, &t1);
    three_sum2(&s3, &t0, &t2);
    t0 = t0 + t1 + t3;

    renorm5_inline(&s0, &s1, &s2, &s3, &t0);
    lo.d0[i] = s0;
    lo.d1[i] = s1;
    lo.d2[i] = s2;
    lo.d3[i] = s3;
  }
  _mm_free(tmp_vt0);
  _mm_free(tmp_vt0_x0);
  _mm_free(tmp_vt0_x1);
  _mm_free(tmp_vt0_x2);
  _mm_free(tmp_vt0_x3);
  _mm_free(tmp_vt0_x4);
  _mm_free(tmp_vt0_x5);
  _mm_free(tmp_vt0_x6);
  _mm_free(tmp_vt0_x7);
}
// hand inline serial no effect
// static inline func no use! complier refuse to inline.
// TODO: unfold in some way, more vec, inline simple func then unfold? done


void qd_arr_mul_inplace(qd_arr lo, qd_arr ro) {
  //assert(lo.size == ro.size);
  int size = lo.size;

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
    lo.d0[i] = p0;
    lo.d1[i] = p1;
    lo.d2[i] = s0;
    lo.d3[i] = t0;
  }
}

void qd_arr_mul_inplace_vec(qd_arr lo, qd_arr ro) {
  //assert(lo.size == ro.size);
  int size = lo.size;

  double p0, p1, p2, p3, p4, p5;
  double q0, q1, q2, q3, q4, q5;
  double p6, p7, p8, p9;
  double q6, q7, q8, q9;
  double r0, r1;
  double t0, t1;
  double s0, s1, s2;

  __m256d ld0, ld1, ld2, ld3;
  __m256d rd0, rd1, rd2, rd3;
  __m256d vp0, vp1, vp2, vp3, vp4, vp5;
  __m256d vq0, vq1, vq2, vq3, vq4, vq5;
  __m256d vp6, vp7, vp8, vp9;
  __m256d vq6, vq7, vq8, vq9;
  __m256d vr0, vr1;
  __m256d vt0, vt1;
  __m256d vs0, vs1, vs2;
  __m256d tmp0_s, tmp1_s, tmp2_s, tmp0_bb, tmp1_bb, tmp2_bb, tmp_t1, tmp_t2, tmp_t3;
  __m256d tmp0_0, tmp0_1, tmp1_0, tmp1_1, tmp2_0, tmp2_1; 
  double *tmp_vec = (double *)_mm_malloc(4 * sizeof(double), ALIGNMENT);
  int i;
  for (i = 0; i + 3 < size; i+=4) {
    ld0 = _mm256_load_pd(lo.d0 + i);
    ld1 = _mm256_load_pd(lo.d1 + i);
    ld2 = _mm256_load_pd(lo.d2 + i);
    ld3 = _mm256_load_pd(lo.d3 + i);
    rd0 = _mm256_load_pd(ro.d0 + i);
    rd1 = _mm256_load_pd(ro.d1 + i);
    rd2 = _mm256_load_pd(ro.d2 + i);
    rd3 = _mm256_load_pd(ro.d3 + i);

    /* p0 = two_prod(lo.d0[i], ro.d0[i], &q0);
    p1 = two_prod(lo.d0[i], ro.d1[i], &q1);
    p2 = two_prod(lo.d1[i], ro.d0[i], &q2);
    p3 = two_prod(lo.d0[i], ro.d2[i], &q3);
    p4 = two_prod(lo.d1[i], ro.d1[i], &q4);
    p5 = two_prod(lo.d2[i], ro.d0[i], &q5); */
    vp0 = _mm256_mul_pd(ld0, rd0);
    vp1 = _mm256_mul_pd(ld0, rd1);
    vp2 = _mm256_mul_pd(ld1, rd0);
    vp3 = _mm256_mul_pd(ld0, rd2);
    vp4 = _mm256_mul_pd(ld1, rd1);
    vp5 = _mm256_mul_pd(ld2, rd0);
    vq0 = _mm256_fmsub_pd(ld0, rd0, vp0);
    vq1 = _mm256_fmsub_pd(ld0, rd1, vp1);
    vq2 = _mm256_fmsub_pd(ld1, rd0, vp2);
    vq3 = _mm256_fmsub_pd(ld0, rd2, vp3);
    vq4 = _mm256_fmsub_pd(ld1, rd1, vp4);
    vq5 = _mm256_fmsub_pd(ld2, rd0, vp5);

    /* Start Accumulation */
    //three_sum(&p1, &p2, &q0);
    tmp0_s = _mm256_add_pd(vp1, vp2);
    tmp0_bb = _mm256_sub_pd(tmp0_s, vp1);
    tmp0_0 = _mm256_sub_pd(tmp0_s, tmp0_bb);
    tmp0_1 = _mm256_sub_pd(vp2, tmp0_bb);
    tmp0_0 = _mm256_sub_pd(vp1, tmp0_0);
    tmp_t2 = _mm256_add_pd(tmp0_0, tmp0_1);
    tmp_t1 = tmp0_s;
    tmp1_s = _mm256_add_pd(vq0, tmp_t1);
    tmp1_bb = _mm256_sub_pd(tmp1_s, vq0);
    tmp1_0 = _mm256_sub_pd(tmp1_s, tmp1_bb);
    tmp1_1 = _mm256_sub_pd(tmp_t1, tmp1_bb);
    tmp1_0 = _mm256_sub_pd(vq0, tmp1_0);
    tmp_t3 = _mm256_add_pd(tmp1_0, tmp1_1);
    vp1 = tmp1_s;
    tmp2_s = _mm256_add_pd(tmp_t2, tmp_t3);
    tmp2_bb = _mm256_sub_pd(tmp2_s, tmp_t2);
    tmp2_0 = _mm256_sub_pd(tmp2_s, tmp2_bb);
    tmp2_1 = _mm256_sub_pd(tmp_t3, tmp2_bb);
    tmp2_0 = _mm256_sub_pd(tmp_t2, tmp2_0);
    vq0 = _mm256_add_pd(tmp2_0, tmp2_1);
    vp2 = tmp2_s;

    /* Six-Three Sum  of p2, q1, q2, p3, p4, p5. */
    //three_sum(&p2, &q1, &q2);
    tmp0_s = _mm256_add_pd(vp2, vq1);
    tmp0_bb = _mm256_sub_pd(tmp0_s, vp2);
    tmp0_0 = _mm256_sub_pd(tmp0_s, tmp0_bb);
    tmp0_1 = _mm256_sub_pd(vq1, tmp0_bb);
    tmp0_0 = _mm256_sub_pd(vp2, tmp0_0);
    tmp_t2 = _mm256_add_pd(tmp0_0, tmp0_1);
    tmp_t1 = tmp0_s;
    tmp1_s = _mm256_add_pd(vq2, tmp_t1);
    tmp1_bb = _mm256_sub_pd(tmp1_s, vq2);
    tmp1_0 = _mm256_sub_pd(tmp1_s, tmp1_bb);
    tmp1_1 = _mm256_sub_pd(tmp_t1, tmp1_bb);
    tmp1_0 = _mm256_sub_pd(vq2, tmp1_0);
    tmp_t3 = _mm256_add_pd(tmp1_0, tmp1_1);
    vp2 = tmp1_s;
    tmp2_s = _mm256_add_pd(tmp_t2, tmp_t3);
    tmp2_bb = _mm256_sub_pd(tmp2_s, tmp_t2);
    tmp2_0 = _mm256_sub_pd(tmp2_s, tmp2_bb);
    tmp2_1 = _mm256_sub_pd(tmp_t3, tmp2_bb);
    tmp2_0 = _mm256_sub_pd(tmp_t2, tmp2_0);
    vq2 = _mm256_add_pd(tmp2_0, tmp2_1);
    vq1 = tmp2_s;

    //three_sum(&p3, &p4, &p5);
    tmp0_s = _mm256_add_pd(vp3, vp4);
    tmp0_bb = _mm256_sub_pd(tmp0_s, vp3);
    tmp0_0 = _mm256_sub_pd(tmp0_s, tmp0_bb);
    tmp0_1 = _mm256_sub_pd(vp4, tmp0_bb);
    tmp0_0 = _mm256_sub_pd(vp3, tmp0_0);
    tmp_t2 = _mm256_add_pd(tmp0_0, tmp0_1);
    tmp_t1 = tmp0_s;
    tmp1_s = _mm256_add_pd(vp5, tmp_t1);
    tmp1_bb = _mm256_sub_pd(tmp1_s, vp5);
    tmp1_0 = _mm256_sub_pd(tmp1_s, tmp1_bb);
    tmp1_1 = _mm256_sub_pd(tmp_t1, tmp1_bb);
    tmp1_0 = _mm256_sub_pd(vp5, tmp1_0);
    tmp_t3 = _mm256_add_pd(tmp1_0, tmp1_1);
    vp3 = tmp1_s;
    tmp2_s = _mm256_add_pd(tmp_t2, tmp_t3);
    tmp2_bb = _mm256_sub_pd(tmp2_s, tmp_t2);
    tmp2_0 = _mm256_sub_pd(tmp2_s, tmp2_bb);
    tmp2_1 = _mm256_sub_pd(tmp_t3, tmp2_bb);
    tmp2_0 = _mm256_sub_pd(tmp_t2, tmp2_0);
    vp5 = _mm256_add_pd(tmp2_0, tmp2_1);
    vp4 = tmp2_s;


    /* compute (s0, s1, s2) = (p2, q1, q2) + (p3, p4, p5). */
    //s0 = two_sum(p2, p3, &t0);
    tmp0_s = _mm256_add_pd(vp2, vp3);
    tmp0_bb = _mm256_sub_pd(tmp0_s, vp2);
    tmp0_0 = _mm256_sub_pd(tmp0_s, tmp0_bb);
    tmp0_1 = _mm256_sub_pd(vp3, tmp0_bb);
    tmp0_0 = _mm256_sub_pd(vp2, tmp0_0);
    vt0 = _mm256_add_pd(tmp0_0, tmp0_1);
    vs0 = tmp0_s;

    //s1 = two_sum(q1, p4, &t1);
    tmp1_s = _mm256_add_pd(vq1, vp4);
    tmp1_bb = _mm256_sub_pd(tmp1_s, vq1);
    tmp1_0 = _mm256_sub_pd(tmp1_s, tmp1_bb);
    tmp1_1 = _mm256_sub_pd(vp4, tmp1_bb);
    tmp1_0 = _mm256_sub_pd(vq1, tmp1_0);
    vt1 = _mm256_add_pd(tmp1_0, tmp1_1);
    vs1 = tmp1_s;

    //s2 = q2 + p5;
    vs2 = _mm256_add_pd(vq2, vp5);

    //s1 = two_sum(s1, t0, &t0);
    tmp2_s = _mm256_add_pd(vs1, vt0);
    tmp2_bb = _mm256_sub_pd(tmp2_s, vs1);
    tmp2_0 = _mm256_sub_pd(tmp2_s, tmp2_bb);
    tmp2_1 = _mm256_sub_pd(vt0, tmp2_bb);
    tmp2_0 = _mm256_sub_pd(vs1, tmp2_0);
    vt0 = _mm256_add_pd(tmp2_0, tmp2_1);
    vs1 = tmp2_s;

    //s2 += (t0 + t1);
    tmp0_0 = _mm256_add_pd(vt0, vt1);
    vs2 = _mm256_add_pd(vs2, tmp0_0);

    /* O(eps^3) order terms */
    /* p6 = two_prod(lo.d0[i], ro.d3[i], &q6);
    p7 = two_prod(lo.d1[i], ro.d2[i], &q7);
    p8 = two_prod(lo.d2[i], ro.d1[i], &q8);
    p9 = two_prod(lo.d3[i], ro.d0[i], &q9); */
    vp6 = _mm256_mul_pd(ld0, rd3);
    vp7 = _mm256_mul_pd(ld1, rd2);
    vp8 = _mm256_mul_pd(ld2, rd1);
    vp9 = _mm256_mul_pd(ld3, rd0);
    vq6 = _mm256_fmsub_pd(ld0, rd3, vp6);
    vq7 = _mm256_fmsub_pd(ld1, rd2, vp7);
    vq8 = _mm256_fmsub_pd(ld2, rd1, vp8);
    vq9 = _mm256_fmsub_pd(ld3, rd0, vp9);
    

    /* Nine-Two-Sum of q0, s1, q3, q4, q5, p6, p7, p8, p9. */
    //q0 = two_sum(q0, q3, &q3);
    tmp0_s = _mm256_add_pd(vq0, vq3);
    tmp0_bb = _mm256_sub_pd(tmp0_s, vq0);
    tmp0_0 = _mm256_sub_pd(tmp0_s, tmp0_bb);
    tmp0_1 = _mm256_sub_pd(vq3, tmp0_bb);
    tmp0_0 = _mm256_sub_pd(vq0, tmp0_0);
    vq3 = _mm256_add_pd(tmp0_0, tmp0_1);
    vq0 = tmp0_s;

    //q4 = two_sum(q4, q5, &q5);
    tmp1_s = _mm256_add_pd(vq4, vq5);
    tmp1_bb = _mm256_sub_pd(tmp1_s, vq4);
    tmp1_0 = _mm256_sub_pd(tmp1_s, tmp1_bb);
    tmp1_1 = _mm256_sub_pd(vq5, tmp1_bb);
    tmp1_0 = _mm256_sub_pd(vq4, tmp1_0);
    vq5 = _mm256_add_pd(tmp1_0, tmp1_1);
    vq4 = tmp1_s;

    //p6 = two_sum(p6, p7, &p7);
    tmp2_s = _mm256_add_pd(vp6, vp7);
    tmp2_bb = _mm256_sub_pd(tmp2_s, vp6);
    tmp2_0 = _mm256_sub_pd(tmp2_s, tmp2_bb);
    tmp2_1 = _mm256_sub_pd(vp7, tmp2_bb);
    tmp2_0 = _mm256_sub_pd(vp6, tmp2_0);
    vp7 = _mm256_add_pd(tmp2_0, tmp2_1);
    vp6 = tmp2_s;

    //p8 = two_sum(p8, p9, &p9);
    tmp0_s = _mm256_add_pd(vp8, vp9);
    tmp0_bb = _mm256_sub_pd(tmp0_s, vp8);
    tmp0_0 = _mm256_sub_pd(tmp0_s, tmp0_bb);
    tmp0_1 = _mm256_sub_pd(vp9, tmp0_bb);
    tmp0_0 = _mm256_sub_pd(vp8, tmp0_0);
    vp9 = _mm256_add_pd(tmp0_0, tmp0_1);
    vp8 = tmp0_s;

    /* Compute (t0, t1) = (q0, q3) + (q4, q5). */
    //t0 = two_sum(q0, q4, &t1);
    tmp1_s = _mm256_add_pd(vq0, vq4);
    tmp1_bb = _mm256_sub_pd(tmp1_s, vq0);
    tmp1_0 = _mm256_sub_pd(tmp1_s, tmp1_bb);
    tmp1_1 = _mm256_sub_pd(vq4, tmp1_bb);
    tmp1_0 = _mm256_sub_pd(vq0, tmp1_0);
    vt1 = _mm256_add_pd(tmp1_0, tmp1_1);
    vt0 = tmp1_s;

    //t1 += (q3 + q5);
    tmp0_0 = _mm256_add_pd(vq3, vq5);
    vt1 = _mm256_add_pd(tmp0_0, vt1);

    /* Compute (r0, r1) = (p6, p7) + (p8, p9). */
    //r0 = two_sum(p6, p8, &r1);
    tmp2_s = _mm256_add_pd(vp6, vp8);
    tmp2_bb = _mm256_sub_pd(tmp2_s, vp6);
    tmp2_0 = _mm256_sub_pd(tmp2_s, tmp2_bb);
    tmp2_1 = _mm256_sub_pd(vp8, tmp2_bb);
    tmp2_0 = _mm256_sub_pd(vp6, tmp2_0);
    vr1 = _mm256_add_pd(tmp2_0, tmp2_1);
    vr0 = tmp2_s;

    //r1 += (p7 + p9);
    tmp1_0 = _mm256_add_pd(vp7, vp9);
    vr1 = _mm256_add_pd(tmp1_0, vr1);

    /* Compute (q3, q4) = (t0, t1) + (r0, r1). */
    //q3 = two_sum(t0, r0, &q4);
    tmp0_s = _mm256_add_pd(vt0, vr0);
    tmp0_bb = _mm256_sub_pd(tmp0_s, vt0);
    tmp0_0 = _mm256_sub_pd(tmp0_s, tmp0_bb);
    tmp0_1 = _mm256_sub_pd(vr0, tmp0_bb);
    tmp0_0 = _mm256_sub_pd(vt0, tmp0_0);
    vq4 = _mm256_add_pd(tmp0_0, tmp0_1);
    vq3 = tmp0_s;

    //q4 += (t1 + r1);
    tmp2_0 = _mm256_add_pd(vt1, vr1);
    vq4 = _mm256_add_pd(tmp2_0, vq4);

    /* Compute (t0, t1) = (q3, q4) + s1. */
    //t0 = two_sum(q3, s1, &t1);
    tmp1_s = _mm256_add_pd(vq3, vs1);
    tmp1_bb = _mm256_sub_pd(tmp1_s, vq3);
    tmp1_0 = _mm256_sub_pd(tmp1_s, tmp1_bb);
    tmp1_1 = _mm256_sub_pd(vs1, tmp1_bb);
    tmp1_0 = _mm256_sub_pd(vq3, tmp1_0);
    vt1 = _mm256_add_pd(tmp1_0, tmp1_1);
    vt0 = tmp1_s;

    //t1 += q4;
    vt1 = _mm256_add_pd(vt1, vq4);

    /* O(eps^4) terms -- Nine-One-Sum */
    //t1 += lo.d1[i] * ro.d3[i] + lo.d2[i] * ro.d2[i] + lo.d3[i] * ro.d1[i] + q6 + q7 + q8 + q9 + s2;
    tmp0_0 = _mm256_fmadd_pd(ld1, rd3, vq6);
    tmp1_0 = _mm256_fmadd_pd(ld2, rd2, vq7);
    tmp2_0 = _mm256_fmadd_pd(ld3, rd1, vq8);
    tmp0_0 = _mm256_add_pd(tmp0_0, vq9);
    tmp1_0 = _mm256_add_pd(tmp1_0, vs2);
    tmp2_0 = _mm256_add_pd(tmp2_0, vt1);
    tmp0_0 = _mm256_add_pd(tmp0_0, tmp1_0);
    vt1 = _mm256_add_pd(tmp0_0, tmp2_0);

    /* renorm5(&p0, &p1, &s0, &t0, &t1);
    lo.d0[i] = p0;
    lo.d1[i] = p1;
    lo.d2[i] = s0;
    lo.d3[i] = t0; */
    _mm256_store_pd(lo.d0 + i, vp0);
    _mm256_store_pd(lo.d1 + i, vp1);
    _mm256_store_pd(lo.d2 + i, vs0);
    _mm256_store_pd(lo.d3 + i, vt0);
    _mm256_store_pd(tmp_vec, vt1);
    renorm5(lo.d0 + i, lo.d1 + i, lo.d2 + i, lo.d3 + i, tmp_vec);
    renorm5(lo.d0 + i + 1, lo.d1 + i + 1, lo.d2 + i + 1, lo.d3 + i + 1, tmp_vec + 1);
    renorm5(lo.d0 + i + 2, lo.d1 + i + 2, lo.d2 + i + 2, lo.d3 + i + 2, tmp_vec + 2);
    renorm5(lo.d0 + i + 3, lo.d1 + i + 3, lo.d2 + i + 3, lo.d3 + i + 3, tmp_vec + 3);
  }

  _mm_free(tmp_vec);

  for (; i < size; i++) {
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
    lo.d0[i] = p0;
    lo.d1[i] = p1;
    lo.d2[i] = s0;
    lo.d3[i] = t0;
  }
}


/* int main() {

  srand(11);
  qd_arr a=qd_arr_create_random_aligned(20,-1,1);
  qd_arr b=qd_arr_create_random_aligned(20,-1,1);
  print_qd_arr(a,2,"a");
  print_qd_arr(b,2,"b");
  qd_arr c=qd_arr_mul(a,b);
  qd_arr_mul_inplace_vec(a,b);
  print_qd_arr(c,2,"ref");
  print_qd_arr(a,2,"ans");
  qd_destroy_aligned(a);
  qd_destroy_aligned(b);
  qd_destroy(c);
  return 0;
} */