#ifndef _TIME_COMPAT_H
#define _TIME_COMPAT_H

#include <sys/types.h>
#include <sys/time.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

/* POSIX.1b Clock Identifiers if missing */
#ifndef CLOCK_REALTIME
#  define CLOCK_REALTIME 0
#endif

#ifndef CLOCK_MONOTONIC
#  define CLOCK_MONOTONIC 1
#endif

/* 
 * If system headers didn't declare prototypes, declare them here.
 * Weak binding in time_compat.c will supply implementation without symbol conflicts.
 */
int utimes(const char *path, const struct timeval times[2]);

#ifdef __cplusplus
}
#endif

#endif /* _TIME_COMPAT_H */
