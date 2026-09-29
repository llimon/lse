/****
 ** Copyright(c) 2025, Luis Enrique Limon
 ** Custom implementation of isinf for C99 floating-point classification.
 ****/

#ifndef COMPAT_MATH_COMPAT_H
#define COMPAT_MATH_COMPAT_H

/* ====================================================================
 * SECTION 1: SYSTEM INCLUDES, TYPEDEFS, AND MACRO DEFINITIONS (Goes to .h)
 * ==================================================================== */

#include <math.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

int isinf_float(float x);
int isinf_double(double x);

#ifdef __cplusplus
}
#endif

/* Macro selector for type dispatch based on expression size */
#ifndef isinf
#define isinf(x) \
    (sizeof(x) == sizeof(float) ? isinf_float(x) : isinf_double(x))
#endif

/* ====================================================================
 * SECTION 2: IMPLEMENTATION & UNIT TEST (Goes to .c)
 * ==================================================================== */
/* BEGIN_IMPL */

#if defined(__GNUC__)
#  if defined(__ELF__) || defined(__solaris__)
#    pragma weak isinf_float
#    pragma weak isinf_double
#  else
     int isinf_float(float x)  __attribute__((weak));
     int isinf_double(double x) __attribute__((weak));
#  endif
#endif

/*
 * Custom isinf for single-precision float types.
 */
int isinf_float(float x) {
    union {
        float f;
        uint32_t i;
    } u;

    u.f = x;
    return (u.i & 0x7FFFFFFF) == 0x7F800000;
}

/*
 * Custom isinf for double-precision float types.
 */
int isinf_double(double x) {
    union {
        double d;
        uint64_t i;
    } u;

    u.d = x;
    return (u.i & 0x7FFFFFFFFFFFFFFFLL) == 0x7FF0000000000000LL;
}

/* Embedded Standalone Unit Test */
#ifdef _TEST_MATH_COMPAT_CUSTOM

#include <stdio.h>
#include <assert.h>

static void verify_float_isinf(void) {
    printf("1. Verifying single-precision float isinf_float()...\n");

    float normal_f = 3.14159f;
    float zero_f = 0.0f;
    float neg_zero_f = -0.0f;
    float pos_inf_f = 1.0f / 0.0f;
    float neg_inf_f = -1.0f / 0.0f;
    float nan_f = 0.0f / 0.0f;

    /* Verify normal numbers, zero, and NaN are NOT infinite */
    assert(isinf(normal_f) == 0);
    assert(isinf(zero_f) == 0);
    assert(isinf(neg_zero_f) == 0);
    assert(isinf(nan_f) == 0);

    /* Verify positive and negative infinity ARE detected */
    assert(isinf(pos_inf_f) != 0);
    assert(isinf(neg_inf_f) != 0);

    printf("   -> Single-precision float tests passed successfully!\n");
}

static void verify_double_isinf(void) {
    printf("2. Verifying double-precision isinf_double()...\n");

    double normal_d = 2.718281828459045;
    double zero_d = 0.0;
    double neg_zero_d = -0.0;
    double pos_inf_d = 1.0 / 0.0;
    double neg_inf_d = -1.0 / 0.0;
    double nan_d = 0.0 / 0.0;

    /* Verify normal numbers, zero, and NaN are NOT infinite */
    assert(isinf(normal_d) == 0);
    assert(isinf(zero_d) == 0);
    assert(isinf(neg_zero_d) == 0);
    assert(isinf(nan_d) == 0);

    /* Verify positive and negative infinity ARE detected */
    assert(isinf(pos_inf_d) != 0);
    assert(isinf(neg_inf_d) != 0);

    printf("   -> Double-precision double tests passed successfully!\n");
}

int main(void) {
    printf("==================================================\n");
    printf(" Running C99 isinf Floating-Point Classification Test\n");
    printf("==================================================\n");
    verify_float_isinf();
    verify_double_isinf();
    printf("\n>>> System-wide math_compat / isinf suite FULLY VERIFIED. <<<\n");
    return 0;
}

#endif /* _TEST_MATH_COMPAT_CUSTOM */

/* END_IMPL */

#endif /* COMPAT_MATH_COMPAT_H */
