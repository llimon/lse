#ifndef ATOMIC_COMPAT_H
#define ATOMIC_COMPAT_H

#include <sys/types.h>

/* ====================================================================
 * SECTION 1: SYSTEM INCLUDES, TYPEDEFS, AND MACRO DEFINITIONS
 * ==================================================================== */

/* Fallback integer types for older SunOS 4 environments lacking <stdint.h> */
#if defined(HAVE_STDINT_H) || defined(SOLARIS_COMPATIBLE) || defined(__STDC_VERSION__)
# include <stdint.h>
#else
# ifndef _INT32_T
#  define _INT32_T
   typedef int int32_t;
# endif
# ifndef _UINT32_T
#  define _UINT32_T
   typedef unsigned int uint32_t;
# endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* --------------------------------------------------------------------
 * Standard GCC 32-bit __sync Atomic Prototypes
 * -------------------------------------------------------------------- */
int32_t __sync_fetch_and_add_4(volatile void *ptr, int32_t value);
int32_t __sync_add_and_fetch_4(volatile void *ptr, int32_t value);
int32_t __sync_val_compare_and_swap_4(volatile void *ptr, int32_t oldval, int32_t newval);
int32_t __sync_lock_test_and_set_4(volatile void *ptr, int32_t value);
void    __sync_lock_release_4(volatile void *ptr);

/* Generic untyped wrappers */
int __sync_fetch_and_add(volatile void *ptr, int value);
int __sync_add_and_fetch(volatile void *ptr, int value);
int __sync_val_compare_and_swap(volatile void *ptr, int oldval, int newval);

/* --------------------------------------------------------------------
 * Fallback Function Prototypes for Legacy/Non-GCC Compilers
 * -------------------------------------------------------------------- */
#if defined(__sun) || defined(sun)
# if !defined(__GNUC__) || (__GNUC__ < 4)
#  ifndef __sync_fetch_and_add
#   define __sync_fetch_and_add(ptr, val)       lse_sync_fetch_and_add((volatile void *)(ptr), (int)(val))
#   define __sync_add_and_fetch(ptr, val)       lse_sync_add_and_fetch((volatile void *)(ptr), (int)(val))
#   define __sync_val_compare_and_swap(p, o, n) lse_sync_val_compare_and_swap((volatile void *)(p), (int)(o), (int)(n))
#  endif

   int lse_sync_fetch_and_add(volatile void *ptr, int value);
   int lse_sync_add_and_fetch(volatile void *ptr, int value);
   int lse_sync_val_compare_and_swap(volatile void *ptr, int oldval, int newval);
# endif
#endif

#ifdef __cplusplus
}
#endif

#endif /* ATOMIC_COMPAT_H */
