#ifndef STRTOIMAX_COMPAT_H
#define STRTOIMAX_COMPAT_H

#include <sys/types.h>

/* ====================================================================
 * SECTION 1: SYSTEM INCLUDES, TYPEDEFS, AND MACRO DEFINITIONS (Goes to .h)
 * ==================================================================== */

#include <stdint.h>
#include <stddef.h>

/* C99 inttypes format specifiers for 64-bit intmax_t / uintmax_t */
#ifndef PRIdMAX
#define PRIdMAX "lld"
#endif

#ifndef PRIuMAX
#define PRIuMAX "llu"
#endif

#ifndef PRIxMAX
#define PRIxMAX "llx"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Standard POSIX prototypes for header file */
intmax_t  strtoimax(const char *nptr, char **endptr, int base);
uintmax_t strtoumax(const char *nptr, char **endptr, int base);

#ifdef __cplusplus
}
#endif

#endif /* STRTOIMAX_COMPAT_H */
