#ifndef WCSSTR_COMPAT_H
#define WCSSTR_COMPAT_H

#include <sys/types.h>

#ifndef COMPAT_WCSSTR_COMPAT_H
#define COMPAT_WCSSTR_COMPAT_H

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

#ifdef __cplusplus
extern "C" {
#endif

wchar_t *wcsstr(const wchar_t *haystack, const wchar_t *needle);

#ifdef __cplusplus
}
#endif

/* ====================================================================
 * SECTION 2: IMPLEMENTATION & UNIT TEST (Goes to .c)
 * ==================================================================== */

#endif /* COMPAT_WCSSTR_COMPAT_H */

#endif /* WCSSTR_COMPAT_H */
