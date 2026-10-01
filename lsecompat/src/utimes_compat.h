#ifndef UTIMES_COMPAT_H
#define UTIMES_COMPAT_H

#include <sys/types.h>


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

#endif /* UTIMES_COMPAT_H */
