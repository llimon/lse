#ifndef _LSE_WCHAR_COMPAT_H
#define _LSE_WCHAR_COMPAT_H

/* Include system /usr/include/wchar.h to get native types and prototypes */
#include_next <wchar.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Provide mbstate_t if missing from system wchar.h */
#if !defined(_MBSTATE_T) && !defined(_MBSTATE_T_DEFINED) && !defined(_GLIBCXX_HAVE_MBSTATE_T)
#define _MBSTATE_T
#define _MBSTATE_T_DEFINED
#define _GLIBCXX_HAVE_MBSTATE_T 1

typedef struct {
    int __count;
    unsigned long __value;
} mbstate_t;
#endif

#ifdef __cplusplus
}
#endif

#endif /* _LSE_WCHAR_COMPAT_H */
