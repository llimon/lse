#ifndef MATH_COMPAT_H
#define MATH_COMPAT_H

#include <sys/types.h>

/****
 ** Copyright(c) 2025, Luis Enrique Limon
 ** Custom implementation of isinf for C99 floating-point classification.
 ****/

/* ====================================================================
 * SECTION 1: SYSTEM INCLUDES, TYPEDEFS, AND MACRO DEFINITIONS (Goes to .h)
 * ==================================================================== */

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

#endif /* MATH_COMPAT_H */
