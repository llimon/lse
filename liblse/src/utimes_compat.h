#ifndef UTIMES_COMPAT_H
#define UTIMES_COMPAT_H

#include <sys/types.h>
#include <sys/time.h>

#ifndef COMPAT_UTIMES_COMPAT_H
#define COMPAT_UTIMES_COMPAT_H

/* ====================================================================
 * SECTION 1: SYSTEM INCLUDES AND DECLARATIONS (Goes to .h)
 * ==================================================================== */


#ifdef __cplusplus
extern "C" {
#endif

/* Standard prototype for header file */
int utimes(const char *path, const struct timeval times[2]);

#ifdef __cplusplus
}
#endif

/* ====================================================================
 * SECTION 2: IMPLEMENTATION & UNIT TEST (Goes to .c)
 * ==================================================================== */

#endif /* COMPAT_UTIMES_COMPAT_H */

#endif /* UTIMES_COMPAT_H */
