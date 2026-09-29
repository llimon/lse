#ifndef GETPROGNAME_COMPAT_H
#define GETPROGNAME_COMPAT_H

#include <sys/types.h>

#ifndef GETPROGNAME_COMPAT_H
#define GETPROGNAME_COMPAT_H

/* ====================================================================
 * SECTION 1: SYSTEM INCLUDES AND DECLARATIONS (Goes to .h)
 * ==================================================================== */

#include <sys/types.h>

/* SVR4 ProcFS header guard for Solaris 2.x vs SunOS 4.x */
#if defined(__sun__) && (defined(__SVR4) || defined(__svr4__))
#  define HAS_PROCFS_SVR4 1
#endif

#ifdef __cplusplus
extern "C" {
#endif

char *compat_pid_to_str(char *buf);
const char *getprogname(void);
void setprogname(const char *name);

#ifdef __cplusplus
}
#endif

/* ====================================================================
 * SECTION 2: IMPLEMENTATION & UNIT TEST (Goes to .c)
 * ==================================================================== */

#endif /* GETPROGNAME_COMPAT_H */

#endif /* GETPROGNAME_COMPAT_H */
