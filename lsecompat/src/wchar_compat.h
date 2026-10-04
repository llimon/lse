#ifndef _LSE_WCHAR_COMPAT_H
#define _LSE_WCHAR_COMPAT_H

#ifdef __cplusplus
extern "C" {
#endif

/* Ensure size_t is defined without dragging in standard headers */
#ifndef _SIZE_T
#define _SIZE_T
typedef unsigned long size_t;
#endif

/* Core Types */
#ifndef _WCHAR_T
#define _WCHAR_T
typedef long wchar_t;
#endif

#ifndef _WINT_T
#define _WINT_T
typedef long wint_t;
#endif

#ifndef WEOF
#define WEOF ((wint_t)(-1))
#endif

#ifndef _MBSTATE_T
#define _MBSTATE_T
typedef struct {
    int __count;
    unsigned long __value;
} mbstate_t;
#endif

/* Clean C Prototypes */
extern wint_t getwchar(void);
extern wint_t putwchar(wchar_t);
extern wint_t fgetwc(void *);
extern wint_t fputwc(wchar_t, void *);
extern wchar_t *wcscpy(wchar_t *, const wchar_t *);
extern size_t wcslen(const wchar_t *);

#ifdef __cplusplus
}
#endif

#endif /* _LSE_WCHAR_COMPAT_H */
