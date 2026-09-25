#ifndef FTELLO_COMPAT_H
#define FTELLO_COMPAT_H

#include <sys/types.h>

#ifndef LSE_FTELLO_COMPAT_H
#define LSE_FTELLO_COMPAT_H

/* ====================================================================
 * SECTION 1: SYSTEM INCLUDES, TYPEDEFS, AND MACRO DEFINITIONS (Goes to .h)
 * ==================================================================== */

#include <stdio.h> /* KEEP */
#include <sys/types.h>

/* Native 64-bit offset type present on all Solaris 2.x releases */
#if defined(SOLARIS2) || defined(__solaris__) || defined(__SVR4)
  /* Native 64-bit offset type present on all Solaris 2.x releases */
  typedef long long offset_t;
#else
  /* SunOS 4.1.x (BSD 4.3): stdio and UFS use 32-bit long offsets */
  typedef long offset_t;
#endif

/*
 * Native SunOS kernel system call. Guaranteed to exist in libc.so.1
 * on EVERY Solaris 2.5.1 installation.
 */
extern offset_t llseek(int fd, offset_t offset, int whence);

#ifdef __cplusplus
extern "C" {
#endif

/* Standard POSIX prototypes for header file */
offset_t ftello(FILE *stream);
int fseeko(FILE *stream, offset_t offset, int whence);

#ifdef __cplusplus
}
#endif

/* ====================================================================
 * SECTION 2: STATIC INLINE FUNCTIONS & IMPLEMENTATION OVERRIDES (Goes to .c)
 * ==================================================================== */

#endif /* LSE_FTELLO_COMPAT_H */

#endif /* FTELLO_COMPAT_H */
