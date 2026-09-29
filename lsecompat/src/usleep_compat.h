#ifndef USLEEP_COMPAT_H
#define USLEEP_COMPAT_H

#include <sys/types.h>

#ifndef COMPAT_USLEEP_COMPAT_H
#define COMPAT_USLEEP_COMPAT_H

/* ====================================================================
 * SECTION 1: SYSTEM INCLUDES AND DECLARATIONS (Goes to .h)
 * ==================================================================== */

#include <sys/types.h>

/* Fallback typedef for systems lacking useconds_t (Solaris 2.5.1 / SunOS 4.x) */
#ifndef _USECONDS_T
#  ifndef __useconds_t_defined
     typedef unsigned long useconds_t;
#    define _USECONDS_T
#    define __useconds_t_defined
#  endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Standard prototype for header file */
int usleep(useconds_t usec);

#ifdef __cplusplus
}
#endif

/* ====================================================================
 * SECTION 2: IMPLEMENTATION & UNIT TEST (Goes to .c)
 * ==================================================================== */

#endif /* COMPAT_USLEEP_COMPAT_H */

#endif /* USLEEP_COMPAT_H */
