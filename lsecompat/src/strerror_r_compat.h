#ifndef STRERROR_R_COMPAT_H
#define STRERROR_R_COMPAT_H

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

int strerror_r(int errnum, char *buf, size_t buflen);

#ifdef __cplusplus
}
#endif

#endif /* STRERROR_R_COMPAT_H */
