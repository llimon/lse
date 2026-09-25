#ifndef MBSTOWCS_COMPAT_H
#define MBSTOWCS_COMPAT_H

#include <sys/types.h>

#ifndef COMPAT_MBSTOWCS_COMPAT_H
#define COMPAT_MBSTOWCS_COMPAT_H

/* ====================================================================
 * SECTION 1: SYSTEM INCLUDES, TYPEDEFS, AND MACRO DEFINITIONS (Goes to .h)
 * ==================================================================== */


#if defined(__sun__) && (defined(__SVR4) || defined(__svr4__))
#endif

#ifndef _WCHAR_T
#  ifndef _WCHAR_T_DEFINED
     typedef long wchar_t;
#    define _WCHAR_T
#    define _WCHAR_T_DEFINED
#  endif
#endif

#ifndef COMPAT_MBSTATE_T_COMPAT
#  ifndef _MBSTATE_T
     typedef struct {
         int __opaque[4];
     } mbstate_t;
#    define _MBSTATE_T
#    define COMPAT_MBSTATE_T_COMPAT
#  endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

size_t mbrtowc(wchar_t *pwc, const char *s, size_t n, mbstate_t *ps);
size_t mbstowcs(wchar_t *dest, const char *src, size_t n);

#ifdef __cplusplus
}
#endif

/* ====================================================================
 * SECTION 2: IMPLEMENTATION & UNIT TEST (Goes to .c)
 * ==================================================================== */

#endif /* COMPAT_MBSTOWCS_COMPAT_H */

#endif /* MBSTOWCS_COMPAT_H */
