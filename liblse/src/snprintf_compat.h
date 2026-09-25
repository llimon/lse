#ifndef SNPRINTF_COMPAT_H
#define SNPRINTF_COMPAT_H

#include <sys/types.h>

#ifndef LSE_SNPRINTF_COMPAT_H
#define LSE_SNPRINTF_COMPAT_H

/* ====================================================================
 * SECTION 1: SYSTEM INCLUDES, TYPEDEFS, AND MACRO DEFINITIONS (Goes to .h)
 * ==================================================================== */

#include <stdarg.h>
#include <stddef.h>

#define PORTABLE_SNPRINTF_VERSION_MAJOR 2
#define PORTABLE_SNPRINTF_VERSION_MINOR 2

#ifdef __cplusplus
extern "C" {
#endif

int snprintf(char *str, size_t str_m, const char *fmt, ...);
int vsnprintf(char *str, size_t str_m, const char *fmt, va_list ap);

int asprintf(char **ptr, const char *fmt, ...);
int vasprintf(char **ptr, const char *fmt, va_list ap);
int asnprintf(char **ptr, size_t str_m, const char *fmt, ...);
int vasnprintf(char **ptr, size_t str_m, const char *fmt, va_list ap);

#ifdef __cplusplus
}
#endif

/* ====================================================================
 * SECTION 2: IMPLEMENTATION & WEAK LINKING (Goes to .c)
 * ==================================================================== */

#endif /* LSE_SNPRINTF_COMPAT_H */
#endif /* SNPRINTF_COMPAT_H */
